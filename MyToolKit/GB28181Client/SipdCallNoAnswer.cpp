#include "StdAfx.h"
#include "SipdCallNoAnswer.h"
#include "DllMediaSession.h"

CSipdCallNoAnswer::CSipdCallNoAnswer(int cid)
:AbstractCommand(false)
{
	m_cid = cid;
}

CSipdCallNoAnswer::~CSipdCallNoAnswer(void)
{
}



int CSipdCallNoAnswer::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;
	CDllMediaSession * pSession = (CDllMediaSession *)pEnviron->SearchSession("CDllMediaSession", m_cid);
	

	if(pSession)
	{
		//	Dll_Call_Response_func  通知 exe 命令调用失败
		/*if(g_Dll_Call_Response_func)
			g_Dll_Call_Response_func(pSession->SerailNumber(), 0, 0, NULL, 0);*/

		CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(pSession->SerailNumber(), 0, -1);
		pEnviron->PostCallBack(pcb);
		
		nResult = pSession->SerailNumber();
		//	清楚现场
		delete pSession;
	}

	return nResult;

}