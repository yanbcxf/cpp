#pragma once
#include "CommandCallback.h"

class CCallbackDllSearchRecordfileResponse :
	public CCommandCallback
{
public:
	CCallbackDllSearchRecordfileResponse(int commandport, int type, int result, 
		sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem);
	virtual ~CCallbackDllSearchRecordfileResponse(void);

	virtual void Execute();

protected:
	int	m_commandport;
	int	m_type;
	int m_result;
	sipd_recordinfo_rsp_basic_info  m_StateBasicInfo;
	sipd_file_item	m_pFileItem[100];
};
