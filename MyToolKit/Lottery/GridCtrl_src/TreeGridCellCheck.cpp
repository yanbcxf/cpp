//
// TreeGridCellCheck.cpp : implementation file
//

#include "StdAfx.h"
#include "TreeGridCtrl.h"
#include "TreeGridCellCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTreeGridCellCheck, CGridCellCheck)

CTreeGridCellCheck::CTreeGridCellCheck() : CTreeCell(this)
{
	
}

CSize CTreeGridCellCheck::GetCellExtent(CDC* pDC)
{
	CSize size = CGridCellCheck::GetCellExtent(pDC);
	OnGetTreeCellExtent(pDC, size);

	return size;
}

BOOL CTreeGridCellCheck::Draw(CDC* pDC, int nItem, int nSubItem, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
	BOOL bRet = FALSE;
	if(OnDrawingTreeCell(pDC, nItem, nSubItem, rect, bEraseBkgnd))
		bRet = CGridCellCheck::Draw(pDC, nItem, nSubItem, rect, bEraseBkgnd);
	
	return bRet;
}

BOOL CTreeGridCellCheck::Edit(int nItem, int nSubItem, CRect rect, CPoint  point, UINT nID, UINT nChar)
{
	BOOL bRet = FALSE;

	CRect rcCheck = GetCheckPlacement();			
			
	BOOL bClick = nChar == VK_LBUTTON && point.x > -1;
	if(!bClick)
	{
		if(OnEditingTreeCell(nItem, nSubItem, rect, point, nID, nChar))
		{
			rect.left = rcCheck.right + 1;		
			bRet = CGridCellCheck::Edit(nItem, nSubItem, rect, point, nID, nChar);
		}

	}
	else
	{
		if(IsSelected())
		{			
			CPoint pt(point.x + rect.left, point.y + rect.top);
			if(rcCheck.PtInRect(pt))
			{
				SetCheck(!GetCheck());
				bRet = TRUE;
			}
			else
			{
				rect.left = rcCheck.right + 1;
				bRet = CGridCellCheck::Edit(nItem, nSubItem, rect, point, nID, nChar);
			}			
		}
	}
	
	return bRet;
}

void CTreeGridCellCheck::OnClick(CRect cellRect, CPoint PointCellRelative)
{
	if(IsSelected())
	{	
		CRect rcCheck = GetCheckPlacement();
		CPoint pt(PointCellRelative.x + cellRect.left, PointCellRelative.y + cellRect.top);
		if(rcCheck.PtInRect(pt))
		{
			SetCheck(!GetCheck());
		}
	}
}

void CTreeGridCellCheck::OnCopy(const CGridCellBase& cell)
{
	const CTreeCell* pCell = dynamic_cast<const CTreeCell*>(&cell);
	if(pCell)
	{
		this->CopyFromTreeCell(*pCell);
	}	
}