#include "StdAfx.h"
#include "SipdAlarmNotify.h"

CSipdAlarmNotify::CSipdAlarmNotify(sipd_alarm_notify * pAlarmNotify)
:AbstractCommand(false)
{
	m_AlarmNotify = *pAlarmNotify;
}

CSipdAlarmNotify::~CSipdAlarmNotify(void)
{
}


int CSipdAlarmNotify::Execute(UsageEnvironment* pEnviron)
{
	int nResult = 0;

	/*if(g_Dll_Alarm_Request_func)
		g_Dll_Alarm_Request_func(&m_AlarmNotify);*/

	CCallbackAlarmNotify * pcb = new CCallbackAlarmNotify(&m_AlarmNotify);
	pEnviron->PostCallBack(pcb);

	return nResult;
}