#pragma once

class CDllRecordControl :
	public AbstractCommand
{
public:
	CDllRecordControl(const sipd_record_control * pRecordControl);
	~CDllRecordControl(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllRecordControl"; }

protected:
	sipd_record_control m_RecordControl;

};
