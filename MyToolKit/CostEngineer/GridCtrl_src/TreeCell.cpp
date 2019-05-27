//
// TreeGridCell.cpp : implementation file
//

#include "StdAfx.h"

#define TREEGRIDCTRL

#include "TreeGridCtrl.h"
#include "TreeCell.h"
#include "GridCell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HTL_BUTTON_IMG_WIDTH  9
#define HTL_BUTTON_IMG_HEIGHT 9

#define HTL_HORZ_LINE_RIGHT_MARGIN 3

#define HTL_PLUS_SIGN_RADIUS 3

CTreeCell::CTreeCell(CGridCell* pCell)
{
	ASSERT(pCell);
	m_pGridCell = pCell;

	m_nTreeItemLevel = UNINITALIZED_TREEITEM_LEVEL;
	m_hTreeItem = NULL;
	m_pGridCell->SetState(GTVIS_EXPANDED);
	
	m_ptVertLineOrigin.x = -1;
	m_ptVertLineOrigin.y = -1;

	m_nItem = -1;
}

void CTreeCell::OnGetTreeCellExtent(CDC* pDC, CSize& size)
{	
	CRect rectTreeBtn(0, 0, size.cx, size.cy);
	if(MeasureTreeButtonRect(rectTreeBtn))
	{
		size.cx += rectTreeBtn.Width() + GetTreeBtnRightPadding();
		if(rectTreeBtn.Height() > size.cy)
		{
			size.cy = rectTreeBtn.Height(); 
		}
	}		
}

BOOL CTreeCell::OnMeasureTreeButtonRect(CRect& rect, int nTreeItemLevel)
{
	BOOL bRet = FALSE;
	if(nTreeItemLevel > ROOT_TREEITEM_LEVEL)
	{
		int x = nTreeItemLevel + 1;		
		rect.right = rect.left + (GetTreeButtonMarginHorz() + HTL_BUTTON_IMG_WIDTH) * x;
		bRet = TRUE;
	}

	return bRet;
}

BOOL CTreeCell::MeasureTreeButtonRect(CRect& rect)
{
	BOOL bRet = OnMeasureTreeButtonRect(rect, m_nTreeItemLevel);
	return bRet;
}

int CTreeCell::GetTreeButtonMarginHorz()
{
	int cx = 12;
	return cx;
}

int CTreeCell::GetTreeButtonMarginVert()
{
	return 2;
}

int CTreeCell::GetTreeBtnRightPadding()
{
	return 6;
}

BOOL CTreeCell::MeasureTreeButtonImageRect(CRect& rect)
{
	BOOL bRet = FALSE;
	if(m_nTreeItemLevel > ROOT_TREEITEM_LEVEL)
	{
		int nLeft = rect.left;
		int nTop = rect.top;
		int nRight = rect.right;
		int nBottom = rect.bottom;

		rect.left = rect.right - HTL_BUTTON_IMG_WIDTH;
		rect.right = rect.left + HTL_BUTTON_IMG_WIDTH;

		rect.bottom -= GetTreeButtonMarginVert();
		rect.top += GetTreeButtonMarginVert();

		CPoint ptCenter = rect.CenterPoint();
		rect.top = ptCenter.y - (HTL_BUTTON_IMG_HEIGHT/2);
		rect.bottom = rect.top + HTL_BUTTON_IMG_HEIGHT;

		rect.left	= max(nLeft, rect.left);
		rect.top	= max(nTop, rect.top);
		rect.right	= min(nRight, rect.right);
		rect.bottom = min(nBottom, rect.bottom);
		bRet = TRUE;
	}

	return bRet;
}

void CTreeCell::DrawTreeItemButton(CDC* pDC, CRect rcButtonArea)
{
	if(HasButton())
	{
		CRect& rcSign = rcButtonArea;
		MeasureTreeButtonImageRect(rcSign);

		pDC->FillRect(rcSign, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
		pDC->FrameRect(rcSign, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));

		m_ptVertLineOrigin = CPoint(rcSign.CenterPoint().x, rcSign.bottom);

		CPoint ptCenter(rcSign.CenterPoint());
		
		if(rcSign.right <= rcButtonArea.right)
		{
			// minus
			pDC->MoveTo(ptCenter.x - HTL_PLUS_SIGN_RADIUS + 1, ptCenter.y);
			pDC->LineTo(ptCenter.x + HTL_PLUS_SIGN_RADIUS, ptCenter.y);

			if(!IsExpanded())
			{
				// plus
				pDC->MoveTo(ptCenter.x, ptCenter.y - HTL_PLUS_SIGN_RADIUS + 1);
				pDC->LineTo(ptCenter.x, ptCenter.y + HTL_PLUS_SIGN_RADIUS);
			}
		}
	}
}

BOOL CTreeCell::OnDrawingTreeCell(CDC* pDC, int nItem, int nSubItem, CRect& rect,  BOOL bEraseBkgnd /*=TRUE*/)
{	
	BOOL bRet = TRUE;

	CTreeGridCtrl* pGridCtrl = (CTreeGridCtrl*) m_pGridCell->GetGrid();
	ASSERT(pGridCtrl);

	if(nSubItem != pGridCtrl->m_nTreeCol)
	{
		return bRet;
	}

	if(m_nTreeItemLevel <= ROOT_TREEITEM_LEVEL)
	{
		// Do Default Drawing
		bRet = TRUE;		
	}
	else
	{
		BOOL bDraw = FALSE;
		CRect rectTreeBtn = rect;
		if(MeasureTreeButtonRect(rectTreeBtn))
		{
			if( (rectTreeBtn.Width() <= rect.Width()) && (rectTreeBtn.Height() <= rect.Height()) )
			{
				bDraw = TRUE;
			}
		}
		
		CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) m_pGridCell->GetDefaultCell();

		// Set up text and background colours
		COLORREF TextClr, TextBkClr;
		TextClr = (m_pGridCell->GetTextClr() == CLR_DEFAULT)? pDefaultCell->GetTextClr() : m_pGridCell->GetTextClr();
		if (m_pGridCell->GetBackClr() == CLR_DEFAULT)
			TextBkClr = pDefaultCell->GetBackClr();
		else
			TextBkClr = m_pGridCell->GetBackClr();

		if(bDraw)
		{
			CRect rectTreeBtnBkArea(rectTreeBtn);
			rectTreeBtnBkArea.right += GetTreeBtnRightPadding();
			pDC->FillSolidRect(rectTreeBtnBkArea, TextBkClr);

			if(!m_hTreeItem->IsLeaf())
			{
				DrawTreeItemButton(pDC, rectTreeBtn);
			}
			
			rect.left += rectTreeBtn.Width() + GetTreeBtnRightPadding();
			
			bRet = TRUE;						
		}
		else
		{
			pDC->SetTextColor(TextClr);
			pDC->FillSolidRect(rectTreeBtn, TextBkClr);			

			CRect rcSign = rectTreeBtn;
			MeasureTreeButtonImageRect(rcSign);
		
			m_ptVertLineOrigin = CPoint(rcSign.CenterPoint().x, rcSign.bottom);
			
			bRet = FALSE;
		}

		// Draw the tree lines
		CPen pen;
		LOGBRUSH LogBrush;
		LogBrush.lbColor = pGridCtrl->GetHorzTreeLineColor();
		LogBrush.lbStyle = BS_SOLID;
		if(pGridCtrl->GetHorzTreeLineColor() == CLR_DEFAULT)
		{
			pen.CreatePen(PS_COSMETIC | PS_ALTERNATE , 1, &LogBrush, 0, NULL);
		}
		else
		{
			pen.CreatePen(PS_SOLID, 0,  pGridCtrl->GetHorzTreeLineColor());
		}
		CPen* pOldPen = pDC->SelectObject(&pen);

		// Draw the tree Horizontal lines	
		int distance = GetTreeButtonMarginHorz() + HTL_BUTTON_IMG_WIDTH;

		CRect rcBtnImg = rectTreeBtn;
		MeasureTreeButtonImageRect(rcBtnImg);
		CPoint ptCenter = rcBtnImg.CenterPoint();
		ptCenter.x = rcBtnImg.left;

		CPoint ptOrigin = ptCenter;
		int nHorzLineStartX = 0;

		// Draw Horizontal lines
		// m_nTreeItemLevel == 0 implies the top most parents i.e child of m_hRootItem of the supergrid
		// These should not have any leading lines drawn..
		// Therefore horizontal lines should be drawn for TreeItemLevel 1 and above
		if((m_nTreeItemLevel > (ROOT_TREEITEM_LEVEL + 1))
				&& (pGridCtrl->GetHorzTreeLineColor() != CLR_NONE))
		{
			if(m_hTreeItem->IsLeaf())
			{
				ptCenter.x -= distance;
				ptCenter.x += rcBtnImg.Width() / 2;

				nHorzLineStartX = ptCenter.x;

				pDC->MoveTo(ptCenter);

				ptCenter.x = rectTreeBtn.right;
				ptCenter.x += GetTreeBtnRightPadding() - HTL_HORZ_LINE_RIGHT_MARGIN;

				ptCenter.x = min(ptCenter.x, rect.right - 1);
				if(nHorzLineStartX < rect.right)
				{
					pDC->LineTo(ptCenter);
				}
				
			}
			else
			{
				int x = ptCenter.x;
				pDC->MoveTo(ptCenter);
				
				ptCenter.x -= distance;
				ptCenter.x += rcBtnImg.Width() / 2;

				if(ptCenter.x < rect.right && x < rect.right)
				{					
					pDC->LineTo(ptCenter);
				}
				
				nHorzLineStartX = ptCenter.x;

				ptCenter.x = rectTreeBtn.right;

				pDC->MoveTo(ptCenter.x, ptCenter.y);

				ptCenter.x += GetTreeBtnRightPadding() - HTL_HORZ_LINE_RIGHT_MARGIN;

				if(ptCenter.x < rect.right)
				{
					pDC->LineTo(ptCenter);
				}
			}
		}
		else if(!m_hTreeItem->IsLeaf() && HasButton())
		{
			// Draw a very small horizontal line after the tree button
			ptCenter.x = rectTreeBtn.right;	
			pDC->MoveTo(ptCenter);
			ptCenter.x += GetTreeBtnRightPadding() - HTL_HORZ_LINE_RIGHT_MARGIN;
			pDC->LineTo(ptCenter);
		}

		// Draw Vertical lines
		if((m_nTreeItemLevel > (ROOT_TREEITEM_LEVEL + 1)) 
				&& (pGridCtrl->GetVertTreeLineColor() != CLR_NONE))
		{

			CCellID cellID;
			cellID = pGridCtrl->GetTopleftNonFixedCell(FALSE);
			CRect rcVisibleNonFixedCellRect;
			CCellRange cellVisibleNonFixedCellRange;
			cellVisibleNonFixedCellRange = pGridCtrl->GetVisibleNonFixedCellRange(rcVisibleNonFixedCellRect, FALSE);
			
			int nTopLeftItem = cellID.row;
			int nParentRowIndex = pGridCtrl->FindRowIndex(m_hTreeItem->m_hParent);

			// Assuming the first column as the column to show the tree hierarchy
			int nTreeCol = pGridCtrl->m_nTreeCol;
			CTreeCell* pParentCell = dynamic_cast<CTreeCell*>(pGridCtrl->GetCell(nParentRowIndex, nTreeCol));
			int nParentTreeBtnImgY = 0;
			if(pParentCell != NULL)
			{
				if(nParentRowIndex >= nTopLeftItem)
				{
					nParentTreeBtnImgY = pParentCell->m_ptVertLineOrigin.y;
				}
				else if(pParentCell->m_ptVertLineOrigin.y != -1)
				{
					nParentTreeBtnImgY = pParentCell->m_ptVertLineOrigin.y;
				}
			}

			CPen penV;
			LOGBRUSH LogBrushV;
			LogBrushV.lbColor = pGridCtrl->GetVertTreeLineColor();
			LogBrushV.lbStyle = BS_SOLID;
			if(pGridCtrl->GetVertTreeLineColor() == CLR_DEFAULT)
			{
				penV.CreatePen(PS_COSMETIC | PS_ALTERNATE , 1, &LogBrush, 0, NULL);
			}
			else
			{
				penV.CreatePen(PS_SOLID, 0,  pGridCtrl->GetVertTreeLineColor());
			}
			CPen* pOldPenV = pDC->SelectObject(&penV);

			if(nHorzLineStartX < rect.right)
			{
				pDC->MoveTo(nHorzLineStartX, ptOrigin.y);
				pDC->LineTo(nHorzLineStartX, nParentTreeBtnImgY);
			}

			if(pParentCell != NULL)
			{
				pParentCell->m_ptVertLineOrigin.y = ptOrigin.y;

				INT_PTR size = pParentCell->GetTreeItem()->m_arChildNodes.GetSize();
				BOOL bLastVisibleChild = (pParentCell->GetTreeItem()->m_arChildNodes[size - 1] == GetTreeItem());
				bLastVisibleChild |= (cellVisibleNonFixedCellRange.GetMaxRow() <= nItem);
				if(bLastVisibleChild)
				{
					pParentCell->m_ptVertLineOrigin.y = -1;
				}
			}

			pDC->SelectObject(pOldPenV);
		}

		pDC->SelectObject(pOldPen);		
	}
	
	return bRet;
}

BOOL CTreeCell::TreeBtnHitTest(CPoint point, CRect rectCell, BOOL& bClickedOnBtnImgArea)
{
	bClickedOnBtnImgArea = FALSE;

	BOOL bRet = FALSE;
	CRect rectTreeBtn = rectCell;
	if(MeasureTreeButtonRect(rectTreeBtn))
	{
		if(rectTreeBtn.PtInRect(point))
		{
			bRet = TRUE;
		}

		if(MeasureTreeButtonImageRect(rectTreeBtn))
		{
			if(rectTreeBtn.PtInRect(point))
			{
				bClickedOnBtnImgArea = TRUE;
			}
		}
	}

	return bRet;
}

BOOL CTreeCell::OnEditingTreeCell(int nItem, int nSubItem, CRect& rect, CPoint  point, UINT nID, UINT nChar)
{	
	CRect rectTreeBtn = rect;
	if(MeasureTreeButtonRect(rectTreeBtn))
	{
		rect.left += rectTreeBtn.Width() ;
	}	

	return TRUE;
}

GTV_HTREENODE CTreeCell::GetTreeItem()
{
	return m_hTreeItem;
}

int CTreeCell::GetItemID()
{
	return m_nItem;
}

int CTreeCell::GetTreeItemLevel()
{
	return m_nTreeItemLevel;
}

void CTreeCell::SetExpanded(BOOL bExpanded/* = TRUE*/)
{
	ASSERT(m_hTreeItem);
	ASSERT(m_pGridCell);

	UINT nState = m_pGridCell->GetState();
	if(bExpanded)
	{
		nState &= ~GTVIS_COLLAPSED;
		nState |= GTVIS_EXPANDED;

		GTV_SETATTRIB_COLLAPSED(m_hTreeItem, FALSE);
	}
	else
	{
		nState &= ~GTVIS_EXPANDED;
		nState |= GTVIS_COLLAPSED;

		GTV_SETATTRIB_COLLAPSED(m_hTreeItem, TRUE);
	}

	m_pGridCell->SetState(nState);
}

BOOL CTreeCell::IsExpanded()
{
	return ((m_pGridCell->GetState() & GTVIS_EXPANDED) == GTVIS_EXPANDED);
}

BOOL CTreeCell::HasButton()
{
	ASSERT(m_hTreeItem);
	return GTV_GETATTRIB_HASBUTTON(m_hTreeItem);
}

DWORD CTreeCell::GetState() const 
{ 
	ASSERT(m_pGridCell);
	return m_pGridCell->GetState(); 
}

BOOL CTreeCell::IsEnabled() const
{ 
	ASSERT(m_pGridCell);
	return TRUE;
}

UINT CTreeCell::GetMargin() const
{
	ASSERT(m_pGridCell);
	return m_pGridCell->GetMargin();
}

BOOL CTreeCell::CopyFromTreeCell(const CTreeCell& cell)
{
	BOOL bRet = TRUE;

	m_ptVertLineOrigin	= cell.m_ptVertLineOrigin;
	m_hTreeItem			= cell.m_hTreeItem;
	m_nTreeItemLevel	= cell.m_nTreeItemLevel;	
	
	return bRet;
}

#undef TREEGRIDCTRL