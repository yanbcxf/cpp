#include "StdAfx.h"
#include "CallbackAlarmNotify.h"

CCallbackAlarmNotify::CCallbackAlarmNotify(sipd_alarm_notify * pAlarmNotify)
{
	if(pAlarmNotify)
		m_AlarmNotify = *pAlarmNotify;
}

CCallbackAlarmNotify::~CCallbackAlarmNotify(void)
{
}


void CCallbackAlarmNotify::Execute()
{
	if(g_Dll_Alarm_Request_func)
		g_Dll_Alarm_Request_func(&m_AlarmNotify);

}