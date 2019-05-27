#include "StdAfx.h"
#include "SipdStateinfoResponse.h"

CSipdStateinfoResponse::CSipdStateinfoResponse(sipd_devicestate_rsp_basic_info* pStateBasicInfo, sipd_alarm_status_item* pStateItem, int tid)
:AbstractCommand(false)
{
	m_tid = tid;
	m_StateBasicInfo = *pStateBasicInfo;
	if(m_StateBasicInfo.AlaramDeviceNum >0)
	{
		m_pStateItem = new sipd_alarm_status_item[m_StateBasicInfo.AlaramDeviceNum];
		if(m_pStateItem)
		{
			memcpy(m_pStateItem, pStateItem, sizeof(sipd_alarm_status_item) * m_StateBasicInfo.AlaramDeviceNum);
		}
	}
	else
	{
		m_pStateItem = NULL;
	}
	
}

CSipdStateinfoResponse::~CSipdStateinfoResponse(void)
{
	if(m_pStateItem)
		delete[] m_pStateItem;
}


int CSipdStateinfoResponse::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;

	int nSn = atoi(m_StateBasicInfo.SN);

	CDllStateinfoSession * pSession = (CDllStateinfoSession *)pEnviron->SearchSession("CDllStateinfoSession", -1 * nSn);
	
	if(pSession)
	{
		/*if(g_Dll_stateinfo_response_func)
			g_Dll_stateinfo_response_func(pSession->SerailNumber(), 1 , 0, &m_StateBasicInfo, m_pStateItem);*/

		nResult = pSession->SerailNumber();

		CCallbackDllStateinfoResponse *pcb = new CCallbackDllStateinfoResponse(pSession->SerailNumber(), 1 , 0, &m_StateBasicInfo, m_pStateItem);
		pSession->PostCallBack(pcb, false,  1, 1);
	}
	else
	{
		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "CSipdStateinfoResponse, CDllStateinfoSession SN = %d Î´ÕÒµ½ ", nSn );
		LOG4CPLUS_DEBUG(pEnviron->m_Logger, string(tmp));
	}

	Sip_Message_Answer(m_tid, true);
	return nResult;
}