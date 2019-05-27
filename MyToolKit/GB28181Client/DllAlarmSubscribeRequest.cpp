#include "StdAfx.h"
#include "DllAlarmSubscribeRequest.h"
#include "DllAlarmSubscribeSession.h"

CDllAlarmSubscribeRequest::CDllAlarmSubscribeRequest(const sipd_alarm_req * pAlarmReq)
:AbstractCommand(false)
{
	// m_RecordFileReq = *pRecordFileReq;
	memcpy(&m_AlarmReq, pAlarmReq, sizeof(sipd_alarm_req));
}

CDllAlarmSubscribeRequest::~CDllAlarmSubscribeRequest(void)
{
}

string CDllAlarmSubscribeRequest::CommandType() 
{ 
	stringstream ss;
	ss << "CDllAlarmSubscribeRequest";
	ss << "( " << string(m_AlarmReq.device_id) << " )";

	return ss.str(); 
}


int CDllAlarmSubscribeRequest::Execute(UsageEnvironment* pEnviron)
{

	if(strlen(m_AlarmReq.device_id)>0)
	{
		CDllAlarmSubscribeSession * pSession = new CDllAlarmSubscribeSession(*pEnviron, m_nSerialNumber);
		memset(m_AlarmReq.SN, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
		sprintf_s(m_AlarmReq.SN, MAX_SIPD_DEVICE_ID_LEN, "%d", pSession->SerailNumber() );

		int sid = Sip_Alarm_Subscribe(&m_AlarmReq);
		if(sid>=0) 
		{
			pSession->m_sid = sid;
			return pSession->SerailNumber();
		}
		else
		{
			CCallbackDllAlarmSubscribeResponse *pcb = new CCallbackDllAlarmSubscribeResponse(pSession->SerailNumber(), -1);
			pEnviron->PostCallBack(pcb);

			delete pSession;
			return sid;
		}
	}
	else
	{
		//	命令发送直接失败
		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "[%08d], device_id 为空", m_nSerialNumber);
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));

		CCallbackDllAlarmSubscribeResponse * pcb = new CCallbackDllAlarmSubscribeResponse(m_nSerialNumber,  -1);
		pEnviron->PostCallBack(pcb);
	}
}
