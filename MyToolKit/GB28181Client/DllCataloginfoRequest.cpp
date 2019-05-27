#include "StdAfx.h"
#include "DllCataloginfoRequest.h"
#include "DllCataloginfoSession.h"

CDllCataloginfoRequest::CDllCataloginfoRequest(const sipd_catalog_req * pCatalogReq)
:AbstractCommand(false)
{
	//m_CatalogReq = *pCatalogReq;
	memcpy(&m_CatalogReq, pCatalogReq, sizeof(sipd_catalog_req));
}

CDllCataloginfoRequest::~CDllCataloginfoRequest(void)
{
}

string CDllCataloginfoRequest::CommandType() 
{ 
	stringstream ss;
	ss << "CDllCataloginfoRequest";
	ss << "( " << string(m_CatalogReq.device_id) << " )";

	return ss.str(); 
}


int CDllCataloginfoRequest::Execute(UsageEnvironment* pEnviron)
{
	if(strlen(m_CatalogReq.device_id)>0)
	{
		CDllCataloginfoSession * pSession = new CDllCataloginfoSession(*pEnviron, m_nSerialNumber, string(m_CatalogReq.device_id));
		memset(m_CatalogReq.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
		sprintf_s(m_CatalogReq.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

		int tid = Sip_cataloginfo_request(&m_CatalogReq);
		if(tid>=0)
		{
			pSession->m_tid = tid;
			return pSession->SerailNumber();
		}
		else
		{
			//	发送直接失败
			CCallbackDllCataloginfoResponse * pcb = new CCallbackDllCataloginfoResponse(pSession->SerailNumber(), 0 , -1, NULL, NULL);
			pEnviron->PostCallBack(pcb);

			delete pSession;
			return tid;
		}
	}
	else
	{
		//	命令发送直接失败
		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "[%08d], device_id 为空", m_nSerialNumber);
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));

		CCallbackDllCataloginfoResponse * pcb = new CCallbackDllCataloginfoResponse(m_nSerialNumber, 0 , -1, NULL, NULL);
		pEnviron->PostCallBack(pcb);
		return -1;
	}
	
}
