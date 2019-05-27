#include "StdAfx.h"
#include "CallbackDllAlarmSubscribeResponse.h"

CCallbackDllAlarmSubscribeResponse::CCallbackDllAlarmSubscribeResponse(int commandport, int result)
{
	m_commandport = commandport;
	m_result = result;
}

CCallbackDllAlarmSubscribeResponse::~CCallbackDllAlarmSubscribeResponse(void)
{
}


void CCallbackDllAlarmSubscribeResponse::Execute()
{
	if(g_Dll_Subscribe_response_func)
		g_Dll_Subscribe_response_func(m_commandport, m_result);

}