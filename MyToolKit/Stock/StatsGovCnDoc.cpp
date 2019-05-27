// StatsGovCnDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "StatsGovCnDoc.h"


// CStatsGovCnDoc

IMPLEMENT_DYNCREATE(CStatsGovCnDoc, CDocument)

CStatsGovCnDoc::CStatsGovCnDoc()
{
}

BOOL CStatsGovCnDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CStatsGovCnDoc::~CStatsGovCnDoc()
{
}


BEGIN_MESSAGE_MAP(CStatsGovCnDoc, CDocument)
END_MESSAGE_MAP()


// CStatsGovCnDoc 诊断

#ifdef _DEBUG
void CStatsGovCnDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CStatsGovCnDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CStatsGovCnDoc 序列化

void CStatsGovCnDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
#endif


// CStatsGovCnDoc 命令
