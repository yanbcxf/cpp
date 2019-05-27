
// StockDoc.cpp : CStockDoc 类的实现
//

#include "stdafx.h"
#include "Stock.h"

#include "StockDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CStockDoc

IMPLEMENT_DYNCREATE(CStockDoc, CDocument)

BEGIN_MESSAGE_MAP(CStockDoc, CDocument)
	
END_MESSAGE_MAP()


// CStockDoc 构造/析构

CStockDoc::CStockDoc()
{
	// TODO: 在此添加一次性构造代码
	m_nRowNumOfShareholding = 0;
	m_nRowNumOfFinanceReport = 0;
	m_nRowNum = 0;
}

CStockDoc::~CStockDoc()
{
}

BOOL CStockDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CStockDoc 序列化

void CStockDoc::Serialize(CArchive& ar)
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


// CStockDoc 诊断

#ifdef _DEBUG
void CStockDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStockDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CStockDoc 命令
