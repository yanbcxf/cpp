#include "StdAfx.h"
#include "DllSearchRecordfileRequest.h"
#include "DllSearchRecordfileSession.h"

CDllSearchRecordfileRequest::CDllSearchRecordfileRequest(sipd_recordinfo_req * pRecordFileReq)
:AbstractCommand(false)
{
	// m_RecordFileReq = *pRecordFileReq;
	memcpy(&m_RecordFileReq, pRecordFileReq, sizeof(sipd_recordinfo_req));
}

CDllSearchRecordfileRequest::~CDllSearchRecordfileRequest(void)
{
}


int CDllSearchRecordfileRequest::Execute(UsageEnvironment* pEnviron)
{
	CDllSearchRecordfileSession * pSession = new CDllSearchRecordfileSession(*pEnviron, m_nSerialNumber);
	memset(m_RecordFileReq.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_RecordFileReq.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_search_recordfile_request(&m_RecordFileReq);
	if(tid>=0) 
	{
		pSession->m_tid = tid;
		return pSession->SerailNumber();
	}
	else
	{
		CCallbackDllSearchRecordfileResponse *pcb = new CCallbackDllSearchRecordfileResponse(pSession->SerailNumber(), 0 , -1, NULL, NULL);
		pEnviron->PostCallBack(pcb);

		delete pSession;
		return tid;
	}


}
