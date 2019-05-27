#include "StdAfx.h"
#include "DllDeviceinfoRequest.h"
#include "DllDeviceinfoSession.h"

CDllDeviceinfoRequest::CDllDeviceinfoRequest(const sipd_deviceinfo_req * pDeviceinfoReq)
:AbstractCommand(false)
{
	// m_DeviceinfoReq = *pDeviceinfoReq;
	memcpy(&m_DeviceinfoReq, pDeviceinfoReq, sizeof(sipd_deviceinfo_req));
}

CDllDeviceinfoRequest::~CDllDeviceinfoRequest(void)
{
}



string CDllDeviceinfoRequest::CommandType() 
{ 
	stringstream ss;
	ss << "CDllDeviceinfoRequest";
	ss << "( " << string(m_DeviceinfoReq.device_id) << " )";

	return ss.str(); 
}

int CDllDeviceinfoRequest::Execute(UsageEnvironment* pEnviron)
{
	if(strlen(m_DeviceinfoReq.device_id)>0)
	{
		CDllDeviceinfoSession * pSession = new CDllDeviceinfoSession(*pEnviron, m_nSerialNumber);
		memset(m_DeviceinfoReq.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
		sprintf_s(m_DeviceinfoReq.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

		int tid = Sip_deviceinfo_request(&m_DeviceinfoReq);
		pSession->m_tid = tid;

		if(tid>=0)
			return pSession->SerailNumber();
		else 
		{
			//	ÃüÁî·¢ËÍÖ±½ÓÊ§°Ü
			CCallbackDllDeviceinfoResponse * pcb = new CCallbackDllDeviceinfoResponse(pSession->SerailNumber(), 0 , -1, NULL);
			pEnviron->PostCallBack(pcb);

			delete pSession;
			return tid;
		}
	}
	else
	{
		//	ÃüÁî·¢ËÍÖ±½ÓÊ§°Ü
		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "[%08d], device_id Îª¿Õ", m_nSerialNumber);
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));

		CCallbackDllDeviceinfoResponse * pcb = new CCallbackDllDeviceinfoResponse(m_nSerialNumber, 0 , -1, NULL);
		pEnviron->PostCallBack(pcb);
	}
	
}
