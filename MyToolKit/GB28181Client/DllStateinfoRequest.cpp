#include "StdAfx.h"
#include "DllStateinfoRequest.h"
#include "DllStateinfoSession.h"

CDllStateinfoRequest::CDllStateinfoRequest(const sipd_devicestate_req * pStateInfoReq)
:AbstractCommand(false)
{
	// m_StateInfoReq = *pStateInfoReq;
	memcpy(&m_StateInfoReq, pStateInfoReq, sizeof(sipd_devicestate_req));
}

CDllStateinfoRequest::~CDllStateinfoRequest(void)
{
}

string CDllStateinfoRequest::CommandType() 
{ 
	stringstream ss;
	ss << "CDllStateinfoRequest";
	ss << "( " << string(m_StateInfoReq.device_id) << " )";

	return ss.str(); 
}


int CDllStateinfoRequest::Execute(UsageEnvironment* pEnviron)
{
	CDllStateinfoSession * pSession = new CDllStateinfoSession(*pEnviron, m_nSerialNumber);
	memset(m_StateInfoReq.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
	sprintf_s(m_StateInfoReq.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

	int tid = Sip_stateinfo_request(&m_StateInfoReq);

	if(tid>=0) 
	{
		pSession->m_tid = tid;
		return pSession->SerailNumber();
	}
	else
	{
		CCallbackDllStateinfoResponse *pcb = new CCallbackDllStateinfoResponse(pSession->SerailNumber(), 0 ,-1, NULL, NULL);
		pEnviron->PostCallBack(pcb);

		delete pSession;
		return tid;
	}
}

