#pragma once
#include "CommandCallback.h"

class CCallbackAlarmNotify :
	public CCommandCallback
{
public:
	CCallbackAlarmNotify(sipd_alarm_notify * pAlarmNotify);
	virtual ~CCallbackAlarmNotify(void);

	virtual void Execute();

protected:
	sipd_alarm_notify	m_AlarmNotify;
};
