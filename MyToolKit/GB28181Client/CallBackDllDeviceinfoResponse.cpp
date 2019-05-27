#include "StdAfx.h"
#include "CallbackDllDeviceinfoResponse.h"

CCallbackDllDeviceinfoResponse::CCallbackDllDeviceinfoResponse(int commandport, int type, int result, 
								sipd_deviceinfo_rsp* pDeviceInfo)
{
	m_commandport = commandport;
	m_type = type;
	m_result = result;
	if(pDeviceInfo)
		m_DeviceInfo = *pDeviceInfo;
}

CCallbackDllDeviceinfoResponse::~CCallbackDllDeviceinfoResponse(void)
{
}


void CCallbackDllDeviceinfoResponse::Execute()
{
	if(g_Dll_deviceinfo_response_func)
	{
		if(m_type == 0)
			g_Dll_deviceinfo_response_func(m_commandport, m_type , m_result, NULL);
		else
			g_Dll_deviceinfo_response_func(m_commandport, m_type, m_result, &m_DeviceInfo);
	}
}