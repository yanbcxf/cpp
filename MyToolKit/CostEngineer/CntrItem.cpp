
// CntrItem.cpp: CCostEngineerCntrItem 类的实现
//

#include "stdafx.h"
#include "CostEngineer.h"

#include "CostEngineerDoc.h"
#include "CostEngineerView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCostEngineerCntrItem 实现

IMPLEMENT_SERIAL(CCostEngineerCntrItem, COleClientItem, 0)

CCostEngineerCntrItem::CCostEngineerCntrItem(CCostEngineerDoc* pContainer)
	: COleClientItem(pContainer)
{
	// TODO: 在此添加一次性构造代码
}

CCostEngineerCntrItem::~CCostEngineerCntrItem()
{
	// TODO: 在此处添加清理代码
}

void CCostEngineerCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// 在编辑某项时(就地编辑或完全打开进行编辑)，
	//  该项将就其自身状态的更改
	//  或其内容的可视外观状态的更改发送 OnChange 通知。

	// TODO: 通过调用 UpdateAllViews 使该项无效
	//  (包括适合您的应用程序的提示)

	GetDocument()->UpdateAllViews(nullptr);
		// 现在只更新所有视图/无提示
}

BOOL CCostEngineerCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// 就地激活期间，CCostEngineerCntrItem::OnChangeItemPosition
	//  以更改就地窗口的位置。
	//  这通常是由于服务器
	//  文档中的数据更改而导致范围改变或是
	//  就地调整大小所引起的。
	//
	// 此处的默认设置调用基类，该基类将调用
	//  COleClientItem::SetItemRects 以移动项
	//  移动到新的位置。

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: 更新该项的矩形/范围可能存在的任何缓存

	return TRUE;
}

BOOL CCostEngineerCntrItem::OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow)
{
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, pFrameWnd);

	if (pMainFrame != nullptr)
	{
		ASSERT_VALID(pMainFrame);
		return pMainFrame->OnShowPanes(bShow);
	}

	return FALSE;
}

void CCostEngineerCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// 就地激活期间，服务器将调用 CCostEngineerCntrItem::OnGetItemPosition
	//  以决定该项的位置。  通常，
	//  该矩形会反映与激活所用视图 
	//  相关项的当前位置。  调用 
	//  CCostEngineerCntrItem::GetActiveView。

	// TODO: 返回 rPosition 中的正确矩形(以像素表示)

	CSize size;
	rPosition.SetRectEmpty();
	if (GetExtent(&size, m_nDrawAspect))
	{
		CCostEngineerView* pView = GetActiveView();
		ASSERT_VALID(pView);
		if (!pView)
			return;
		CDC *pDC = pView->GetDC();
		ASSERT(pDC);
		if (!pDC)
			return;
		pDC->HIMETRICtoLP(&size);
		rPosition.SetRect(10, 10, size.cx + 10, size.cy + 10);
	}
	else
		rPosition.SetRect(10, 10, 210, 210);
}

void CCostEngineerCntrItem::OnActivate()
{
	// 每个框架只能有一个就地激活项
	CCostEngineerView* pView = GetActiveView();
	ASSERT_VALID(pView);
	if (!pView)
		return;
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
	if (pItem != nullptr && pItem != this)
		pItem->Close();
	
	COleClientItem::OnActivate();
}

void CCostEngineerCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	DWORD dwMisc = 0;
	m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
	if (dwMisc & OLEMISC_INSIDEOUT)
		DoVerb(OLEIVERB_HIDE, nullptr);
}

void CCostEngineerCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// 首先调用基类以读取 COleClientItem 数据。
	// 因为这样会设置从 [!output CONTAINER_ITEM_CLASS]::GetDocument 返回的 m_pDocument 指针，
	//  CCostEngineerCntrItem::GetDocument，最好调用
	//  基类 Serialize。
	COleClientItem::Serialize(ar);

	// 现在存储/检索特定于 CCostEngineerCntrItem 的数据
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CCostEngineerCntrItem 诊断

#ifdef _DEBUG
void CCostEngineerCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CCostEngineerCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

