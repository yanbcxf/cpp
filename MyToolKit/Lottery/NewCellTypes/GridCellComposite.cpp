#include "StdAfx.h"
#include "GridCellComposite.h"
#include "GridObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// CGridCellComposite 
//

IMPLEMENT_DYNCREATE(CGridCellComposite, CGridCell)

CGridCellComposite::CGridCellComposite()
{
	
}

CGridCellComposite::~CGridCellComposite()
{
	if(GetGridObj() != NULL)
	{
		GetGridObj()->DeleteAllItems();
		
		if(m_cellInfo.m_bGridAutoRelease)
		{
			delete GetGridObj(); 
		}
	}
}

void CGridCellComposite::SetText(LPCTSTR szText)
{
	// Grid can be NULL when SetText is called from the operator= overload method
	if(GetGridObj() != NULL)
	{
		CCellID cellID = GetGridObj()->GetFocusCell();
		CGridCellBase* pFocusCell = GetGridObj()->GetCell(cellID.row, cellID.col);
		if(pFocusCell != NULL)
		{
			pFocusCell->SetText(szText);		
		}
	}

	CGridCell::SetText(szText);
}

// Create a control to do the editing
BOOL CGridCellComposite::Edit(int nItem, int nSubItem, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	BOOL bRet = FALSE;
	m_bEditing = TRUE;
	
	ASSERT(GetGridObj());
	if(GetGridObj() != NULL)
	{
		bRet = GetGridObj()->Edit(nItem, nSubItem, rect, point, nID, nChar);
	}
	
	return bRet;
}

void CGridCellComposite::OnEndEdit()
{
	ASSERT(GetGridObj());

	if(GetGridObj() != NULL)
	{
		CCellID cell(GetGridObj()->GetFocusCell());
		GetGridObj()->OnEndEdit(cell.row, cell.col);		
	}

    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}

BOOL CGridCellComposite::ValidateEdit(LPCTSTR str)
{
	BOOL bRet = FALSE;

	CCellID cellID = GetGridObj()->GetFocusCell();
	CGridCellBase* pFocusCell = GetGridObj()->GetCell(cellID.row, cellID.col);
	if(pFocusCell != NULL)
	{
		if(pFocusCell->ValidateEdit(str))
		{
			bRet = TRUE;
		}
	}	

	return bRet;
}

CWnd* CGridCellComposite::GetEditWnd() const
{
	return NULL;
}

CSize CGridCellComposite::GetCellExtent(CDC* pDC)
{
	CSize size(0, 0);
	if(GetGridObj() != NULL)
	{
		BOOL bAutoSizing = ((GetState() & GVIS_AUTOSIZING) == GVIS_AUTOSIZING);
		size = GetGridObj()->GetGridExtent(bAutoSizing, pDC);
	}
	
	return size;
}

void CGridCellComposite::EndEdit()
{
	if(GetGridObj() != NULL)
	{
		GetGridObj()->EndEdit();
	}
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellComposite::Draw(CDC* pDC, int nItem, int nSubItem, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
#ifdef _WIN32_WCE
	return CGridCell::Draw(pDC, nItem, nSubItem, rect, bEraseBkgnd);
#else	
	
	ASSERT(GetGridObj());
	if(GetGridObj() != NULL)
	{
		ASSERT(GetGrid());

		GetGridObj()->SetCoords(nItem , nSubItem);		
		GetGridObj()->SetSelected(IsSelected());
		GetGridObj()->SetFocus(IsFocused());

		GetGridObj()->Draw(pDC, rect);		
	}

	return TRUE;
#endif
}

void CGridCellComposite::SetCellInfo(CCompositeCellInfo& info)
{
	m_cellInfo = info;
}

void CGridCellComposite::GetCellInfo(CCompositeCellInfo& info)
{
	info = m_cellInfo;
}

CGridObj* CGridCellComposite::GetGridObj() const
{
	return m_cellInfo.m_pGrid;
}

void CGridCellComposite::OnClickDown(CRect cellRect, CPoint PointCellRelative)
{
	ASSERT(GetGridObj());

	if(GetGridObj() != NULL)
	{
		CCellID cell = GetGridObj()->GetCellFromPt(PointCellRelative);
		GetGridObj()->SetFocusCell(cell);
		
		GetGridObj()->OnClickDown(cellRect, PointCellRelative);

		ASSERT(GetGrid());
		if(GetGrid() != NULL)
		{			
			((CWnd*)GetGrid())->InvalidateRect(cellRect);
		}
	}	
}

void CGridCellComposite::OnClick(CRect cellRect, CPoint PointCellRelative)
{
	ASSERT(GetGridObj());

	if(GetGridObj() != NULL)
	{
		GetGridObj()->OnClick(cellRect, PointCellRelative);
	}

	ASSERT(GetGrid());
	if(GetGrid() != NULL)
	{			
		((CWnd*)GetGrid())->InvalidateRect(cellRect);
	}
}

void CGridCellComposite::OnDblClick(CRect cellRect, CPoint PointCellRelative)
{
	ASSERT(GetGridObj());

	if(GetGridObj() != NULL)
	{
		GetGridObj()->OnDblClick(cellRect, PointCellRelative);
	}

	ASSERT(GetGrid());
	if(GetGrid() != NULL)
	{			
		((CWnd*)GetGrid())->InvalidateRect(cellRect);
	}
}

BOOL CGridCellComposite::OnSetCursor()
{
	BOOL bRet = FALSE;

	ASSERT(GetGridObj());

	if(GetGridObj() != NULL)
	{
		bRet = GetGridObj()->OnSetCursor();
	}

	return bRet;
}


DWORD CGridCellComposite::GetState() const
{
	ASSERT(GetGridObj());	
	return CGridCell::GetState();	
}

void CGridCellComposite::SetState(DWORD nState)
{
	CGridCell::SetState(nState);
}