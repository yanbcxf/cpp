
#if !defined(__EMCGridObj_HeaderFile__)
#define __EMCGridObj_HeaderFile__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGridCellBase;
class CGridCell;
class CGridCtrl;

#include <afxtempl.h>
#include <vector>
#include "../GridCtrl_src/CellRange.h"

#define GRIDOBJ_DEFAULT_ROWHEIGHT		-2

//
// Definition of the CGridObj class
//
class CGridObj : public CObject
{
	typedef std::vector<int> intlist;
	// storage typedef for each row in the grid
	typedef CTypedPtrArray<CObArray, CGridCellBase*> GRID_ROW;

public:
	// Constructor
	//
	//  owner	: Specifies the existing CGridCtrl which contains the CT_GRID cell and the CGridObj
	//
	CGridObj(CGridCtrl& owner);
	virtual ~CGridObj();

///////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////
public:
	// Gets the row count
    int  GetRowCount() const;
	// Gets the column count
    int  GetColumnCount() const;

	// Sets the row count
    BOOL SetRowCount(int nRows);
	// Sets the column count
    BOOL SetColumnCount(int nCols);

	// Deletes all items
	BOOL DeleteAllItems();	

	// Checks if a given cell is in valid range
	BOOL IsValid(int nRow, int nCol) const;
    
	// Gets the row height
    int  GetRowHeight(int nRow) const;
	// Sets the row height
    BOOL SetRowHeight(int row, int height);

	// Gets the column width
    int  GetColumnWidth(int nCol) const;
	// Sets the column widht
    BOOL SetColumnWidth(int col, int width);

	// Sets the default row height to use if row height is not specified
	void SetDefaultRowHeight(int nHeight);

	// Specifies the column which should be stretched if the grid is expanded
	void SetStretchColIndex(int nCol);
	
	// Gets pointer to a cell given row and column index
    CGridCellBase* GetCell(int nRow, int nCol) const;	
	// Sets a new type of cell given a row and column index
	BOOL SetCell(int nRow, int nCol, CGridCellBase* pCell);	
    
	// Sets a new type of cell given a row and column index removing the old cell given runtime class of the cell
    BOOL SetCellType(int nRow, int nCol, CRuntimeClass* pRuntimeClass);
	
	// Sets cell text to a cell given row and column index
	BOOL SetCellText(int nRow, int nCol, LPCTSTR pszText);
	// Gets the cell text
	LPCTSTR GetCellText(int nRow, int nCol);	
	
	// Gets cell id given a point relative to the grid
	CCellID GetCellFromPt(CPoint point);
	// Gets cell origin of a cell
	BOOL GetCellOrigin(int nRow, int nCol, LPPOINT p);
	// Gets cell area given row and column index
    BOOL GetCellRect(int nRow, int nCol, LPRECT pRect);    
	// Gets cell text area given row and column index
    BOOL GetTextRect(int nRow, int nCol, LPRECT pRect);	

	// Specifies whether to show horizontal and vertical grid lines
	void SetGridLines(BOOL bHorzGridLines, BOOL bVertGridLines, COLORREF clrGridLines = RGB(192,192,192));	

	// Specifies whether the grid is in selected mode
	void SetSelected(BOOL bSelected = TRUE);
	// Determines if the grid is in selected mode
	BOOL IsSelected();	

	// Specifies whether the grid is in focus mode
	void SetFocus(BOOL bFocus = TRUE);
	// Determines if the grid is in focus mode
	BOOL IsFocused();
	
	// Gets total sum of cell extents of the grid
	//
	// bAutoSizeing : This should be passed as TRUE if this API is called because of 
	// due to the invoking of GridCtrl::AutoSize... routines.
	// When this is TRUE, the GetGridExtent actually changes resizes the internal cells
	// to their extents (useful when user double clicks on a composite cell containing the GridObj)
	//
	virtual CSize GetGridExtent(BOOL bAutoSizing, CDC* pDC);

///////////////////////////////////////////////////////////////////////////////////
//Operations
///////////////////////////////////////////////////////////////////////////////////
	//
	// Start editing of a cell in the grid given row and col index
	BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	// Ends the current editing in the grid
	void EndEdit();

	// Specifies the focus cell in the grid
	CCellID SetFocusCell(CCellID cell);	
	// Retrieves the focus cell in the grid
	CCellID GetFocusCell();

///////////////////////////////////////////////////////////////////////////////////
//virtual overrides
///////////////////////////////////////////////////////////////////////////////////
	virtual void OnEndEdit(int nRow, int nCol);

	virtual void OnClickDown(CRect cellRect, CPoint PointCellRelative);	

	virtual void OnClick(CRect cellRect, CPoint PointCellRelative);

	virtual void OnDblClick(CRect cellRect, CPoint PointCellRelative);

	virtual BOOL OnSetCursor();

///////////////////////////////////////////////////////////////////////////////////
//Drawing
///////////////////////////////////////////////////////////////////////////////////
	// Draws the content of the grid in a device context
    virtual void Draw(CDC* pDC, CRect rcDraw);	
	// Specifies a coordinate to use for cells in the grid
	void SetCoords(int nRow, int nCol);	

protected:

///////////////////////////////////////////////////////////////////////////////////
//Creation and clean up
///////////////////////////////////////////////////////////////////////////////////

    virtual CGridCellBase* CreateCell(int nRow, int nCol);
    virtual void DestroyCell(int nRow, int nCol);    

///////////////////////////////////////////////////////////////////////////////////
//Implementation
///////////////////////////////////////////////////////////////////////////////////
	
    CCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE);
    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE);

	// Cell details
    int         m_nRows, m_nCols;
    CUIntArray  m_arRowHeights, m_arColWidths;
	intlist m_arRowOrder, m_arColOrder;	

    CRuntimeClass*   m_pRtcDefault; // determines kind of Grid Cell created by default

	// Cell data
    CTypedPtrArray<CObArray, GRID_ROW* > m_RowData;

	CGridCtrl& m_Grid;

	UINT m_nGridLines;
	COLORREF m_clrGridLines;

	CCellID m_idTopLeftCell;
	CCellID m_idCurrentCell;	

	CRect m_rcDraw;
	CPoint m_ptGridOrigin;
	CCellID m_idCoords;

	BOOL m_bSelected;
	BOOL m_bFocus;

	int m_nDefaultRowHeight;
	int m_nStretchColIndex;	

	// for future use
	int m_nVertScroll;
	int m_nHorzScroll;
};

#endif // !defined(__EMCGridObj_HeaderFile__)
