#include "StdAfx.h"
#include "DllRecordControl.h"

CDllRecordControl::CDllRecordControl(const sipd_record_control * pRecordControl)
:AbstractCommand(false)
{
	m_RecordControl = *pRecordControl;
}

CDllRecordControl::~CDllRecordControl(void)
{
}


int CDllRecordControl::Execute(UsageEnvironment* pEnviron)
{

	CDllDeviceControlSession * pSession = new CDllDeviceControlSession(*pEnviron, m_nSerialNumber);
	memset(m_RecordControl.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_RecordControl.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_record_control(&m_RecordControl);
	
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
