#pragma once


class CDllAlarmControl :
	public AbstractCommand
{
public:
	CDllAlarmControl(const sipd_alarm_control * pAlarmControl);
	~CDllAlarmControl(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllAlarmControl"; }

protected:
	sipd_alarm_control  m_AlarmControl;
};
