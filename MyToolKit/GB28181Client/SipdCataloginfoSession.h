#pragma once
#include "abstractobject.h"

class CSipdCataloginfoSession :
	public CAbstractObject
{
public:
	CSipdCataloginfoSession(UsageEnvironment &	environ, int nSerialNumber , const sipd_catalog_req * pCatalogReq);
	~CSipdCataloginfoSession(void);

	string ObjectType() {  return "CSipdCataloginfoSession"; }

public :
	sipd_catalog_req m_CatalogReq;
};
