#include "StdAfx.h"
#include "SipdSearchRecordfileResponse.h"

CSipdSearchRecordfileResponse::CSipdSearchRecordfileResponse(sipd_recordinfo_rsp_basic_info* pStateBasicInfo, sipd_file_item* pFileItem, int tid)
:AbstractCommand(false)
{
	m_tid = tid;
	m_StateBasicInfo = *pStateBasicInfo;

	m_pFileItem = new sipd_file_item[m_StateBasicInfo.SumNum];
	if(m_pFileItem)
	{
		memcpy(m_pFileItem, pFileItem, sizeof(sipd_file_item) * m_StateBasicInfo.SumNum);
	}
}

CSipdSearchRecordfileResponse::~CSipdSearchRecordfileResponse(void)
{
	if(m_pFileItem)
		delete[] m_pFileItem;
}


int CSipdSearchRecordfileResponse::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;

	int nSn = atoi(m_StateBasicInfo.SN);

	CDllSearchRecordfileSession * pSession = (CDllSearchRecordfileSession *)pEnviron->SearchSession("CDllSearchRecordfileSession", -1 * nSn);
	
	if(pSession)
	{
		/*if(g_Dll_search_recordfile_response_func)
			g_Dll_search_recordfile_response_func(pSession->SerailNumber(), 1 , 0, &m_StateBasicInfo, m_pFileItem);*/

		nResult = pSession->SerailNumber();
		pSession->m_KeepaliveTime = GetTickCount();

		CCallbackDllSearchRecordfileResponse *pcb = new CCallbackDllSearchRecordfileResponse(pSession->SerailNumber(), 1 , 0, &m_StateBasicInfo, m_pFileItem);
		pSession->PostCallBack(pcb, false,  m_StateBasicInfo.SumNum, m_StateBasicInfo.RecordListNum);
	}

	Sip_Message_Answer(m_tid, true);
	return nResult;
}