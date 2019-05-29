
#include "stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree() noexcept
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CViewTree::OnNMCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != nullptr);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != nullptr)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}




void CViewTree::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)pNMHDR;

	switch (pNMCD->dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Need to process this case and set pResult to CDRF_NOTIFYITEMDRAW, 
		// otherwise parent will never receive CDDS_ITEMPREPAINT notification. (GGH) 
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;

	case CDDS_ITEMPREPAINT:		//失去焦点后被选项各种颜色
		if (this->IsWindowEnabled() == 1)
		{
			if ((pNMCD->uItemState & (CDIS_FOCUS)) == 0
				&& (pNMCD->uItemState & (CDIS_SELECTED)) == CDIS_SELECTED) // selected
			{
				pCustomDraw->clrTextBk = RGB(0, 128, 220);  //失去焦点后被选项背景颜色
				pCustomDraw->clrText = RGB(255, 255, 255);  //失去焦点后被选项文本颜色
			}
			*pResult = CDRF_NOTIFYPOSTPAINT;
			return;
		}
		else {
			*pResult = CDRF_DODEFAULT;
			return;
		}
	case CDDS_ITEMPOSTPAINT:	//失去焦点后给被选项画一个外框，这个case根据需要可有可无
		if (this->IsWindowEnabled() == 1)
		{
			if ((pNMCD->uItemState & (CDIS_FOCUS)) == 0
				&& (pNMCD->uItemState & (CDIS_SELECTED)) == CDIS_SELECTED) // selected
			{
				CRect   rcText;
				HTREEITEM hItem = (HTREEITEM)pNMCD->dwItemSpec;
				GetItemRect(hItem, &rcText, true);
				CPen penBlue(PS_SOLID, 1, RGB(0, 0, 255));
				CDC* pDC = CDC::FromHandle(pNMCD->hdc);
				CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
				CBrush* pOldBrush = pDC->SelectObject(pBrush);
				CPen* pOldPen = pDC->SelectObject(&penBlue);
				pDC->Rectangle(&rcText);
				pDC->SelectObject(pOldBrush);
				pDC->SelectObject(pOldPen);
			}
			else if ((pNMCD->uItemState & CDIS_FOCUS) == 0)
			{
				CRect   rcText;
				HTREEITEM hItem = (HTREEITEM)pNMCD->dwItemSpec;
				CString strText = GetItemText(hItem);
				string tmp = strText.GetBuffer();
				if (tmp.find("(退)") != string::npos || tmp.find("(暂)") != string::npos)
				{
					GetItemRect(hItem, &rcText, true);
					CPen penBlue(PS_SOLID, 1, RGB(255, 0, 0));		//	对于暂停上市 和 终止上市 使用 红色框
					CDC* pDC = CDC::FromHandle(pNMCD->hdc);
					CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
					CBrush* pOldBrush = pDC->SelectObject(pBrush);
					CPen* pOldPen = pDC->SelectObject(&penBlue);
					pDC->Rectangle(&rcText);
					pDC->SelectObject(pOldBrush);
					pDC->SelectObject(pOldPen);
				}


			}
			*pResult = CDRF_SKIPDEFAULT;
			return;
		}
		else {
			*pResult = CDRF_DODEFAULT;
			return;
		}

	}

	*pResult = 0;
}
