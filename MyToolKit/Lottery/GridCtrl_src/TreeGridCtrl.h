//
// TreeGridCtrl.h : header file
//

#if !defined(__TreeGridCtrl_H__)
#define __TreeGridCtrl_H__

#include "GridCtrl.h"
#include "TreeNode.h"
#include "TreeCell.h"

#include <afxtempl.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// some useful data structures
//
extern GTV_HTREENODE GLVI_ROOT;
extern GTV_HTREENODE GLVI_FIRST;
extern GTV_HTREENODE GLVI_LAST;

enum CellTypeID
{
	CT_DEFAULT	= 0,
	CT_CHECKBOX
};
//

// CTreeGridCtrl Notification Messages
//
#define GTVN_ITEMEXPANDING		(GVN_LAST + 0x0001)
#define GTVN_ITEMEXPANDED		(GVN_LAST + 0x0002)
#define GTVN_ITEMCOLLAPSING		(GVN_LAST + 0x0003)
#define GTVN_ITEMCOLLAPSED		(GVN_LAST + 0x0004)
#define GTVN_ITEMTRAVERSE		(GVN_LAST + 0x0005)
//

typedef GRID_ROW* GRID_ROWS;
typedef CCellRange CItemRange;
typedef int GV_ITEMHANDLE;

typedef CMap<int, int&, void*, void*&> CMapIntToPtr;
typedef CMap<int, int&, int, int&> CMapIntToInt;
typedef CMap<void*, void*&, int, int&> CMapPtrToInt;
typedef CMap<int, int&, DWORD, DWORD&> CMapIntToWord;


#define TREECELL(T) (dynamic_cast<CTreeCell*>(T))

// CTreeGridCtrl class definition
class CTreeGridCtrl : public CGridCtrl
{
	friend class CTreeCell;

public:
	CTreeGridCtrl();
	virtual ~CTreeGridCtrl();

public:
	// Insert a Tree Item
	//
	//  strHeading	: Specifies the default text of the inserted item
	//  hParent		: Specifies the parent node of the row
	//  hInsertAfter	: Specifies the sibling node after which the row will be inserted
	//  bRefresh		: Specifies whether the list will be refreshed immediately
	//
	// Returns the handle to the inserted node
	GTV_HTREENODE InsertItem(LPCTSTR strHeading, GTV_HTREENODE hParent = GLVI_ROOT,
					GTV_HTREENODE hInsertAfter = GLVI_LAST , BOOL bRefresh = TRUE);

	// Insert a Tree Item
	//
	//  pItem		: Specifies the pointer to a GV_ITEM structure that defines the attributes of an item
	//  hParent		: Specifies the parent node of the row
	//  hInsertAfter	: Specifies the sibling node after which the row will be inserted
	//  bRefresh		: Specifies whether the list will be refreshed immediately
	//
	// Returns the handle to the inserted node
	GTV_HTREENODE InsertItem( GV_ITEM* pItem, GTV_HTREENODE hParent = GLVI_ROOT,
					GTV_HTREENODE hInsertAfter = GLVI_LAST, BOOL bRefresh = TRUE);

	// Insert a row
	//
	//  strHeading	: Specifies the default text of the inserted item
	//  nItem		: Specifies the position of the row (-1 to indicate at the end of the list)
	//  bRefresh		: Specifies the whether the list will be refreshed immediately
	//
	// Returns the index of the inserted row, -1 if failed
	int InsertItem(LPCTSTR strHeading, int nItem, BOOL bRefresh);

	// Insert a row
	//
	//  pItem	: Specifies the pointer to a GV_ITEM structure that defines the attributes of an item
	//  bRefresh	: Specifies the whether the list will be refreshed immediately
	//
	// Returns the index of the inserted row, -1 if failed
	int InsertItem(GV_ITEM* pItem, BOOL bRefresh);

	// Delete a Tree Item as well as the corresponding row
	//
	//  hNode	: Specifies the node to be deleted
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL DeleteItem(GTV_HTREENODE hNode);

	// Delete a row
	//
	//  nItem	: Specifies the row to be deleted
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL DeleteItem(int nItem);

	// Delete all rows including tree nodes information
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL DeleteAllItems();

	// Expand a tree node to display its child
	//
	//  hNode	: Specifies the node to be expanded
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL ExpandTreeNode(GTV_HTREENODE hNode);

	// Collapse a tree node to hide its child
	//
	//  hNode	: Specifies the node to be collapsed
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL CollapseTreeNode(GTV_HTREENODE hNode);

	// Specifies whether the expand/collapse button should be shown next to a tree node
	// 
	//  hNode	: Specifies the node
	//  bShow	: Specifies whether to show the button
	//
	void ShowTreeNodeButton(GTV_HTREENODE hNode, BOOL bShow = TRUE);

	// Gets whether the expand/collapse button should be shown next to a tree node
	//
	// Returns TRUE if tree node button is shown, FALSE otherwise
	BOOL IsShownTreeNodeButton(GTV_HTREENODE hNode);

	// Move a row (and its children) under a new parent node
	//
	//  hNode		: Specifies the node to be moved
	//  hNewParent	: Specifies the parent node
	//  hInsertAfter	: Specifies the sibling node after which the row will be inserted
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL MoveTreeNode(GTV_HTREENODE hNode, GTV_HTREENODE hNewParent, GTV_HTREENODE hInsertAfter = GLVI_LAST);

	// Retrieve the row index of a node
	//
	//  hNode	: Specifies the node
	//
	// Returns the row index of the specified node
	int FindRowIndex(GTV_HTREENODE hNode);

	// Retrieve the range of rows of the descendant nodes of a given node
	//
	//  hParentNode	: Specifies the node
	//  range		: Output parameter to retrieve the row range
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL FindChildrenRowRange(GTV_HTREENODE hParentNode, CItemRange& range);

	// Set the vertical and horizontal colors of the tree hierarchy lines
	//
	//  clrTreeVertLine	: Specifies the vertical line color
	//  clrTreeHorzLine	: Specifies the horizontal line color
	//
	void SetTreeLineColors(COLORREF clrTreeVertLine, COLORREF clrTreeHorzLine);

	// Retrieve the horizontal line color of the tree hierarchy lines
	//
	// Returns the color
	COLORREF GetHorzTreeLineColor();

	// Retrieve the vertical line color of the tree hierarchy lines
	//
	// Returns the color
	COLORREF GetVertTreeLineColor();

	// Sort based on a column
	//
	//  nColumn		: Specifies the column to use for sorting
	//  bAscending	: Specifies the sort direction
	//  data			: Specifies the user data
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SortItems(int nColumn, BOOL bAscending, LPARAM data = 0);

	// Sort the tree list based on a column using compare function
	//
	//  pfnCompare	: Specifies the function pointer to the compare function
	//  nColumn		: Specifies the column to use for sorting
	//  bAscending	: Specifies the sort direction
	//  data			: Specifies the user data
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SortItems(PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data = 0);

	// Sort the tree list based on a column by text (using a built-in text compare function)
	//
	//  nColumn		: Specifies the column to use for sorting
	//  bAscending	: Specifies the sort direction
	//  data			: Specifies the user data
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SortTextItems(int nColumn, BOOL bAscending, LPARAM data = 0);

	// Sort items under a node based on a column
	//
	//  hParent		: Specifies the node
	//  nColumn		: Specifies the column to use for sorting
	//  bAscending	: Specifies the sort direction
	//  data			: Specifies the user data
	//  bRecursive	: Specifies if recursively all children will be sorted
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SortItems(GTV_HTREENODE hParent, int nColumn, BOOL bAscending, LPARAM data, BOOL bRecursive = TRUE);

	// Sort items under a node based on a column
	//
	//  hParent		: Specifies the node
	//  pfnCompare	: Specifies the callback compare function
	//  nColumn		: Specifies the column to use for sorting
	//  bAscending	: Specifies the sort direction
	//  data			: Specifies the user data
	//  bRecursive	: Specifies if recursively all children will be sorted
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SortItems(GTV_HTREENODE hParent, PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data = 0, BOOL bRecursive = TRUE);

	// Determine whether the specified tree node has child nodes
	//
	//  hNode	: Specifies the handle of a tree node
	//
	// Returns TRUE if the tree node specified by hNode has child nodes, otherwise FALSE
	BOOL NodeHasChildren(GTV_HTREENODE hNode);

	// Retrieve the tree ctrl node that is the child of a specified node
	//
	//  hNode	: Specifies the handle of a tree node
	//
	// Returns the handle of the child node if successful; otherwise NULL
	GTV_HTREENODE GetChildNode(GTV_HTREENODE hNode);

	// Retrieve the tree ctrl node that is the next sibling of a specified node
	//
	//  hNode	: Specifies the handle of a tree node
	//
	// Returns the handle of the next sibling item if successful; otherwise NULL
	GTV_HTREENODE GetNextSiblingNode(GTV_HTREENODE hNode);

	// Retrieve the tree ctrl node that is the previous sibling of a specified node
	//
	//  hNode	: Specifies the handle of a tree node
	//
	// Returns the handle of the previous sibling node if successful; otherwise NULL
	GTV_HTREENODE GetPrevSiblingNode(GTV_HTREENODE hNode);

	// Retrieve the parent of a specified node
	//
	//  hNode	: Specifies the handle of a tree node
	//
	// Returns the handle of the parent item; otherwise NULL
	GTV_HTREENODE GetParentNode(GTV_HTREENODE hNode);

	// Retrieves the root of the specified tree control
	//
	// Returns the handle of the root node; otherwise NULL
	GTV_HTREENODE GetRootNode();

	// Retrieves the a node given a row
	//
	// 	nItem	: Specifies the row index
	//
	// Returns the handle of the root node; otherwise NULL
	GTV_HTREENODE GetTreeNode(int nItem);

	// Retrieve the corresponding tree cell given a handle of a item
	//
	//  hItem	: Specifies the handle of the item
	//
	// Return the pointer of the tree list control cell
	CTreeCell* GetTreeCell(GTV_HTREENODE hItem);

	// Traverse through all child nodes given a parent and send GTVN_ITEMTRAVERSE notifications along the way when coming across a node
	//
	//  hParentItem		: Specifies the handle of the parent item
	//  nRowIndex		: Specifies the row index of the parent item. This is required for the fast traversal of node. (Use -1 if hParentItem == GetRootItem())
	//
	void TraverseNodes(GTV_HTREENODE hParentItem, int& nRowIndex);

	// Creates a number of top level tree items
	BOOL SetItemCount(int nItems);	

	int GetItemCount()
	{
		return GetRowCount();
	}

	// Create a array of items under a parent item given parent item row index
	//
	// Returns the row index of the last child inserted
	int SetChildItemCount(int nParentItemIndex, int nItems);

	// Show or hide a row
	//
	//  nRow		: Specifies the row
	//  bShow	: Specifies whether to show or hide the row
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL ShowRow(int nRow, BOOL bShow = TRUE);

	/*******************************************************************************/
	// Set the default type of a tree column cell (combo, check-box, color, etc. with tree buttons)
	//
	//  cellType	: Specifies the type
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SetTreeColumnCellTypeID(enum CellTypeID cellType);

	// Set the type of a cell (combo, check-box, color, etc. with tree buttons)
	//
	//  nRow		: Specifies the row index of the cell
	//  nColumn	: Specifies the column index of the cell
	//  cellType	: Specifies the type
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SetCellTypeID(int nRow, int nColumn, enum CellTypeID cellType);
	
	// Retrieve the type of a cell (combo, check-box, color, etc.)
	//
	//  nRow		: Specifies the row index of the cell
	//  nColumn	: Specifies the column index of the cell
	//
	// Returns the type of the the cell (combo, check-box, color, etc.)
	CellTypeID GetCellTypeID(int nRow, int nColumn);	
	
	// Set the type of a cell in the column (combo, check-box, color, etc.)
	//
	//  nColumn	: Specifies the column
	//  cellType	: Specifies the type
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL SetColumnCellTypeID(int nColumn, enum CellTypeID cellType);
	

	//Sets the cell text
	void SetCellText(int row, int col, LPCTSTR pszText);

	//Sets the cell state
	void SetCellState(int row, int col, DWORD nState);
	
	//Gets the cell state
	DWORD GetCellState(int row, int col);

	// Sets the tree column index
	void SetTreeColumn(int nTreeCol)
	{
		m_nTreeCol = nTreeCol;
	}

	// Gets the tree column index
	int GetTreeColumn()
	{
		return m_nTreeCol;
	}

protected:	
	// Create a tree node structure only
	//
	// Return the handle of the tree node
	GTV_HTREENODE CreateNode();

	// Create an array of tree node structure
	//
	// Return the pointer to an array of tree node structures
	GTV_TREENODESPTR CreateNodes(int nCount);

	// Create a root tree node
	//
	// Return the handle of the root tree node
	GTV_HTREENODE CreateRootNode();	

	// Determine whether a node is a root node
	//
	//  hItem	: Specifies the node
	//
	// Return TRUE if the item is a root node of the tree, otherwise FALSE
	BOOL IsRootNode(GTV_HTREENODE hItem);

	// Determine whether a node is a top level parent node
	//
	//  hItem	: Specifies the node
	//
	// Return TRUE if the item is a top level parent node of the tree, otherwise FALSE
	BOOL IsTopLevelParent(GTV_HTREENODE hItem);

	// Insert a tree node as well as a corresponding row
	//
	//  hParent		: Specifies the handle of the parent node
	//  hInsertAfter	: Specifies the handle of the item after which the new item is to be inserted
	//  nRowIndex	: Output parameter for retrieving the newly inserted items row index
	//
	// Return the handle of the inserted tree node if success, otherwise NULL
	GTV_HTREENODE InsertNode(GTV_HTREENODE hParent, GTV_HTREENODE hInsertAfter, /*OUT*/int& nRowIndex);

	// Deletes a node from the tree control as well as the corresponding row
	//
	//  hItem	: Specifies handle of the deleted item
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL DeleteNode(GTV_HTREENODE hItem);

	// Put a tree node under a new parent
	//
	//  hItem		: Specifies the handle of the node
	//  hNewParent	: Specifies the handle of the new parent item
	//  hInsertAfter	: Specifies the handle of the item after which the new item is to be inserted
	//
	// Return the row index
	int PutNode(GTV_HTREENODE hItem, GTV_HTREENODE hNewParent, GTV_HTREENODE hInsertAfter);

	// Remove a node from the tree control, but not delete the node
	//
	//  hItem	: Specifies handle of the removed item
	//  hParent	: Specifies the handle of the parent node
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL RemoveNode(GTV_HTREENODE hItem, GTV_HTREENODE hParent);

	// Shows an item only when bShow is TRUE, and the item state is not hidden (GTVIS_HIDDEN) and the item's parent is expanded, 
	// other wise hides the item
	// This method will not modify the GTVIS_HIDDEN item state flag
	//
	//  pTreeCell	: Specifies the cell
	//  bShow		: Specifies the show/hide mode
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL ShowVisibleTreeItemsOnly(CTreeCell* pTreeCell, BOOL bShow);

	// Shows all the child nodes as well as the corresponding row and modifies the GTVIS_HIDDEN item state flag
	//
	//  hParentItem	: Specifies handle of the parent item
	//  bExpanded	: Specifies whether the parent item is expanded
	//  nRowIndex	: Specifies the row index of the parent
	//  bShow		: Specifies whether to show/hide the node
	//
	void ShowChildNodes(GTV_HTREENODE hParentItem, BOOL bExpanded, int& nRowIndex, BOOL bShow);

	// Set the corresponding row
	// 
	//  hItem				: Specifies the node
	//  pCorrespondingRow	: Pointer of the GRID_ROW
	// 
	void SetCorrespondingRow(GTV_HTREENODE hItem, GRID_ROW* pCorrespondingRow);

	// Useful to find the range of children
	//
	//  hItem	: Specifies the node
	//
	// Returns the the last child's leaf, if next sibling row index = x, then GetUpperBound returns x-1 row item	
	GTV_HTREENODE GetUpperBound(GTV_HTREENODE hItem);

	// Inserts the corresponding rows of the item and it's child items in the specified index of (*m_RowData)
	//
	//  hItem			: Specifies the node
	//  nInsertAtIndex	: Output parameter for retrieving the attached index
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL AttachRows(GTV_HTREENODE hItem, /*[IN OUT]*/int& nInsertAtIndex);

	// Removes the corresponding rows of the item and it's child items from (*m_RowData)
	// 
	//  hItem				: Specifies the node
	//  bExcludeRoot			: Specifies whether the root node is excluded
	//  dataDetachedRows		: Pointer of the detached rows
	//  nTreeItemLevelFilter	: Specifies the tree item level filter
	//  heightDetachedRows	: Pointer of the detached rows height
	// 
	// Returns TRUE if successful, otherwise FALSE
	BOOL DetachRows(GTV_HTREENODE hItem, BOOL bExcludeRoot = FALSE, 
					CTypedPtrArray<CObArray, GRID_ROW*>* dataDetachedRows = NULL, 
					int nTreeItemLevelFilter = -1, CUIntArray* heightDetachedRows = NULL);

	// Sort the tree list based on a column
	// 
	//  pfnCompare	: Specifies the function pointer to the compare function
	//	 nColumn		: Specifies the actual column index
	//	 bAscending	: Specifies the sort direction
	//	 data			: Specifies the user data
	//  low			: Specifies the low value
	//  high			: Specifies the high value
	// 
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL SortItems(PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data, int low, int high);

	// Sort the tree list items based on a column
	//
	//  hParent		: Specifies the handle of the parent item
	//  pfnCompare	: Specifies the function pointer to the compare function
	//	 nColumn		: Specifies the actual column index
	//	 bAscending	: Specifies the sort direction
	//	 data			: Specifies the user data
	//  bRecursive	: Specifies the recursive mode
	// 
	// Returns TRUE if successful, otherwise FALSE
	virtual BOOL SortTreeItems(GTV_HTREENODE hParent, PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, 
								LPARAM data = 0, BOOL bRecursive = FALSE);

	// Sort the child tree list items based on a column
	//
	//  hCurItem		: Specifies the handle of the current item
	//  pfnCompare	: Specifies the function pointer to the compare function
	//	 nColumn		: Specifies the actual column index
	//	 bAscending	: Specifies the sort direction
	//	 data			: Specifies the user data
	// 
	// Returns TRUE if successful, otherwise FALSE
	BOOL SortChildItems(GTV_HTREENODE hCurItem, PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data);

	// Compare functions for tree cell
	// 
	//  pfnCompare	: CallBack Function
	//  bAscending	: Specifies the sort direction
	//  lParam1		: Specifies the first CGridCellBase object for comparing
	//  lParam2		: Specifies the second CGridCellBase object for comparing
	//  lParamSort	: Unused parameter
	//
	// Returns -1 if lParam1 value is less than lParam2 value, 1 if lParam1 value is greater than lParam2 value, 0 if equal
	static int CALLBACK pfnTreeCellCompare(PFNLVCOMPARE pfnCompare, BOOL bAscending, LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	// If somehow row orders get changed (usually after a sort applied or a node moved),
	// then from the corresponding rows. The order of Child Items in the Tree is updated
	// Update the child row order
	//
	//  hItem		: Specifies the item
	//  nRowIndex	: Output parameter for retrieving the row index
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL UpdateChildOrder(GTV_HTREENODE hItem, /*[IN OUT]*/ int& nRowIndex);

	// Update the child level order
	//
	//  hItem	: Specifies the item
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL UpdateChildLevel(GTV_HTREENODE hItem);

	// Should be called when a row is about to be deleted
	//
	//  nRow : Handle of row which is about to be deleted
	//
	virtual void OnDeletingRow(GV_ITEMHANDLE nRow);

	// Should be called when a column is about to be deleted
	//
	//  nColumn : Handle of column which is about to be deleted
	//
	virtual void OnDeletingColumn(int nColumn);

	// The item with more depth (i.e greater tree item level) must be the first argument
	//
	//  hItem1		: Specifies the handle of the item1
	//  hItem2		: Specifies the handle of the item2
	//  hLastItem1	: Specifies the handle of last the item1
	//  hLastItem2	: Specifies the handle of the last item2
	//
	// Return the handle of common root if found, otherwise NULL
	GTV_HTREENODE FindCommonRoot(GTV_HTREENODE hItem1, GTV_HTREENODE hItem2, 
									GTV_HTREENODE& hLastItem1, GTV_HTREENODE& hLastItem2);

	// Draw the tree list control
	//
	//  pDC	: Specifies the device context
	//
	virtual void OnDraw(CDC* pDC);

	// Find out if the tree node was hit or not
	//
	//  nFlags	: Specifies the mouse flags
	//   point	: Specifies the coordinates which must be tested in screen coordinates
	//  
	// Returns TRUE if point defines a position over the button, otherwise FALSE
	virtual BOOL OnTreeHitTest(UINT nFlags, CPoint point);

	// Show the title tip
	//
	//  pCell			: Specifies the pointer of the cell
	//  nRow				: Specifies the row index
	//  nColumn			: Specifies the column index
	//  rectCell			: Specifies the rectangle of the cell
	//  rectTitle		: Specifies the rectangle of the title tip
	//  lpszTitleText	: Specifies the title tip text
	//  xoffset			: Specifies the offset value
	//  lpHoverRect		: Specifies the rectangle when the cell is hover
	//  lpLogFont		: Pointer of the log-font structure
	//  crTextClr		: Specifies the text color
	//  crBackClr		: Specifies the background color
	//
	virtual void ShowTitleTip(CGridCellBase* pCell, int nRow, int nColumn,
								CRect rectCell, CRect rectTitle, LPCTSTR lpszTitleText, 
								int xoffset = 0, LPRECT lpHoverRect = NULL, const LOGFONT* lpLogFont = NULL,
								COLORREF crTextClr = CLR_DEFAULT, COLORREF crBackClr = CLR_DEFAULT);

	// Enables the fast row look up mode so that FindRowIndex will fast look up row index of a given node
	// It also improves vastly the performance of Node Expand and Collapse 
	// However this mode is turned off by default so that the behavior remains the same as before
	//
	//  bEnable	: Specifies whether the fast row look up is enabled
	//
	void EnableFastRowLookup(BOOL bEnable = TRUE);

	// Determines if fast row look up mode is enabled
	//
	// Returns TRUE if fast row look up mode is enabled, FALSE otherwise
	BOOL IsEnabledFastRowLookup();

	// Determines if clip rect should be checked during drawing
	// Vertical Line drawing demands that click rect shouldn't be checked
	virtual BOOL CheckClipRect(int /*row*/, int /*col*/)
	{
		return FALSE;
	}

	// Shows or hides a row, as well as modify attributes of a node
	//
	//  hItem	: Specifies the node
	//  nRow		: Specifies the row
	//  bShow	: Specifies whether to show or hide the row
	//
	// Returns TRUE if successful, otherwise FALSE
	BOOL ShowRow(GTV_HTREENODE hItem, int nRow, BOOL bShow);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

protected:
	// Map for the detached rows height
	CMapPtrToInt m_mapDetachRowHeights;

	// Handle of the root node item
	GTV_HTREENODE m_hRootNode;

	// Color for the vertical line
	COLORREF m_clrTreeVertLine;

	// Color for the horizontal line
	COLORREF m_clrTreeHorzLine;

	// Determines if FindRowIndex will fast look up row index of a given node using item, subitem
	// Only problem with it is, during sorting we cannot use the item, subitem for row look up of a given node
	BOOL m_bFastRowLookupMode;

	// Determines if DeleteAllItems is activated
	BOOL m_bDeletingAllItems;

	// Determines if tree hit test is successful (useful in OnLButtonUp)
	BOOL m_bTreeHitTest;

	// Holds the allocated row created in SetChildItemCount()
	CArray<GRID_ROWS, GRID_ROWS> m_arAllocatedRows;
	// Holds the allocated nodes created in SetChildItemCount()
	CArray<GTV_TREENODESPTR, GTV_TREENODESPTR> m_arAllocatedNodes;

	// Pointer of runtime class of tree column cell
	CRuntimeClass* m_pRtcTreeColumnDefault;
	//
	// CGridCellBase Creation and Cleanup

	// Create a cell in the memory
	//
	//  nItem	: Specifies the row index of the cell
	//  nSubItem	: Specifies the column index of the cell
	//
	// Returns the pointer of the newly created cell pointer, if failed return NULL
	virtual CGridCellBase* CreateCell(int nItem, int nSubItem);

	CMap<GTV_HTREENODE, GTV_HTREENODE, int, int>m_mapHiddenRowHeights;

	int m_nTreeCol;

};

#endif // !defined(__TreeGridCtrl_H__)
