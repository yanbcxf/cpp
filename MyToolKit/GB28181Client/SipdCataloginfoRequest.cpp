#include "StdAfx.h"
#include "SipdCataloginfoRequest.h"

CSipdCataloginfoRequest::CSipdCataloginfoRequest(const sipd_catalog_req * pCatalogReq)
:AbstractCommand(false)
{
	memcpy(&m_CatalogReq, pCatalogReq, sizeof(sipd_catalog_req));
}

CSipdCataloginfoRequest::~CSipdCataloginfoRequest(void)
{
}


int CSipdCataloginfoRequest::Execute(UsageEnvironment* pEnviron)
{
	int nResult = 0;

	//CSipdCataloginfoSession * pSession = new CSipdCataloginfoSession(*pEnviron, m_nSerialNumber, &m_CatalogReq);

	/*if(g_Dll_cataloginfo_response_func)
	{
		if(m_type == 0)
			g_Dll_cataloginfo_response_func(m_commandport, m_type , m_result, NULL, NULL);
		else
			g_Dll_cataloginfo_response_func(m_commandport, m_type , m_result, &m_CataBasicInfo, m_pCataItem);
	}*/

	return nResult;
}