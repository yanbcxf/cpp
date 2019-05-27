#include "StdAfx.h"
#include "SipdKeepaliveRequest.h"



CSipdKeepaliveRequest::CSipdKeepaliveRequest(int tid, sipd_keepalive_notify * pKeepaliveReq)
:AbstractCommand(false)
{
	m_KeepaliveReq = *pKeepaliveReq;
	m_tid = tid;
}

CSipdKeepaliveRequest::~CSipdKeepaliveRequest(void)
{
}


int CSipdKeepaliveRequest::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;
	CDllRegisterSession * pSession = NULL;
	{
		JMutexAutoLock lock(pEnviron->m_mtxObjects);
		map<int , CAbstractObject *>::iterator it = pEnviron->m_mapObjects.begin();
		for(; it!=pEnviron->m_mapObjects.end(); it++)
		{
			if(it->second->ObjectType()=="CDllRegisterSession")
			{
				pSession = (CDllRegisterSession *)it->second;

				string strUsername = string(pSession->m_RegisterReq.username);
				if(strUsername.find(string(m_KeepaliveReq.device_id))!=string::npos)
					break;
			}
			pSession = NULL;
		}
	}
	
	if(pSession)
	{
		pSession->m_KeepaliveTime = GetTickCount();
		nResult = pSession->SerailNumber();

		Sip_Message_Answer(m_tid, true);
	}
	else
	{
		//	该注册超时，已经被删除
		Sip_Message_Answer(m_tid, false);
	}

	return nResult;
}