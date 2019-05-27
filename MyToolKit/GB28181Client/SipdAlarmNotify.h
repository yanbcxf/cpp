#pragma once
#include "abstractcommand.h"

class CSipdAlarmNotify :
	public AbstractCommand
{
public:
	CSipdAlarmNotify(sipd_alarm_notify * pAlarmNotify);
	~CSipdAlarmNotify(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdAlarmNotify"; }

protected:
	sipd_alarm_notify	m_AlarmNotify;
};
