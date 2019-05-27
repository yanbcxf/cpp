#include "StdAfx.h"
#include "SipdCataloginfoResponse.h"

CSipdCataloginfoResponse::CSipdCataloginfoResponse(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem, int tid)
:AbstractCommand(false)
{
	m_tid = tid;
	m_CataBasicInfo = *pCataBasicInfo;

	if(m_CataBasicInfo.DeviceList_Num > 0)
	{
		m_pCataItem = new sipd_catalog_item[m_CataBasicInfo.DeviceList_Num];
		if(m_pCataItem)
		{
			memcpy(m_pCataItem, pCataItem, sizeof(sipd_catalog_item) * m_CataBasicInfo.DeviceList_Num);
		}
	}
	else
	{
		m_pCataItem = NULL;
	}
}

CSipdCataloginfoResponse::~CSipdCataloginfoResponse(void)
{
	if(m_pCataItem)
		delete[] m_pCataItem;
}


int CSipdCataloginfoResponse::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;
	stringstream ss;

	int nSn = atoi(m_CataBasicInfo.SN);

	CDllCataloginfoSession * pSession = (CDllCataloginfoSession *)pEnviron->SearchSession("CDllCataloginfoSession", -1 * nSn);
	
	if(pSession)
	{
		/*if(g_Dll_cataloginfo_response_func)
			g_Dll_cataloginfo_response_func(pSession->SerailNumber(), 1 , 0, &m_CataBasicInfo, m_pCataItem);*/

		nResult = pSession->SerailNumber();
		pSession->m_KeepaliveTime = GetTickCount();

		CCallbackDllCataloginfoResponse * pcb = new CCallbackDllCataloginfoResponse(pSession->SerailNumber(), 1 , 0, &m_CataBasicInfo, m_pCataItem);
		pSession->PostCallBack(pcb, false, m_CataBasicInfo.SumNum,  m_CataBasicInfo.DeviceList_Num);
	}
	else
	{
		char tmp[256] = {0 };
		sprintf_s(tmp, 256, "CSipdCataloginfoResponse, CDllCataloginfoSession SN = %d Œ¥’“µΩ", nSn);
	}

	Sip_Message_Answer(m_tid, true);

	return nResult;
}