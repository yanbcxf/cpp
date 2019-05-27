#include "StdAfx.h"
#include "SipdMessage200ok.h"
#include "DllCataloginfoSession.h"
#include "DllStateinfoSession.h"
#include "DllDeviceinfoSession.h"
#include "DllSearchRecordfileSession.h"
#include "DllDeviceControlSession.h"

CSipdMessage200ok::CSipdMessage200ok(int tid)
:AbstractCommand(false)
{
	m_tid = tid;
}

CSipdMessage200ok::~CSipdMessage200ok(void)
{
}


int CSipdMessage200ok::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;
	CAbstractObject * pSession = pEnviron->SearchSession("", m_tid);
	

	if(pSession)
	{

		nResult = pSession->SerailNumber();
		pSession->m_KeepaliveTime = GetTickCount();

		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "CSipdMessage200ok, %s[%08d]", pSession->ObjectType().c_str(),  pSession->SerailNumber());
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));

		if(pSession->ObjectType() == "CDllDeviceControlSession")
		{
			/*if(g_Dll_ptz_response_func)
				g_Dll_ptz_response_func(pSession->SerailNumber(), 0);*/
			CCallbackDllPtzResponse * pcb = new CCallbackDllPtzResponse(pSession->SerailNumber(), 0);
			pSession->PostCallBack(pcb, true, 0,  0);

			//	清楚现场
			//delete pSession;
		}
		else if(pSession->ObjectType()=="CDllDeviceinfoSession")
		{
			/*if(g_Dll_deviceinfo_response_func)
				g_Dll_deviceinfo_response_func(pSession->SerailNumber(), 0, 0, NULL);*/

			CCallbackDllDeviceinfoResponse * pcb = new CCallbackDllDeviceinfoResponse(pSession->SerailNumber(), 0 , 0, NULL);
			pSession->PostCallBack(pcb, true, 1,  0);
		}
		else if(pSession->ObjectType()=="CDllStateinfoSession")
		{
			/*if(g_Dll_stateinfo_response_func)
				g_Dll_stateinfo_response_func(pSession->SerailNumber(), 0 ,0, NULL, NULL);*/

			CCallbackDllStateinfoResponse *pcb = new CCallbackDllStateinfoResponse(pSession->SerailNumber(), 0 ,0, NULL, NULL);
			pSession->PostCallBack(pcb, true, 1,  0);
		}
		else if(pSession->ObjectType()=="CDllCataloginfoSession")
		{
			/*if(g_Dll_cataloginfo_response_func)
				g_Dll_cataloginfo_response_func(pSession->SerailNumber(), 0 , 0, NULL, NULL);*/

			CCallbackDllCataloginfoResponse * pcb = new CCallbackDllCataloginfoResponse(pSession->SerailNumber(), 0 , 0, NULL, NULL);
			pSession->PostCallBack(pcb, true, 1,  0);
		}
		else if(pSession->ObjectType()=="CDllSearchRecordfileSession")
		{
			/*if(g_Dll_search_recordfile_response_func)
				g_Dll_search_recordfile_response_func(pSession->SerailNumber(), 0 , 0, NULL, NULL);*/

			CCallbackDllSearchRecordfileResponse *pcb = new CCallbackDllSearchRecordfileResponse(pSession->SerailNumber(), 0 , 0, NULL, NULL);
			pSession->PostCallBack(pcb, true, 1,  0);
		}
		
	}
	else
	{
		stringstream ss;
		ss << "SipdMessage200ok, 未找到对应 session，tid = " << m_tid;
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());
	}

	return nResult;
}