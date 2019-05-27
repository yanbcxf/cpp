#include "InterfaceGlobal.h"
#include "InterfaceImp.h"
#include "./db/DBOperator.h"
#include "./xml/xmlClient.hpp"
#include "./xml/UrlUtils.hpp"
#include "./xml/XmlUtils.hpp"
#include <direct.h>

InterfaceImp *g_pInterfaceImp = NULL;

InterfaceImp::InterfaceImp()
{
	m_pConfigCB = NULL; //配置信息变更通知回调函数
	m_pAlarmCB = NULL;	//报警通知回调函数
	m_pControlReqCB = NULL; //控制返回通知回调函数
	m_pInfoReqCB = NULL; //命令返回通知回调函数
	m_pMediaCB = NULL; //媒体交互通知回调函数

	m_pJMSConsumer = NULL;
	m_pJMSProducer = NULL;

	memset(&m_cmsThread,0,sizeof(m_cmsThread));
	memset(&m_SipServerCfg,0,sizeof(sipd_servercfg_config_ind));

	m_DeviceMapMutex.Init();
	m_DeviceRightMapMutex.Init();
	m_PTZControlLstMutex.Init();
	m_QueryThreadPoolMutex.Init();
	m_ControlThreadPoolMutex.Init();

	m_CMSServer.strServerIP = "";
	m_CMSServer.strLocalIP = "127.0.0.1";
	m_CMSServer.nServerPort = 0;
	m_CMSServer.strPassWord = "";
	m_CMSServer.strUserName="";
	m_CMSServer.strUserID = "";

	m_mediaServer.strServerIP = "127.0.0.1";
	m_mediaServer.nServerPort = 7077;

	m_ReadSipCfgStep = -1;

}

InterfaceImp::~InterfaceImp()
{
	if(m_pJMSConsumer!=NULL)
	{
		m_pJMSConsumer->close();
		delete m_pJMSConsumer;
		m_pJMSConsumer = NULL;
	}

	if(m_pJMSProducer!=NULL)
	{
		m_pJMSProducer->close();
		delete m_pJMSProducer;
		m_pJMSProducer = NULL;
	}

	m_ControlThreadPool.Shutdown();
	m_QueryThreadPool.Shutdown();

	m_cmsThread.isExit = true;
	m_MediaServerProcThread.isExit = true;
	for(int i=0;i<30;i++)
	{
		if(!m_cmsThread.isExit)
			break;
		else
			Sleep(100);
	}
	if(!m_cmsThread.isExit)
		CloseHandle(m_cmsThread.ThreadHandle);
	else
	{
		TerminateThread(m_cmsThread.ThreadHandle,0);
		CloseHandle(m_cmsThread.ThreadHandle);
	}

	for(int i=0;i<30;i++)
	{
		if(!m_MediaServerProcThread.isExit)
			break;
		else
			Sleep(100);
	}
	if(!m_cmsThread.isExit)
		CloseHandle(m_MediaServerProcThread.ThreadHandle);
	else
	{
		TerminateThread(m_MediaServerProcThread.ThreadHandle,0);
		CloseHandle(m_MediaServerProcThread.ThreadHandle);
	}


}

int InterfaceImp::Init()
{
	PropertyConfigurator::doConfigure("log4cplus.properties");
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<__FUNCTION__<<"enter Init";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	WORD version = MAKEWORD(2,2);
	WSADATA wsaData;
	int err = ::WSAStartup(version, &wsaData);
	if( 0 != err )
	{
		DWORD dw = ::WSAGetLastError();
		return -1;
	}

	m_QueryThreadPool.Initialize((void *)WorkerProc,MAX_QUERY_POOL);
	m_ControlThreadPool.Initialize((void *)WorkerProc,MAX_CONTROL_POOL);

	activemq::library::ActiveMQCPP::initializeLibrary();
	

	ReadCfg();

	if(m_pJMSConsumer==NULL)
	{
		char brokerURI[64] ={0};
		sprintf(brokerURI,"failover:(tcp://%s:51616)",m_LocalCfg.strWebIP.c_str());
		bool useTopics = true;
		bool sessionTransacted = true;
		int numMessages = 10;
		m_pJMSConsumer = new JMSConsumer(brokerURI, numMessages, useTopics, sessionTransacted);
		if(m_pJMSConsumer==NULL)
		{
			return -1;
		}
		m_pJMSConsumer->setCallBackParam(this,JMSNotifyProc);

		// Start the consumer thread.
		Thread consumerThread(m_pJMSConsumer);
		consumerThread.start();

		// Wait for the consumer to indicate that its ready to go.
		m_pJMSConsumer->waitUntilReady();
		consumerThread.join();
	}

	if(m_pJMSProducer==NULL)
	{
		char brokerURI[64] ={0};
		sprintf(brokerURI,"failover:(tcp://%s:51616)",m_LocalCfg.strWebIP.c_str());

		bool useTopics = false;
		bool sessionTransacted = false;
		int numMessages = 10;
		m_pJMSProducer = new JMSProducer(brokerURI, numMessages, useTopics, sessionTransacted);
		if(m_pJMSProducer==NULL)
		{
			return -1;
		}

		m_pJMSProducer->setCallBackParam(this,JMSQueryNotifyProc);
		// Start the producer thread.
		Thread producerThread(m_pJMSProducer);
		producerThread.start();
		producerThread.join();

	}
	return 0;
}

int InterfaceImp::Start()
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<__FUNCTION__<<"enter Start";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	int nRet = 0;
	////连接媒体网关
	nRet = StartConnectMediaSever();
	if(nRet<0)
	{
		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<" 开启媒体网关线程失败！";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());

		return -1;
	}

	while(1)
	{
		//读取网关配置信息
		nRet = GetGWCfg();
		if(nRet<0)
		{
			ss.clear();
			ss.str("");
			ss<<__FUNCTION__<<" 获取网关配置失败！";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			Sleep(1000);
			continue;
		}

		//获取用户名ID
		nRet = GetUserID();
		if(nRet<0)
		{
			ss.clear();
			ss.str("");
			ss<<__FUNCTION__<<" 获取用户名ID失败！";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			Sleep(1000);
			continue;
		}

		//获取设备列表
		nRet = GetAllDevice();
		if(nRet<0)
		{
			ss.clear();
			ss.str("");
			ss<<__FUNCTION__<<" 获取设备列表失败！";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			Sleep(1000);
			continue;
		}

		//获取位置信息
		nRet = GetAllDevicePos();
		if(nRet<0)
		{
			ss.clear();
			ss.str("");
			ss<<__FUNCTION__<<" 获取位置信息失败！";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			Sleep(1000);
			continue;
		}

		//获取各服务信息
		nRet = GetServerInfo();
		if(nRet<0)
		{
			ss.clear();
			ss.str("");
			ss<<__FUNCTION__<<" 获取各服务信息失败！";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			Sleep(1000);
			continue;
		}
		break;
	}
	//登陆CMS
	nRet = StartConnectCMS();
	if(nRet<0)
	{

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<" 登陆CMS失败！user="<<m_CMSServer.strUserID<<"_"<<m_CMSServer.strPassWord;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());

		return -1;
	}
	ss.clear();
	ss.str("");
	ss<<__FUNCTION__<<"_Start suc";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	return nRet;

}

int InterfaceImp::GetGWCfg()
{
	m_ReadSipCfgStep = 0;

	if(m_pJMSProducer!=NULL)
	{
		m_pJMSProducer->Sendms("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root actionType=\"4\" hql=\"from SysConfig\"></root>");
		m_ReadSipCfgStep = 0;
	}
	while(m_ReadSipCfgStep!=1)
		Sleep(100);

	return 0;
}

int InterfaceImp::GetAllDevice()
{
	JMutexAutoLock aLock(m_DeviceMapMutex);
	m_DeviceMap.clear();
	int nRet = getAllGBSnDevice(m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_DeviceMap,m_CMSServer.strUserName);
	return nRet;
}

int InterfaceImp::GetServerInfo()
{
	int nRet = -1;
	//获取CMS信息
	while(1)
	{
		nRet = getServer(CMS_SERVER_ID,m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_CMSServer.strServerIP,m_CMSServer.nServerPort);
		if(nRet>=0)
			break;
		Sleep(1000);
	}

	//获取DMS信息
	nRet = getServer(DMS_SERVER_ID,m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_DMSServer.strServerIP,m_DMSServer.nServerPort);
	if(nRet<0)
		return -1;

	//获取VOD信息
	nRet = getServer(VOD_SERVER_ID,m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_VODServer.strServerIP,m_VODServer.nServerPort);
	if(nRet<0)
		return -1;

	return 0;
}

int InterfaceImp::GetUserID()
{
	int nRet = getUserID(m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_CMSServer.strUserName,m_CMSServer.strUserID);
	return nRet;
}

int InterfaceImp::GetAllDevicePos()
{
	JMutexAutoLock aLock(m_DevicePosMapMutex);
	m_DevicePosMap.clear();
	int nRet = GetSnPosition(m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_DevicePosMap);
	return nRet;
}

int InterfaceImp::ReadCfg()
{
	stringstream ss;
	ss.clear();
	ss.str("");

	char chCurPath[100]={0};
	getcwd(chCurPath,100);

	string fullpath = chCurPath;
	int ipos = fullpath.find_last_of("\\");

	fullpath = fullpath.substr(0,ipos);
	fullpath += "\\GBGate.ini";


	char chIP[24]={0};
	char chPort[6]={0};
	int nRet = read_profile_string("MediaGate","g_iCommandPort",chIP,24,"0",fullpath.c_str());
	m_LocalCfg.nMediaServerPort = atoi(chIP);

	nRet = read_profile_string("SipGate","webIP",chIP,24,"127.0.0.1",fullpath.c_str());
	m_LocalCfg.strWebIP = chIP;

	nRet = read_profile_string("SipGate","gateway",chIP,24,"127.0.0.1",fullpath.c_str());
	m_LocalCfg.strGateway = chIP;

	ss.clear();
	ss.str("");
	ss<<__FUNCTION__<<"webip="<<m_LocalCfg.strWebIP<<"	g_iCommandPort="<<m_LocalCfg.nMediaServerPort;
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	return 0;
}

void InterfaceImp::AddTask(LPTaskInfo pTaskInfo,int nTaskType)//nTaskType 0:查询 1:控制
{
	if(nTaskType==0)//查询
	{
		JMutexAutoLock autoLock(m_QueryThreadPoolMutex);
		m_QueryThreadPool.QueueRequest((CMyWorker::RequestType)pTaskInfo);
	}
	else if(nTaskType==1)//控制
	{
		JMutexAutoLock autoLock(m_ControlThreadPoolMutex);
		m_ControlThreadPool.QueueRequest((CMyWorker::RequestType)pTaskInfo);
	}

}

/*
rtsp://10.18.99.10:10050:350001/10.18.72.235:20012/fact=10&chnl=1&uuid=8a92b276442e183d01442e187a310095
&user=admin&pwd=123456&devsn=Auto13921065878411043505984&pltype=1&frmcond=0_0_1&streamid=0&plymul=1
&ifrminvl=1&tinvl=1&trcode=0&prim=0&setime=20140216000000_20140216235959
*/
int InterfaceImp::QueryRecord(sipd_recordinfo_req * pRecordFileReq)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"QueryRecord";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	sipd_recordinfo_req *pNewRecordFileReq = (sipd_recordinfo_req*)malloc(sizeof(sipd_recordinfo_req));
	LPTaskInfo pTaskInfo = (LPTaskInfo)malloc(sizeof(TaskInfo)); 
	if(pTaskInfo==NULL || pNewRecordFileReq==NULL)
	{
		if(pTaskInfo!=NULL)
			free(pTaskInfo);

		if(pNewRecordFileReq!=NULL)
			free(pNewRecordFileReq);

		return -1;
	}
	memcpy(pNewRecordFileReq,pRecordFileReq,sizeof(sipd_recordinfo_req));

	pTaskInfo->param1 = (void *)pNewRecordFileReq;
	pTaskInfo->lparam = (void *)this;
	pTaskInfo->nTaskType = TASK_QUERY_RECORD;
	AddTask(pTaskInfo,0);
	return 0;
}

int InterfaceImp::QuerySingleRecord(SN_DEVICE snDevice,ServerInfo VODServer,char *cRecordStartTime,char *cRecordEndTime,list<RecordInfo>&RecordTimeLst)
{
	char cUrl[512]={0};
	int nReqCount = 0;
	char cStarTime[MAX_SIPD_TIME_LEN]={0};
	char cEndTime[MAX_SIPD_TIME_LEN]={0};
	sscanf(cRecordStartTime, "%4s-%2s-%2sT%2s:%2s:%2s", cStarTime,cStarTime+4,cStarTime+6,cStarTime+8,cStarTime+10,cStarTime+12);
	sscanf(cRecordEndTime, "%4s-%2s-%2sT%2s:%2s:%2s", cEndTime,cEndTime+4,cEndTime+6,cEndTime+8,cEndTime+10,cEndTime+12);
	sprintf(cUrl,"rtsp://%s:%d:350001/%s:%d/fact=%d&chnl=%d&uuid=%s"
		"&user=%s&pwd=%s&devsn=%s&pltype=2&frmcond=0_0_0&streamid=0&plymul=1&ifrminvl=1"
		"&tinvl=1&trcode=0&prim=0&setime=%s_%s"
		,VODServer.strServerIP.c_str(),VODServer.nServerPort,snDevice.strDevIP.c_str(),snDevice.nDevPort,snDevice.nDevTypeChild,snDevice.nChannelNO,snDevice.strDevId.c_str()
		,snDevice.strDevUser.c_str(),snDevice.strDevPass.c_str(),snDevice.strDevSN.c_str(),cStarTime,cEndTime);

	return QuerySingleRecordEx(VODServer,snDevice.strGBID,snDevice.strDevName,RecordTimeLst,cUrl);
	return 0;
}

int InterfaceImp::SetServerConfigCB(sip_config_pcb_func config_fun)
{
	m_pConfigCB = config_fun; //配置信息变更通知回调函数
	return 0;
}

int InterfaceImp::SetAlarmIndCB(sip_alarm_ind_pcb_func config_fun)
{
	m_pAlarmCB = config_fun;	//报警通知回调函数
	return 0;
}

int InterfaceImp::SetControlRspCB(sip_control_req_pcb_func config_fun)
{
	m_pControlReqCB = config_fun; //控制返回通知回调函数
	return 0;
}

int InterfaceImp::SetInfoRspCB(sip_info_req_pcb_func config_fun)
{
	m_pInfoReqCB = config_fun; //命令返回通知回调函数
	return 0;
}

int InterfaceImp::SetMediaControlCB(sip_media_pcb_func config_fun)
{
	m_pMediaCB = config_fun; //媒体交互通知回调函数
	return 0;
}

int InterfaceImp::RecordControl(sipd_record_control * pRecordControl)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"RecordControl";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	LPTaskInfo pTask = (LPTaskInfo)malloc(sizeof(TaskInfo));
	sipd_record_control *pParam = (sipd_record_control * )malloc(sizeof(sipd_record_control));
	if(pTask==NULL || pParam==NULL)
	{
		if(pTask!=NULL)
			free(pTask);
		if(pParam!=NULL)
			free(pParam);

		return -1;
	}
	memcpy(pParam,pRecordControl,sizeof(sipd_record_control));
	pTask->lparam = this;
	pTask->param1 = pParam;
	pTask->nTaskType = TASK_RECORD_CONTROL;

	AddTask(pTask,0);
	return 0;
}

int InterfaceImp::setSipServerCfg(SIPD_GATEWAY_CFG* pCfg)
{

	if(pCfg==NULL)
		return -1;
	else
		memcpy(pCfg,&m_SipServerCfg.new_cfg,sizeof(SIPD_GATEWAY_CFG));
	return 0;
}

int InterfaceImp::GetDevPos(int nGetType,char *DevPosID,char *Devpos)
{
	JMutexAutoLock aLock(m_DevicePosMapMutex);
	map<string,SN_Position>::iterator iter;
	iter = m_DevicePosMap.find(DevPosID);
	if(iter!=m_DevicePosMap.end())
	{
		strcpy_s(Devpos,MAX_SIPD_NORMAL_LEN,iter->second.strPosName.c_str());
		return 0;
	}
	else
		return -1;
}

int InterfaceImp::StartConnectCMS()
{
	if(m_cmsThread.ThreadHandle!=NULL)
		return 0;

	DWORD dwFlag = 0;
	m_cmsThread.isExit = false;
	m_cmsThread.ThreadHandle = CreateThread(NULL,0,CMSProc,this,0,&dwFlag);
	if(m_cmsThread.ThreadHandle==NULL)
		return -1;
	return 0;
}

int InterfaceImp::StartConnectMediaSever()
{

	m_mediaServer.nServerPort = m_LocalCfg.nMediaServerPort;
	m_mediaServer.strServerIP = m_LocalCfg.strMediaServerIP;

	//int nRet = xml_OpenCon(m_mediaServer);

	//if(nRet<0)
	//{
	//	xml_closeCon(m_mediaServer);
	//	return -1;
	//}
	if(m_MediaServerProcThread.ThreadHandle==NULL)
	{
		DWORD dwFlag = 0;
		m_MediaServerProcThread.isExit = false;
		m_MediaServerProcThread.ThreadHandle = CreateThread(NULL,0,MediaServerProc,this,0,&dwFlag);
		if(m_MediaServerProcThread.ThreadHandle==NULL)
			return -1;
	}
	else
		return 1;

}

int InterfaceImp::RequestMedia(int nType)
{
	if(nType==1)
	{
		MediaRequestInfo MediaSessionInfo;
		bool bFind = false;
		{	
			JMutexAutoLock aLock(m_MediaSeesionMapMutex);
			if(m_MediaRequestLst.size()>0)
			{

				memcpy(&MediaSessionInfo,&m_MediaRequestLst.front(),sizeof(MediaRequestInfo));
				m_MediaRequestLst.pop_front();
				bFind = true;
			}
		}
		if(bFind)
		{
			int nRet = SendMediaRequest(MediaSessionInfo);
			if(nRet<0)
			{
				sipd_sender_sdp_info senderReq;
				memset(&senderReq,0,sizeof(sipd_sender_sdp_info));
				senderReq.did = MediaSessionInfo.sipMediaSeesion.did;
				senderReq.tid = MediaSessionInfo.sipMediaSeesion.tid;
				//sip_media_pcb_func(SIPD_MEDIA_CONTROL_CB_INVITE_RSP,&senderReq);
			}
			return nRet;
		}
		sipd_sender_sdp_info senderReq;
		memset(&senderReq,0,sizeof(sipd_sender_sdp_info));
		senderReq.did = MediaSessionInfo.sipMediaSeesion.did;
		senderReq.tid = MediaSessionInfo.sipMediaSeesion.tid;
		//sip_media_pcb_func(SIPD_MEDIA_CONTROL_CB_INVITE_RSP,&senderReq);
		return -1;
	}
	else if(nType==2)
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		m_MediaRequestLst.clear();
	}


}

int InterfaceImp::SendMediaRequest(MediaRequestInfo MediaSession)
{
	if(MediaSession.nMediaRequestType==1)//setup
		return SendMediaSetupRequest(MediaSession.sipMediaSeesion);
	else if(MediaSession.nMediaRequestType==2)//play
		return SendMediaPlayRequest(MediaSession.sipMediaSeesion);
	else if(MediaSession.nMediaRequestType==3)//control
		return SendMediaControlRequest(MediaSession);
	else if(MediaSession.nMediaRequestType==4)//stop play
		return SendMediaStopRequest(MediaSession);
	else if(MediaSession.nMediaRequestType==5)//stop playback
		return SendMediaStopRequest(MediaSession);
	else if(MediaSession.nMediaRequestType==6)//reboot
		return SendMediaRebootRequest();
}

int InterfaceImp::SendMediaSetupRequest(sipd_media_session MediaSession)
{
	stringstream ss;
	JMutexAutoLock aLock(m_DeviceMapMutex);
	map<string,SN_DEVICE>::iterator iter = m_DeviceMap.find(MediaSession.channel_id);
	if(iter!=m_DeviceMap.end())
	{
		char RequestBuf[1024]={0};
		if(MediaSession.stream_type == SIPD_STREAM_TYPE_PLAY)
		{
			sprintf(RequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root>"
				"<msg_type	sessionid = '%d'>realplay</msg_type>",MediaSession.did);
		}
		else if(MediaSession.stream_type == SIPD_STREAM_TYPE_PLAYBACK)
		{
			sprintf(RequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root>"
				"<msg_type	sessionid = '%d'>playback</msg_type>",MediaSession.did);
		}
		else if(MediaSession.stream_type == SIPD_STREAM_TYPE_DOWNLOAD)
		{
			sprintf(RequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root>"
				"<msg_type	sessionid = '%d'>download</msg_type>",MediaSession.did);
		}
		sprintf(RequestBuf+strlen(RequestBuf),"<device ip='%s' port='%d' user='%s' pwd='%s' devtypechild='%d' rpcport='%d' videoport='%d' webport='%d' firm='%d'  devsn='%s' mode='%d'></device>\r\n"
			"<data media_recv_ip='%s' media_recv_port='%s' vod_ip='%s' vod_port='%d' cms_ip='%s' cms_port='%d' domain='35000101' fact='%d' chnl='%d' uuid='%s' "
			,iter->second.strDevIP.c_str(),iter->second.nDevPort,iter->second.strDevUser.c_str(),iter->second.strDevPass.c_str(),iter->second.nDevTypeChild
			,iter->second.nDevPort,iter->second.nVideoPort,iter->second.nWebPort,iter->second.nDevFirm,iter->second.strDevSN.c_str(),iter->second.nMode,MediaSession.media_recv_ip,MediaSession.media_recv_port,m_VODServer.strServerIP.c_str(),m_VODServer.nServerPort
			,m_CMSServer.strServerIP.c_str(),m_CMSServer.nServerPort,iter->second.nDevFirm,iter->second.nChannelNO,iter->second.strDevId.c_str());
		if(MediaSession.stream_type==SIPD_STREAM_TYPE_PLAY)
			sprintf(RequestBuf+strlen(RequestBuf),"pltype='0' streamid='0' plymul='1' ifrminvl='0' tinvl='3' trcode='0' prim='0' setime='0'></data></root>%s",CMS_END_FLAG);
		else
		{
			char cTime[64]={0};
			struct tm *startTm,*endTm;
			time_t stm = _atoi64(MediaSession.t_start_time);
			time_t etm = _atoi64(MediaSession.t_end_time);
			startTm = localtime(&stm);
			sprintf(cTime,"%04d%02d%02d%02d%02d%02d",startTm->tm_year+1900,startTm->tm_mon+1,startTm->tm_mday,startTm->tm_hour,startTm->tm_min,startTm->tm_sec);
			endTm = localtime(&etm);
			sprintf(cTime+strlen(cTime),"_%04d%02d%02d%02d%02d%02d",endTm->tm_year+1900,endTm->tm_mon+1,endTm->tm_mday,endTm->tm_hour,endTm->tm_min,endTm->tm_sec);
			sprintf(RequestBuf+strlen(RequestBuf),"pltype='0' streamid='0' plymul='1' ifrminvl='0' tinvl='3' trcode='0' prim='1' setime='%s'></data></root>%s"
				,cTime,CMS_END_FLAG);
		}
		int nRet = send(m_mediaServer.client_socket,RequestBuf,strlen(RequestBuf),0);
		if(nRet==SOCKET_ERROR)
		{
			ss.clear();
			ss.str("");
			ss<<__FUNCTION__<<" 发送失败，socket被关闭！";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			return -2;
		}
		return 0;
	}
	else
	{
		ss.clear();
		ss.str("");
		ss<<"error setup id="<<MediaSession.channel_id;
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}
}

int InterfaceImp::SendMediaPlayRequest(sipd_media_session MediaSession)
{
	char cRequestBuf[1024]={0};
	sprintf(cRequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root><msg_type sessionid='%d'>play</msg_type></root>%s"
		,MediaSession.did,CMS_END_FLAG);
	int nRet = send(m_mediaServer.client_socket,cRequestBuf,strlen(cRequestBuf),0);
	if(nRet==SOCKET_ERROR)
		return -1;
	return 0 ;
}

int InterfaceImp::SendMediaControlRequest(MediaRequestInfo MediaSession)
{
	char cRequestBuf[1024]={0};
	sprintf(cRequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root><msg_type sessionid='%d' >control</msg_type><data plymul='%s' setime='%s'></data></root>%s"
		,MediaSession.sipMediaSeesion.did,MediaSession.cSpeed,MediaSession.npt,CMS_END_FLAG);
	int nRet = send(m_mediaServer.client_socket,cRequestBuf,strlen(cRequestBuf),0);
	if(nRet==SOCKET_ERROR)
		return -1;
	return 0 ;
}

int InterfaceImp::SendMediaStopRequest(MediaRequestInfo MediaSession)
{
	char cRequestBuf[1024]={0};
	sprintf(cRequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root><msg_type sessionid='%d'>teardown</msg_type></root>%s"
		,MediaSession.sipMediaSeesion.did,CMS_END_FLAG);
	int nRet = send(m_mediaServer.client_socket,cRequestBuf,strlen(cRequestBuf),0);
	if(nRet==SOCKET_ERROR)
		return -1;
	return 0 ;
}

int InterfaceImp::SendMediaRebootRequest()
{
	stringstream ss;
	char cRequestBuf[1024]={0};
	sprintf(cRequestBuf,"<?xml version=\"1.0\" encoding=\"GBK\"?><root><msg_type>restart</msg_type></root>%s"
		,CMS_END_FLAG);
	int nRet = send(m_mediaServer.client_socket,cRequestBuf,strlen(cRequestBuf),0);
	if(nRet==SOCKET_ERROR)
		return -1;

	ss.clear();
	ss.str("");
	ss<<"reboot";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	Sleep(500);

	exit(0);
	return 0 ;
}

int InterfaceImp::AddMediaRequest(int did, int tid,int nRequestType,sipd_media_session* pMediaInfo,char *pRtspInfo)
{
	stringstream ss;

	MediaRequestInfo RequestInfo;
	map<int,sipd_media_session>::iterator iter;
	if(nRequestType==1)//setup
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		memcpy(&RequestInfo.sipMediaSeesion,pMediaInfo,sizeof(sipd_media_session));
		iter = m_MediaSeesionMap.find(pMediaInfo->did);
		if(iter!=m_MediaSeesionMap.end())
		{
			memcpy(&iter->second,pMediaInfo,sizeof(sipd_media_session));
		}
		else
			m_MediaSeesionMap.insert(make_pair(pMediaInfo->did,RequestInfo.sipMediaSeesion));
		RequestInfo.nMediaRequestType = nRequestType;
		m_MediaRequestLst.push_back(RequestInfo);

		ss.clear();
		ss.str("");
		ss<<"setup id="<<pMediaInfo->channel_id;
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
		return 0;
	}
	else if(nRequestType==2)//play
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		map<int,sipd_media_session>::iterator iter;
		iter = m_MediaSeesionMap.find(did);
		if(iter!=m_MediaSeesionMap.end())
		{
			RequestInfo.nMediaRequestType = nRequestType;
			memcpy(&RequestInfo.sipMediaSeesion,&iter->second,sizeof(sipd_media_session));
			m_MediaRequestLst.push_back(RequestInfo);

			ss.clear();
			ss.str("");
			ss<<"play id="<<iter->second.channel_id<<"_did="<<did;
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return 0 ;
		}
		else
		{
			ss.clear();
			ss.str("");
			ss<<"don't find play did="<<did;
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;
		}
	}
	else if(nRequestType==3)//control
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		map<int,sipd_media_session>::iterator iter;
		iter = m_MediaSeesionMap.find(did);
		if(iter!=m_MediaSeesionMap.end())
		{
			char *pScale = strstr(pRtspInfo,"Scale:");
			char *pNpt = strstr(pRtspInfo,"npt=");
			char *pPAUSE = strstr(pRtspInfo,"PAUSE");
			if((pScale!=NULL && pNpt!=NULL)||pPAUSE!=NULL)
			{
				if(pScale!=NULL && pNpt!=NULL)
				{
					char cStarTime[128]={0},cEndTime[128]={0};
					__int64 nStarTm = 0,nEndTm = 0;
					int nOutLen = 0;
					ParseHttpParam(pScale+strlen("Scale"),":","\r\n",RequestInfo.cSpeed,8,1,nOutLen);


					sscanf(pNpt+strlen("npt="),"%[0-9] - %[0-9]",cStarTime,cEndTime);
					nStarTm = _atoi64(cStarTime);
					nEndTm = _atoi64(cEndTime);

					//struct tm *startTm = localtime(&nStarTm);
					if(strlen(cStarTime)>0)
					{
						sprintf(RequestInfo.npt,"%s",cStarTime);
						//struct tm *endTm = localtime(&nEndTm);
						sprintf(RequestInfo.npt+strlen(RequestInfo.npt),"_%s",cEndTime);
					}

				}
				if(pPAUSE!=NULL)
				{
					char cPause[64]={0};
					int nPause = 0;
					char *pos = strstr(pRtspInfo,"PauseTime:");
					sprintf(RequestInfo.cSpeed,"0.0");
					if(pos!=NULL)
					{
						sscanf(pos+strlen("PauseTime:"),"%d",&nPause);
						memset(RequestInfo.npt,0,128);
						sprintf(RequestInfo.npt,"%d_",nPause);
					}
				}
				RequestInfo.nMediaRequestType = nRequestType;
				memcpy(&RequestInfo.sipMediaSeesion,&iter->second,sizeof(sipd_media_session));
				m_MediaRequestLst.push_back(RequestInfo);

				ss.clear();
				ss.str("");
				ss<<"control id="<<iter->second.channel_id<<"_did="<<did;
				LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
				return 0 ;
			}
			else
			{
				ss.clear();
				ss.str("");
				ss<<"error control did="<<did;
				LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
				return -1;
			}

		}
		else
			return -1;
	}
	else if(nRequestType==4)//stop
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		map<int,sipd_media_session>::iterator iter;
		iter = m_MediaSeesionMap.find(did);
		if(iter!=m_MediaSeesionMap.end())
		{
			if(iter->second.stream_type==SIPD_STREAM_TYPE_PLAY)
				RequestInfo.nMediaRequestType = 4;
			else
				RequestInfo.nMediaRequestType = 5;
			memcpy(&RequestInfo.sipMediaSeesion,&iter->second,sizeof(sipd_media_session));
			m_MediaRequestLst.push_back(RequestInfo);

			ss.clear();
			ss.str("");
			ss<<"stop id="<<iter->second.channel_id<<"_did="<<did;
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return 0 ;
		}
		else
		{
			ss.clear();
			ss.str("");
			ss<<"error stop did="<<did;
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;
		}
	}

	ss.clear();
	ss.str("");
	ss<<"error media cmd did="<<did;
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	return -1;

}

DWORD WINAPI MediaServerProc(LPVOID lparam)
{
	InterfaceImp *pInterfaceImp = (InterfaceImp*)lparam;
	char cRecvBuf[1024+1]={0};
	stringstream ss;
	string strBuf;
	int nEndLen = strlen(CMS_END_FLAG);

	while(!pInterfaceImp->m_MediaServerProcThread.isExit)
	{
		int nRet = xml_OpenCon(pInterfaceImp->m_mediaServer);
		if(nRet<0)
		{
			xml_closeCon(pInterfaceImp->m_mediaServer);
			Sleep(500);

			ss.clear();
			ss.str("");
			ss<<"媒体网关断开,重连_"<<GetLastError();;
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		}
		else
			break;
	}

	DWORD dwBgein = GetTickCount();
	while(!pInterfaceImp->m_MediaServerProcThread.isExit)
	{
		bool bRead = IsRead(pInterfaceImp->m_mediaServer.client_socket);
		if(bRead)
		{
			int nRecv=recv(pInterfaceImp->m_mediaServer.client_socket,cRecvBuf,1024,0);
			if(nRecv<0)
			{
				int nError = GetLastError();
				if(nError==10060)
				{
					ss.clear();
					ss.str("");
					ss<<"媒体网关接收超时_";
					LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
				}
				else
				{
					//reconnect
					ss.clear();
					ss.str("");
					ss<<"媒体网关断开,重连_"<<nError;
					LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());

					while(!pInterfaceImp->m_MediaServerProcThread.isExit)
					{
						pInterfaceImp->RequestMedia(2);
						int nRet = xml_OpenCon(pInterfaceImp->m_mediaServer);
						if(nRet<0)
						{
							xml_closeCon(pInterfaceImp->m_mediaServer);
							Sleep(500);
						}
						else
							break;
					}
				}
			}
			else if(nRecv>0)
			{
				cRecvBuf[nRecv]='\0';
				char ctemBuf[1024]={0};

				strBuf += cRecvBuf;
				int npos = strBuf.find(CMS_END_FLAG);
				if(npos==string::npos)
				{
					continue;
				}
				else
				{
					while(npos>0)
					{	
						string strCMSMessage = strBuf.substr(0,npos);
						pInterfaceImp->ProcMediaServerMessage((char *)strCMSMessage.c_str());
						string strTmp = strBuf.substr(npos+nEndLen,strBuf.length()-1);
						strBuf = strTmp;
						npos = strBuf.find(CMS_END_FLAG);
					}

				}

			}
		}
		else
		{

			if(strBuf.length()>0)
			{
				ss.clear();
				ss.str("");
				ss<<"CMS接收出错_"<<strBuf;
				LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());

				strBuf="";
			}
			//break;
		}
		pInterfaceImp->RequestMedia(1);

	}
	pInterfaceImp->m_MediaServerProcThread.isExit = false;
	return 0;
}



DWORD WINAPI CMSProc(LPVOID lparam)
{
	InterfaceImp *pInterfaceImp = (InterfaceImp*)lparam;
	char cRecvBuf[1024+1]={0};
	stringstream ss;
	string strBuf;
	int nEndLen = strlen(CMS_END_FLAG);

	pInterfaceImp->re_cms_logon(pInterfaceImp->m_strLocalIP,pInterfaceImp->m_CMSServer,&pInterfaceImp->m_cmsThread,true);

	DWORD dwBgein = GetTickCount();
	while(!pInterfaceImp->m_cmsThread.isExit)
	{
		DWORD dwCurTime = GetTickCount();
		int nDiff = dwCurTime - dwBgein;
		if(nDiff>=HEART_TIME)
		{
			SendHeart(pInterfaceImp->m_CMSServer);
			dwBgein = dwCurTime;
		}

		while(1)
		{
			bool bRead = IsRead(pInterfaceImp->m_CMSServer.client_socket);
			if(bRead)
			{
				int nRecv=recv(pInterfaceImp->m_CMSServer.client_socket,cRecvBuf,1024,0);
				if(nRecv<0)
				{
					pInterfaceImp->re_cms_logon(pInterfaceImp->m_strLocalIP,pInterfaceImp->m_CMSServer,&pInterfaceImp->m_cmsThread);
				}
				else if(nRecv>0)
				{
					cRecvBuf[nRecv]='\0';
					char ctemBuf[1024]={0};

					strBuf += cRecvBuf;
					int npos = strBuf.find(CMS_END_FLAG);
					if(npos==string::npos)
					{
						continue;
					}
					else
					{
						while(npos>0)
						{	
							string strCMSMessage = strBuf.substr(0,npos);
							pInterfaceImp->ProcCMSNotify((char *)strCMSMessage.c_str());
							string strTmp = strBuf.substr(npos+nEndLen,strBuf.length()-1);
							strBuf = strTmp;
							npos = strBuf.find(CMS_END_FLAG);
						}

					}

				}
			}
			else
			{
				if(strBuf.length()>0)
				{
					ss.clear();
					ss.str("");
					ss<<"CMS接收出错_"<<strBuf;
					LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());

					strBuf="";
				}
				break;
			}
		}

		pInterfaceImp->CheckDeviceRight();

	}
	pInterfaceImp->m_cmsThread.isExit = false;
	return 0;
}

void WorkerProc(LPVOID lpparam)
{
	LPTaskInfo pTask = (LPTaskInfo)lpparam;
	InterfaceImp *pInterfaceImp = (InterfaceImp *)pTask->lparam;
	pInterfaceImp->DoTask(lpparam);
}

void JMSNotifyProc(LPVOID lpparam,char *cMessage)
{
	InterfaceImp *pInterfaceImp = (InterfaceImp *)lpparam;
	if(pInterfaceImp!=NULL)
		pInterfaceImp->ProcJMSNotify(cMessage);	
}

int InterfaceImp::re_cms_logon(string strSourceIP,ServerInfo &cmsInfo,LPThreadInfo pThreadInfo,bool bFirst)
{
	stringstream ss;
	if(!bFirst)
	{
		int nErr = GetLastError();
		ss.clear();
		ss.str("");
		ss<<"cms 连接断开 err="<<nErr;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
	}

	xml_closeCon(cmsInfo);



	bool suc = false;
	while(!pThreadInfo->isExit)
	{
		string strUserID="";
		int nRet = getUserID(m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,m_CMSServer.strUserName,strUserID);
		if(nRet==0 && strUserID!=m_CMSServer.strUserID)
		{
			m_CMSServer.strUserID = strUserID;
			GetAllDevice();
			GetAllDevicePos();

			ss.clear();
			ss.str("");
			ss<<"用户id改变_"<<strUserID;
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		}

		if(cms_logon(strSourceIP,cmsInfo)<0)
		{
			ss.clear();
			ss.str("");
			ss<<"cms 登陆失败";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		}
		else
		{
			suc = true;
			break;
		}
		Sleep(1000);
	}
	return suc;
}

void InterfaceImp::ProcJMSNotify(char *cMessage)
{
	string strClassName = GetXMLFieldValue(cMessage,"className","/root");
	string strActionType = GetXMLFieldValue(cMessage,"actionType","/root");
	string strKeyId = GetXMLFieldValue(cMessage,"keyId","/root");


	int nActionType = atoi(strActionType.c_str());
	if(nActionType>=1 && nActionType<=3)
	{
		printf("UpdateDBTable\r\n");
		if(m_pJMSProducer!=NULL)
		{
			m_pJMSProducer->Sendms("<?xml version=\"1.0\" encoding=\"UTF-8\"?><root actionType=\"4\" hql=\"from SysConfig\"></root>");
			m_ReadSipCfgStep = 0;
		}
	}
	else if(nActionType==99)//重启
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		MediaRequestInfo RequestInfo;
		RequestInfo.nMediaRequestType = 6;
		m_MediaRequestLst.push_back(RequestInfo);
	}
}

void JMSQueryNotifyProc(LPVOID lpparam,char *cMessage)
{
	InterfaceImp *pInterfaceImp = (InterfaceImp *)lpparam;
	pInterfaceImp->parseSipCfgMessage(cMessage);

}

int InterfaceImp::parseSipCfgMessage(char *cMessage)
{

	list<map<string,string > > lstPropMap;

	xmlDocPtr generalDoc = XmlUtils::StringToDoc(cMessage);
	//printf("Xml encoding : %s \r\n",generalDoc->encoding );

	xmlNode *  tnode =  generalDoc->children->children;
	while(tnode!=NULL){
		xmlNode *  tnode1 =  tnode->children;
		map<string,string> tmpMap;
		bool bInsert = false;
		while(tnode1!=NULL)
		{
			if(tnode1->type ==XML_ELEMENT_NODE){
				//cout<< tnode->name << "  type = " << tnode->type << endl;
				char *pChr = (char*)xmlNodeGetContent(tnode1);

				if((strstr(pChr,"gb")!=NULL || strstr(pChr,"app")!=NULL) && strstr((char *)tnode1->name,"name")!=NULL)
				{
					tmpMap.insert(make_pair(pChr,(char *)tnode1->name));
					bInsert = true;
				}
				else
					tmpMap.insert(make_pair((char *)tnode1->name,pChr));
			}
			tnode1 = tnode1->next;
		}
		if(bInsert)
			lstPropMap.push_back(tmpMap);
		tnode = tnode->next;
	}
	xmlFreeDoc(generalDoc);

	list<map<string,string > > ::iterator iter;
	bool bFind = false;
	SIPD_GATEWAY_CFG GatewayCfg;
	string strPlatUserName,strPlatUserPassword;
	memset(&GatewayCfg,0,sizeof(SIPD_GATEWAY_CFG));


	for(iter=lstPropMap.begin();iter!=lstPropMap.end();iter++)
	{
		map<string,string >::iterator tmp_iter = iter->find("gb_id");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
				strcpy(GatewayCfg.local_domain_id,tmp_iter->second.c_str());
			continue;
		}

		tmp_iter = iter->find("app_name");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				char *c1=XmlUtils::ConvertEnc("utf-8","gbk",(char *)tmp_iter->second.c_str());
				if(c1!=NULL)
				{
					strcpy(GatewayCfg.local_domain_name,c1);
					free(c1);
				}
			}
			continue;
		}

		tmp_iter = iter->find("app_ip");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
				strcpy(GatewayCfg.local_ip,tmp_iter->second.c_str());
			continue;
		}
		tmp_iter = iter->find("app_port");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
				GatewayCfg.local_port=atoi(tmp_iter->second.c_str());
			continue;
		}

		tmp_iter = iter->find("gb_username");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				char *c1=XmlUtils::ConvertEnc("utf-8","gbk",(char *)tmp_iter->second.c_str());
				if(c1!=NULL)
				{
					strPlatUserName=c1;
					free(c1);
				}
			}
			continue;
		}
		tmp_iter = iter->find("gb_password");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				strPlatUserPassword=tmp_iter->second.c_str();
				//m_CMSServer.strPassWord=tmp_iter->second;
			}
			continue;
		}
		tmp_iter = iter->find("gb_parent_gb_id");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
				strcpy(GatewayCfg.upper_server_id,tmp_iter->second.c_str());
			continue;
		}
		tmp_iter = iter->find("gb_parent_name");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				char *c1=XmlUtils::ConvertEnc("utf-8","gbk",(char *)tmp_iter->second.c_str());
				if(c1!=NULL)
				{
					strcpy(GatewayCfg.upper_server_name,c1);
					free(c1);
				}
			}
			continue;
		}
		tmp_iter = iter->find("gb_parent_ip");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
				strcpy(GatewayCfg.upper_server_ip,tmp_iter->second.c_str());
			continue;
		}
		tmp_iter = iter->find("gb_parent_port");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
				GatewayCfg.upper_server_port = atoi(tmp_iter->second.c_str());
			continue;
		}

		tmp_iter = iter->find("gb_parent_password");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				strcpy(GatewayCfg.register_upper_server_passwd,tmp_iter->second.c_str());
			}
			continue;
		}

		//数据库ip和port
		tmp_iter = iter->find("gb_app_ip");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				m_LocalCfg.strDBIP=tmp_iter->second;
			}
			continue;
		}

		tmp_iter = iter->find("gb_app_port");
		if(tmp_iter!=iter->end())
		{
			tmp_iter = iter->find("value");
			if(tmp_iter!=iter->end())
			{
				m_LocalCfg.nDBPort=atoi(tmp_iter->second.c_str());
			}
			continue;
		}
		bFind = true;
	}

	if(m_CMSServer.strUserName.length()>0 && m_CMSServer.strUserName!=strPlatUserName)
	{
		//获取用户名ID
		GetUserID();
		GetAllDevice();
	}
	m_CMSServer.strUserName = strPlatUserName;
	m_CMSServer.strPassWord = strPlatUserPassword;	

	GatewayCfg.register_keepalive = 3600;
	GatewayCfg.keepalive_internal = 60;
	strcpy(GatewayCfg.local_gateway,m_LocalCfg.strGateway.c_str());
	SipCfgChangeNotify(GatewayCfg);
	if(bFind)
		m_ReadSipCfgStep = 1;
	return 0;
}

void InterfaceImp::SipCfgChangeNotify(SIPD_GATEWAY_CFG new_cfg)
{
	if(m_SipServerCfg.new_cfg.upper_server_port==0)//第一次
	{
		memcpy(&m_SipServerCfg.new_cfg,&new_cfg,sizeof(SIPD_GATEWAY_CFG));

	}
	else
	{
		memcpy(&m_SipServerCfg.old_cfg,&m_SipServerCfg.new_cfg,sizeof(SIPD_GATEWAY_CFG));
		memcpy(&m_SipServerCfg.new_cfg,&new_cfg,sizeof(SIPD_GATEWAY_CFG));
		if(m_pConfigCB!=NULL)
		{
			int nType = SIPD_CONFIG_NOTIFY_CB_CFG;
			m_pConfigCB(&nType,&m_SipServerCfg);
		}
	}
}


void InterfaceImp::DoTask(void *param)
{
	LPTaskInfo pTask = (LPTaskInfo)param;
	switch (pTask->nTaskType)
	{
	case TASK_PTZ:
		printf("TASK_PTZ\r\n");
		{
			LPPTZControlInfo pPTZInfo = (LPPTZControlInfo)pTask->param1;
			SendPTZControlRequest(pPTZInfo);
		}
		break;

	case TASK_QUERY_DEVICE:
		printf("TASK_QUERY_DEVICE");
		{
			sipd_deviceinfo_req *pDeviceinfo_req = (sipd_deviceinfo_req*)pTask->param1;
			map<string,SN_DEVICE>::iterator dev_iter;
			dev_iter = m_DeviceMap.find(pDeviceinfo_req->device_id);
			if(dev_iter!=m_DeviceMap.end())
			{
				sipd_deviceinfo_rsp deviceinfo_rsp;
				strcpy(deviceinfo_rsp.device_id,pDeviceinfo_req->device_id);
				strcpy(deviceinfo_rsp.from_uri,pDeviceinfo_req->from_uri);
				strcpy(deviceinfo_rsp.to_uri,pDeviceinfo_req->to_uri);
				strcpy(deviceinfo_rsp.SN,pDeviceinfo_req->SN);
				strcpy(deviceinfo_rsp.manufacturer,"starnet");

				GetDevTypeName(dev_iter->second.nDevTypeChild,deviceinfo_rsp.DeviceType);


				deviceinfo_rsp.MaxAlarm = dev_iter->second.nAlarmInIOCnt;
				deviceinfo_rsp.MaxCamera = dev_iter->second.nChannelCnt;
				deviceinfo_rsp.Result = SIPD_RESULT_OK;

				int nType = SIPD_INFO_RSP_CB_DEVICEINFO;
				m_pInfoReqCB(&nType,&deviceinfo_rsp,NULL);
			}
			else
			{
				//无设备信息
				sipd_deviceinfo_rsp deviceinfo_rsp;
				memset(&deviceinfo_rsp,0,sizeof(sipd_deviceinfo_rsp));
				int nType = SIPD_INFO_RSP_CB_DEVICEINFO;
				deviceinfo_rsp.Result = SIPD_RESULT_ERROR;
				m_pInfoReqCB(&nType,&deviceinfo_rsp,NULL);
			}
		}
		break;

	case TASK_QUERY_DEV_CATA:
		printf("TASK_QUERY_DEVICE");
		{

			map<string,SN_DEVICE>::iterator dev_iter;
			sipd_catalog_req *pCatalog_req = (sipd_catalog_req *)pTask->param1;
			if(strcmp(m_SipServerCfg.new_cfg.local_domain_id,pCatalog_req->device_id)==0)
			{
				int nDevCount = 0;
				list<SN_DEVICE> SnDevLst;
				list<SN_DEVICE>::iterator sndev_iter;
				{
					JMutexAutoLock aLock(m_DeviceMapMutex);
					for(dev_iter=m_DeviceMap.begin();dev_iter!=m_DeviceMap.end();dev_iter++)
					{
						if(dev_iter->second.nDevTypeId==1 || dev_iter->second.nDevTypeId==2)
							SnDevLst.push_back(dev_iter->second);
					}
				}

				nDevCount = SnDevLst.size();
				for(sndev_iter=SnDevLst.begin();sndev_iter!=SnDevLst.end();sndev_iter++)
				{
					if(sndev_iter->nDevTypeId==1 || sndev_iter->nDevTypeId==2)
					{
						sipd_catalog_item dev_catalog_item;
						string strDevPos;
						memset(&dev_catalog_item,0,sizeof(sipd_catalog_item));
						strcpy(dev_catalog_item.device_id,sndev_iter->strGBID.c_str());
						strcpy(dev_catalog_item.Name,sndev_iter->strDevName.c_str());

						GetDevPos(0,(char *)sndev_iter->strPosID.c_str(),dev_catalog_item.Owner);
						strcpy(dev_catalog_item.Manufacturer,"starnet");

						strcpy(dev_catalog_item.CivilCode,"350001");
						strcpy(dev_catalog_item.Model,sndev_iter->strDevSN.c_str());

						strcpy(dev_catalog_item.IPAddress,sndev_iter->strDevIP.c_str());
						strcpy(dev_catalog_item.ParentID,pCatalog_req->device_id);

						if(sndev_iter->nChannelCnt>0 || sndev_iter->nAlarmInIOCnt>0)
							dev_catalog_item.Parental = 1;
						else
							dev_catalog_item.Parental = 0;

						dev_catalog_item.Status=(SIPD_STATUS_TYPE)(sndev_iter->nDevState==0?1:0);

						dev_catalog_item.SafteyWay= 0;
						dev_catalog_item.RegisterWay = 1;


						sipd_catalog_rsp_basic_info catalog_rsp_basic_info;
						strcpy(catalog_rsp_basic_info.to_uri,pCatalog_req->to_uri);
						strcpy(catalog_rsp_basic_info.from_uri,pCatalog_req->from_uri);
						strcpy(catalog_rsp_basic_info.SN,pCatalog_req->SN);
						strcpy(catalog_rsp_basic_info.device_id,pCatalog_req->device_id);
						catalog_rsp_basic_info.DeviceList_Num = 1;
						catalog_rsp_basic_info.SumNum = nDevCount;

						int nType = SIPD_INFO_RSP_CB_CATALOG;
						m_pInfoReqCB(&nType,&catalog_rsp_basic_info,&dev_catalog_item);
					}

				}
			}
			else
			{
				bool bFind = false;
				int nDevCount = 0;
				list<SN_DEVICE> SnDevLst;
				list<SN_DEVICE>::iterator sndev_iter;
				{
					JMutexAutoLock aLock(m_DeviceMapMutex);
					dev_iter=m_DeviceMap.find(pCatalog_req->device_id);
					if(dev_iter!=m_DeviceMap.end())
					{
						bFind = true;
						if(dev_iter->second.nDevTypeId==1 || dev_iter->second.nDevTypeId==2)//parant
						{
							string strDevID = dev_iter->second.strDevId;


							for(dev_iter=m_DeviceMap.begin();dev_iter!=m_DeviceMap.end();dev_iter++)
							{
								if(strDevID==dev_iter->second.strParentDevId)
									SnDevLst.push_back(dev_iter->second);


								nDevCount = SnDevLst.size();
							}
						}
					}
				}
				if(bFind)
				{
					if(nDevCount>0)
					{
						for(sndev_iter=SnDevLst.begin();sndev_iter!=SnDevLst.end();sndev_iter++)
						{
							sipd_catalog_item dev_catalog_item;
							memset(&dev_catalog_item,0,sizeof(sipd_catalog_item));
							strcpy(dev_catalog_item.device_id,sndev_iter->strGBID.c_str());
							strcpy(dev_catalog_item.Name,sndev_iter->strDevName.c_str());

							GetDevPos(0,(char *)sndev_iter->strPosID.c_str(),dev_catalog_item.Owner);
							strcpy(dev_catalog_item.Manufacturer,"starnet");

							strcpy(dev_catalog_item.CivilCode,"350001");
							strcpy(dev_catalog_item.Model,sndev_iter->strDevSN.c_str());

							strcpy(dev_catalog_item.IPAddress,sndev_iter->strDevIP.c_str());
							strcpy(dev_catalog_item.ParentID,pCatalog_req->device_id);

							if(sndev_iter->nChannelCnt>0 || sndev_iter->nAlarmInIOCnt>0)
								dev_catalog_item.Parental = 1;
							else
								dev_catalog_item.Parental = 0;

							dev_catalog_item.Status=(SIPD_STATUS_TYPE)(sndev_iter->nDevState==0?1:0);

							dev_catalog_item.SafteyWay= 0;
							dev_catalog_item.RegisterWay = 1;


							sipd_catalog_rsp_basic_info catalog_rsp_basic_info;
							strcpy(catalog_rsp_basic_info.to_uri,pCatalog_req->to_uri);
							strcpy(catalog_rsp_basic_info.from_uri,pCatalog_req->from_uri);
							strcpy(catalog_rsp_basic_info.SN,pCatalog_req->SN);
							strcpy(catalog_rsp_basic_info.device_id,pCatalog_req->device_id);
							catalog_rsp_basic_info.DeviceList_Num = 1;
							catalog_rsp_basic_info.SumNum = nDevCount;

							int nType = SIPD_INFO_RSP_CB_CATALOG;
							m_pInfoReqCB(&nType,&catalog_rsp_basic_info,&dev_catalog_item);
						}
					}
				}
				if(!bFind || nDevCount==0)
				{

					//无子设备
					sipd_catalog_item dev_catalog_item;
					memset(&dev_catalog_item,0,sizeof(sipd_catalog_item));
					dev_catalog_item.Parental = 0;

					sipd_catalog_rsp_basic_info catalog_rsp_basic_info;
					strcpy(catalog_rsp_basic_info.to_uri,pCatalog_req->to_uri);
					strcpy(catalog_rsp_basic_info.from_uri,pCatalog_req->from_uri);
					strcpy(catalog_rsp_basic_info.SN,pCatalog_req->SN);
					strcpy(catalog_rsp_basic_info.device_id,pCatalog_req->device_id);
					catalog_rsp_basic_info.DeviceList_Num = 0;
					catalog_rsp_basic_info.SumNum = 0;

					int nType = SIPD_INFO_RSP_CB_CATALOG;
					m_pInfoReqCB(&nType,&catalog_rsp_basic_info,&dev_catalog_item);
				}
			}

		}

		break;

	case TASK_QUERY_RECORD:
		printf("TASK_QUERY_RECORD");
		{
			list<SN_DEVICE> SnDevLst;
			list<RecordInfo> RecordInfoLst;
			list<SN_DEVICE> SnUperDev;
			list<SN_DEVICE>::iterator iter;
			char cStartTime[MAX_SIPD_TIME_LEN]={0};
			char cEndTime[MAX_SIPD_TIME_LEN]={0};
			sipd_recordinfo_req * pRecordinfo_req = (sipd_recordinfo_req *)pTask->param1;
			if(pRecordinfo_req->StartTime[0]==0)
				sprintf(cStartTime,"2010-01-01T00:00:00");
			else
				strcpy(cStartTime,pRecordinfo_req->StartTime);

			if(pRecordinfo_req->EndTime[0]==0)
			{
				SYSTEMTIME sysTime;
				GetSystemTime(&sysTime);
				sprintf(cEndTime,"%04d-%02d-%02dT%02d:%02d:%02d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
			}
			else
				strcpy(cEndTime,pRecordinfo_req->EndTime);

			int nRet = GetDevChannel(pRecordinfo_req->device_id,SnDevLst,0,SnUperDev);	
			if(nRet>=0)
			{
				for(iter=SnDevLst.begin();iter!=SnDevLst.end();iter++)
				{
					QuerySingleRecord((*iter),m_VODServer,cStartTime,cEndTime,RecordInfoLst);
				}
			}

			if(m_pInfoReqCB!=NULL)
			{
				list<RecordInfo>::iterator record_iter;
				int nType = SIPD_INFO_RSP_CB_RECORDFILE;
				sipd_recordinfo_rsp_basic_info recordinfo_rsp_basic_info;
				strcpy(recordinfo_rsp_basic_info.device_id,pRecordinfo_req->device_id);
				strcpy(recordinfo_rsp_basic_info.from_uri,pRecordinfo_req->from_uri);
				strcpy(recordinfo_rsp_basic_info.SN,pRecordinfo_req->SN);
				strcpy(recordinfo_rsp_basic_info.to_uri,pRecordinfo_req->to_uri);
				recordinfo_rsp_basic_info.SumNum = RecordInfoLst.size();

				if(RecordInfoLst.size()>0)
				{
					for(record_iter=RecordInfoLst.begin();record_iter!=RecordInfoLst.end();record_iter++)
					{
						recordinfo_rsp_basic_info.RecordListNum = 1;
						sipd_file_item record_file_item;
						memset(&record_file_item,0,sizeof(record_file_item));
						strcpy(record_file_item.device_id,record_iter->strGBID.c_str());
						strcpy(record_file_item.Name,record_iter->strDevName.c_str());
						strcpy(record_file_item.StartTime,record_iter->strStartRecordTime.c_str());
						strcpy(record_file_item.EndTime,record_iter->strEndRecordTime.c_str());

						m_pInfoReqCB(&nType,&recordinfo_rsp_basic_info,&record_file_item);
					}
				}
				else
				{
					//无录像
					recordinfo_rsp_basic_info.RecordListNum = 0;
					sipd_file_item record_file_item;
					memset(&record_file_item,0,sizeof(record_file_item));
					m_pInfoReqCB(&nType,&recordinfo_rsp_basic_info,&record_file_item);
				}


			}

		}
		break;

	case TASK_QUERY_DEV_STATE:
		printf("TASK_QUERY_DEV_STATE");
		{
			sipd_devicestate_req * pDevicestate_req = (sipd_devicestate_req *)pTask->param1;
			list<SN_DEVICE> SnDeviceLst;
			list<SN_DEVICE> SnUperDev;
			sipd_devicestate_rsp_basic_info devicestate_rsp_basic_info;
			int nType = SIPD_INFO_RSP_CB_DEVICESTATE;
			int nAlarmStatus=0;
			memset(&devicestate_rsp_basic_info,0,sizeof(sipd_devicestate_rsp_basic_info));

			strcpy(devicestate_rsp_basic_info.device_id,pDevicestate_req->device_id);
			strcpy(devicestate_rsp_basic_info.SN,pDevicestate_req->SN);
			strcpy(devicestate_rsp_basic_info.from_uri,pDevicestate_req->from_uri);
			strcpy(devicestate_rsp_basic_info.to_uri,pDevicestate_req->to_uri);
			SYSTEMTIME sysTime;
			GetSystemTime(&sysTime);
			sprintf(devicestate_rsp_basic_info.DeviceTime,"%04d-%02d-%02dT%02d:%02d:%02d",sysTime.wYear,sysTime.wMonth,sysTime.wDay
				,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

			int nRet = GetDevChannel(pDevicestate_req->device_id,SnDeviceLst,1,SnUperDev);
			list<sipd_alarm_status_item> alarm_status_list;

			if(nRet>0)
			{
				list<SN_DEVICE>::iterator uper_iter = SnUperDev.begin();
				list<SN_DEVICE>::iterator iter;
				sipd_alarm_status_item alarm_status_item[6];
				;

				QueryDevAlarmStatus(m_DMSServer,(*uper_iter),nAlarmStatus);
				int nAlarmDevCount = 0;
				devicestate_rsp_basic_info.Result = SIPD_RESULT_OK;
				devicestate_rsp_basic_info.Online = nRet<0?SIPD_STATUS_ONLINE_TYPE_OFFLINE:SIPD_STATUS_ONLINE_TYPE_ONLINE;
				devicestate_rsp_basic_info.Status = (*uper_iter).nDevState==0?SIPD_RESULT_ERROR:SIPD_RESULT_OK;


				devicestate_rsp_basic_info.AlaramDeviceNum = 0;
				if(nRet==1)//parent
				{
					devicestate_rsp_basic_info.Encode = (*uper_iter).nDevTypeId==1?SIPD_STATUS_ON:SIPD_STATUS_OFF;
					if(((*uper_iter).nDevTypeId==1 && (*uper_iter).nDevTypeChild==2) || (*uper_iter).nDevTypeId!=1)
						devicestate_rsp_basic_info.Record = SIPD_STATUS_OFF;	
					else if((*uper_iter).nDevTypeId==1)
						devicestate_rsp_basic_info.Record = SIPD_STATUS_ON;

					nAlarmDevCount = uper_iter->nAlarmInIOCnt>MAX_SIPD_ALARM_ITEM_RSP?MAX_SIPD_ALARM_ITEM_RSP:uper_iter->nAlarmInIOCnt;
					if(uper_iter->nAlarmInIOCnt>0)
					{
						int i=0;
						//memset(alarm_status_item,0,MAX_SIPD_ALARM_ITEM_RSP*sizeof(sipd_alarm_status_item));

						for(iter=SnDeviceLst.begin();iter!=SnDeviceLst.end();iter++)
						{
							if(iter->nDevTypeId == 6)//报警输入
							{
								sipd_alarm_status_item alarm_status;

								strcpy(alarm_status.device_id,iter->strGBID.c_str());
								if(nAlarmStatus==0) //撤防
									alarm_status.Status = SIPD_ALARM_STATUS_OFFDUTY;
								else if(nAlarmStatus==1) //布防
									alarm_status.Status = SIPD_ALARM_STATUS_ONDUTY;
								else //报警
									alarm_status.Status = SIPD_ALARM_STATUS_ALARM;
								i++;

								alarm_status_list.push_back(alarm_status);
								/*if(i>=MAX_SIPD_ALARM_ITEM_RSP)
								break;*/
							}


						}
						devicestate_rsp_basic_info.AlaramDeviceNum = alarm_status_list.size();
					}

				}
				else if(SnDeviceLst.size()>0)
				{
					iter=SnDeviceLst.begin();
					if(iter->nDevTypeId == 5)//视频通道
					{
						devicestate_rsp_basic_info.Encode = (*uper_iter).nDevTypeId==1?SIPD_STATUS_ON:SIPD_STATUS_OFF;
						if(((*uper_iter).nDevTypeId==1 && (*uper_iter).nDevTypeChild==2) || (*uper_iter).nDevTypeId!=1)
							devicestate_rsp_basic_info.Record = SIPD_STATUS_OFF;	
						else if((*uper_iter).nDevTypeId==1)
							devicestate_rsp_basic_info.Record = SIPD_STATUS_ON;
					}
					else
					{
						devicestate_rsp_basic_info.Encode = SIPD_STATUS_OFF;
						devicestate_rsp_basic_info.Record = SIPD_STATUS_OFF;

						if(nAlarmStatus==0) //撤防
							alarm_status_item[0].Status = SIPD_ALARM_STATUS_OFFDUTY;
						else if(nAlarmStatus==1) //布防
							alarm_status_item[0].Status = SIPD_ALARM_STATUS_ONDUTY;
						else //报警
							alarm_status_item[0].Status = SIPD_ALARM_STATUS_ALARM;
						devicestate_rsp_basic_info.AlaramDeviceNum = 1;
					}

				}


				if(m_pInfoReqCB!=NULL)
				{
					list<sipd_alarm_status_item>::iterator alarm_item;
					int nAlarmCount = alarm_status_list.size();
					int i=0;
					for(alarm_item=alarm_status_list.begin();alarm_item!=alarm_status_list.end();alarm_item++)
					{
						strcpy(alarm_status_item[i].device_id,alarm_item->device_id);
						alarm_status_item[i].Status = alarm_item->Status;
						i++;
						if(i==6)
						{
							devicestate_rsp_basic_info.AlaramDeviceNum = i;
							m_pInfoReqCB(&nType,&devicestate_rsp_basic_info,alarm_status_item);
							i=0;
						}

					}
					devicestate_rsp_basic_info.AlaramDeviceNum = i;
					m_pInfoReqCB(&nType,&devicestate_rsp_basic_info,alarm_status_item);
				}

			}
			else
			{
				devicestate_rsp_basic_info.Result = SIPD_RESULT_ERROR;
				devicestate_rsp_basic_info.Status = SIPD_RESULT_ERROR;
				if(m_pInfoReqCB!=NULL)
					m_pInfoReqCB(&nType,&devicestate_rsp_basic_info,NULL);
			}
		}
		break;

	case TASK_ALARM_CONTROL:
		printf("TAST_ALARM_CONTROL");
		{
			sipd_alarm_control * pAlarmControl = (sipd_alarm_control *)pTask->param1;
			list<SN_DEVICE> SnDeviceLst;
			list<SN_DEVICE> SnUperDev;
			list<SN_DEVICE>::iterator iter;
			sipd_control_rsp control_rsp;
			strcpy(control_rsp.device_id,pAlarmControl->device_id);
			strcpy(control_rsp.from_uri,pAlarmControl->from_uri);
			strcpy(control_rsp.to_uri,pAlarmControl->to_uri);
			strcpy(control_rsp.SN,pAlarmControl->SN);
			int nRet = GetDevChannel(pAlarmControl->device_id,SnDeviceLst,1,SnUperDev);
			if(SnUperDev.size()>0)
			{
				iter = SnUperDev.begin();
				int nAlarmStatus = 0;
				if(pAlarmControl->cmd_type==SIPD_ALARM_CONTROL_SET_GUARD)
					nAlarmStatus = 1;
				else if(pAlarmControl->cmd_type==SIPD_ALARM_CONTROL_RESET_GUARD)
					nAlarmStatus = 0;
				else if(pAlarmControl->cmd_type==SIPD_ALARM_CONTROL_RESET_ALARM)
					nAlarmStatus = 2;

				char cReqBuf[512]={0};
				sprintf(cReqBuf,"<root>"
					"<data commandtype=\"cmd_type_dev_param\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" "
					"command=\"000f\" motionType=\"0\" channel=\"0\" masterstate=\"%d\" armeddelay=\"0\" />"
					"</root>%s"
					,iter->strDevId.c_str(),iter->nDevFirm,iter->strDevIP.c_str(),iter->nDevPort,iter->strDevUser.c_str(),iter->strDevPass.c_str(),iter->nDevTypeChild
					,iter->nWebPort,nAlarmStatus,CMS_END_FLAG);
				nRet = SendDMSCmd(m_DMSServer,cReqBuf,strlen(cReqBuf),true);
				if(nRet>=0)
				{
					control_rsp.result = 0;
					m_pControlReqCB(&control_rsp);
				}
				else
				{
					if(pAlarmControl->cmd_type==SIPD_ALARM_CONTROL_RESET_ALARM)
						control_rsp.result = 0;
					else
						control_rsp.result = 1;
					m_pControlReqCB(&control_rsp);
				}
			}
			else
			{
				if(pAlarmControl->cmd_type==SIPD_ALARM_CONTROL_RESET_ALARM)
					control_rsp.result = 0;
				else
					control_rsp.result = 1;
				m_pControlReqCB(&control_rsp);
			}

		}
		break;

	case TASK_RECORD_CONTROL:
		printf("TASK_RECORD_CONTROL\r\n");
		{
			sipd_record_control * pRecordControl = (sipd_record_control *)pTask->param1;
			sipd_control_rsp control_rsp ;
			list<SN_DEVICE> SnDev;
			list<SN_DEVICE> SnDevChannel;
			list<SN_DEVICE>::iterator dev_iter;

			memset(&control_rsp,0,sizeof(sipd_control_rsp));
			strcpy(control_rsp.device_id,pRecordControl->device_id);
			strcpy(control_rsp.from_uri,pRecordControl->from_uri);
			strcpy(control_rsp.SN,pRecordControl->SN);
			strcpy(control_rsp.to_uri,pRecordControl->to_uri);
			int nRet = GetDevChannel(pRecordControl->device_id,SnDevChannel,0,SnDev);
			int nStartFlag = pRecordControl->cmd_type==SIPD_RECORD_CONTROL_START?1:0;
			if(SnDevChannel.size()>0)
			{
				for(dev_iter=SnDevChannel.begin();dev_iter!=SnDevChannel.end();dev_iter++)
				{
					char cRequestBuf[512]={0};
					sprintf(cRequestBuf,"<root>"
						"<data commandtype=\"cmd_type_dev_param\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" "
						"port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"0012\" channel=\"%d\" start=\"%d\" />"
						"</root>%s"
						,dev_iter->strParentDevId.c_str(),dev_iter->nDevFirm,dev_iter->strDevIP.c_str(),dev_iter->nDevPort,dev_iter->strDevUser.c_str(),dev_iter->strDevPass.c_str()
						,dev_iter->nDevTypeChild,dev_iter->nWebPort,dev_iter->nChannelNO,nStartFlag,CMS_END_FLAG);
					nRet = SendDMSCmd(m_DMSServer,cRequestBuf,strlen(cRequestBuf),true);
					if(nRet<0)
					{
						control_rsp.result = 1;
						if(m_pControlReqCB!=NULL)
							m_pControlReqCB(&control_rsp);
					}
					else
					{
						control_rsp.result = 0;
						if(m_pControlReqCB!=NULL)
							m_pControlReqCB(&control_rsp);
					}

				}
			}
			else
			{
				control_rsp.result = 1;
				if(m_pControlReqCB!=NULL)
					m_pControlReqCB(&control_rsp);
			}
		}

	default:
		break;
	}

	if(pTask->param1!=NULL)
	{
		free(pTask->param1);
		pTask->param1=NULL;
	}
	/*if(pTask->param2!=NULL)
	{
	free(pTask->param2);
	pTask->param2=NULL;
	}*/
	free(pTask);
	pTask = NULL;
}

void InterfaceImp::ProcMediaServerMessage(char *cMessage)
{
	stringstream ss;
	int nSessionLen = strlen("sessionid");
	char *pSessionPos = strstr(cMessage,"sessionid");
	if(pSessionPos==NULL)
	{
		ss.clear();
		ss.str("");
		ss<<"未找到sessionid,格式错误_"<<cMessage;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return;
	}
	char cSessionID[64]={0};
	int nOutLen = 0;
	bool bRet = ParseHttpParam(pSessionPos+nSessionLen,"\"","\"",cSessionID,sizeof(cSessionID),1,nOutLen);
	if(!bRet)
	{
		ss.clear();
		ss.str("");
		ss<<"未找到sessionid,解析错误_"<<cMessage;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return;
	}

	int did = atoi(cSessionID);
	sipd_media_session sipMediaSession;	
	{
		JMutexAutoLock aLock(m_MediaSeesionMapMutex);
		map<int,sipd_media_session>::iterator iter;
		iter = m_MediaSeesionMap.find(did);
		if(iter==m_MediaSeesionMap.end())
		{
			ss.clear();
			ss.str("");
			ss<<"未找到MediaSession";
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			return;
		}
		memcpy(&sipMediaSession,&iter->second,sizeof(sipd_media_session));
	}

	char *pRealPlay = strstr(cMessage,"realplay");
	char *pPlayback = strstr(cMessage,"playback");
	char *pDownload = strstr(cMessage,"download");
	if(pRealPlay!=NULL || pPlayback!=NULL || pDownload!=NULL)//play responed
	{
		char cMedia_send_ip[64]={0};
		char cMedia_send_port[8]={0};
		char cMedia_send_ssrc[64]={0};
		char *pRespond = strstr(pSessionPos,"ok");
		char *pMedia_send_ip = strstr(pSessionPos,"media_send_ip");
		char *pMedia_send_port = strstr(pSessionPos,"media_send_port");
		char *pMedia_send_ssrc = strstr(pSessionPos,"ssrc");
		sipd_sender_sdp_info SenderInfo;
		memset(&SenderInfo,0,sizeof(sipd_sender_sdp_info));
		SenderInfo.did = sipMediaSession.did;
		SenderInfo.tid = sipMediaSession.tid;
		if(pMedia_send_ip==NULL || pMedia_send_port==NULL || pMedia_send_ssrc==NULL)
		{
			ss.clear();
			ss.str("");
			ss<<"未找到Media信息,格式错误_"<<cMessage;
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			return;
		}

		bRet = ParseHttpParam(pMedia_send_ip+strlen("media_send_ip"),"\"","\"",cMedia_send_ip,sizeof(cMedia_send_ip),1,nOutLen);
		if(!bRet)
		{
			ss.clear();
			ss.str("");
			ss<<"未找到Media_send_ip信息,解析错误_"<<cMessage;
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			return;
		}

		bRet = ParseHttpParam(pMedia_send_port+strlen("media_send_port"),"\"","\"",cMedia_send_port,sizeof(cMedia_send_port),1,nOutLen);
		if(!bRet)
		{
			ss.clear();
			ss.str("");
			ss<<"未找到Media_send_port信息,解析错误_"<<cMessage;
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			return;
		}

		bRet = ParseHttpParam(pMedia_send_ssrc+strlen("ssrc"),"\"","\"",cMedia_send_ssrc,sizeof(cMedia_send_ssrc),1,nOutLen);
		if(!bRet)
		{
			ss.clear();
			ss.str("");
			ss<<"未找到Media_send_ssrc信息,解析错误_"<<cMessage;
			LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
			return;
		}
		strcpy(SenderInfo.media_send_ip,cMedia_send_ip);
		memcpy(SenderInfo.media_send_port,cMedia_send_port,sizeof(cMedia_send_port));
		SenderInfo.media_send_ip_type = SIPD_IPType_IPv4;
		strcpy(SenderInfo.media_sender_id,sipMediaSession.channel_id);
		strcpy(SenderInfo.ssrc,cMedia_send_ssrc);
		int nType = SIPD_MEDIA_CONTROL_CB_INVITE_RSP;
		m_pMediaCB(&nType,(void *)&SenderInfo);
		//SendMediaPlayRequest(sipMediaSession);
	}
	else if(strstr(cMessage,"play")!=NULL)//play responed
	{
		char *pRespond = strstr(pSessionPos,"ok");
		/*if(sipMediaSession.stream_type==SIPD_STREAM_TYPE_PLAY)
		{
		sipd_sender_sdp_info SenderInfo;
		memset(&SenderInfo,0,sizeof(sipd_sender_sdp_info));
		SenderInfo.did = sipMediaSession.did;
		SenderInfo.tid = sipMediaSession.tid;
		sip_media_pcb_func(SIPD_MEDIA_CONTROL_CB_INVITE_RSP,&SenderInfo);
		}
		else
		{
		sipd_media_file_end_ind MediaFileInfo;
		memset(&MediaFileInfo,0,sizeof(sipd_media_file_end_ind));
		MediaFileInfo.did = sipMediaSession.did;
		MediaFileInfo.tid = sipMediaSession.tid;
		MediaFileInfo.o_id = sipMediaSession.o_id;
		sip_media_pcb_func(SIPD_MEDIA_CONTROL_CB_FILE_TO_END,&MediaFileInfo);
		}*/

	}
	else if(strstr(cMessage,"teardown")!=NULL)//teardown responed
	{
		char *pRespond = strstr(pSessionPos,"ok");
		printf("teardown");
	}
	else if(strstr(cMessage,"control")!=NULL)//control responed
	{
		char *pRespond = strstr(pSessionPos,"ok");
	}
	else if(strstr(cMessage,"fileend")!=NULL)//fileend responed
	{
		char *pRespond = strstr(pSessionPos,"ok");
		sipd_media_file_end_ind MediaFileInfo;
		memset(&MediaFileInfo,0,sizeof(sipd_media_file_end_ind));
		MediaFileInfo.did = sipMediaSession.did;
		MediaFileInfo.tid = sipMediaSession.tid;
		strcpy(MediaFileInfo.o_id,sipMediaSession.o_id);
		int nType = SIPD_MEDIA_CONTROL_CB_INVITE_RSP;
		m_pMediaCB(&nType,(void *)&MediaFileInfo);
	}
}

int InterfaceImp::RebootDevice(sipd_id_info * pIdInfo)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"RebootDevice";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	map<string,SN_DEVICE>::iterator iter;
	list<SN_DEVICE> DeviceInfo;
	{
		JMutexAutoLock aLock(m_DeviceMapMutex);
		iter = m_DeviceMap.find(pIdInfo->device_id);
		if(iter!=m_DeviceMap.end())
		{
			DeviceInfo.push_back(iter->second);
		}
	}

	if(DeviceInfo.size()>0)
		return RebootDeviceImp(m_DMSServer,DeviceInfo.front());
	else
		return -1;
	return 0;
}

int InterfaceImp::RebootDeviceImp(ServerInfo DMSServer,SN_DEVICE SnDevice)
{
	char cReqBuf[1024]={0};
	char cSessionID[64]={0};
	char cRecvBuf[1024*10+1]={0};
	int nRet = xml_OpenCon(DMSServer);
	int nRtspSeq = 0;
	stringstream ss;
	if(nRet<0)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	sprintf(cReqBuf,"<root>"
		"<data commandtype=\"cmd_type_dev_manage\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"0002\" />"
		"</root>%s"
		,SnDevice.strDevId.c_str(),SnDevice.nDevFirm,SnDevice.strDevIP.c_str(),SnDevice.nDevPort,SnDevice.strDevUser.c_str(),SnDevice.strDevPass.c_str()
		,SnDevice.nDevTypeChild,SnDevice.nWebPort,CMS_END_FLAG);
	nRet = send(DMSServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR == nRet)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	nRet = recv(DMSServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR==nRet)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	xml_closeCon(DMSServer);
	return 0;
}

int InterfaceImp::QueryDevAlarmStatus(ServerInfo DMSServer,SN_DEVICE SnDevice,int &nAlarmStatus)
{
	char cReqBuf[1024]={0};
	char cSessionID[64]={0};
	char cRecvBuf[1024*10+1]={0};
	int nRet = xml_OpenCon(DMSServer);
	int nRtspSeq = 0;
	stringstream ss;
	if(nRet<0)
	{
		xml_closeCon(DMSServer);
		return -1;
	}

	sprintf(cReqBuf,"<root>"
		"<data commandtype=\"cmd_type_dev_param\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" "
		"command=\"000f\" motionType=\"1\" channel=\"0\" masterstate=\"0\" armeddelay=\"0\" />"
		"</root>%s"
		,SnDevice.strDevId.c_str(),SnDevice.nDevFirm,SnDevice.strDevIP.c_str(),SnDevice.nDevPort,SnDevice.strDevUser.c_str(),SnDevice.strDevPass.c_str(),SnDevice.nDevTypeChild,SnDevice.nWebPort,CMS_END_FLAG);
	nRet = send(DMSServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR == nRet)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	nRet = recv(DMSServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR==nRet)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	xml_closeCon(DMSServer);
	cReqBuf[nRet]='\0';
	char cAlarmStatus[10]={0};
	int nLen = 0;
	bool bRet = ParseHttpParam(cReqBuf,"masterstate=\"","\"",cAlarmStatus,10,1,nLen);
	if(bRet)
		nAlarmStatus = atoi(cAlarmStatus);
	else
		return -1;
	//nAlarmStatus = 0;

	return 0;
}

//查询设备通道 1：父设备 2:子设备
int InterfaceImp::GetDevChannel(string strDevGBID,list<SN_DEVICE>&SnDevice,int nType,list<SN_DEVICE>&SnUperDev)//nType 0:视频通道 1:报警输入通道 2:报警输出 3:所有报警通道
{
	JMutexAutoLock aLock(m_DeviceMapMutex);
	map<string,SN_DEVICE>::iterator iter;
	int nRet = 0; 
	iter = m_DeviceMap.find(strDevGBID);
	if(iter!=m_DeviceMap.end())
	{

		if(iter->second.nDevTypeId==1 || iter->second.nDevTypeId==2)//parant
		{
			string strDevID = iter->second.strDevId;
			nRet = 1;
			SnUperDev.push_back(iter->second);
			for(iter=m_DeviceMap.begin();iter!=m_DeviceMap.end();iter++)
			{
				if(strDevID==iter->second.strParentDevId)
				{
					if(nType==0 && iter->second.nDevTypeId==5)
						SnDevice.push_back(iter->second);
					else if(nType==1 && iter->second.nDevTypeId==6)
						SnDevice.push_back(iter->second);
					else if(nType==2 && iter->second.nDevTypeId==8)
						SnDevice.push_back(iter->second);
					else if(nType==3 && (iter->second.nDevTypeId==8 || iter->second.nDevTypeId==6))
						SnDevice.push_back(iter->second);
				}
			}
		}
		else
		{
			SnDevice.push_back(iter->second);
			nRet = 2;
			string strDevID = iter->second.strParentDevId;
			/*map<string,SN_DEVICE>::iterator parent_iter;
			parent_iter = m_DeviceMap.find()*/
			for(iter=m_DeviceMap.begin();iter!=m_DeviceMap.end();iter++)
			{
				if(strDevID == iter->second.strDevId)
				{
					SnUperDev.push_back(iter->second);
					break;
				}
			}
		}

		return nRet;
	}
	else
		return -1;
}



int InterfaceImp::SendPTZControlRequest(LPPTZControlInfo pPTZControlInfo)
{
	list<SN_DEVICE> SnDev;
	list<SN_DEVICE> ::iterator dev_iter;
	int nChannel=0;
	{
		JMutexAutoLock aLock(m_DeviceMapMutex);
		map<string,SN_DEVICE>::iterator iter;
		iter = m_DeviceMap.find(pPTZControlInfo->cDevGBID);
		if(iter!=m_DeviceMap.end())
		{
			SnDev.push_back(iter->second);
		}
	}

	if(SnDev.size()>0)
	{
		dev_iter = SnDev.begin();
		switch(pPTZControlInfo->nPTZCmdType)
		{
		case 1: //move
			{
				int nCmd = transPTZCmdCode(pPTZControlInfo->nPTZCmdType,pPTZControlInfo->ptzMoveControl.cmd_type);
				int nStart = pPTZControlInfo->ptzMoveControl.cmd_type==SIPD_PTZ_MOVE_STOP?0:1;
				pPTZControlInfo->ptzMoveControl.speed = pPTZControlInfo->ptzMoveControl.speed>256?256:pPTZControlInfo->ptzMoveControl.speed;
				int nSpeed = 0;
				if(nCmd==0x0a || nCmd==0x0b)
					nSpeed = pPTZControlInfo->ptzMoveControl.speed;
				else
					nSpeed = pPTZControlInfo->ptzMoveControl.speed*15/256;
				char cRequestBuf[1024]={0};
				sprintf(cRequestBuf,"<root>"
					"<data commandtype=\"cmd_type_ptz\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" "
					"port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"%04x\" channel=\"%d\" start=\"%d\" speed=\"%d\" />"
					"</root>%s"
					,dev_iter->strParentDevId.c_str(),dev_iter->nDevFirm,dev_iter->strDevIP.c_str(),dev_iter->nDevPort,dev_iter->strDevUser.c_str(),dev_iter->strDevPass.c_str()
					,dev_iter->nDevTypeChild,dev_iter->nWebPort,nCmd,dev_iter->nChannelNO,nStart,nSpeed,CMS_END_FLAG);
				int nRet = SendDMSCmd(m_DMSServer,cRequestBuf,strlen(cRequestBuf));	
			}
			break;

		case 2://Tour
			{
				//int nSpeed = pPTZControlInfo->ptzTourControl.speed==0?0:pPTZControlInfo->ptzTourControl.speed
				int nCmd = transPTZCmdCode(pPTZControlInfo->nPTZCmdType,pPTZControlInfo->ptzTourControl.tour_cmd);
				int nStart = pPTZControlInfo->ptzMoveControl.cmd_type==SIPD_PTZ_MOVE_STOP?0:1;
				//pPTZControlInfo->ptzMoveControl.speed = pPTZControlInfo->ptzMoveControl.speed>256?256:pPTZControlInfo->ptzMoveControl.speed;
				int nSpeed = 8;
				int nStayTime = 3;
				char cRequestBuf[1024]={0};
				sprintf(cRequestBuf,"<root>"
					"<data commandtype=\"cmd_type_ptz\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" "
					"port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"%04x\" channel=\"%d\" "
					"cruiseline=\"%d\" cruiseid=\"%d\" presetindex=\"%d\" speed=\"%d\" staytime=\"%d\" />"
					"</root>%s"
					,dev_iter->strParentDevId.c_str(),dev_iter->nDevFirm,dev_iter->strDevIP.c_str(),dev_iter->nDevPort,dev_iter->strDevUser.c_str(),dev_iter->strDevPass.c_str()
					,dev_iter->nDevTypeChild,dev_iter->nWebPort,nCmd,dev_iter->nChannelNO,pPTZControlInfo->ptzTourControl.tour_id
					,pPTZControlInfo->ptzTourControl.preset_id,pPTZControlInfo->ptzTourControl.preset_id,nSpeed,nStayTime,CMS_END_FLAG);
				int nRet = SendDMSCmd(m_DMSServer,cRequestBuf,strlen(cRequestBuf),true);	
			}
			break;

		case 3://preset
			{
				int nCmd = transPTZCmdCode(pPTZControlInfo->nPTZCmdType,pPTZControlInfo->ptzPresetControl.cmd_type);
				int nStart = pPTZControlInfo->ptzMoveControl.cmd_type==SIPD_PTZ_MOVE_STOP?0:1;
				char cRequestBuf[1024]={0};
				sprintf(cRequestBuf,"<root>"
					"<data commandtype=\"cmd_type_ptz\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" "
					"port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"%04x\" channel=\"%d\" value=\"%d\" />"
					"</root>%s"
					,dev_iter->strParentDevId.c_str(),dev_iter->nDevFirm,dev_iter->strDevIP.c_str(),dev_iter->nDevPort,dev_iter->strDevUser.c_str(),dev_iter->strDevPass.c_str()
					,dev_iter->nDevTypeChild,dev_iter->nWebPort,nCmd,dev_iter->nChannelNO,pPTZControlInfo->ptzPresetControl.preset_id,CMS_END_FLAG);
				int nRet = SendDMSCmd(m_DMSServer,cRequestBuf,strlen(cRequestBuf));	
			}
			break;

		case 4://scan
			{
				int nCmd = transPTZCmdCode(pPTZControlInfo->nPTZCmdType,pPTZControlInfo->ptzAutoScanControl.cmd_type);
				int nStart = pPTZControlInfo->ptzAutoScanControl.cmd_type==SIPD_PTZ_MOVE_STOP?0:1;
				char cRequestBuf[1024]={0};
				sprintf(cRequestBuf,"<root>"
					"<data commandtype=\"cmd_type_ptz\" key=\"%s\" proxyclass=\"%d\" IP=\"%s\" "
					"port=\"%d\" username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"%04x\" channel=\"%d\" start=\"%d\" />"
					"</root>%s"
					,dev_iter->strParentDevId.c_str(),dev_iter->nDevFirm,dev_iter->strDevIP.c_str(),dev_iter->nDevPort,dev_iter->strDevUser.c_str(),dev_iter->strDevPass
					,dev_iter->nDevTypeChild,dev_iter->nWebPort,nCmd,dev_iter->nChannelNO,nStart,CMS_END_FLAG);
				int nRet = SendDMSCmd(m_DMSServer,cRequestBuf,strlen(cRequestBuf));	
			}
			break;

		default:
			break;
		}
		return 0;
	}
	return -1;
}

void InterfaceImp::ProcCMSNotify(char *cMessage)
{
	stringstream ss;

	if(strstr(cMessage,"user_login")!=NULL)
	{

	}
	else if(strstr(cMessage,"heartbeat")!=NULL)
	{
		if(strstr(cMessage,"fail")!=NULL)
		{
			ss.clear();
			ss.str("");
			ss<<"登陆失败，重新登陆_"<<cMessage;
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

			re_cms_logon(m_strLocalIP,m_CMSServer,&m_cmsThread);
		}
	}
	else if(strstr(cMessage,"ptz_control")!=NULL)
	{
		if(strstr(cMessage,"ok")!=NULL)
		{
			JMutexAutoLock autoLock(m_PTZControlLstMutex);
			if(m_PTZControlLst.size()==0)
				return;

			PTZControlInfo *pPTZControlInfoReq = (PTZControlInfo*)malloc(sizeof(PTZControlInfo));
			LPTaskInfo pTaskInfo = (LPTaskInfo)malloc(sizeof(TaskInfo)); 
			if(pTaskInfo==NULL || pPTZControlInfoReq==NULL)
			{
				if(pTaskInfo!=NULL)
					free(pTaskInfo);

				if(pPTZControlInfoReq!=NULL)
					free(pPTZControlInfoReq);

				return;
			}

			memcpy(pPTZControlInfoReq,&m_PTZControlLst.front(),sizeof(PTZControlInfo));
			m_PTZControlLst.pop_front();
			pTaskInfo->param1 = (void *)pPTZControlInfoReq;
			pTaskInfo->lparam = (void *)this;
			pTaskInfo->nTaskType = TASK_PTZ;
			AddTask(pTaskInfo,1);
		}
		else
			int i=0;

	}
	else if(strstr(cMessage,"alarm_report")!=NULL)
	{
		list<AlarmInfo>alarmLst;
		list<AlarmInfo>::iterator alarm_iter;
		map<string,SN_DEVICE>::iterator dev_iter;

		ss.clear();
		ss.str("");
		ss<<"alarm come in_"<<cMessage;
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

		ParaseAlarmReport(m_LocalCfg.strDBIP,m_LocalCfg.nDBPort,cMessage,alarmLst);
		for(alarm_iter=alarmLst.begin();alarm_iter!=alarmLst.end();alarm_iter++)
		{
			string strDevGBID;
			string strParentDevGBID;
			int nFind = 0;
			{
				JMutexAutoLock aLock(m_DeviceMapMutex);
				for(dev_iter = m_DeviceMap.begin();dev_iter!=m_DeviceMap.end();dev_iter++)
				{
					if(dev_iter->second.strDevId==alarm_iter->strDevID)
					{
						strDevGBID = dev_iter->second.strGBID;
						nFind++;
						if(nFind==2)
							break;
					}
					else if(dev_iter->second.strDevId==alarm_iter->strParentDevID)
					{
						strParentDevGBID = dev_iter->second.strGBID;
						nFind++;
						if(nFind==2)
							break;
					}
				}
			}
			if(nFind==2)
			{
				sipd_alarm_ind alarm_ind;
				strcpy(alarm_ind.alarm_id,strDevGBID.c_str());
				strcpy(alarm_ind.device_id,strParentDevGBID.c_str());
				strcpy(alarm_ind.alarm_time,alarm_iter->strAlarmTime.c_str());
				alarm_ind.alarm_priority = (enum sipd_alarm_level)(alarm_iter->nAlarmLevel>4?4:alarm_iter->nAlarmLevel);
				if(alarm_iter->nAlarmType==32)//视频丢失
					alarm_ind.alarm_type = SIPD_ALARM_VIDEO;
				else if(alarm_iter->nAlarmType>=601 && alarm_iter->nAlarmType<=699)//IO报警
					alarm_ind.alarm_type = SIPD_ALARM_IO;
				else //其他报警
					alarm_ind.alarm_type = SIPD_ALARM_OTHER;

				if(m_pAlarmCB!=NULL)
					m_pAlarmCB(&alarm_ind);

			}

		}

	}
	else if(strstr(cMessage,"UpdateDBTable")!=NULL)
	{
		ss.clear();
		ss.str("");
		ss<<"UpdateDBTable_"<<cMessage;
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

		int nUpdataFlag = 0;
		xmlDocPtr generalDoc = XmlUtils::StringToDoc(cMessage);
		//printf("Xml encoding : %s \r\n",generalDoc->encoding );

		xmlNode *  tnode =  generalDoc->children->children;
		list<string> strTableNameList;
		while(tnode!=NULL){
			if(tnode->type ==XML_ELEMENT_NODE && strcmp((char *)tnode->name,"data")==0){
				string strName= (char*)xmlGetProp(tnode, BAD_CAST "tableName");

				strTableNameList.push_back(strName);
				//channel.chanNo =  atoi(dev_no.c_str());
			}
			tnode = tnode->next;
		}
		xmlFreeDoc(generalDoc);
		list<string>::iterator iter;
		for(iter = strTableNameList.begin();iter!=strTableNameList.end();iter++)
		{
			string strTableName = *iter;
			if(strTableName=="SnDevice" || strTableName=="SnVideoDvr" || strTableName=="SnDeviceRole")
			{
				if(nUpdataFlag!=2)
					nUpdataFlag = 1;

			}
			else if(strTableName=="SnRoleUser" || strTableName=="SnRole" || strTableName=="SnPerson")
			{
				nUpdataFlag = 2;
			}
		}

		if(nUpdataFlag==1)
		{
			ss.clear();
			ss.str("");
			ss<<"重新获取设备列表";
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			GetAllDevice();
		}
		else if(nUpdataFlag==2)
		{
			GetUserID();
			GetAllDevice();
		}


	}
	else if(strstr(cMessage,"balance_gate")!=NULL)
	{

	}
}

int InterfaceImp::PTZControl(void *PTZCmd,int nPTZtype)
{

	stringstream ss;
	PTZControlInfo ptzInfo;
	if(nPTZtype==1)//move
	{
		JMutexAutoLock alock(m_PTZControlLstMutex);
		sipd_ptz_control *pCmd = (sipd_ptz_control*)PTZCmd;
		strcpy(ptzInfo.cDevGBID,pCmd->device_id);
		ptzInfo.nPTZCmdType = nPTZtype;
		memcpy(&ptzInfo.ptzMoveControl,pCmd,sizeof(sipd_ptz_control));
		m_PTZControlLst.push_back(ptzInfo);

		ss.clear();
		ss.str("");
		ss<<"PTZ move";
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	}
	else if(nPTZtype==2)//Tour
	{
		JMutexAutoLock alock(m_PTZControlLstMutex);
		sipd_tour_control *pCmd = (sipd_tour_control*)PTZCmd;
		strcpy(ptzInfo.cDevGBID,pCmd->device_id);
		ptzInfo.nPTZCmdType = nPTZtype;
		memcpy(&ptzInfo.ptzTourControl,pCmd,sizeof(sipd_tour_control));
		m_PTZControlLst.push_back(ptzInfo);

		ss.clear();
		ss.str("");
		ss<<"PTZ Tour";
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	}
	else if(nPTZtype==3)//Preset
	{
		JMutexAutoLock alock(m_PTZControlLstMutex);
		sipd_preset_control *pCmd = (sipd_preset_control*)PTZCmd;
		strcpy(ptzInfo.cDevGBID,pCmd->device_id);
		ptzInfo.nPTZCmdType = nPTZtype;
		memcpy(&ptzInfo.ptzPresetControl,pCmd,sizeof(sipd_preset_control));
		m_PTZControlLst.push_back(ptzInfo);

		ss.clear();
		ss.str("");
		ss<<"PTZ Preset";
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	}
	else if(nPTZtype==4)//AutoScan
	{
		JMutexAutoLock alock(m_PTZControlLstMutex);
		sipd_autoscan_control *pCmd = (sipd_autoscan_control*)PTZCmd;
		strcpy(ptzInfo.cDevGBID,pCmd->device_id);
		ptzInfo.nPTZCmdType = nPTZtype;
		memcpy(&ptzInfo.ptzAutoScanControl,pCmd,sizeof(sipd_autoscan_control));
		m_PTZControlLst.push_back(ptzInfo);

		ss.clear();
		ss.str("");
		ss<<"PTZ AutoScan";
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
	}
	else
	{
		ss.clear();
		ss.str("");
		ss<<"UNKNOW PTZ cmd";
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	return 0;
}

int InterfaceImp::RequestDevCataloginfo(sipd_catalog_req * pCatalogReq)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"RequestDevCataloginfo";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	LPTaskInfo pTask = (LPTaskInfo)malloc(sizeof(TaskInfo));
	sipd_catalog_req *pParam = (sipd_catalog_req * )malloc(sizeof(sipd_catalog_req));
	if(pTask==NULL || pParam==NULL)
	{
		if(pTask!=NULL)
			free(pTask);
		if(pParam!=NULL)
			free(pParam);

		return -1;
	}
	memcpy(pParam,pCatalogReq,sizeof(sipd_catalog_req));
	pTask->lparam = this;
	pTask->param1 = pParam;
	pTask->nTaskType = TASK_QUERY_DEV_CATA;

	AddTask(pTask,0);
	return 0;

}

int InterfaceImp::RequestSetDevAlarmStatus(sipd_alarm_control * pAlarmControl)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"RequestSetDevAlarmStatus";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());

	LPTaskInfo pTask = (LPTaskInfo)malloc(sizeof(TaskInfo));
	sipd_alarm_control *pParam = (sipd_alarm_control * )malloc(sizeof(sipd_alarm_control));
	if(pTask==NULL || pParam==NULL)
	{
		if(pTask!=NULL)
			free(pTask);
		if(pParam!=NULL)
			free(pParam);

		return -1;
	}
	memcpy(pParam,pAlarmControl,sizeof(sipd_alarm_control));
	pTask->lparam = this;
	pTask->param1 = pParam;
	pTask->nTaskType = TASK_ALARM_CONTROL;

	AddTask(pTask,1);
	return 0;
}

int InterfaceImp::RequestDeviceInfo(sipd_deviceinfo_req * pDeviceinfoReq)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"RequestDeviceInfo";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());


	LPTaskInfo pTask = (LPTaskInfo)malloc(sizeof(TaskInfo));
	sipd_deviceinfo_req *pParam = (sipd_deviceinfo_req * )malloc(sizeof(sipd_deviceinfo_req));
	if(pTask==NULL || pParam==NULL)
	{
		if(pTask!=NULL)
			free(pTask);
		if(pParam!=NULL)
			free(pParam);

		return -1;
	}
	memcpy(pParam,pDeviceinfoReq,sizeof(sipd_deviceinfo_req));
	pTask->lparam = this;
	pTask->param1 = pParam;
	pTask->nTaskType = TASK_QUERY_DEVICE;

	AddTask(pTask,0);
	return 0;

}

int InterfaceImp::RequestDeviceState(sipd_devicestate_req * pStateInfoReq)
{
	stringstream ss;
	ss.clear();
	ss.str("");
	ss<<"RequestDeviceState";
	LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());


	LPTaskInfo pTask = (LPTaskInfo)malloc(sizeof(TaskInfo));
	sipd_devicestate_req *pParam = (sipd_devicestate_req * )malloc(sizeof(sipd_devicestate_req));
	if(pTask==NULL || pParam==NULL)
	{
		if(pTask!=NULL)
			free(pTask);
		if(pParam!=NULL)
			free(pParam);

		return -1;
	}
	memcpy(pParam,pStateInfoReq,sizeof(sipd_devicestate_req));
	pTask->lparam = this;
	pTask->param1 = pParam;
	pTask->nTaskType = TASK_QUERY_DEV_STATE;

	AddTask(pTask,0);
	return 0;
}

bool InterfaceImp::CheckDeviceRight()
{
	JMutexAutoLock autoLock(m_PTZControlLstMutex);
	list<PTZControlInfo>::iterator iter;
	for(iter=m_PTZControlLst.begin();iter!=m_PTZControlLst.end();iter++)
	{
		//查询设备权限
		list<SN_DEVICE> SnUperDev;
		list<SN_DEVICE>::iterator devLst_iter;
		{
			JMutexAutoLock aLock(m_DeviceMapMutex);
			map<string,SN_DEVICE>::iterator dev_iter;
			dev_iter = m_DeviceMap.find(iter->cDevGBID);
			if(dev_iter!=m_DeviceMap.end())
				SnUperDev.push_back(dev_iter->second);
		}

		if(SnUperDev.size()>0)
		{
			devLst_iter = SnUperDev.begin();
			int nRet = QueryDeviceRight(m_CMSServer,(*devLst_iter).strParentDevId);
			if(nRet==-2)
				re_cms_logon(m_CMSServer.strLocalIP,m_CMSServer,&m_cmsThread);
			//else 
			//{
			//	SetDeviceRight(iter->cDevGBID,nRet);
			//	//有PTZ控制权限，加入到任务队列中
			//	if(nRet==0)
			//	{
			//		PTZControlInfo *pPTZControlInfoReq = (PTZControlInfo*)malloc(sizeof(PTZControlInfo));
			//		LPTaskInfo pTaskInfo = (LPTaskInfo)malloc(sizeof(TaskInfo)); 
			//		if(pTaskInfo==NULL || pPTZControlInfoReq==NULL)
			//		{
			//			if(pTaskInfo!=NULL)
			//				free(pTaskInfo);

			//			if(pPTZControlInfoReq!=NULL)
			//				free(pPTZControlInfoReq);

			//			return false;
			//		}
			//		memcpy(pPTZControlInfoReq,&(*iter),sizeof(PTZControlInfo));

			//		pTaskInfo->param1 = (void *)pPTZControlInfoReq;
			//		pTaskInfo->lparam = (void *)this;
			//		pTaskInfo->nTaskType = TASK_PTZ;
			//		AddTask(pTaskInfo,1);
			//	}
			//}


		}


	}
	//m_PTZControlLst.clear();
	return true;
}

void InterfaceImp::SetDeviceRight(string strDeviceID,int nDeviceRight)
{
	JMutexAutoLock autoLock(m_DeviceRightMapMutex);
	map<string,DeviceRight>::iterator iter;
	iter = m_DeviceRightMap.find(strDeviceID);
	if(iter!=m_DeviceRightMap.end())
	{
		iter->second.bEnablePTZ = nDeviceRight<0?false:true;
		iter->second.check_time = time(NULL);
	}
}

