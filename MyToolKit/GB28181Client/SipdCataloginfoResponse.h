#pragma once
#include "abstractcommand.h"

class CSipdCataloginfoResponse :
	public AbstractCommand
{
public:
	CSipdCataloginfoResponse(sipd_catalog_rsp_basic_info* pCataBasicInfo, sipd_catalog_item* pCataItem, int tid);
	virtual ~CSipdCataloginfoResponse(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdCataloginfoResponse"; }

protected:
	sipd_catalog_rsp_basic_info		m_CataBasicInfo;
	sipd_catalog_item	*			m_pCataItem;
	int								m_tid;

};
