#pragma once
#include "cninfoannouncepdf.h"

class CPDFHaveCatalog :
	public CCninfoAnnouncePDF
{
public:
	CPDFHaveCatalog(void);
	~CPDFHaveCatalog(void);

	virtual string SaveAsCsv();
	string SqlScript();

	virtual void ImportToDatabase(string csv);
	virtual list<CCninfoAnnouncePDF *> CreatePDF() ;

public:
	map<int, CPdfCatalog>	mapCatalog;
};
