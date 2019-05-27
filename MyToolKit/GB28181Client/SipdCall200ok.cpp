#include "StdAfx.h"
#include "SipdCall200ok.h"
#include "DllMediaSession.h"

CSipdCall200ok::CSipdCall200ok(int cid, int did, int tid,  sipd_media_session* pMediaInfo)
:AbstractCommand(false)
{
	m_MediaInfo = *pMediaInfo;
	m_cid = cid;
	m_did = did;
	m_tid = tid;
}

CSipdCall200ok::~CSipdCall200ok(void)
{
}


int CSipdCall200ok::Execute(UsageEnvironment* pEnviron)
{
	CDllMediaSession * pSession = (CDllMediaSession *)pEnviron->SearchSession("CDllMediaSession", m_cid);
	
	if(pSession)
	{
		pSession->m_response_sdp = m_MediaInfo;
		pSession->m_did = m_did;
		pSession->m_tid = m_tid;
		pSession->m_KeepaliveTime = GetTickCount();

		Sip_Call_Ack(pSession->m_did, pSession->m_tid, NULL);

		//	Dll_Call_Response_func  通知 exe 命令调用成功
		/*if(g_Dll_Call_Response_func)
			g_Dll_Call_Response_func(pSession->SerailNumber(), 0, 1, NULL, 0);*/

		CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(pSession->SerailNumber(), 0, 0);
		pEnviron->PostCallBack(pcb);

		return pSession->SerailNumber();
	}

	return -1;

}