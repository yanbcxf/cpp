#include "StdAfx.h"
#include "CallbackDllCallResponse.h"

CCallbackDllCallResponse::CCallbackDllCallResponse(int commandport, int type,  int result)
{
	m_commandport = commandport;
	m_type = type;
	m_result = result;
}

CCallbackDllCallResponse::~CCallbackDllCallResponse(void)
{
}


void CCallbackDllCallResponse::Execute()
{
	if(g_Dll_Call_Response_func)
		g_Dll_Call_Response_func(m_commandport, m_type, m_result, NULL, NULL);

}