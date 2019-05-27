#pragma once
#include "CommandCallback.h"

class CCallbackDllCataloginfoResponse :
	public CCommandCallback
{
public:
	CCallbackDllCataloginfoResponse(int commandport, int type, int result, 
		sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem);
	virtual ~CCallbackDllCataloginfoResponse(void);

	virtual void Execute();

protected:
	int	m_commandport;
	int	m_type;
	int m_result;
	sipd_catalog_rsp_basic_info  m_CataBasicInfo;
	sipd_catalog_item	m_pCataItem[100];
};
