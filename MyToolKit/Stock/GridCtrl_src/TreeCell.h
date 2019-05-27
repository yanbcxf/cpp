//
// TreeGridCell.h : header file
//

#if !defined(__EMCTreeCell_H__)
#define __EMCTreeCell_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CGridCell;

// Tree Item levels
#define UNINITALIZED_TREEITEM_LEVEL	-2
#define ROOT_TREEITEM_LEVEL			-1

// Tree Cell states
#define GTVIS_HIDDEN		( GVIS_LAST << 1 ) 
#define GTVIS_EXPANDED		( GVIS_LAST << 2 )
#define GTVIS_COLLAPSED		( GVIS_LAST << 3 )

extern int nTreeButtonWidth;
extern int nTreeButtonHeight;

//  CTreeCell class definition
class CTreeCell
{
	friend class CTreeGridCtrl;
	
public:
	// Construction
	CTreeCell(CGridCell* cell);

	// Determine the location of the specified point relative to the client area of a tree list control
	//
	//  point				: Specifies the client coordinates of the point to test
	//  rectCell				: Specifies the rectangle of the cell
	//  bClickedOnBtnImgArea	: Output parameter for determining the point relative to the client area is in the button image area
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL TreeBtnHitTest(CPoint point, CRect rectCell, BOOL& bClickedOnBtnImgArea);

	// Computes the width and height of a cell on the attribute device context
	//
	//  pDC			: Specifies the device context
	//  pSize		: Output parameter to receive the cell extent
	//  pImgExtent	: Output parament to receive the image extent
	//  pTextExtent	: Output parament to receive the text extent
	//
	// Return the dimensions of the cell (in logical units) in a CSize object
	virtual void OnGetTreeCellExtent(CDC* pDC, CSize& size);

	// Draw the item
	//
	//  pDC			: Specifies the device context
	//  nItem		: Specifies the row index of the cell
	//  nSubItem		: Specifies the column index of the cell
	//  rect			: Specifies the rectangle which might be modified inside the method
	//  bEraseBkgnd	: Specifies whether the cell erase the background when repaint
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL OnDrawingTreeCell(CDC* pDC, int nItem, int nSubItem, CRect& rect,  BOOL bEraseBkgnd = TRUE);

	// Enable the edit mode of the cell
	//
	//  nItem	: Specifies the row index of the cell
	//  nSubItem	: Specifies the column index of the cell
	//  rect		: Specifies the rectangle which might be modified inside the method
	//  point	: Specifies the point
	//  nID		: Specifies the in-place edit control ID
	//  nChar	: Specifies the character
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL OnEditingTreeCell(int nItem, int nSubItem, CRect& rect, CPoint  point, UINT nID, UINT nChar);

	// Measure the tree button rectangle
	//
	//  rect	: Specifies the rectangle
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL MeasureTreeButtonRect(CRect& rect);
	
	// Retrieve the tree buttons horizontal margin
	//
	// Returns the tree buttons horizontal margin
	virtual int GetTreeButtonMarginHorz();

	// Retrieve the tree buttons vertical margin
	//
	// Returns the tree buttons vertical margin
	virtual int GetTreeButtonMarginVert();

	// Retrieve the tree buttons right padding
	//
	// Returns the tree buttons right padding
	virtual int GetTreeBtnRightPadding();

	// Draw the tree item buttons
	//
	//  pDC			: Specifies the device context
	//  rcButtonArea	: Specifies the rectangle of button area
	//
	virtual void DrawTreeItemButton(CDC* pDC, CRect rcButtonArea);

	// This is calculated using the horizontal and vertical Tree Button Margin
	//
	//  rect	: Output parameter for retrieving the rectangle
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL MeasureTreeButtonImageRect(CRect& rect);

	// Retrieve the tree item
	//
	// Return the tree item
	GTV_HTREENODE GetTreeItem();
	
	// Gets the item index
	int GetItemID();

	// Retrieve the tree item level (level 0 is the top parents, level 1 is the first level child, level 2 is the second level child and so on..)
	//
	// Return the tree level
	int GetTreeItemLevel();
	
	// Enable/Disable the tree node expanded mode
	//
	//  bExpanded	: Specifies the tree node expanded mode
	//
	void SetExpanded(BOOL bExpanded = TRUE);
	
	// Determine whether the tree node is expanded or not
	//
	// Return TRUE if tree node is expanded, otherwise FALSE
	BOOL IsExpanded();

	// Determine whether the tree node button shown or not
	//
	// Return TRUE if tree node button is shown, otherwise FALSE
	BOOL HasButton();

	// Retrieve the Cell state
	//
	// Return the cell state
	//
	DWORD GetState() const;

	// Determine the Cell is enabled
	//
	// Returns TRUE if the cell is enabled, otherwise FALSE
	BOOL IsEnabled() const;

	// Retrieve the margin of the Cell
	//
	// Return the margin of the cell
	UINT GetMargin() const;

protected:
	// Measure the tree button rectangle
	//
	//  rect				: Output parameter for retrieving the buttons rectangle
	//  nTreeItemLevel	: Specifies the tree item level
	//
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL OnMeasureTreeButtonRect(CRect& rect, int nTreeItemLevel);

	// Copy the value of member variables from a given tree cell
	//
	//  cell	: Specifies the cell
	//
	// Return TRUE if successful, otherwise FALSE
	virtual BOOL CopyFromTreeCell(const CTreeCell& cell);

	// Hold the point of where vertical line to child should start
	CPoint m_ptVertLineOrigin;

	// Hold the tree item
	GTV_HTREENODE m_hTreeItem;

	// Hold the tree item level
	int m_nTreeItemLevel;	

	//! Hold the tree item row index
	int m_nItem;

	// Hold the corresponding GridCtrl Cell which will gain the tree cell attributes
	// To be used sparingly (only to get cell states)
	CGridCell* m_pGridCell;
};

#endif // !defined(__EMCTreeCell_H__)
