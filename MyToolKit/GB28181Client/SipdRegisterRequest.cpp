#include "StdAfx.h"
#include "SipdRegisterRequest.h"

CSipdRegisterRequest::CSipdRegisterRequest(sipd_register_req * pRegisterReq)
:AbstractCommand(false)
{
	m_RegisterReq = *pRegisterReq;
}

CSipdRegisterRequest::~CSipdRegisterRequest(void)
{
}


int CSipdRegisterRequest::Execute(UsageEnvironment* pEnviron)
{

	int nResult = -1;
	stringstream ss;

	//	yangbin 此处估计要改为 根据 from_url 进行比对
	CDllRegisterSession * pSession = (CDllRegisterSession *)pEnviron->SearchSession("CDllRegisterSession", m_RegisterReq.rid);
	

	if(pSession)
	{
		pSession->m_RegisterReq = m_RegisterReq;
		nResult =  pSession->SerailNumber();
		if(m_RegisterReq.expires >0)
		{
			//	本次注册为 Heartbeat 
			gettimeofday(&(pSession->m_HeartbeatTime), NULL);

			Sip_Register_Responce(pSession->m_RegisterReq.tid, 1);

			ss << "CDllRegisterSession 【" << pSession->SerailNumber() << "】，Heartbeat " ;
			LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());
		}
		else
		{
			// 本次为 logout , CDllRegisterSession 析构函数中 发出 离线通知
			ss << "CDllRegisterSession 【" << pSession->SerailNumber() << "】，Logout " ;
			LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());

			//	发送离线通知
			/*if(g_Dll_Unregister_Request_func)
				g_Dll_Unregister_Request_func(pSession->SerailNumber());*/

			CCallbackDllUnregisterRequest * pCb = new CCallbackDllUnregisterRequest(pSession->SerailNumber());
			pEnviron->PostCallBack(pCb);

			delete pSession;
		}
	}
	else
	{
		if(m_RegisterReq.expires>0)
		{
			//	新的下级域注册
			pSession = new CDllRegisterSession(*pEnviron, m_nSerialNumber);
			pSession->m_RegisterReq = m_RegisterReq;
			gettimeofday(&(pSession->m_HeartbeatTime), NULL);
			pSession->m_KeepaliveTime = GetTickCount();

			ss << "CDllRegisterSession 【" << pSession->SerailNumber() << "】，Login " ;
			ss << ", username = " << m_RegisterReq.username;
			ss << ", from_uri = " << string(m_RegisterReq.from_uri);
			ss << ", to_uri = " << string(m_RegisterReq.to_uri);
			LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());

			nResult = pSession->SerailNumber();
			/*if(g_Dll_Register_Request_func)
				g_Dll_Register_Request_func(pSession->SerailNumber(),  &m_RegisterReq);*/
			CCallbackDllRegisterRequest * pCb = new CCallbackDllRegisterRequest(pSession->SerailNumber(),  &m_RegisterReq);
			pEnviron->PostCallBack(pCb);
		}
		
	}

	return nResult;
}