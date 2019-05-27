#include "StdAfx.h"

#include "UsageEnvironment.h"

#include "CallbackDllCallDataResponse.h"


int UsageEnvironment::g_SerialNumber = 0;

UsageEnvironment * UsageEnvironment::instance = NULL;

UsageEnvironment::UsageEnvironment(void)
:liveMediaPriv(NULL)
{

	m_mtxCommands.Init();
	m_mtxCallbacks.Init();
	m_mtxObjects.Init();
	m_mtxDataCallbacks.Init();

	m_Logger =  Logger::getInstance(LOG4CPLUS_TEXT("Dll_28181"));

	m_ThreadHandle  =  CreateThread(NULL,  0 , CommandHandler1 /*MessageHandler1*/, (LPVOID)this ,  0 ,  &m_ThreadId);
	if(NULL == m_ThreadHandle)
	{
		SetThreadPriority( m_ThreadHandle, THREAD_PRIORITY_HIGHEST );
	}

	m_ThreadHandle_Callback  =  CreateThread(NULL,  0 , CallbackHandler1 , (LPVOID)this,  0 , &m_ThreadId_Callback);
	if(NULL == m_ThreadHandle_Callback)
	{

	}

	m_ThreadHandle_DataCallback  =  CreateThread(NULL,  0 , DataCallbackHandler1 , (LPVOID)this,  0 , &m_ThreadId_DataCallback);
	if(NULL == m_ThreadHandle_DataCallback)
	{

	}

	m_nDeviceControlSession = 0;
	m_nDeviceinfoSession = 0;
	m_nStateinfoSession = 0;
	m_nCataloginfoSession = 0;
	m_nSearchRecordfileSession = 0;
	m_nMediaSession = 0;

	m_iRtpPort = UDP_MEDIA_START_PORT;

	m_tvMonitorSession = GetTickCount();
	m_tvMonitorSessionLog = GetTickCount();

	stringstream ss;
	ss << "ThrId-" << m_ThreadId;

	LOG4CPLUS_DEBUG(m_Logger, "线程开始执行......");
}

UsageEnvironment::~UsageEnvironment(void)
{
	LOG4CPLUS_DEBUG(m_Logger, "线程执行 End.");

	if(m_ThreadHandle)
		CloseHandle(m_ThreadHandle);
}


int UsageEnvironment::fetchSerialNumber()
{
	JMutexAutoLock lock(m_mtxObjects);
	do{
		g_SerialNumber++;
		if(g_SerialNumber >= (100 * 10000))
			g_SerialNumber = 1;
		if(m_mapObjects.count(g_SerialNumber)==0)
			break;
			
	}while(1);
	return g_SerialNumber;
}


void UsageEnvironment::SendMessageEx(int srcid,  int msgtype, void *data, int msg_len)
{
	SN_MSG *pmsg = (SN_MSG *)malloc(sizeof(SN_MSG) + msg_len);

	pmsg->thread_id = srcid;
	pmsg->msg_len = msg_len;

	if (!(msg_len == 0 || data == NULL))
	{
		memcpy(pmsg->para, data, msg_len);
	}

	if (!PostThreadMessage(m_ThreadId, msgtype, (WPARAM)pmsg, 0))
	{
		char tmp[1024];
		sprintf(tmp, "sendmessage err %d\n", GetLastError());
		free(pmsg);
		pmsg = NULL;
	}

	return;
}


DWORD WINAPI UsageEnvironment::MessageHandler1(void *data)
{
	UsageEnvironment * pThis = (UsageEnvironment *)data;
	
	MSG msg;
	SN_MSG * tmp = NULL;
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	for(;;)
	{
		if( GetMessage( &msg, 0, 0, 0) )
		{
			tmp = (SN_MSG *)msg.wParam;
			pThis->MessageHandler(&msg);

			tmp = (SN_MSG *)msg.wParam;
			if (tmp != NULL)
			{
				free((void *)msg.wParam);
			}
		}
	}
}


bool UsageEnvironment::PostCallBack(CCommandCallback * cb)
{
	{
		JMutexAutoLock	lock(m_mtxCallbacks);
		m_vecCallbacks.insert(m_vecCallbacks.end(), cb);
	}

	return true;
}


DWORD WINAPI UsageEnvironment::CallbackHandler1(void *data)
{
	UsageEnvironment * pThis = (UsageEnvironment *)data;

	pThis->CallbackHandler();
	return 0 ;
}


DWORD UsageEnvironment::CallbackHandler()
{
	do{
		CCommandCallback * pCmd = NULL;

		{
			JMutexAutoLock lock(m_mtxCallbacks);
			if(m_vecCallbacks.size()>0)
			{
				pCmd = m_vecCallbacks[0];
				m_vecCallbacks.erase(m_vecCallbacks.begin());
			}
		}

		if(pCmd)
		{
			pCmd->Execute();
			delete pCmd;
		}
		else
		{
			Sleep(50);
		}
	}while(true);
}


bool UsageEnvironment::PostDataCallBack(CCommandCallback * cb)
{	

	{
		JMutexAutoLock	lock(m_mtxDataCallbacks);
		m_vecDataCallbacks.insert(m_vecDataCallbacks.end(), cb);

		//	数据包进出统计
		int commandport = cb->FrameType();
		if(commandport>0)
		{
			if(m_mapDataInput.count(commandport)>0)
				m_mapDataInput[commandport]++;
			else
				m_mapDataInput[commandport] = 1;
		}
	}

	return true;
}


DWORD WINAPI UsageEnvironment::DataCallbackHandler1(void *data)
{
	UsageEnvironment * pThis = (UsageEnvironment *)data;

	pThis->DataCallbackHandler();
	return 0 ;
}


DWORD UsageEnvironment::DataCallbackHandler()
{
	unsigned long tvLastLog = GetTickCount();

	do{
		CCommandCallback * pCmd = NULL;

		int nRemain = 0;
		{
			JMutexAutoLock lock(m_mtxDataCallbacks);
			if(m_vecDataCallbacks.size()>0)
			{
				pCmd = m_vecDataCallbacks[0];
				m_vecDataCallbacks.erase(m_vecDataCallbacks.begin());
			}

			nRemain = m_vecDataCallbacks.size();

			//	数据包进出统计
			if(pCmd)
			{
				int commandport = pCmd->FrameType();
				if(commandport>0)
				{
					if(m_mapDataOutput.count(commandport)>0)
						m_mapDataOutput[commandport]++;
					else
						m_mapDataOutput[commandport] = 1;
				}
			}
			
		}

		if(1)
		{
			unsigned long tvNow = GetTickCount();

			long  delay = tvNow - tvLastLog; 

			if( delay > 10 * 1000 )
			{
				char tmp[256] = { 0 };
				sprintf_s(tmp, 256, "========== remain %d 个数据包", nRemain);
				LOG4CPLUS_DEBUG(m_Logger, string(tmp));

				tvLastLog = tvNow;

				//	数据包进出统计
				JMutexAutoLock lock(m_mtxDataCallbacks);
				map<int, int>::iterator it = m_mapDataOutput.begin();
				for( ; it!=m_mapDataOutput.end(); it++)
				{
					char tmp[256] = { 0 };
					int nInput = 0;
					if(m_mapDataInput.count(it->first)>0)
						nInput = m_mapDataInput[it->first];
					sprintf_s(tmp, 256, "[%08d], input = %d , output = %d ", it->first,  nInput, it->second );
					LOG4CPLUS_DEBUG(m_Logger, string(tmp));
				}
				m_mapDataOutput.clear();
				m_mapDataInput.clear();
			}
		}

		if(pCmd)
		{
			pCmd->Execute();
			delete pCmd;
		}
		else
		{
			Sleep(10);
		}
	}while(true);
}



int  UsageEnvironment::PostCommand(AbstractCommand * pCmd)
{
	stringstream ss;
	int	nRet;
	if(pCmd->IsSnyc())
	{
		DWORD dwThreadId =  ::GetCurrentThreadId();
		if(dwThreadId == m_ThreadId)
		{
			ss.str("");
			ss <<  pCmd->CommandType() << " , 同步命令，不可以重入调用, 投递 Error!";
			LOG4CPLUS_DEBUG(m_Logger, ss.str());
			return -1;
		}
	}
	
	nRet = pCmd->m_nSerialNumber = fetchSerialNumber();

	char tmp[256] = { 0 };
	sprintf_s(tmp, 256, "[%08d]", nRet); 

	ss.str("");
	ss <<  pCmd->CommandType() << " , 投递成功!";
	ss << ", 返回的 命令句柄 = " << string(tmp) ;

	bool bLog = true;
	{
		JMutexAutoLock	lock(m_mtxCommands);
		if(pCmd->CommandType().find("CDllCataloginfoRequest")!=string::npos)
		{
			m_vecCommandsCataloginfo.insert(m_vecCommandsCataloginfo.end(), pCmd);
			ss <<  ", Cataloginfo 共有任务 " << m_vecCommandsCataloginfo.size() << "条"; 
		}
		else if(pCmd->CommandType().find("CDllDeviceinfoRequest")!=string::npos )
		{
			m_vecCommandsDeviceinfo.insert(m_vecCommandsDeviceinfo.end(), pCmd);
			ss <<  ", Deviceinfo 共有任务 " << m_vecCommandsDeviceinfo.size() << "条"; 
		}
		else if(pCmd->CommandType().find("CDllStateinfoRequest")!=string::npos )
		{
			m_vecCommandsStateinfo.insert(m_vecCommandsStateinfo.end(), pCmd);
			ss <<  ", Stateinfo 共有任务 " << m_vecCommandsStateinfo.size() << "条"; 
		}
		else
		{
			m_vecCommands.insert(m_vecCommands.end(), pCmd);
			ss <<  ", Other 共有任务 " << m_vecCommands.size() << "条"; 

			if(pCmd->CommandType().find("CSipdCataloginfoResponse")!=string::npos )
			{
				bLog = false;
			}				
						
		}
		
	}
	if(bLog)
		LOG4CPLUS_DEBUG(m_Logger, ss.str());
	
	return nRet;
}


DWORD WINAPI UsageEnvironment::CommandHandler1(void *data)
{
	UsageEnvironment * pThis = (UsageEnvironment *)data;
	
	pThis->CommandHandler();
	return 0 ;
}


DWORD UsageEnvironment::CommandHandler()
{
	do{
		AbstractCommand * pCmd = NULL;
		int nRemain = 0;
		{
			JMutexAutoLock lock(m_mtxCommands);

			if(m_nCataloginfoSession < CATALOG_SESSION_NUM)
			{
				int nNeed = CATALOG_SESSION_NUM - m_nCataloginfoSession;
				for(int i=0; i< nNeed && m_vecCommandsCataloginfo.size()>0; i++)
				{	
					pCmd = m_vecCommandsCataloginfo[0];
					m_vecCommandsCataloginfo.erase(m_vecCommandsCataloginfo.begin());

					m_vecCommands.insert(m_vecCommands.end(), pCmd);
				}
			}

			if(m_nDeviceinfoSession < DEVICEINFO_SESSION_NUM)
			{
				int nNeed = DEVICEINFO_SESSION_NUM - m_nDeviceinfoSession;
				for(int i=0; i< nNeed && m_vecCommandsDeviceinfo.size()>0; i++)
				{	
					pCmd = m_vecCommandsDeviceinfo[0];
					m_vecCommandsDeviceinfo.erase(m_vecCommandsDeviceinfo.begin());

					m_vecCommands.insert(m_vecCommands.end(), pCmd);
				}
			}

			if(m_nStateinfoSession < STATUSINFO_SESSION_NUM)
			{
				int nNeed = STATUSINFO_SESSION_NUM - m_nStateinfoSession;
				for(int i=0; i< nNeed && m_vecCommandsStateinfo.size()>0; i++)
				{	
					pCmd = m_vecCommandsStateinfo[0];
					m_vecCommandsStateinfo.erase(m_vecCommandsStateinfo.begin());

					m_vecCommands.insert(m_vecCommands.end(), pCmd);
				}
			}


			if(m_vecCommands.size()>0)
			{
				pCmd = m_vecCommands[0];
				m_vecCommands.erase(m_vecCommands.begin());
			}
			nRemain = m_vecCommands.size();
		}

		if(pCmd)
		{
			pCmd->CommandExecute(this);
		}
		
		if(nRemain > 10 )
		{
			char tmp[256] = { 0};
			sprintf_s(tmp, 256, "UsageEnvironment::CommandHandler, 待处理任务 %d 条", nRemain);
			LOG4CPLUS_DEBUG(m_Logger, string(tmp));
		}

		{
			//////////////////////////////////////////////////////////////////////////
			//	处理已经超时的 会话
			
			MonitorSession();

			//////////////////////////////////////////////////////////////////////////
			//	进行流的 接收 
			bool bRecieved = false;


			CDllMediaSession * pSession = NULL;
			map<int , CAbstractObject *>::iterator it = m_mapObjects.begin();
			for(; it!= m_mapObjects.end(); it++)
			{
				if(it->second->ObjectType()=="CDllMediaSession")
				{

					pSession = (CDllMediaSession *)it->second;

					int status = pSession->m_rtp_session.Poll();
										
					pSession->m_rtp_session.BeginDataAccess();

					// check incoming packets
					if (pSession->m_rtp_session.GotoFirstSourceWithData())
					{

						do
						{
							RTPPacket *pack;

							while ((pack = pSession->m_rtp_session.GetNextPacket()) != NULL)
							{
								pSession->PreparePsParser();

								bool hasMarker = pack->HasMarker();
								uint16_t seq = pack->GetSequenceNumber();

								bool bContinue = false;
								if(seq == pSession->m_rtp_seq + 1 ||(seq==0 && pSession->m_rtp_seq==65535) ||
									pSession->m_rtp_seq == -1)
								{
									// rtp 序号连续
									bContinue = true;
								}

								if(pSession->m_hasMarker)
								{
									pSession->m_pPsParser->ResetVisitBuffer();
									if(bContinue)
									{
										if(pSession->m_bPaused==true)
										{
											char tmp[256] = { 0 };
											sprintf_s(tmp, 256, "[%08d], 恢复发送 ",pSession->SerailNumber());
											LOG4CPLUS_DEBUG(m_Logger, string(tmp) );
										}
										pSession->m_bPaused = false;
									}
									else
									{
										pSession->m_bPaused = true;

										char tmp[256] = { 0 };
										sprintf_s(tmp, 256, "[%08d], Marker，暂停发送 ",pSession->SerailNumber());
										LOG4CPLUS_DEBUG(m_Logger, string(tmp) );
									}
								}
								else
								{
									if(bContinue==false)
									{
										pSession->m_bPaused = true;

										char tmp[256] = { 0 };
										sprintf_s(tmp, 256, "[%08d], No Marker，暂停发送 ",pSession->SerailNumber());
										LOG4CPLUS_DEBUG(m_Logger, string(tmp) );
									}
								}
																
								pSession->m_rtp_seq = seq;
								pSession->m_hasMarker = hasMarker;

								bRecieved = true;

								if(pSession->m_bPaused==false)
								{
									uint32_t rtptime = pack->GetTimestamp();
									rtptime = rtptime * 100.0 / 9;
									if (pSession->m_pPsParser)
									{

										unsigned long tvBegin = GetTickCount();
										pSession->m_KeepaliveTime = tvBegin;

										pSession->m_pPsParser->ParsePSData(pack->GetPayloadData(), pack->GetPayloadLength() ,rtptime);

										unsigned long tvNow = GetTickCount();
										long delay = tvNow-tvBegin;

										if(delay > 16)
										{
											char tmp[256] = { 0 };
											sprintf_s(tmp, 256, "[%08d], ParsePSData，用时 %d ms", pSession->SerailNumber(), delay);
											LOG4CPLUS_DEBUG(m_Logger, string(tmp) );
										}
									}
								}
								
								// pSession->SaveToFile(pack->GetPayloadData(), pack->GetPayloadLength());

								// You can examine the data here
								

								// we don't longer need the packet, so
								// we'll delete it
								pSession->m_rtp_session.DeletePacket(pack);
							}
						} while (pSession->m_rtp_session.GotoNextSourceWithData());
					}

					pSession->m_rtp_session.EndDataAccess();
				}
			}


			if(bRecieved== false)
			{
				Sleep(10);
			}
			else
			{
				//LOG4CPLUS_DEBUG(m_Logger, "Poll ,Data Getted");
			}
		}

	}while(1);

}

UsageEnvironment& UsageEnvironment::getInstance()
{
	if(instance==NULL)
		instance = new UsageEnvironment;
	return *instance;
}


void UsageEnvironment::MonitorSession()
{

	//	每隔 500 ms 进行一次超时处理
	unsigned long tvNow = GetTickCount();
	long  delay = tvNow - m_tvMonitorSession; 
	if( delay <  500)
		return;

	vector<CAbstractObject *>	deleteSession;

	CAbstractObject * pSession = NULL;
	map<int , CAbstractObject *>::iterator it = m_mapObjects.begin();
	for(; it!= m_mapObjects.end(); it++)
	{
		pSession = it->second;
		if(pSession->IsTimeout(tvNow))
			deleteSession.insert(deleteSession.end(), pSession);

		if(tvNow - m_tvMonitorSessionLog > 10 * 1000 )
		{
			if(pSession->ObjectType()=="CDllMediaSession")
			{
				char tmp[256] = { 0 };
				sprintf_s(tmp, 256, "[%08d] VideoNum = %d ", 
					pSession->SerailNumber(), ((CDllMediaSession *)pSession)->m_nVideoNum);
				
				LOG4CPLUS_DEBUG(m_Logger, string(tmp));

				((CDllMediaSession *)pSession)->m_nVideoNum = 0;
			}
		}
	}

	for(int k = 0; k< deleteSession.size(); k++)
	{
		deleteSession[k]->ProcessTimeout();

		delete deleteSession[k];
	}


	if(tvNow - m_tvMonitorSessionLog >10 * 1000 )
	{
		stringstream ss;
		ss.str("");
		ss << "=============== MonitorSession ";
		ss << ", nCataloginfo " << m_nCataloginfoSession;
		ss << ", nMedia " << m_nMediaSession;
		ss << ", nDeviceControl " << m_nDeviceControlSession;
		ss << ", nDeviceinfo " << m_nDeviceinfoSession;
		ss << ", nStateinfo " << m_nStateinfoSession;
		ss << ", nSearchRecordfile " << m_nSearchRecordfileSession;

		LOG4CPLUS_DEBUG(m_Logger, ss.str());

		m_tvMonitorSessionLog = tvNow;
	}

	m_tvMonitorSession = tvNow;
}

CAbstractObject * UsageEnvironment::SearchSession(string sessionType, int nId)
{
	CAbstractObject * pSession = NULL;
	JMutexAutoLock lock(m_mtxObjects);
	map<int , CAbstractObject *>::iterator it = m_mapObjects.begin();
	for(; it!=m_mapObjects.end(); it++)
	{
		pSession = it->second;

		if(sessionType.empty()==false)
		{
			//	已知 要查找的目录 session 类型
			if(pSession->ObjectType() == sessionType)
			{
				if(nId<0)
				{
					if(pSession->SerailNumber() == -1 * nId)
						break;
				}
				else
				{
					if(sessionType=="CDllAlarmSubscribeSession")
					{
						if(((CDllAlarmSubscribeSession *)pSession)->m_sid == nId)
							break;
					}

					if(sessionType=="CDllRegisterSession")
					{
						if(((CDllRegisterSession *)pSession)->m_RegisterReq.rid == nId)
							break;
					}
					if(sessionType=="CDllMediaSession")
					{
						if(((CDllMediaSession *)pSession)->m_cid == nId)
							break;
					}
					
				}
			}
		}
		else
		{
			// 未知 session 类型
			if(pSession->ObjectType()=="CDllDeviceControlSession")
			{
				if(((CDllDeviceControlSession *)pSession)->m_tid == nId)
					break;
			}
			if(pSession->ObjectType()=="CDllDeviceinfoSession")
			{
				if(((CDllDeviceinfoSession *)pSession)->m_tid == nId)
					break;
			}
			if(pSession->ObjectType()=="CDllStateinfoSession")
			{
				if(((CDllStateinfoSession *)pSession)->m_tid == nId)
					break;
			}
			if(pSession->ObjectType()=="CDllCataloginfoSession")
			{
				if(((CDllCataloginfoSession *)pSession)->m_tid == nId)
					break;
			}
			if(pSession->ObjectType()=="CDllSearchRecordfileSession")
			{
				if(((CDllSearchRecordfileSession *)pSession)->m_tid == nId)
					break;
			}
		}
		
		
	}

	if(it!=m_mapObjects.end())
	{
		return pSession;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 对应 Superior

int  UsageEnvironment::PostRequest(AbstractCommand * pCmd)
{
	stringstream ss;
	int	nRet;
	
	nRet = pCmd->m_nSerialNumber = fetchSerialNumber();

	char tmp[256] = { 0 };
	sprintf_s(tmp, 256, "[%08d]", nRet); 

	ss.str("");
	ss <<  pCmd->CommandType() << " , 投递成功!";
	ss << ", 返回的 命令句柄 = " << string(tmp) ;



	return nRet;
}


DWORD WINAPI UsageEnvironment::RequestHandler1(void *data)
{
	UsageEnvironment * pThis = (UsageEnvironment *)data;

	pThis->RequestHandler();
	return 0 ;
}

DWORD  UsageEnvironment::RequestHandler()
{
	do{
		AbstractCommand * pCmd = NULL;
		int nRemain = 0;

		if(m_vecCommands.size()>0)
		{
			pCmd = m_vecCommands[0];
			m_vecCommands.erase(m_vecCommands.begin());
		}
		nRemain = m_vecCommands.size();

		if(pCmd)
		{
			pCmd->CommandExecute(this);
		}

		{
			//	处理 
		}

	}while(1);

}