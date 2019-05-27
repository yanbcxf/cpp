// BaseHtmlView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "BaseHtmlView.h"


// CBaseHtmlView

IMPLEMENT_DYNCREATE(CBaseHtmlView, CHtmlView)

CBaseHtmlView::CBaseHtmlView()
{

}

CBaseHtmlView::~CBaseHtmlView()
{
}

void CBaseHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaseHtmlView, CHtmlView)
END_MESSAGE_MAP()


// CBaseHtmlView 诊断

#ifdef _DEBUG
void CBaseHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CBaseHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


// CBaseHtmlView 消息处理程序

void CBaseHtmlView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	Navigate2("http://q.stock.sohu.com/cn/bk.shtml");
}
