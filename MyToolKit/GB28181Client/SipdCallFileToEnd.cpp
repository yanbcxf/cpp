#include "StdAfx.h"
#include "SipdCallFileToEnd.h"
#include "DllMediaSession.h"

CSipdCallFileToEnd::CSipdCallFileToEnd(int cid)
:AbstractCommand(false)
{
	m_cid = cid;
	
}

CSipdCallFileToEnd::~CSipdCallFileToEnd(void)
{
}


int CSipdCallFileToEnd::Execute(UsageEnvironment* pEnviron)
{
	CDllMediaSession * pSession = (CDllMediaSession *)pEnviron->SearchSession("CDllMediaSession", m_cid);
	
	if(pSession)
	{
		
		//	Dll_Call_Response_func  通知 exe 命令调用成功
		/*if(g_Dll_Call_Response_func)
			g_Dll_Call_Response_func(pSession->SerailNumber(), 2, 0, NULL, 0);*/

		CCallbackDllCallResponse *pcb = new CCallbackDllCallResponse(pSession->SerailNumber(), 2, 1);
		pEnviron->PostCallBack(pcb);

		return pSession->SerailNumber();
	}

	return -1;

}