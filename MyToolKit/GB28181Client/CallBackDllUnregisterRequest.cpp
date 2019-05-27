#include "StdAfx.h"
#include "CallbackDllUnregisterRequest.h"

CCallbackDllUnregisterRequest::CCallbackDllUnregisterRequest(int commandport)
{
	m_commandport = commandport;
	
}

CCallbackDllUnregisterRequest::~CCallbackDllUnregisterRequest(void)
{
}


void CCallbackDllUnregisterRequest::Execute()
{
	if(g_Dll_Unregister_Request_func)
		g_Dll_Unregister_Request_func(m_commandport);

}