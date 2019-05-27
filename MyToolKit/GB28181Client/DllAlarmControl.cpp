#include "StdAfx.h"
#include "DllAlarmControl.h"

CDllAlarmControl::CDllAlarmControl(const sipd_alarm_control * pAlarmControl)
:AbstractCommand(false)
{
	m_AlarmControl = * pAlarmControl;
}

CDllAlarmControl::~CDllAlarmControl(void)
{
}


int CDllAlarmControl::Execute(UsageEnvironment* pEnviron)
{

	CDllDeviceControlSession * pSession = new CDllDeviceControlSession(*pEnviron, m_nSerialNumber);
	memset(m_AlarmControl.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_AlarmControl.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_alarm_control(&m_AlarmControl);
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
