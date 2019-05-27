#include "StdAfx.h"
#include "DllPtzAutoScan.h"

CDllPtzAutoScan::CDllPtzAutoScan(const sipd_autoscan_control * pAutoScanControl)
:AbstractCommand(false)
{
	// m_AutoScanControl = *pAutoScanControl;
	memcpy(&m_AutoScanControl, pAutoScanControl, sizeof(sipd_autoscan_control));
}

CDllPtzAutoScan::~CDllPtzAutoScan(void)
{
}


int CDllPtzAutoScan::Execute(UsageEnvironment* pEnviron)
{
	
	CDllDeviceControlSession * pSession = new CDllDeviceControlSession(*pEnviron, m_nSerialNumber);
	memset(m_AutoScanControl.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_AutoScanControl.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_PTZ_AutoScan(&m_AutoScanControl);
	if(tid>=0) 
	{
		pSession->m_tid = tid;
		return pSession->SerailNumber();
	}
	else
	{
		CCallbackDllPtzResponse * pcb = new  CCallbackDllPtzResponse(pSession->SerailNumber(), -1);
		pEnviron->PostCallBack(pcb);

		delete pSession;
		return tid;
	}
}
