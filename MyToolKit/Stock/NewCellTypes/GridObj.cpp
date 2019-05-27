//
// EMCGridCellCombo.cpp : implementation file
//

#include "StdAfx.h"

#include "GridObj.h"

#include "../GridCtrl_src/GridCell.h"
#include "../GridCtrl_src/GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RUNTIME_CLASS_GRID(class_suffix)	RUNTIME_CLASS(CGrid##class_suffix)

// Grid line/scrollbar selection
#define GVL_NONE                0L      // Neither
#define GVL_HORZ                1L      // Horizontal line or scrollbar
#define GVL_VERT                2L      // Vertical line or scrollbar
#define GVL_BOTH                3L      // Both

#define EMC_UNINITIALIZED_COLWIDTH	0

class CGridFriendCell : public CGridCellBase
{
	friend class CGridObj;
};

CGridObj::CGridObj(CGridCtrl& owner) : m_Grid(owner)
{
	m_pRtcDefault = RUNTIME_CLASS_GRID(Cell);
	m_nRows = m_nCols = 0;

	m_nVertScroll = m_nHorzScroll = 0;
	m_idTopLeftCell.row = m_idTopLeftCell.col = 0;

	m_nDefaultRowHeight = GRIDOBJ_DEFAULT_ROWHEIGHT;

	m_nGridLines = GVL_NONE;
	m_clrGridLines = RGB(192,192,192);

	m_rcDraw.SetRectEmpty();
	m_ptGridOrigin.x = -1;
	m_ptGridOrigin.y = -1;

	m_nStretchColIndex = 0;

	m_idCoords.row = -1;
	m_idCoords.col = -1;

	m_bFocus = FALSE;
	m_bSelected = FALSE;
}

CGridObj::~CGridObj()
{
	DeleteAllItems();
}

int  CGridObj::GetRowCount() const                    
{ 
	return m_nRows; 
}

int  CGridObj::GetColumnCount() const
{ 
	return m_nCols; 
}

BOOL CGridObj::SetRowCount(int nRows)
{
    BOOL bResult = TRUE;

    ASSERT(nRows >= 0);
    if (nRows == GetRowCount())
        return bResult;

    int addedRows = nRows - GetRowCount();

    // If we are about to lose rows, then we need to delete the GridCell objects
    // in each column within each row
    if (addedRows < 0)
    {        
        for (int row = nRows; row < m_nRows; row++)
        {
            // Delete cells
            for (int col = 0; col < m_nCols; col++)
                DestroyCell(row, col);
        
            // Delete rows
            GRID_ROW* pRow = m_RowData[row];
            if (pRow)
                delete pRow;
        }        
        m_nRows = nRows;
    }
    
    TRY
    {
        m_arRowHeights.SetSize(nRows);

        // Change the number of rows.
        m_RowData.SetSize(nRows);

        // If we have just added rows, we need to construct new elements for each cell
        // and set the default row height
        if (addedRows > 0)
        {
            // initialize row heights and data
            int startRow = nRows - addedRows;
            for (int row = startRow; row < nRows; row++)
            {
                m_arRowHeights[row] = m_nDefaultRowHeight;

                m_RowData[row] = new GRID_ROW;
                m_RowData[row]->SetSize(m_nCols);
                for (int col = 0; col < m_nCols; col++)
                {
                    GRID_ROW* pRow = m_RowData[row];
                    if (pRow)
                        pRow->SetAt(col, CreateCell(row, col));
                }
                m_nRows++;
            }
        }        
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        bResult = FALSE;
    }
    END_CATCH
    
    return bResult;
}

BOOL CGridObj::SetColumnCount(int nCols)
{
    BOOL bResult = TRUE;

    ASSERT(nCols >= 0);

    if (nCols == GetColumnCount())
        return bResult;        
    
    int addedCols = nCols - GetColumnCount();

    // If we are about to lose columns, then we need to delete the GridCell objects
    // within each column
    if (addedCols < 0)
    {
        for (int row = 0; row < m_nRows; row++)
            for (int col = nCols; col < GetColumnCount(); col++)
                DestroyCell(row, col);
    }

    TRY 
    {
        // Change the number of columns.
        m_arColWidths.SetSize(nCols);
    
        // Change the number of columns in each row.
        for (int i = 0; i < m_nRows; i++)
            if (m_RowData[i])
                m_RowData[i]->SetSize(nCols);
        
        // If we have just added columns, we need to construct new elements for each cell
        // and set the default column width
        if (addedCols > 0)
        {
            int row, col;

            // initialized column widths
            int startCol = nCols - addedCols;
            for (col = startCol; col < nCols; col++)
                m_arColWidths[col] = EMC_UNINITIALIZED_COLWIDTH;
        
            // initialise column data
            for (row = 0; row < m_nRows; row++)
                for (col = startCol; col < nCols; col++)
                {
                    GRID_ROW* pRow = m_RowData[row];
                    if (pRow)
                        pRow->SetAt(col, CreateCell(row, col));
                }            
        }        
    }
    CATCH (CMemoryException, e)
    {
        e->ReportError();
        bResult = FALSE;
    }
    END_CATCH

	m_arColOrder.resize(nCols);  // Reset Column Order
    for (int i = 0; i < nCols; i++)
	{
		m_arColOrder[i] = i;	
	}

    m_nCols = nCols;
    
    return bResult;
}

// Is a given cell designation valid (ie within the bounds of our number
// of columns/rows)?
BOOL CGridObj::IsValid(int nRow, int nCol) const
{
    return (nRow >= 0 && nRow < m_nRows && nCol >= 0 && nCol < m_nCols);
}

int CGridObj::GetRowHeight(int nRow) const
{
    ASSERT(nRow >= 0 && nRow < m_nRows);
    if (nRow < 0 || nRow >= m_nRows)
        return -1;

    int h = m_arRowHeights[nRow];
	if(h == GRIDOBJ_DEFAULT_ROWHEIGHT)
	{
		h = m_rcDraw.Height();
	}

	return h;
}

int CGridObj::GetColumnWidth(int nCol) const
{
    ASSERT(nCol >= 0 && nCol < m_nCols);
    if (nCol < 0 || nCol >= m_nCols)
        return -1;

	if(nCol == m_nStretchColIndex)
	{
		int nTotalWidth = 0;
		for(int i = 0; i < m_nCols; i++)
		{
			nTotalWidth += m_arColWidths[m_arColOrder[i]];
		}

		int nWidth = m_arColWidths[m_arColOrder[nCol]] + m_rcDraw.Width() - nTotalWidth;
		return max(nWidth, 0);
	}

    return m_arColWidths[m_arColOrder[nCol]];
}

BOOL CGridObj::SetRowHeight(int nRow, int height)
{
    ASSERT(nRow >= 0 && nRow < m_nRows && height >= 0);
    if (nRow < 0 || nRow >= m_nRows || height < 0)
        return FALSE;

    m_arRowHeights[nRow] = height;

    return TRUE;
}

BOOL CGridObj::SetColumnWidth(int nCol, int width)
{
    ASSERT(nCol >= 0 && nCol < m_nCols && width >= 0);
    if (nCol < 0 || nCol >= m_nCols || width < 0)
        return FALSE;

    m_arColWidths[m_arColOrder[nCol]] = width;
    
    return TRUE;
}

BOOL CGridObj::SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass)
{
    ASSERT(IsValid(nRow, nCol));
    if (!IsValid(nRow, nCol))
        return FALSE;

    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS_GRID(CellBase)))
    {
        ASSERT( FALSE);
        return FALSE;
    }

    CGridCellBase* pNewCell = (CGridCellBase*) pRuntimeClass->CreateObject();

    CGridCellBase* pCurrCell = GetCell(nRow, nCol);
    if (pCurrCell)
        *pNewCell = *pCurrCell;

    SetCell(nRow, nCol, pNewCell);
    delete pCurrCell;

    return TRUE;
}

CGridCellBase* CGridObj::GetCell(int nRow, int nCol) const
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) 
        return NULL;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) 
		return NULL;
    
	return pRow->GetAt(m_arColOrder[nCol]);
}

BOOL CGridObj::SetCell(int nRow, int nCol, CGridCellBase* pCell)
{    
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) 
        return FALSE;

    GRID_ROW* pRow = m_RowData[nRow];
    if (!pRow) return FALSE;

    pCell->SetCoords( nRow, nCol); 
    pRow->SetAt(nCol, pCell);

    return TRUE;
}

// Removes all rows, columns and data from the grid.
BOOL CGridObj::DeleteAllItems()
{    
    m_arColWidths.RemoveAll();
    m_arRowHeights.RemoveAll();

    // Delete all cells in the grid
    for (int row = 0; row < m_nRows; row++)
    {
        for (int col = 0; col < m_nCols; col++)
            DestroyCell(row, col);

        GRID_ROW* pRow = m_RowData[row];
        delete pRow;
    }

    // Remove all rows
    m_RowData.RemoveAll();

    m_nRows = m_nCols = 0;

    return TRUE;
}

BOOL CGridObj::SetCellText(int nRow, int nCol, LPCTSTR pszText)
{
	CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->SetText(pszText);

    return TRUE;	
}

LPCTSTR CGridObj::GetCellText(int nRow, int nCol)
{
	CGridCellBase* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return NULL;

    return pCell->GetText();
}

// Creates a new grid cell and performs any necessary initialisation
CGridCellBase* CGridObj::CreateCell(int nRow, int nCol)
{    
    if (!m_pRtcDefault || !m_pRtcDefault->IsDerivedFrom(RUNTIME_CLASS_GRID(CellBase)))
    {
        ASSERT( FALSE);
        return NULL;
    }
    CGridCellBase* pCell = (CGridCellBase*) m_pRtcDefault->CreateObject();
    if (!pCell)
        return NULL;

    pCell->SetGrid(&m_Grid);
    pCell->SetCoords(nRow, nCol);
    
    pCell->SetFormat(pCell->GetDefaultCell()->GetFormat());

    return pCell;
}

// Performs any cell cleanup necessary to maintain grid integrity
void CGridObj::DestroyCell(int nRow, int nCol)
{
    delete GetCell(nRow, nCol);
}

void CGridObj::SetGridLines(BOOL bHorzGridLines, BOOL bVertGridLines, COLORREF clrGridLines /*= RGB(192,192,192)*/)
{
	m_nGridLines = GVL_NONE;

	if(bHorzGridLines)
		m_nGridLines |= GVL_HORZ;

	if(bVertGridLines)
		m_nGridLines |= GVL_VERT;

	m_clrGridLines = clrGridLines;
}

void CGridObj::SetDefaultRowHeight(int nHeight)
{
	m_nDefaultRowHeight = nHeight;
}

void CGridObj::SetStretchColIndex(int nCol)
{
	m_nStretchColIndex = nCol;
}

BOOL CGridObj::IsSelected()
{
	return m_bSelected;
}

void CGridObj::SetSelected(BOOL bSelected /*= TRUE*/)
{
	m_bSelected = bSelected;
}

BOOL CGridObj::IsFocused()
{
	return m_bFocus;
}

void CGridObj::SetFocus(BOOL bFocus /*= TRUE*/)
{
	m_bFocus = bFocus;
}

CCellID CGridObj::GetFocusCell()
{
	return m_idCurrentCell;
}

CSize CGridObj::GetGridExtent(BOOL bAutoSizing, CDC* pDC)
{
	int nStretchColIndex = m_nStretchColIndex;
	m_nStretchColIndex = -1;

	CSize size(0, 0);
	CSize sizeCur(0, 0);

	CSize sizeSet(0, 0);

	sizeCur.cy = 0;
	for(int row = 0; row < m_nRows; row++)			
	{
		sizeCur.cx = 0;
		for(int col = 0; col < m_nCols; col++)
		{
			CGridCellBase* pCell = GetCell(row, col);
			if(pCell != NULL)
			{
				CSize sizeExt = pCell->GetCellExtent(pDC);
				
				sizeSet.cx = sizeExt.cx;
				if(row > 0)
				{
					int nWidth = GetColumnWidth(col);
					if(nWidth > sizeSet.cx)
					{
						sizeSet.cx = nWidth;
					}
				}
				if(bAutoSizing)
				{				
					SetColumnWidth(col, sizeSet.cx);				
				}

				sizeCur.cx += sizeSet.cx;
				sizeCur.cy = max(sizeCur.cy, sizeExt.cy);				
			}
		}

		sizeSet.cy = sizeCur.cy;		
		if(bAutoSizing)
		{		
			SetRowHeight(row, sizeSet.cy);
		}		

		size.cx = max(size.cx, sizeCur.cx);
		size.cy += sizeSet.cy;
	}
	
	CCellID cellID = GetFocusCell();
	if(!cellID.IsValid())
	{
		cellID = CCellID(0, 0);
	}
	
	CGridCellBase* pCell = GetCell(cellID.row, cellID.col);
	if(pCell != NULL)
	{
		pCell->GetCellExtent(pDC);
	}	
	
	m_nStretchColIndex = nStretchColIndex;

	return size;
}

BOOL CGridObj::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	BOOL bRet = FALSE;	
			
	CCellID cell = GetFocusCell();

	CRect rcTemp;
	if(IsValid(cell.row, cell.col) && !GetCellRect(cell.row, cell.col, rcTemp))
	{
		cell.row = -1;
		cell.col = -1;
	}

	if(!IsValid(cell.row, cell.col) && (point == CPoint(-1, -1)))
	{
		// Lets try with the first visible row, and column		
		int row = -1, col = -1;
		
		BOOL bBreak = FALSE;
		for(row = 0; row < m_nRows; row++)
		{			
			for(col = 0; col < m_nCols; col++)
			{
				if(GetCellRect(row, col, rcTemp))
				{
					bBreak = TRUE;
					break;
				}
			}

			if(bBreak)
				break;
		}

		cell.row = row;
		cell.col = col;
	}
	
	SetFocusCell(cell);
	if(IsValid(cell.row, cell.col))
	{				
		CGridCellBase* pCell = GetCell(cell.row, cell.col);
		ASSERT(pCell);

		if(pCell != NULL)
		{			
			CRect rectCell;
			GetCellRect(cell.row, cell.col, rectCell);

			// Make sure point is relative to the cell in focus
			point.x -= rectCell.left;
			point.y -= rectCell.top;

			rect.left += rectCell.left;
			rect.top += rectCell.top;
			rect.right = rect.left + rectCell.Width();
			rect.bottom = rect.top + rectCell.Height();
			
			bRet = pCell->Edit(nRow, nCol, rect, point, nID, nChar);
		}
	}


	return bRet;
}

void CGridObj::EndEdit()
{	
	CCellID cell(GetFocusCell());
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	
	if(pCell != NULL)
	{
		pCell->EndEdit();
	}		
}

void CGridObj::OnEndEdit(int nRow, int nCol)
{
	if (!IsValid(nRow, nCol))
		return;
		
	CGridCellBase *pCell = (CGridCellBase*) GetCell(nRow, nCol);
	if (pCell)
	{
		CGridFriendCell* pGridFriendCell = (CGridFriendCell*)pCell;
		pGridFriendCell->OnEndEdit();
	}
}

void CGridObj::OnClickDown(CRect cellRect, CPoint PointCellRelative)
{
	CCellID cellID = GetFocusCell();
	CGridCellBase* pFocusCell = GetCell(cellID.row, cellID.col);
	if(pFocusCell != NULL)
	{
		CRect rectCell;
		GetCellRect(cellID.row, cellID.col, rectCell);

		// Make sure point is relative to the cell in focus
		PointCellRelative.x -= rectCell.left;
		PointCellRelative.y -= rectCell.top;

		cellRect.left	+= rectCell.left;
		cellRect.top	+= rectCell.top;
		cellRect.right	= cellRect.left + rectCell.Width();
		cellRect.bottom = cellRect.top + rectCell.Height();

		CGridFriendCell* pGridFriendCell = (CGridFriendCell*)pFocusCell;
		pGridFriendCell->OnClickDown(cellRect, PointCellRelative);
	}
}

void CGridObj::OnClick(CRect cellRect, CPoint PointCellRelative)
{
	CCellID cellID = GetFocusCell();
	CGridCellBase* pFocusCell = GetCell(cellID.row, cellID.col);
	if(pFocusCell != NULL)
	{
		CRect rectCell;
		GetCellRect(cellID.row, cellID.col, rectCell);

		// Make sure point is relative to the cell in focus
		PointCellRelative.x -= rectCell.left;
		PointCellRelative.y -= rectCell.top;

		cellRect.left	+= rectCell.left;
		cellRect.top	+= rectCell.top;
		cellRect.right	= cellRect.left + rectCell.Width();
		cellRect.bottom = cellRect.top + rectCell.Height();
		
		CGridFriendCell* pGridFriendCell = (CGridFriendCell*)pFocusCell;
		pGridFriendCell->OnClick(cellRect, PointCellRelative);
	}
}

void CGridObj::OnDblClick(CRect cellRect, CPoint PointCellRelative)
{
	CCellID cellID = GetFocusCell();
	CGridCellBase* pFocusCell = GetCell(cellID.row, cellID.col);
	if(pFocusCell != NULL)
	{
		CRect rectCell;
		GetCellRect(cellID.row, cellID.col, rectCell);

		// Make sure point is relative to the cell in focus
		PointCellRelative.x -= rectCell.left;
		PointCellRelative.y -= rectCell.top;

		cellRect.left	+= rectCell.left;
		cellRect.top	+= rectCell.top;
		cellRect.right	= cellRect.left + rectCell.Width();
		cellRect.bottom = cellRect.top + rectCell.Height();
		
		CGridFriendCell* pGridFriendCell = (CGridFriendCell*)pFocusCell;
		pGridFriendCell->OnDblClick(cellRect, PointCellRelative);
	}
}

BOOL CGridObj::OnSetCursor()
{
	BOOL bRet = FALSE;
	
	CPoint pt(GetMessagePos());	
	m_Grid.ScreenToClient(&pt);
	pt.x -= m_ptGridOrigin.x;
	pt.y -= m_ptGridOrigin.y;

	CCellID cellID = GetCellFromPt(pt);
	
	CGridCellBase* pCell = GetCell(cellID.row, cellID.col);
	if(pCell != NULL)
	{
		CGridFriendCell* pGridFriendCell = (CGridFriendCell*)pCell;
		bRet = pGridFriendCell->OnSetCursor();
	}

	return bRet;
}

CCellID CGridObj::SetFocusCell(CCellID cell)
{
	CCellID cellFocus = m_idCurrentCell;
	m_idCurrentCell = cell;

	return cellFocus;
}

void CGridObj::Draw(CDC* pDC, CRect rcDraw)
{	
	BOOL bGridLines = (m_nGridLines != GVL_NONE);
	
	m_ptGridOrigin.x = rcDraw.left;
	m_ptGridOrigin.y = rcDraw.top;

	m_rcDraw.SetRect(0, 0, rcDraw.Width() + 1, rcDraw.Height() + 1);
	CRect clipRect(m_rcDraw);
	
	CRect rect;

    int row, col;
    CGridCellBase* pCell;

    CCellID idTopLeft = GetTopleftNonFixedCell();
    int minVisibleRow = idTopLeft.row,
        minVisibleCol = idTopLeft.col;

    CRect VisRect;
    CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
    int maxVisibleRow = VisCellRange.GetMaxRow(),
        maxVisibleCol = VisCellRange.GetMaxCol();    
    
    int nFixedRowHeight = 0;
	int nFixedColWidth = 0;

	CPen pen;
    pen.CreatePen(PS_SOLID, 0, m_clrGridLines);
    CPen* pOldPen = pDC->SelectObject(&pen);

	// draw non-fixed cells
	
    rect.bottom = nFixedRowHeight-1;

    for (row = minVisibleRow; row <= maxVisibleRow; row++)
    {
        if (GetRowHeight(row) <= 0) continue;

        rect.top = rect.bottom+1;
        rect.bottom = rect.top + GetRowHeight(row)-1;

        // rect.bottom = bottom pixel of previous row
        if (rect.top > clipRect.bottom)
            break;                // Gone past cliprect
        if (rect.bottom < clipRect.top)
            continue;             // Reached cliprect yet?

        rect.right = nFixedColWidth-1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            rect.left = rect.right+1;
            rect.right = rect.left + GetColumnWidth(col)-1;

            if (rect.left > clipRect.right)
                break;        // gone past cliprect
            if (rect.right < clipRect.left)
                continue;     // Reached cliprect yet?

            pCell = GetCell(row, col);
            // TRACE(_T("Cell %d,%d type: %s\n"), row, col, pCell->GetRuntimeClass()->m_lpszClassName);
            if (pCell)
			{
				pCell->SetCoords(m_idCoords.row, m_idCoords.col);

				DWORD dwState = pCell->GetState();
						
				if(IsFocused())
				{					
					if(row == m_idCurrentCell.row && col == m_idCurrentCell.col)
					{
						dwState |= GVIS_FOCUSED;
					
						CRect rcFocus = rect;
						if(bGridLines)
						{
							rcFocus.DeflateRect(1, 1);
						}
						COLORREF clr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
						pDC->Draw3dRect(rcFocus, clr, clr);						

						pCell->SetState(dwState);
					}
				}

				if(IsSelected())
				{
					pCell->SetState(dwState | GVIS_SELECTED);
				}

				CGridFriendCell* pGridFriendCell = (CGridFriendCell*)pCell;

				CRect rcCell(rect.left + rcDraw.left, rect.top + rcDraw.top,
								rect.right + rcDraw.left, rect.bottom + rcDraw.top);
                pGridFriendCell->Draw(pDC, m_idCoords.row, m_idCoords.col, rcCell, FALSE);

				pCell->SetState(dwState & ~GVIS_FOCUSED);
			}
        }
    }
    
    // draw vertical lines (drawn at ends of cells)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT)
    {
        int x = nFixedColWidth;
        for (col = minVisibleCol; col <= maxVisibleCol; col++)
        {
            if (GetColumnWidth(col) <= 0) continue;

            x += GetColumnWidth(col);
            pDC->MoveTo(x-1, nFixedRowHeight);
            pDC->LineTo(x-1, VisRect.bottom);
        }
    }

    // draw horizontal lines (drawn at bottom of each cell)
    if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ)
    {
        int y = nFixedRowHeight;
        for (row = minVisibleRow; row <= maxVisibleRow; row++)
        {
            if (GetRowHeight(row) <= 0) continue;

            y += GetRowHeight(row);
            pDC->MoveTo(nFixedColWidth, y-1);
            pDC->LineTo(VisRect.right,  y-1);
        }
    }

    pDC->SelectObject(pOldPen);
}

// Gets the first non-fixed cell ID
CCellID CGridObj::GetTopleftNonFixedCell(BOOL bForceRecalculation /*=FALSE*/)
{
    // Used cached value if possible
    if (m_idTopLeftCell.IsValid() && !bForceRecalculation)
        return m_idTopLeftCell;

    int nVertScroll = m_nVertScroll,
        nHorzScroll = m_nHorzScroll;

    m_idTopLeftCell.col = 0;//m_nFixedCols;
    int nRight = 0;
    while (nRight < nHorzScroll && m_idTopLeftCell.col < (GetColumnCount()-1))
        nRight += GetColumnWidth(m_idTopLeftCell.col++);

    m_idTopLeftCell.row = 0; //m_nFixedRows;
    int nTop = 0;
    while (nTop < nVertScroll && m_idTopLeftCell.row < (GetRowCount()-1))
        nTop += GetRowHeight(m_idTopLeftCell.row++);

    //TRACE2("TopLeft cell is row %d, col %d\n",m_idTopLeftCell.row, m_idTopLeftCell.col);
    return m_idTopLeftCell;
}


// This gets even partially visible cells
CCellRange CGridObj::GetVisibleNonFixedCellRange(LPRECT pRect /*=NULL*/, 
                                                  BOOL bForceRecalculation /*=FALSE*/)
{
    int i;
    CRect rect;
    //GetClientRect(rect);
	rect = m_rcDraw;

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int bottom = 0; //GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++)
    {
        bottom += GetRowHeight(i);
        if (bottom >= rect.bottom)
        {
            bottom = rect.bottom;
            break;
        }
    }
    int maxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int right = 0; //GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++)
    {
        right += GetColumnWidth(i);
        if (right >= rect.right)
        {
            right = rect.right;
            break;
        }
    }
    int maxVisibleCol = min(i, GetColumnCount() - 1);
    if (pRect)
    {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

// returns the top left point of the cell. Returns FALSE if cell not visible.
BOOL CGridObj::GetCellOrigin(int nRow, int nCol, LPPOINT p)
{
    int i;

    if (!IsValid(nRow, nCol))
        return FALSE;

	int nFixedCols = 0;
	int nFixedRows = 0;

    CCellID idTopLeft;
    if (nCol >= nFixedCols/*m_nFixedCols*/ || nRow >= nFixedRows/*m_nFixedRows*/)
        idTopLeft = GetTopleftNonFixedCell();

    /*if ((nRow >= m_nFixedRows && nRow < idTopLeft.row) ||
        (nCol>= m_nFixedCols && nCol < idTopLeft.col))
        return FALSE;*/
	if ((nRow >= nFixedRows && nRow < idTopLeft.row) ||
        (nCol>= nFixedCols && nCol < idTopLeft.col))
        return FALSE;

    p->x = 0;
    if (nCol < nFixedCols/*m_nFixedCols*/)                      // is a fixed column
        for (i = 0; i < nCol; i++)
            p->x += GetColumnWidth(i);
        else 
        {                                        // is a scrollable data column
            for (i = 0; i < nFixedCols/*m_nFixedCols*/; i++)
                p->x += GetColumnWidth(i);
            for (i = idTopLeft.col; i < nCol; i++)
                p->x += GetColumnWidth(i);
        }
        
        p->y = 0;
        if (nRow < nFixedRows/*m_nFixedRows*/)                      // is a fixed row
            for (i = 0; i < nRow; i++)
                p->y += GetRowHeight(i);
            else 
            {                                        // is a scrollable data row
                for (i = 0; i < nFixedRows/*m_nFixedRows*/; i++)
                    p->y += GetRowHeight(i);
                for (i = idTopLeft.row; i < nRow; i++)
                    p->y += GetRowHeight(i);
            }
            
            return TRUE;
}

BOOL CGridObj::GetCellRect(int nRow, int nCol, LPRECT pRect)
{
    CPoint CellOrigin;
    if (!GetCellOrigin(nRow, nCol, &CellOrigin))
        return FALSE;

    pRect->left   = CellOrigin.x;
    pRect->top    = CellOrigin.y;
    pRect->right  = CellOrigin.x + GetColumnWidth(nCol)-1;
    pRect->bottom = CellOrigin.y + GetRowHeight(nRow)-1;

	// Extra checkings to return FALSE if the cell is not visible at all
	if((pRect->right - pRect->left <= 0) || (pRect->bottom - pRect->top <= 0))
		return FALSE;

    //TRACE("Row %d, col %d: L %d, T %d, W %d, H %d:  %d,%d - %d,%d\n",
    //      nRow,nCol, CellOrigin.x, CellOrigin.y, GetColumnWidth(nCol), GetRowHeight(nRow),
    //      pRect->left, pRect->top, pRect->right, pRect->bottom);

    return TRUE;
}

BOOL CGridObj::GetTextRect(int nRow, int nCol, LPRECT pRect)
{
    CGridCellBase* pCell = GetCell( nRow, nCol);
    if( pCell == NULL)
        return FALSE;
    
    if( !GetCellRect( nRow, nCol, pRect) )
        return FALSE;

    return pCell->GetTextRect( pRect);
}

CCellID CGridObj::GetCellFromPt(CPoint point)
{
	BOOL bAllowFixedCellCheck  = FALSE;

    CCellID cellID; // return value

    CCellID idTopLeft = GetTopleftNonFixedCell();
	if (!bAllowFixedCellCheck && !IsValid(idTopLeft.row, idTopLeft.col))
		return cellID;

    // calculate column index
    int fixedColWidth = 0; //GetFixedColumnWidth();

    if (point.x < 0 || (!bAllowFixedCellCheck && point.x < fixedColWidth)) // not in window
        cellID.col = -1;
    else if (point.x < fixedColWidth) // in fixed col
    {
		int nFixedCols = 0;

        int xpos = 0;
        int col = 0;
        while (col < nFixedCols/*m_nFixedCols*/)
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x)
                break;
			col++;
        }

        cellID.col = col;
    }
    else    // in non-fixed col
    {
        int xpos = fixedColWidth;
		int col = idTopLeft.col; //m_nFixedCols;
        while ( col < GetColumnCount())
        {
            xpos += GetColumnWidth(col);
            if (xpos > point.x)
                break;
			col++;
        }

        if (col >= GetColumnCount())
            cellID.col = -1;
        else
            cellID.col = col;
    }

    // calculate row index
    int fixedRowHeight = 0; //GetFixedRowHeight();
    if (point.y < 0 || (!bAllowFixedCellCheck && point.y < fixedRowHeight)) // not in window
        cellID.row = -1;
    else if (point.y < fixedRowHeight) // in fixed col
    {
		int nFixedRows = 0;

        int ypos = 0;
        int row = 0;
        while (row < nFixedRows/*m_nFixedRows*/) 
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y)
                break;
			row++;
        }
        cellID.row = row;
    }
    else
    {
        int ypos = fixedRowHeight;
		int row = idTopLeft.row;
        while ( row < GetRowCount() )
        {
            ypos += GetRowHeight(row);
            if (ypos > point.y)
                break;
			row++;
        }

        if (row >= GetRowCount())
            cellID.row = -1;
        else
            cellID.row = row;
    }
    
	return cellID;
}

void CGridObj::SetCoords(int nRow, int nCol)
{
	m_idCoords.row = nRow;
	m_idCoords.col = nCol;
}
