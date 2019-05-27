#include "StdAfx.h"
#include "SipdSubscribe200ok.h"
#include "DllAlarmSubscribeSession.h"


CSipdSubscribe200ok::CSipdSubscribe200ok(sipd_subscribe_200_ok_info *pInfo, int sid)
:AbstractCommand(false)
{
	m_sid = sid;
	memcpy(&m_subscribe_200_ok, pInfo, sizeof(sipd_subscribe_200_ok_info));
}

CSipdSubscribe200ok::~CSipdSubscribe200ok(void)
{
}


int CSipdSubscribe200ok::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;
	CAbstractObject * pSession = pEnviron->SearchSession("CDllAlarmSubscribeSession", m_sid);
	
	if(pSession)
	{

		nResult = pSession->SerailNumber();
		pSession->m_KeepaliveTime = GetTickCount();

		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "CSipdSubscribe200ok, %s[%08d], %s", pSession->ObjectType().c_str(), pSession->SerailNumber(), \
			m_subscribe_200_ok.Result == SIPD_RESULT_OK ? "SIPD_RESULT_OK": "SIPD_RESULT_ERROR" );
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));

		if(m_subscribe_200_ok.Result == SIPD_RESULT_OK)
		{
			CCallbackDllAlarmSubscribeResponse * pcb = new CCallbackDllAlarmSubscribeResponse(pSession->SerailNumber(), 0);
			pSession->PostCallBack(pcb, true, 0,  0);
		}
		else
		{
			CCallbackDllAlarmSubscribeResponse * pcb = new CCallbackDllAlarmSubscribeResponse(pSession->SerailNumber(), -1);
			pSession->PostCallBack(pcb, true, 0,  0);
		}
	}
	else
	{
		stringstream ss;
		ss << "SipdSubscribe200ok, Î´ÕÒµ½¶ÔÓ¦ session£¬tid = " << m_sid;
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, ss.str());
	}

	return nResult;
}