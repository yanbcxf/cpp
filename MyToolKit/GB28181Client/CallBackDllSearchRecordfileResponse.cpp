#include "StdAfx.h"
#include "CallbackDllSearchRecordfileResponse.h"

CCallbackDllSearchRecordfileResponse::CCallbackDllSearchRecordfileResponse(int commandport, int type, int result, 
								sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem)
{
	m_commandport = commandport;
	m_type = type;
	m_result = result;
	if(pStateBasicInfo)
	{
		m_StateBasicInfo = *pStateBasicInfo;
		for(int i =0 ; i<  m_StateBasicInfo.RecordListNum; i++)
		{
			memcpy(m_pFileItem + i, pFileItem + i, sizeof(sipd_file_item));
		}
	}
	
}

CCallbackDllSearchRecordfileResponse::~CCallbackDllSearchRecordfileResponse(void)
{
}


void CCallbackDllSearchRecordfileResponse::Execute()
{
	if(g_Dll_search_recordfile_response_func)
	{
		if(m_type == 0)
			g_Dll_search_recordfile_response_func(m_commandport, m_type , m_result, NULL, NULL);
		else
			g_Dll_search_recordfile_response_func(m_commandport, m_type , m_result, &m_StateBasicInfo, m_pFileItem);
	}
}