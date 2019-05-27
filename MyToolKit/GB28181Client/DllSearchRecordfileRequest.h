#pragma once

class CDllSearchRecordfileRequest :
	public AbstractCommand
{
public:
	CDllSearchRecordfileRequest(sipd_recordinfo_req * pRecordFileReq);
	~CDllSearchRecordfileRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllSearchRecordfileRequest"; }

protected:
	sipd_recordinfo_req m_RecordFileReq;

};
