#include "StdAfx.h"
#include "CallbackDllPtzResponse.h"

CCallbackDllPtzResponse::CCallbackDllPtzResponse(int commandport, int result)
{
	m_commandport = commandport;
	m_result = result;
}

CCallbackDllPtzResponse::~CCallbackDllPtzResponse(void)
{
}


void CCallbackDllPtzResponse::Execute()
{
	if(g_Dll_ptz_response_func)
		g_Dll_ptz_response_func(m_commandport, m_result);

}