#include "StdAfx.h"
#include "SipdDeviceinfoResponse.h"

CSipdDeviceinfoResponse::CSipdDeviceinfoResponse(sipd_deviceinfo_rsp* pDeviceInfo, int tid)
:AbstractCommand(false)
{
	m_DeviceInfo = *pDeviceInfo;
	m_tid = tid;
}

CSipdDeviceinfoResponse::~CSipdDeviceinfoResponse(void)
{
}


int CSipdDeviceinfoResponse::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;

	int nSn = atoi(m_DeviceInfo.SN);

	CDllDeviceinfoSession * pSession = (CDllDeviceinfoSession *)pEnviron->SearchSession("CDllDeviceinfoSession",-1 * nSn);
	
	if(pSession)
	{
		/*if(g_Dll_deviceinfo_response_func)
			g_Dll_deviceinfo_response_func(pSession->SerailNumber(), 1 , 0, &m_DeviceInfo);*/

		nResult = pSession->SerailNumber();

		CCallbackDllDeviceinfoResponse * pcb = new CCallbackDllDeviceinfoResponse(pSession->SerailNumber(), 1 , 0, &m_DeviceInfo);
		pSession->PostCallBack(pcb, false, 1, 1);
	}
	else
	{
		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "CSipdDeviceinfoResponse, CDllDeviceinfoSession SN = %d Î´ÕÒµ½ ", nSn );
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));
	}

	Sip_Message_Answer(m_tid, true);
	return nResult;
}