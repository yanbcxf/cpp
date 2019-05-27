#include "StdAfx.h"
#include "SipdCataloginfoSession.h"

CSipdCataloginfoSession::CSipdCataloginfoSession(UsageEnvironment &	environ, int nSerialNumber , const sipd_catalog_req * pCatalogReq)
:CAbstractObject(environ, nSerialNumber)
{
	m_CatalogReq = * pCatalogReq;
}

CSipdCataloginfoSession::~CSipdCataloginfoSession(void)
{
}
