#pragma once
#include "abstractcommand.h"

class CSipdCataloginfoRequest : public AbstractCommand
{
public:
	CSipdCataloginfoRequest(const sipd_catalog_req * pCatalogReq);
	~CSipdCataloginfoRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CSipdCataloginfoRequest"; }

protected:
	sipd_catalog_req  m_CatalogReq;
};
