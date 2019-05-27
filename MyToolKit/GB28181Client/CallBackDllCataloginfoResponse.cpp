#include "StdAfx.h"
#include "CallbackDllCataloginfoResponse.h"

CCallbackDllCataloginfoResponse::CCallbackDllCataloginfoResponse(int commandport, int type, int result, 
								sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem)
{
	m_commandport = commandport;
	m_type = type;
	m_result = result;
	if(pCataBasicInfo)
	{
		m_CataBasicInfo = *pCataBasicInfo;
		for(int i =0 ; i<  m_CataBasicInfo.DeviceList_Num; i++)
		{
			memcpy(m_pCataItem + i, pCataItem + i, sizeof(sipd_catalog_item));
		}
	}
	
}

CCallbackDllCataloginfoResponse::~CCallbackDllCataloginfoResponse(void)
{
}


void CCallbackDllCataloginfoResponse::Execute()
{
	if(g_Dll_cataloginfo_response_func)
	{
		if(m_type == 0)
			g_Dll_cataloginfo_response_func(m_commandport, m_type , m_result, NULL, NULL);
		else
			g_Dll_cataloginfo_response_func(m_commandport, m_type , m_result, &m_CataBasicInfo, m_pCataItem);
	}
}