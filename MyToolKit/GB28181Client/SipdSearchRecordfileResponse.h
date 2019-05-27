#pragma once
#include "abstractcommand.h"

class CSipdSearchRecordfileResponse :
	public AbstractCommand
{
public:
	CSipdSearchRecordfileResponse(sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem, int tid);
	~CSipdSearchRecordfileResponse(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdSearchRecordfileResponse"; }

protected:
	sipd_recordinfo_rsp_basic_info		m_StateBasicInfo;
	sipd_file_item	*			m_pFileItem;
	int							m_tid;
};
