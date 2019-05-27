#include "StdAfx.h"
#include "SipdMessageFailure.h"

CSipdMessageFailure::CSipdMessageFailure(int tid)
:AbstractCommand(false)
{
	m_tid = tid;
}

CSipdMessageFailure::~CSipdMessageFailure(void)
{
}


int CSipdMessageFailure::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;
	CAbstractObject * pSession = pEnviron->SearchSession("", m_tid);


	if(pSession)
	{

		nResult = pSession->SerailNumber();

		if(pSession->ObjectType() == "CDllDeviceControlSession")
		{
			/*if(g_Dll_ptz_response_func)
				g_Dll_ptz_response_func(pSession->SerailNumber(), -1);*/

			CCallbackDllPtzResponse * pcb = new  CCallbackDllPtzResponse(pSession->SerailNumber(), -1);
			pEnviron->PostCallBack(pcb);
		}

		if(pSession->ObjectType()=="CDllDeviceinfoSession")
		{
			/*if(g_Dll_deviceinfo_response_func)
				g_Dll_deviceinfo_response_func(pSession->SerailNumber(), 0, -1, NULL);*/

			CCallbackDllDeviceinfoResponse * pcb = new CCallbackDllDeviceinfoResponse(pSession->SerailNumber(), 0 , -1, NULL);
			pEnviron->PostCallBack(pcb);
		}

		if(pSession->ObjectType()=="CDllStateinfoSession")
		{
			/*if(g_Dll_stateinfo_response_func)
				g_Dll_stateinfo_response_func(pSession->SerailNumber(), 0 ,-1, NULL, NULL);*/

			CCallbackDllStateinfoResponse *pcb = new CCallbackDllStateinfoResponse(pSession->SerailNumber(), 0 ,-1, NULL, NULL);
			pEnviron->PostCallBack(pcb);
		}

		if(pSession->ObjectType()=="CDllCataloginfoSession")
		{
			/*if(g_Dll_cataloginfo_response_func)
				g_Dll_cataloginfo_response_func(pSession->SerailNumber(), 0 , -1, NULL, NULL);*/

			CCallbackDllCataloginfoResponse * pcb = new CCallbackDllCataloginfoResponse(pSession->SerailNumber(), 0 , -1, NULL, NULL);
			pEnviron->PostCallBack(pcb);
		}

		if(pSession->ObjectType()=="CDllSearchRecordfileSession")
		{
			/*if(g_Dll_search_recordfile_response_func)
				g_Dll_search_recordfile_response_func(pSession->SerailNumber(), 0 , -1, NULL, NULL);*/

			CCallbackDllSearchRecordfileResponse *pcb = new CCallbackDllSearchRecordfileResponse(pSession->SerailNumber(), 0 , -1, NULL, NULL);
			pEnviron->PostCallBack(pcb);
		}

		//	清楚现场
		delete pSession;
	}
	else
	{
		stringstream ss;
		ss << "CSipdMessageFailure, 未找到对应 session，tid = " << m_tid;
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());
	}


	return nResult;
}