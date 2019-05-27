//
// TreeGridCell.h : header file
//

#if !defined(__TreeGridCell_H__)
#define __TreeGridCell_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GridCell.h"
#include "TreeCell.h"

//  CTreeGridCell class definition
class CTreeGridCell : public CGridCell, public CTreeCell
{	
	DECLARE_DYNCREATE(CTreeGridCell)

public:
	// Construction
	CTreeGridCell();	
	
	// Computes the width and height of a cell on the attribute device context
	//
	//  pDC			: Specifies the device context
	//
	// Return the dimensions of the cell (in logical units) in a CSize object
	virtual CSize GetCellExtent(CDC* pDC);

	// Draw the item
	//
	//  pDC			: Specifies the device context
	//  nItem		: Specifies the row index of the cell
	//  nSubItem		: Specifies the column index of the cell
	//  rect			: Specifies the rectangle
	//  bEraseBkgnd	: Specifies whether the cell erase the background when repaint
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL Draw(CDC* pDC, int nItem, int nSubItem, CRect rect,  BOOL bEraseBkgnd = TRUE);

	// Enable the edit mode of the cell
	//
	//  nItem	: Specifies the row index of the cell
	//  nSubItem	: Specifies the column index of the cell
	//  rect		: Specifies the rectangle
	//  point	: Specifies the point
	//  nID		: Specifies the in-place edit control ID
	//  nChar	: Specifies the character
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL Edit(int nItem, int nSubItem, CRect rect, CPoint  point, UINT nID, UINT nChar);

	virtual void  SetCoords(int nRow, int /*nCol*/) 
	{
		m_nItem = nRow;
	}

protected:

	// Invoked after coping the value of member variables from a given cell
	//
	//  cell	: Specifies the cell
	//	
	virtual void OnCopy(const CGridCellBase& cell);
};

#endif // !defined(__TreeGridCell_H__)
