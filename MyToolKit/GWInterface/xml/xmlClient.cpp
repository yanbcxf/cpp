#include "UrlUtils.hpp"
#include "XmlUtils.hpp"
#include "xmlClient.hpp"
#include "MD5.h"
#include "../InterfaceGlobal.h"

bool SetSocketTimeout(SOCKET sock, int nRecvTimeout, int nSendTimeout)
{
	int ret;
	if(sock == INVALID_SOCKET)
		return false;

	ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&nSendTimeout, sizeof(nSendTimeout)); 
	if(ret == SOCKET_ERROR)
	{
		return false;
	}

	ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nRecvTimeout, sizeof(nRecvTimeout));
	if(ret == SOCKET_ERROR){
		return false;
	}

	return true;
}

int SetTCPNoDelay(int socket, int on)
{
	if(SOCKET_ERROR == setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on,sizeof(on)))
	{
		return -1;
	}
	return 0;
}

bool GetHostAddress(char *hostname, unsigned long *netaddr)
{
	struct hostent *pHostent = gethostbyname(hostname);
	if(NULL == pHostent)
		return false;
	*netaddr = *(unsigned long *)pHostent->h_addr_list[0];
	return true;
}

LONG xml_OpenCon(ServerInfo &cmsInfo)
{
	int   nSocketRet;
	//创建HTTP SOCKET

	cmsInfo.client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( INVALID_SOCKET == cmsInfo.client_socket )
	{
		int ui = GetLastError();
		printf("create socket failed error: %d\n", ::WSAGetLastError());
		return -1;
	}

	//RTSP STEP 1: connect to server	
	if(!SetSocketTimeout(cmsInfo.client_socket, XML_SND_TIMEOUT * 1000, XML_REV_TIMEOUT * 1000))
		return -1;

	struct sockaddr_in HTTP_SockAddr,Local_SockAddr;
	/*	int nRecvTimeOut;*/

	unsigned long netaddr;
	if(!GetHostAddress((char *)cmsInfo.strServerIP.c_str(), &netaddr))
		return GetLastError();

	unsigned long TurnOn = 1;
	if(SOCKET_ERROR == ioctlsocket(cmsInfo.client_socket, FIONBIO, &TurnOn))
	{
		return GetLastError();
	}

	HTTP_SockAddr.sin_family = AF_INET;
	HTTP_SockAddr.sin_addr.s_addr = netaddr;  
	HTTP_SockAddr.sin_port = htons(cmsInfo.nServerPort);
	nSocketRet = connect(cmsInfo.client_socket, (const sockaddr *)&HTTP_SockAddr, sizeof(HTTP_SockAddr));

	if(nSocketRet == SOCKET_ERROR)
	{
		int io = WSAGetLastError();
		int i=0;
		/*if(io != WSAEWOULDBLOCK)
		return GetLastError();*/
	}

	int nSocketSize =sizeof(HTTP_SockAddr);
	/*if(getsockname(xmlrpcClientInfo.rpcSocket,(sockaddr *)&Local_SockAddr,&nSocketSize)!=SOCKET_ERROR)
	{
	strcpy(xmlrpcClientInfo.cIP,inet_ntoa(Local_SockAddr.sin_addr));
	}*/


	fd_set write_set;
	struct timeval TimeOut;
	FD_ZERO(&write_set);
	FD_SET(cmsInfo.client_socket, &write_set);
	TimeOut.tv_sec = XML_SEL_TIMEOUT;
	TimeOut.tv_usec = 0;
	nSocketRet = select(cmsInfo.client_socket+1, NULL, &write_set, NULL, &TimeOut);
	if(nSocketRet <= 0)
	{
		int nError = GetLastError();
		return -1;
	}

	TurnOn = 0;
	if(SOCKET_ERROR == ioctlsocket(cmsInfo.client_socket, FIONBIO, &TurnOn))
	{
		int nError = GetLastError();
		return -1;
	}

	bool   bDontLinger=false; 
	cmsInfo.bConnect = true;
	SetTCPNoDelay(cmsInfo.client_socket,1);
	return 0;
}

bool IsRead(SOCKET sock)
{
	///
	fd_set fdRead;
	FD_ZERO(&fdRead);
	FD_SET(sock, &fdRead); 
	timeval tv={0,100000};
	int nRes = select( 0, &fdRead, NULL, NULL, &tv );
	if(nRes <= 0)
	{
		return false;
	}
	///
	return true;
}

void  xml_closeCon(ServerInfo &cmsInfo)
{
	if( cmsInfo.client_socket != INVALID_SOCKET)
	{
		closesocket(cmsInfo.client_socket);
		cmsInfo.client_socket = INVALID_SOCKET;
		/*//DebugTrace("rtsp_closeCon,ip=%s,handle=%d\n",
		((LPLOGON_INFO)(pClientInfo->param))->hostAddress,pClientInfo->ContextInfo.lRealHandle);*/
	}
	cmsInfo.bConnect=false;
}

int cms_logon(string strSourceIP,ServerInfo &cmsInfo)
{
	char cLogonText[1024*10]={0};
	char cMD5PW[512]={0};
	char cRecvBuf[512+1]={0};
	Get_MD5_data((unsigned char *)cMD5PW,512,(unsigned char *)cmsInfo.strPassWord.c_str());
	sprintf(cLogonText,"<root>\r\n"
		"<msg_type>user_login</msg_type>\r\n"
		"<data pers_id = '%s' pers_pwd = '%s' pers_ip = '%s' client_type='1'></data>\r\n"
		"</root>%s",cmsInfo.strUserID.c_str(),cMD5PW,cmsInfo.strLocalIP.c_str(),CMS_END_FLAG);

	int nRet = xml_OpenCon(cmsInfo);
	if(nRet<0)
	{
		xml_closeCon(cmsInfo);
		return -1;
	}

	int nSend = send(cmsInfo.client_socket,cLogonText,strlen(cLogonText),0);
	if(SOCKET_ERROR == nSend)
	{
		int nErr = GetLastError();
		xml_closeCon(cmsInfo);
		return -1;
	}

	int nRecv = recv(cmsInfo.client_socket,cRecvBuf,512,0);
	if(SOCKET_ERROR == nRecv)
	{
		int nErr = GetLastError();
		xml_closeCon(cmsInfo);
		return -1;
	}

	char  *npos = strstr(cRecvBuf,CMS_END_FLAG);
	if(npos==NULL)
	{
		xml_closeCon(cmsInfo);
		return -1;
	}

	npos = strstr(cRecvBuf,"ok");
	if(npos==NULL)
	{
		xml_closeCon(cmsInfo);
		return -1;
	}

	return 0;
}

bool SendHeart(ServerInfo &cmsInfo)
{
	char cHeartText[256]={0};
	sprintf(cHeartText,"<root><msg_type>heartbeat</msg_type><data pers_id='%s'/></root>%s",cmsInfo.strUserID.c_str(),CMS_END_FLAG);
	int nSend = send(cmsInfo.client_socket,cHeartText,strlen(cHeartText),0);
	if(SOCKET_ERROR == nSend)
	{
		int nErr = GetLastError();
		xml_closeCon(cmsInfo);
		return false;
	}
	return true;

}

bool Get_MD5_data(unsigned char *out_data, int out_data_length,unsigned char *in_data)	//获得MD5处理后的数据保存在out_password中，成功则返回true
{
	try
	{
		unsigned char temp_buffer[120];
		memset(temp_buffer,0,120);

		memset(out_data, 0, out_data_length);

		MD5_CTX md5;

		MD5Init(&md5); //初始化用于md5加密的结构

		MD5Update(&md5,in_data,strlen((const char *)in_data)); //对欲加密的字符进行加密

		MD5Final(temp_buffer,&md5); //获得最终结果

		for(int i=0;i<16;i++)
		{
			if(i==15)
				int j = 0;
			int nLen = strlen((char *)out_data);
			sprintf((char *)out_data + nLen, "%02x", temp_buffer[i]);
		}

		return true;
	}
	catch(std::exception &e)
	{
		std::cerr<<"Get_MD5_data Exception："<<e.what()<<endl;
		//INTERFACE_ERRORLOG("[Get_MD5_data] Exception："<<e.what());
		return false;
	}

}

int QueryDeviceRight(ServerInfo &cmsInfo,string strUUID)
{
	char cHeartText[256]={0};
	char cRecvBuf[1024+1]={0};
	sprintf(cHeartText,"<root><msg_type>ptz_control</msg_type>"
		"<data pers_id = '%s' node_id = '%s'></data>"
		"</root>%s",cmsInfo.strUserID.c_str(),strUUID.c_str(),CMS_END_FLAG);
	int nRet = send(cmsInfo.client_socket,cHeartText,strlen(cHeartText),0);
	if(SOCKET_ERROR == nRet)
	{
		int nErr = GetLastError();
		xml_closeCon(cmsInfo);
		return -2;
	}
	return 0;
	/*nRet = recv(cmsInfo.client_socket,cRecvBuf,1024,0);
	if(SOCKET_ERROR == nRet)
	{
		int nErr = GetLastError();
		xml_closeCon(cmsInfo);
		return -2;
	}
	if(strstr(cRecvBuf,"ok")!=NULL)
		return 0;
	else
		return -1;*/
}

int QuerySingleRecordEx(ServerInfo VODServer,string strDevGBID,string strDevName,list<RecordInfo>&RecordTimeLst,char *cUrl)
{
	char cReqBuf[1024]={0};
	char cSessionID[64]={0};
	char cRecvBuf[1024*10+1]={0};
	int nRet = xml_OpenCon(VODServer);
	int nRtspSeq = 0;
	stringstream ss;
	if(nRet<0)
	{
		xml_closeCon(VODServer);
		return -1;
	}

	//describe
	sprintf(cReqBuf,"DESCRIBE %s RTSP/1.0\r\n"
		"CSeq: %d\r\n"
		"Accept: application/sdp\r\n"
		"User-Agent: StarNet/1.0.0\r\n\r\n", cUrl, nRtspSeq);//Grandstream Client

	int nSend = send(VODServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR == nSend)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	int nRecv = recv(VODServer.client_socket,cRecvBuf,1024*10,0);
	if(SOCKET_ERROR == nRecv)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}
	cRecvBuf[nRecv]='\0';

	if(NULL==strstr(cRecvBuf,"200 OK"))
	{
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<" fail_"<<cRecvBuf;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}
	nRtspSeq++;

	//setup
	memset(cReqBuf,0,sizeof(cReqBuf));
	sprintf(cReqBuf,"SETUP %s RTSP/1.0\r\n"
					"CSeq: %d\r\n"
					"Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n"
					"User-Agent: StarNet/1.0.0\r\n\r\n",cUrl,nRtspSeq);
	

	nSend = send(VODServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR == nSend)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	nRecv = recv(VODServer.client_socket,cRecvBuf,1024*10,0);
	if(SOCKET_ERROR == nRecv)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}
	cRecvBuf[nRecv]='\0';

	if(NULL==strstr(cRecvBuf,"200 OK"))
	{
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<" fail_"<<cRecvBuf;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	char *pStrSession = strstr(cRecvBuf, "Session:");
	if(pStrSession)
	{
		char *pSrc = pStrSession + 9;
		char *pDst = strstr(pSrc, "\r\n");
		char *pDst1 =strstr(pSrc,";");
		pDst = pDst>pDst1?pDst1:pDst;
		if (pDst != NULL)
		{
			memcpy(cSessionID, pSrc, pDst - pSrc);
			string strTmp = cSessionID;
			string drop = " ";
			// trim right
			strTmp.erase(strTmp.find_last_not_of(drop)+1);
			// trim left
			strTmp.erase(0,strTmp.find_first_not_of(drop));

			memset(cSessionID,0,64);
			strcpy(cSessionID,(char *)strTmp.c_str());
		}
	}
	nRtspSeq++;


	//setup2
	memset(cReqBuf,0,sizeof(cReqBuf));
	sprintf(cReqBuf,"SETUP %s RTSP/1.0\r\n"
		"CSeq: %d\r\n"
		"Transport: RTP/AVP/TCP;unicast;interleaved=2-3\r\n"
		"User-Agent: StarNet/1.0.0\r\n\r\n",cUrl,nRtspSeq);


	nSend = send(VODServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR == nSend)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	nRecv = recv(VODServer.client_socket,cRecvBuf,1024*10,0);
	if(SOCKET_ERROR == nRecv)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}
	cRecvBuf[nRecv]='\0';

	if(NULL==strstr(cRecvBuf,"200 OK"))
	{
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<" fail_"<<cRecvBuf;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}


	//getparameter
	memset(cReqBuf,0,sizeof(cRecvBuf));
	sprintf(cReqBuf,"GET_PARAMETER %s RTSP/1.0\r\n"
		"CSeq: %d\r\n"
		"Content-Type: text/parameters\r\n"
		"Content-Length: 16\r\n"
		"Session:%s\r\n"
		"ANR-Recordlist"
		"\r\n\r\n",cUrl, nRtspSeq, cSessionID);//Client

	nSend = send(VODServer.client_socket,cReqBuf,strlen(cReqBuf),0);
	if(SOCKET_ERROR == nSend)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	nRecv = recv(VODServer.client_socket,cRecvBuf,1024*10,0);
	if(SOCKET_ERROR == nRecv)
	{
		int nErr = GetLastError();
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<"_socket is closed";
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}
	cRecvBuf[nRecv]='\0';

	if(NULL==strstr(cRecvBuf,"200 OK"))
	{
		xml_closeCon(VODServer);

		ss.clear();
		ss.str("");
		ss<<__FUNCTION__<<" fail_"<<cRecvBuf;
		LOG4CPLUS_ERROR(g_InterfaceLogger,ss.str().c_str());
		return -1;
	}

	char *pos =strstr(cRecvBuf, "FILE-Recordlist:");
	if(NULL!=pos)
	{
		pos+=strlen("FILE-Recordlist:");
		int nDiff = pos-cRecvBuf;
		ParseRecordInfo(pos,nRecv-nDiff,strDevGBID,strDevName,RecordTimeLst);
		return 0;
	}
	else
		return -1;

	

}

void ParseRecordInfo(char *pos,int nMaxLen,string strDevGBID,string strDevName,list<RecordInfo>&RecordTime)
{
	char *pos1=pos;
	if(pos==NULL)
		return;
	while(1)
	{
		RecordInfo file_record_ayyay;
		char *ptmp = strstr(pos1, ":");
		if(ptmp-pos>=nMaxLen && nMaxLen>0)
			break;
		if(ptmp==NULL)
			break;
		pos1 = ptmp+1;
		ptmp=strstr(pos1, "-");
		if(ptmp==NULL)
			break;
		file_record_ayyay.strStartRecordTime.assign(pos1,ptmp-pos1);
		file_record_ayyay.strStartRecordTime = trans(file_record_ayyay.strStartRecordTime);
		pos1=ptmp;
		pos1++;
		ptmp=strstr(pos1, "\r\n");
		if(ptmp==NULL)
			break;
		file_record_ayyay.strEndRecordTime.assign(pos1,ptmp-pos1);
		file_record_ayyay.strEndRecordTime = trans(file_record_ayyay.strEndRecordTime);
		pos1=pos1+2;
		file_record_ayyay.strDevName = strDevName;
		file_record_ayyay.strGBID = strDevGBID;
		RecordTime.push_back(file_record_ayyay);
	}
}

bool ParseHttpParam(char *pBuf,string strKey,string strDelimiter,void *pOut,int nOutLen,int nType,int &nLen)
{
	char *pos = strstr(pBuf,(char *)strKey.c_str());
	if(pos==NULL)
		return false;
	pos+=strKey.length();
	char *EndPos = strstr(pos,(char *)strDelimiter.c_str());
	if(EndPos!=NULL)
	{
		string strTmp;
		int i = 0;
		for(i=0;;i++)
		{
			if(pos[i]==' ')
				continue;
			else
				break;
		}
		pos+=i;
		if(pos>=EndPos)
			return false;

		strTmp.assign(pos,EndPos-pos);
		int nCopyLen = 0;
		int tmpLen = strTmp.length();
		if(tmpLen>=nOutLen)
			nCopyLen = nOutLen-1;
		else
			nCopyLen = tmpLen;
		if(nType==0)
		{
			int nParam = atoi(strTmp.c_str());
			memcpy(pOut,&nParam,sizeof(int));
		}
		else
		{
			memcpy(pOut,strTmp.c_str(),nCopyLen);
		}
		nLen = EndPos+strDelimiter.length() - pBuf;
	}
	else
	{
		string strTmp;
		strTmp.assign(pos+strKey.length());
		int nCopyLen = 0;
		int tmpLen = strTmp.length();
		if(tmpLen>=nOutLen)
			nCopyLen = nOutLen-1;
		else
			nCopyLen = tmpLen;
		if(nType==0)
		{
			int nParam = atoi(strTmp.c_str());
			memcpy(pOut,&nParam,sizeof(int));
		}
		else
		{
			memcpy(pOut,strTmp.c_str(),nCopyLen);
		}
	}
	return true;
}


string trans(string time){	
	int year 	= atoi(time.substr(0,4).c_str());	
	int month 	= atoi(time.substr(4,2).c_str());	
	int day 	= atoi(time.substr(6,2).c_str());	
	int hour 	= atoi(time.substr(8,2).c_str());	
	int min 	= atoi(time.substr(10,2).c_str());	
	int sec 	= atoi(time.substr(12,2).c_str());	
	char  formateTime[100]={0};	
	sprintf	(formateTime,"%04d-%02d-%02dT%02d:%02d:%02d",		year,		month,		day,		hour,		min,		sec	);	
	return string(formateTime);
}


int SendPTZMoveRequest(ServerInfo DMSServer,SN_DEVICE SnDev,LPPTZControlInfo pPTZInfo)
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
		"<data commandtype=\"cmd_type_ptz\" key=\"%s\" proxyclass=\"SNCmdServer\" IP=\"%s\" port=\"%d\"" 
		"username=\"%s\" password=\"%s\" devclass=\"%d\" httpport=\"%d\" command=\"0001\" channel=\"1\" start=\"1\" speed=\"7\"/>"
		"</root>"
		,SnDev.strDevId.c_str(),SnDev.strDevIP.c_str(),SnDev.nDevPort,SnDev.strDevUser.c_str(),SnDev.strDevPass.c_str(),SnDev.nDevTypeChild
		,SnDev.nWebPort,pPTZInfo->ptzMoveControl.speed);
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

int SendDMSCmd(ServerInfo DMSServer,char *SendBuf,int nLen,bool bRecv)
{	
	char cReqBuf[1024]={0};
	int nRet = xml_OpenCon(DMSServer);
	int nRtspSeq = 0;
	stringstream ss;
	if(nRet<0)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	nRet = send(DMSServer.client_socket,SendBuf,nLen,0);
	if(SOCKET_ERROR == nRet)
	{
		xml_closeCon(DMSServer);
		return -1;
	}
	if(bRecv)
	{
		nRet = recv(DMSServer.client_socket,cReqBuf,1024,0);
		
		if(SOCKET_ERROR==nRet)
		{
			xml_closeCon(DMSServer);
			return -1;
		}
		if(strstr(cReqBuf,"result=\"1\"")!=NULL)
		{
			xml_closeCon(DMSServer);
			return 0;
		}
		else
		{
			xml_closeCon(DMSServer);
			return -1;
		}
	}
	
	xml_closeCon(DMSServer);
	return 0;
}

int transPTZCmdCode(int nPTZCmdType,int nCmd)
{
	int transCmd = -1;
	if(nPTZCmdType==1)//move
	{
		switch (nCmd)
		{
		case SIPD_PTZ_MOVE_UP:
			transCmd = 1;
			break;
		
		case SIPD_PTZ_MOVE_DOWN:
			transCmd = 2;
			break;

		case SIPD_PTZ_MOVE_LEFT:
			transCmd = 3;
			break;

		case SIPD_PTZ_MOVE_RIGHT:
			transCmd = 4;
			break;

		case SIPD_PTZ_MOVE_LU:
			transCmd = 5;
			break;

		case SIPD_PTZ_MOVE_LD:
			transCmd = 7;
			break;

		case SIPD_PTZ_MOVE_RU:
			transCmd = 6;
			break;

		case SIPD_PTZ_MOVE_RD:
			transCmd = 8;
			break;

		case SIPD_PTZ_MOVE_STOP:
			transCmd = 1;
			break;

		case SIPD_PTZ_ZOOMTELE:
			transCmd = 0x0a;
			break;

		case SIPD_PTZ_ZOOMWIDE:
			transCmd = 0x0b;
			break;

		case SIPD_PTZ_FOCUS_NEAR:
			transCmd = 0x0c;
			break;

		case SIPD_PTZ_FOCUS_FAR:
			transCmd = 0x0d;
			break;

		case SIPD_PTZ_HALLO_SMALL:
			transCmd = 0x0f;
			break;

		case SIPD_PTZ_HALLO_LARGE:
			transCmd = 0x0e;
			break;

		default:
			break;
		}
	}
	else if(nPTZCmdType==2)//tour
	{
		switch (nCmd)
		{
		case SIPD_PTZ_TOUR_ADD:
			transCmd = 0x16;
			break;

		case SIPD_PTZ_TOUR_DEL:
			transCmd = 0x17;
			break;

		case SIPD_PTZ_TOUR_START:
			transCmd = 0x18;
			break;

		case SIPD_PTZ_TOUR_SET_STAYTIME:
			transCmd = 1;
			break;

		case SIPD_PTZ_TOUR_SET_SPEED:
			transCmd = 1;
			break;

		case SIPD_PTZ_TOUR_STOP:
			transCmd = 0x0019;
			break;

		default:
			break;

		}
	}
	else if(nPTZCmdType==3)//preset
	{
		switch (nCmd)
		{
		case SIPD_PTZ_PRESET_ADD:
			transCmd = 0x10;
			break;

		case SIPD_PTZ_PRESET_DEL:
			transCmd = 0x11;
			break;

		case SIPD_PTZ_PRESET_GOTO:
			transCmd = 0x12;
			break;
		}
	}
	else if(nPTZCmdType==4)//scan
	{
		transCmd = 9;
	}
	return transCmd;
}