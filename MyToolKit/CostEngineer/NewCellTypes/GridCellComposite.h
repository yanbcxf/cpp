
#pragma once

#include "GridObj.h"
#include "../GridCtrl_src/GridCell.h"


class CCompositeCellInfo
{
public:
	// Constructor
	CCompositeCellInfo()
	{
		m_pGrid = NULL;
		m_bGridAutoRelease = TRUE;
	}
	// Destructor
	virtual ~CCompositeCellInfo()
	{
	}

	// Pointer to a persistent grid object
	CGridObj* m_pGrid;
	// Specifies whether the grid object will be destroyed upon the destruction of the cell
	// Set to TRUE by default
	BOOL m_bGridAutoRelease;
};

// CGridCellComposite class definition
class CGridCellComposite : public CGridCell
{
	DECLARE_DYNCREATE(CGridCellComposite)

public:
	CGridCellComposite();
	virtual ~CGridCellComposite();

public:
	// Set the args of CGridCellComposite cell
	//
	//  gridArgs	: Specifies the CComboCellTypeArgs class object
	//
	void SetCellInfo(CCompositeCellInfo& info);
	
	// Retrieves the args of CGridCellComposite cell
	//
	//  gridArgs	: Receives the CComboCellTypeArgs class object
	//
	void GetCellInfo(CCompositeCellInfo& info);

	// Set the cell text
	//
	//  szText	: Specifies the text
	//
	virtual void SetText(LPCTSTR szText);


	CGridObj* GetGridObj() const;

protected:
	// Enable the edit mode of the cell
	//
	//  nItem	: Specifies the row index of the cell
	//  nSubItem	: Specifies the column index of the cell
	//  rect		: Specifies the rectangle
	//  point	: Specifies the point
	//  nID		: Specifies the inplace edit control ID
	//  nChar	: Specifies the character
	//
	//  Returns TRUE if successful, otherwise FALSE
	virtual BOOL Edit(int nItem, int nSubItem, CRect rect, CPoint point, UINT nID, UINT nChar);

	// Called when editing is finished from a cell
	//
	virtual void OnEndEdit();

	// Validate the cell with a given text
	//
	//  str	: Specifies the string
	//
	//  Returns TRUE if validation successful, otherwise FALSE
	
	virtual BOOL ValidateEdit(LPCTSTR str);

	// Retrieve the inplace edit control window
	//
	// Return pointer of the inplace edit control window
	virtual CWnd* GetEditWnd() const;

	// Called when the cell editable mode is ended
	//
	virtual void EndEdit();

	// Retrieve the cell extent
	//
	//  pDC			: Specifies the device context
	//
	//  Returns the dimensions of the cell (in logical units) in a CSize object
	virtual CSize GetCellExtent(CDC* pDC);

	// Draw the item
	//
	//  pDC			: Specifies the device context
	//  nItem		: Specifies the row index of the cell
	//  nSubItem		: Specifies the column index of the cell
	//  rect			: Specifies the rectangle
	//  bEraseBkgnd	: Specifies whether the cell erase the background when repaint
	//
	//  Returns TRUE if successful, otherwise FALSE
	virtual BOOL Draw(CDC* pDC, int nItem, int nSubItem, CRect rect, BOOL bEraseBkgnd = TRUE);	

	// Called when the user presses the left mouse button on a cell
	//
	//  cellRect			: Specifies the rectangle of the cell
	//  PointCellRelative: Specifies the x and ycoordinate of the cursor. 
	//				These coordinates are always relative to the upper-left corner of the cell
	//
	virtual void OnClickDown(CRect cellRect, CPoint PointCellRelative);	

	// Called when the user releases the left mouse button on a cell
	//
	//  cellRect			: Specifies the rectangle of the cell
	//  PointCellRelative: Specifies the x and ycoordinate of the cursor. 
	//				These coordinates are always relative to the upper-left corner of the cell
	//
	virtual void OnClick(CRect cellRect, CPoint PointCellRelative);

	// Called when the user double-clicks the left mouse button in a cell
	//
	//  cellRect			: Specifies the rectangle of the cell
	//  PointCellRelative: Specifies the x and ycoordinate of the cursor. 
	//				These coordinates are always relative to the upper-left corner of the cell
	//
	virtual void OnDblClick(CRect cellRect, CPoint PointCellRelative);

	// Called when cursor needs to beset
	//
	// Return TRUE if you handle the event, FALSE otherwise
	virtual BOOL OnSetCursor();

	// Retrieve the cell state
	//
	//  Return the cell state
	//
	virtual DWORD GetState() const;

	// Set the cell state
	//
	//  nState	: Specifies the cell state
	//
	virtual void SetState(DWORD nState);

protected:	
	// Pointer of the grid
	CCompositeCellInfo m_cellInfo;
};