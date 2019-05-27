#pragma once

class CDllAlarmSubscribeRequest :
	public AbstractCommand
{
public:
	CDllAlarmSubscribeRequest(const sipd_alarm_req * pRecordFileReq);
	~CDllAlarmSubscribeRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() ;

protected:
	sipd_alarm_req m_AlarmReq;

};
