#include "StdAfx.h"
#include "CallbackDllStateinfoResponse.h"

CCallbackDllStateinfoResponse::CCallbackDllStateinfoResponse(int commandport, int type, int result, 
								sipd_devicestate_rsp_basic_info* pStateBasicInfo, sipd_alarm_status_item* pStateItem)
{
	m_commandport = commandport;
	m_type = type;
	m_result = result;
	if(pStateBasicInfo)
	{
		m_StateBasicInfo = *pStateBasicInfo;
		for(int i =0 ; i<  m_StateBasicInfo.AlaramDeviceNum; i++)
		{
			memcpy(m_pStateItem + i, pStateItem + i, sizeof(sipd_alarm_status_item));
		}
	}
	
}

CCallbackDllStateinfoResponse::~CCallbackDllStateinfoResponse(void)
{
}


void CCallbackDllStateinfoResponse::Execute()
{
	if(g_Dll_stateinfo_response_func)
	{
		if(m_type == 0)
			g_Dll_stateinfo_response_func(m_commandport, m_type , m_result, NULL, NULL);
		else
			g_Dll_stateinfo_response_func(m_commandport, m_type , m_result, &m_StateBasicInfo, m_pStateItem);
	}
}