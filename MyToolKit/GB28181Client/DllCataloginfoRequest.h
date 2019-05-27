#pragma once

class CDllCataloginfoRequest :
	public AbstractCommand
{
public:
	CDllCataloginfoRequest(const sipd_catalog_req * pCatalogReq);
	~CDllCataloginfoRequest(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType();

protected:
	sipd_catalog_req  m_CatalogReq;

};
