
// LotteryView.cpp: CLotteryView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Lottery.h"
#endif

#include "LotteryDoc.h"
#include "LotteryView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLotteryView

IMPLEMENT_DYNCREATE(CLotteryView, CView)

BEGIN_MESSAGE_MAP(CLotteryView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CLotteryView 构造/析构

CLotteryView::CLotteryView()
{
	// TODO: 在此处添加构造代码

}

CLotteryView::~CLotteryView()
{
}

BOOL CLotteryView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CLotteryView 绘图

void CLotteryView::OnDraw(CDC* /*pDC*/)
{
	CLotteryDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CLotteryView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLotteryView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLotteryView 诊断

#ifdef _DEBUG
void CLotteryView::AssertValid() const
{
	CView::AssertValid();
}

void CLotteryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLotteryDoc* CLotteryView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLotteryDoc)));
	return (CLotteryDoc*)m_pDocument;
}
#endif //_DEBUG


// CLotteryView 消息处理程序
