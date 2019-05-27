
// GB28181ClientMDIView.cpp : CGB28181ClientMDIView 类的实现
//

#include "stdafx.h"
#include "GB28181ClientMDI.h"

#include "GB28181ClientMDIDoc.h"
#include "GB28181ClientMDIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGB28181ClientMDIView

IMPLEMENT_DYNCREATE(CGB28181ClientMDIView, CView)

BEGIN_MESSAGE_MAP(CGB28181ClientMDIView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGB28181ClientMDIView::OnFilePrintPreview)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CGB28181ClientMDIView 构造/析构

CGB28181ClientMDIView::CGB28181ClientMDIView()
{
	// TODO: 在此处添加构造代码

}

CGB28181ClientMDIView::~CGB28181ClientMDIView()
{
}

BOOL CGB28181ClientMDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CGB28181ClientMDIView 绘制

void CGB28181ClientMDIView::OnDraw(CDC* /*pDC*/)
{
	CGB28181ClientMDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CGB28181ClientMDIView 打印


void CGB28181ClientMDIView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CGB28181ClientMDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGB28181ClientMDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CGB28181ClientMDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CGB28181ClientMDIView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGB28181ClientMDIView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CGB28181ClientMDIView 诊断

#ifdef _DEBUG
void CGB28181ClientMDIView::AssertValid() const
{
	CView::AssertValid();
}

void CGB28181ClientMDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGB28181ClientMDIDoc* CGB28181ClientMDIView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGB28181ClientMDIDoc)));
	return (CGB28181ClientMDIDoc*)m_pDocument;
}
#endif //_DEBUG


// CGB28181ClientMDIView 消息处理程序

/*
	DROPEFFECT_MOVE：允许对象落在此窗口，落下时要删除原业的对象
	DROPEFFECT_COPY：允许对象落在此窗口，落下时不删除原来的对象
	DROPFFECT_NONE： 不允许对象落在此窗口
*/

DROPEFFECT CGB28181ClientMDIView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	return DROPEFFECT_COPY;
	return CView::OnDragEnter(pDataObject, dwKeyState, point);
}

void CGB28181ClientMDIView::OnDragLeave()
{
	// TODO: 在此添加专用代码和/或调用基类

	CView::OnDragLeave();
}

DROPEFFECT CGB28181ClientMDIView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	return DROPEFFECT_COPY;

	return CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CGB28181ClientMDIView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	HGLOBAL hgMem = NULL; 
	if (pDataObject->IsDataAvailable(CF_TEXT))
	{
		hgMem = pDataObject->GetGlobalData(CF_TEXT);

		sipd_catalog_item * pItem = (sipd_catalog_item *)GlobalLock(hgMem);

		if (pItem != NULL)
		{
			int kkk = 1;
		}
	}
	GlobalUnlock(hgMem);

	return CView::OnDrop(pDataObject, dropEffect, point);
}

int CGB28181ClientMDIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	m_CTreeDropTarget.Register(this);

	return 0;
}
