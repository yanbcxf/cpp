#include "StdAfx.h"
#include "CallbackDllRegisterRequest.h"

CCallbackDllRegisterRequest::CCallbackDllRegisterRequest(int commandport, sipd_register_req * pRegisterReq)
{
	m_commandport = commandport;
	if(pRegisterReq)
		m_RegisterReq = *pRegisterReq;
}

CCallbackDllRegisterRequest::~CCallbackDllRegisterRequest(void)
{
}


void CCallbackDllRegisterRequest::Execute()
{
	if(g_Dll_Register_Request_func)
		g_Dll_Register_Request_func(m_commandport,  &m_RegisterReq);

}