// AutoDownloadDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "AutoDownloadDoc.h"


// CAutoDownloadDoc

IMPLEMENT_DYNCREATE(CAutoDownloadDoc, CDocument)

CAutoDownloadDoc::CAutoDownloadDoc()
{
}

BOOL CAutoDownloadDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CAutoDownloadDoc::~CAutoDownloadDoc()
{
}


BEGIN_MESSAGE_MAP(CAutoDownloadDoc, CDocument)
END_MESSAGE_MAP()


// CAutoDownloadDoc 诊断

#ifdef _DEBUG
void CAutoDownloadDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoDownloadDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CAutoDownloadDoc 序列化

void CAutoDownloadDoc::Serialize(CArchive& ar)
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


// CAutoDownloadDoc 命令
