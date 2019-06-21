//
// TreeGridCtrl.cpp : implementation file
//

#include "StdAfx.h"


#include "TreeGridCtrl.h"

#include "TreeGridCell.h"
#include "TreeGridCellCheck.h"


#if _MSC_VER < 1400
	#define ULONG_PTR ULONG
#endif

//
// declared in the header as extern
//
GTV_HTREENODE GLVI_ROOT  = ((GTV_HTREENODE)(ULONG_PTR)-0x10000);
GTV_HTREENODE GLVI_FIRST = ((GTV_HTREENODE)(ULONG_PTR)-0x0FFFF);
GTV_HTREENODE GLVI_LAST  = ((GTV_HTREENODE)(ULONG_PTR)-0x0FFFE);
//
//
//

#define __PFNTREECOMPARE(pfnCompare, bAscending, lParam1, lParam2, lParamSort) pfnTreeCellCompare(pfnCompare, bAscending, lParam1, lParam2, lParamSort)

CTreeGridCtrl::CTreeGridCtrl()
{
	m_pRtcDefault = RUNTIME_CLASS(CTreeGridCell);
	m_pRtcTreeColumnDefault = NULL;

	//m_clrTreeVertLine = CLR_DEFAULT;
	//m_clrTreeHorzLine = CLR_DEFAULT;
	m_clrTreeVertLine = RGB(220, 0, 50);
	m_clrTreeHorzLine = RGB(0, 0, 250);
	
	m_bFastRowLookupMode = TRUE;
	m_bDeletingAllItems  = FALSE;
	m_nTreeCol = 1;

	CreateRootNode();
}

CTreeGridCtrl::~CTreeGridCtrl()
{
	DeleteAllItems();
	delete m_hRootNode;
}

BEGIN_MESSAGE_MAP(CTreeGridCtrl, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//
// User Defined Operations
//

CTreeCell* CTreeGridCtrl::GetTreeCell(GTV_HTREENODE hItem)
{
	CTreeCell* pTreeCell = NULL;
	if( (hItem != NULL) && (hItem->m_pCorrespondingRow != NULL) )
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeColActual = m_nTreeCol;
		if( (GetColumnCount() + nTreeColActual) > nTreeColActual)
		{
			pTreeCell = TREECELL(hItem->m_pCorrespondingRow->GetAt(nTreeColActual));
		}
	}
	
	return pTreeCell;
}

int CTreeGridCtrl::FindRowIndex(GTV_HTREENODE hNode)
{
	int nItem = GetFixedRowCount()-1;

	if( (hNode != NULL ) && (hNode->m_pCorrespondingRow != NULL) )
	{
		if(!IsEnabledFastRowLookup())
		{
			for (int row = 0; row < m_nRows; row++)
			{
				GRID_ROW* pRow = (*m_pRowData)[row];
				if(pRow == hNode->m_pCorrespondingRow)
				{
					nItem = row;
					break;
				}
			}
		}
		else
		{
			nItem = GetTreeCell(hNode)->GetItemID();			
		}
	}

	return nItem;
}

BOOL CTreeGridCtrl::FindChildrenRowRange(GTV_HTREENODE hParentNode, CItemRange& range)
{
	BOOL bRet = FALSE;
	if(hParentNode != NULL && !hParentNode->IsLeaf())
	{
		int nStartRowIndex = -1;
		int nEndRowIndex = -1; 

		// Compute Start Index (the start index will be just one more than the item itself)
		nStartRowIndex = FindRowIndex(hParentNode) + 1;

		// Compute End Index
		// First check if it has a next sibling, then it would be easier to find the row range	
		if(hParentNode->m_hNextSibling != NULL)
		{
			nEndRowIndex = FindRowIndex(hParentNode->m_hNextSibling) - 1;
		}
		else
		{
			GTV_HTREENODE hChildItem  = GetUpperBound(hParentNode);
			nEndRowIndex = FindRowIndex(hChildItem);
		}

		range.SetMinRow(nStartRowIndex);
		range.SetMaxRow(nEndRowIndex);
		bRet = TRUE;
	}

	return bRet;
}

// Create a CGridCtrl Row as well as a SuperGridNode 
GTV_HTREENODE CTreeGridCtrl::InsertItem(LPCTSTR strHeading, GTV_HTREENODE hParent/*=GLVI_ROOT*/,
				GTV_HTREENODE hInsertAfter/*=GLVI_LAST*/ , BOOL bRefresh/*=TRUE*/)
{
	int nRowIndex = -1;
	GTV_HTREENODE hNewItem = InsertNode(hParent, hInsertAfter, nRowIndex);
	if(nRowIndex > -1)
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;
		SetCellText(nRowIndex, nTreeCol, strHeading);

		if(bRefresh)
			Refresh();
	}
	
	return hNewItem;
}

GTV_HTREENODE CTreeGridCtrl::InsertItem( GV_ITEM* pItem, GTV_HTREENODE hParent/*=GLVI_ROOT*/,
				GTV_HTREENODE hInsertAfter/*=GLVI_LAST*/, BOOL bRefresh/*=TRUE*/)
{
	int nRowIndex = -1;
	GTV_HTREENODE hNewItem = InsertNode(hParent, hInsertAfter, nRowIndex);
	if(nRowIndex  > -1)
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;

		// Specify at which row the item should be set
		pItem->row = nRowIndex;
		pItem->col = nTreeCol;

		SetItem(pItem);

		if(bRefresh)
			Refresh();
	}
	
	return hNewItem;
}

// delete the corresponding CGridCtrl Row as well as the SuperGridNode 
BOOL CTreeGridCtrl::DeleteItem(GTV_HTREENODE hNode)
{
	BOOL bRet =	TRUE;
	if(hNode != NULL)
	{		
		bRet &= DeleteNode(hNode);
	}
	else
	{
		bRet = FALSE;
	}
	
	if (bRet)
	{
		CGridCtrl::Refresh();
	}

	return bRet;
}

BOOL CTreeGridCtrl::DeleteAllItems()
{
	m_bDeletingAllItems = TRUE;	
	
	SetRedraw(FALSE);

	BOOL bRet = TRUE;
	

	// Assuming the first column as the column to show the tree hierarchy
	int nTreeCol = m_nTreeCol;

	///////////////////////Take out the non deletable rows/////////////////////	
	GRID_ROW* pRow = NULL;
	GTV_HTREENODE hItem = NULL;
	for(int row = GetItemCount() - 1; row >= 0; row--)
	{
		CTreeCell* pCell = TREECELL(GetCell(row, nTreeCol));
		if(pCell != NULL)
		{
			hItem = pCell->GetTreeItem();
			if(hItem != NULL)
			{
				if(!GTV_GETATTRIB_DELETABLE(hItem))
				{
					pRow = (*m_pRowData)[row];
					for (int col = 0; col < m_nCols; col++)
						DestroyCell(row, col);
				
					(*m_pRowData).RemoveAt(row);
					(*m_pArrayRowHeights).RemoveAt(row);

					m_nRows--;
				}
				else
				{
					delete hItem;
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////
	bRet &= CGridCtrl::DeleteAllItems();	

	// Remove the allocated nodes resulting from SetChildCount() (if invoked)
	INT_PTR nSize = m_arAllocatedNodes.GetSize();
	INT_PTR i;
	for(i = 0; i < nSize; i++)
	{
		delete[] m_arAllocatedNodes[i];
	}
	m_arAllocatedNodes.RemoveAll();

	nSize = m_arAllocatedRows.GetSize();
	for(i = 0; i < nSize; i++)
	{
		delete[] m_arAllocatedRows[i];
	}
	m_arAllocatedRows.RemoveAll();

	GetRootNode()->m_arChildNodes.RemoveAll();	
	
	SetRedraw(TRUE);
	
	m_bDeletingAllItems = FALSE;

	return bRet;
}

BOOL CTreeGridCtrl::MoveTreeNode(GTV_HTREENODE hNode, GTV_HTREENODE hNewParent, GTV_HTREENODE hInsertAfter/*=GLVI_LAST*/)
{
	BOOL bRet =	FALSE;
	if( (hNode != NULL) && (hNewParent != NULL) && (hNode != hNewParent) )
	{
		int nStartAt = -1;

		BOOL bFastRowLookup = IsEnabledFastRowLookup();
		EnableFastRowLookup(FALSE);
		
		if(DetachRows(hNode))
		{
			if(RemoveNode(hNode, hNode->m_hParent))
			{
				int nInsertAtIndex = PutNode(hNode, hNewParent, hInsertAfter);
				if(nInsertAtIndex > -1)
				{
					nStartAt = nInsertAtIndex;

					if(AttachRows(hNode, nInsertAtIndex))
					{
						UpdateChildLevel(hNewParent);
						bRet = TRUE;

						Invalidate();
					}
				}
			}
		}

		////////////////////////////////Coord settings for fast row look up mode/////////////////////////////////////////
		EnableFastRowLookup(bFastRowLookup);

		if(bFastRowLookup)
		{
			// Since fast row look up mode depend upon row/item value, so we need to update the item value after the move item operation is over
			
			// Assuming the first column as the column to show the tree hierarchy
			int nTreeCol = m_nTreeCol;

			// Update the item, subitem info properly
			for(int i = nStartAt; i < GetItemCount(); i++)
			{
				CGridCell* pCell = (CGridCell*)GetCell(i, nTreeCol);
				if(pCell != NULL)
				{
					pCell->SetCoords(i, nTreeCol);
				}
			}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	return bRet;
}

BOOL CTreeGridCtrl::ExpandTreeNode(GTV_HTREENODE hNode)
{
	ASSERT(hNode);

	SetRedraw(FALSE);
	BOOL bRet = FALSE;
	CTreeCell* pTreeCell = GetTreeCell(hNode);
	if(pTreeCell != NULL)
	{
		pTreeCell->SetExpanded(TRUE);
		bRet = ShowVisibleTreeItemsOnly(pTreeCell, TRUE);
	}
	SetRedraw(TRUE);

	ResetScrollBars();
	
	return bRet;
}

BOOL CTreeGridCtrl::CollapseTreeNode(GTV_HTREENODE hNode)
{
	ASSERT(hNode);

	SetRedraw(FALSE);
	BOOL bRet = FALSE;
	CTreeCell* pTreeCell = GetTreeCell(hNode);
	if(pTreeCell != NULL)
	{
		pTreeCell->SetExpanded(FALSE);	
		bRet = ShowVisibleTreeItemsOnly(pTreeCell, FALSE);
	}
	SetRedraw(TRUE);

	ResetScrollBars();

	return bRet;
}

void CTreeGridCtrl::ShowTreeNodeButton(GTV_HTREENODE hNode, BOOL bShow/* = TRUE*/)
{
	GTV_SETATTRIB_HASBUTTON(hNode, bShow);
}

BOOL CTreeGridCtrl::IsShownTreeNodeButton(GTV_HTREENODE hNode)
{
	return GTV_GETATTRIB_HASBUTTON(hNode);
}

GTV_HTREENODE CTreeGridCtrl::FindCommonRoot(GTV_HTREENODE hItem1, GTV_HTREENODE hItem2, GTV_HTREENODE& hLastItem1, GTV_HTREENODE& hLastItem2)
{
	GTV_HTREENODE hCommonRoot = NULL;
	if(hItem1->m_hParent == hItem2->m_hParent)
	{
		hLastItem1 = hItem1;
		hLastItem2 = hItem2;
		
		hCommonRoot = hItem1->m_hParent;
	}
	else if(hItem1->m_hParent == NULL)
	{
		hCommonRoot = NULL;
	}
	else if(hItem2->m_hParent == NULL)
	{
		hCommonRoot = NULL;
	}
	else
	{
		hCommonRoot = FindCommonRoot(hItem1->m_hParent, hItem2, hLastItem1, hLastItem2);
		if(hCommonRoot == NULL)
		{
			hCommonRoot = FindCommonRoot(hItem1, hItem2->m_hParent, hLastItem1, hLastItem2);
		}
	}

	return hCommonRoot;
}

int CALLBACK CTreeGridCtrl::pfnTreeCellCompare(PFNLVCOMPARE pfnCompare, BOOL bAscending, LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNUSED_ALWAYS(bAscending);

	return pfnCompare(lParam1, lParam2, lParamSort);
}

BOOL CTreeGridCtrl::SortItems(GTV_HTREENODE hParent, int nColumn, BOOL bAscending, LPARAM data/*=0*/, BOOL bRecursive/*=TRUE*/)
{
	if( hParent == NULL || (nColumn < 0 || nColumn >= GetColumnCount()) )
		return FALSE;
	
	return SortTreeItems(hParent, NULL, nColumn, bAscending, data, bRecursive);
}

BOOL CTreeGridCtrl::SortItems(GTV_HTREENODE hParent, PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data/*=0*/, BOOL bRecursive/*=TRUE*/)
{
	if( hParent == NULL || (nColumn < 0 || nColumn >= GetColumnCount()) )
		return FALSE;

	return SortTreeItems(hParent, pfnCompare, nColumn, bAscending, data, bRecursive);
}

BOOL CTreeGridCtrl::SortTreeItems(GTV_HTREENODE hParent, PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data/*=0*/, BOOL bRecursive/*=FALSE*/)
{		
	// Extra code put in for Fast Row Lookup mode
	static GTV_HTREENODE hTopParent = NULL;
	if(hTopParent == NULL)
		hTopParent = hParent;

	BOOL bFastRowLookup = IsEnabledFastRowLookup();
	EnableFastRowLookup(FALSE);

	BOOL bRet = SortChildItems(hParent, pfnCompare, nColumn, bAscending, data);

	if(bRecursive)
	{
		GTV_HTREENODE hChildItem  = NULL;
		INT_PTR nSize = hParent->m_arChildNodes.GetSize();
		for(INT_PTR i = 0; i < nSize; i++)
		{
			hChildItem = hParent->m_arChildNodes[i];
			bRet |= SortTreeItems(hChildItem, pfnCompare, nColumn, bAscending, data, bRecursive);

			CString str = hChildItem->m_pCorrespondingRow->GetAt(m_nTreeCol)->GetText();
			OutputDebugString(str);
			OutputDebugString(_T("\n"));
		}
	}

	EnableFastRowLookup(bFastRowLookup);

	// Since fast row look up mode depend upon row/item value, so we need to update the item value after the sorting is over
	if(hParent == hTopParent)
	{
		hTopParent = NULL;
		if(bFastRowLookup)
		{
			// Assuming the first column as the column to show the tree hierarchy
			int nTreeCol = m_nTreeCol;

			// Update the item, subitem info properly
			for(int i = 0; i < GetItemCount(); i++)
			{
				CGridCell* pCell = (CGridCell*)GetCell(i, nTreeCol);
				if(pCell != NULL)
				{
					pCell->SetCoords(i, nTreeCol);
				}
			}
		}
	}

	return bRet;
}

BOOL CTreeGridCtrl::SortChildItems(GTV_HTREENODE hCurItem, PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data)
{
	CTypedPtrArray<CObArray, GRID_ROW*>* rowsToSort = new CTypedPtrArray<CObArray, GRID_ROW*>;
	CUIntArray* arHeights = new CUIntArray;

	CTypedPtrArray<CObArray, GRID_ROW*>* pOriginalRowData = m_pRowData;
	CUIntArray* pOriginalRowHeights = m_pArrayRowHeights;

	//DetachRows(hCurItem, TRUE, rowsToSort, (int)(hCurItem->m_lAttributes + 1), arHeights);
	DetachRows(hCurItem, TRUE, rowsToSort, (int)(GTV_GETATTRIB_TREEITEMLEVEL(hCurItem) + 1), arHeights);

	int nRows = m_nRows;
	
	// take care of the header row
	(*rowsToSort).InsertAt(0, (*pOriginalRowData)[0]);
	(*arHeights).InsertAt(0, (*pOriginalRowHeights)[0]);

	m_pRowData = rowsToSort;
	m_pArrayRowHeights = arHeights;
	m_nRows = (int)((*rowsToSort).GetSize());

	m_idTopLeftCell.row = -1;
	
	BOOL bRet = FALSE;
	if (pfnCompare == NULL)
	{
		bRet = CGridCtrl::SortItems(nColumn, bAscending, data);
	}
	else
	{
		bRet = CGridCtrl::SortItems(pfnCompare, nColumn, bAscending, data);
	}

	m_pRowData = pOriginalRowData;
	m_pArrayRowHeights = pOriginalRowHeights;
	m_nRows = nRows;

	int nRowIndex = FindRowIndex(hCurItem);
	int nInsertAtIndex = nRowIndex + 1;

	// Assuming the first column as the column to show the tree hierarchy
	int nTreeCol = m_nTreeCol;

	CTreeCell* pTreeCell = NULL;

	GTV_HTREENODE hItem = NULL;

	// GridCtrl Rows are sorted, therefore change the order of the child 
	// in actual tree data structure
	hCurItem->m_arChildNodes.RemoveAll();

	for(INT_PTR i = 1; i < (*rowsToSort).GetSize(); i++)
	{
		pTreeCell =	TREECELL((*rowsToSort)[i]->GetAt(nTreeCol));
		hItem = pTreeCell->m_hTreeItem;	
		hItem->m_pCorrespondingRow = (*rowsToSort)[i];
		hCurItem->m_arChildNodes.Add(hItem);

		// Update Sibling Info
		hItem->m_hPrevSibling = NULL;
		hItem->m_hNextSibling = NULL;
		if(i > 1)
		{
			pTreeCell = TREECELL((*rowsToSort)[i-1]->GetAt(nTreeCol));
			hItem->m_hPrevSibling = pTreeCell->m_hTreeItem;
			hItem->m_hPrevSibling->m_hNextSibling = hItem;
		}
		AttachRows(hItem, nInsertAtIndex);
	}

	delete rowsToSort;
	delete arHeights;
	
	return bRet;
}

void CTreeGridCtrl::TraverseNodes(GTV_HTREENODE hParentItem, int& nRowIndex)
{
	CTreeCell* pTreeCell = NULL;	

	// Assuming the first column as the column to show the tree hierarchy
	int nTreeCol = m_nTreeCol;
	for(int i = 0; i < hParentItem->m_arChildNodes.GetSize(); i++)
	{
		nRowIndex++;
		
		pTreeCell = TREECELL(GetCell(nRowIndex, nTreeCol));
		if(pTreeCell != NULL)
		{
			GTV_HTREENODE hItem = (hParentItem->m_arChildNodes[i]);

			//OutputDebugString(_T("Begin Node\n"));

			//OutputDebugString(_T("Node : "));
			//OutputDebugString(pTreeCell->GetText());
			//OutputDebugString(_T("\n"));

			SendMessageToParent(nRowIndex, nTreeCol, GTVN_ITEMTRAVERSE);

			/*if(hItem->m_hPrevSibling != NULL)
			{
				pTreeCell = GetTreeCell(hItem->m_hPrevSibling);
				OutputDebugString(_T("Prev Node : "));
				OutputDebugString(pTreeCell->GetText());
			}
			else
			{
				OutputDebugString(_T("Prev Node : NULL"));
			}
			OutputDebugString(_T("\n"));

			if(hItem->m_hNextSibling != NULL)
			{
				pTreeCell = GetTreeCell(hItem->m_hNextSibling);
				OutputDebugString(_T("Next Node : "));
				OutputDebugString(pTreeCell->GetText());
			}
			else
			{
				OutputDebugString(_T("Next Node : NULL"));
			}
			OutputDebugString(_T("\n"));		
			OutputDebugString(_T("End Node\n"));		

			OutputDebugString(_T("\n"));
			OutputDebugString(_T("\n"));
			*/

			TraverseNodes(hItem, nRowIndex);
		}
	}
}

void CTreeGridCtrl::SetTreeLineColors(COLORREF clrTreeVertLine, COLORREF clrTreeHorzLine)
{
	m_clrTreeVertLine = clrTreeVertLine;
	m_clrTreeHorzLine = clrTreeHorzLine;
}

GTV_HTREENODE CTreeGridCtrl::CreateNode()
{
	GTV_HTREENODE hNewItem = new CTreeNode();
	hNewItem->m_hParent = NULL;
	hNewItem->m_hNextSibling = NULL;
	hNewItem->m_hPrevSibling = NULL;
	hNewItem->m_lParam = NULL;
	hNewItem->m_pCorrespondingRow = NULL;
	hNewItem->m_lAttributes = 0;
	GTV_SETATTRIB_TREEITEMLEVEL(hNewItem, UNINITALIZED_TREEITEM_LEVEL);
	hNewItem->m_arChildNodes.RemoveAll();
	
	return hNewItem;
}

GTV_TREENODESPTR CTreeGridCtrl::CreateNodes(int nCount)
{
	GTV_TREENODESPTR hNewItems = new CTreeNode[nCount];
	return hNewItems;
}

GTV_HTREENODE CTreeGridCtrl::CreateRootNode()
{
	m_hRootNode = CreateNode();
	m_hRootNode->m_lAttributes = 0;
	GTV_SETATTRIB_TREEITEMLEVEL(m_hRootNode, ROOT_TREEITEM_LEVEL);

	return m_hRootNode;
}

BOOL CTreeGridCtrl::DeleteNode(GTV_HTREENODE hItem)
{
	BOOL bRet =	TRUE;
	if(hItem != NULL)
	{
		if(!hItem->IsLeaf())
		{
			//while(hItem->m_arChildNodes.GetSize() > 0)
			INT_PTR nSize = hItem->m_arChildNodes.GetSize();
			for(INT_PTR i = nSize - 1; i >=0; i--)
			{
				bRet &= DeleteNode(hItem->m_arChildNodes[i]);
			}
		}

		BOOL bDeletable = GTV_GETATTRIB_DELETABLE(hItem);
		
		//if(!m_bDeletingAllItems)
		{
			RemoveNode(hItem, hItem->m_hParent);

			int nItem = FindRowIndex(hItem);
			if(nItem >= 0)
			{
				if(bDeletable)
				{
					bRet &= CGridCtrl::DeleteRow(nItem);
					delete hItem;
				}
				else
				{
					int row = nItem;

					for (int col = 0; col < m_nCols; col++)
						DestroyCell(row, col);
				
					(*m_pRowData).RemoveAt(row);
					(*m_pArrayRowHeights).RemoveAt(row);

					m_nRows--;
				}
				
				if(IsEnabledFastRowLookup())
				{
					// Assuming the first column as the column to show the tree hierarchy
					int nTreeCol = m_nTreeCol;

					// Decrement the item value of the cells of next following rows 
					for(int i = nItem; i < GetItemCount(); i++)
					{
						CGridCell* pCell = (CGridCell*)GetCell(i, nTreeCol);
						if(pCell != NULL)
						{
							pCell->SetCoords(i, nTreeCol);
						}
					}
				}
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

GTV_HTREENODE CTreeGridCtrl::GetUpperBound(GTV_HTREENODE hItem)
{
	if(hItem != NULL)
	{
		if(hItem->IsLeaf())
		{
			return hItem;
		}

		// Find the last child
		GTV_HTREENODE hChildItem  = NULL;
		INT_PTR nSize = hItem->m_arChildNodes.GetSize();
		for(INT_PTR i = 0; i < nSize; i++)
		{
			hChildItem = hItem->m_arChildNodes[i];
		}

		return GetUpperBound(hChildItem);
	}

	return NULL;
}

// Insert a SuperGridNode structure only, doesn't create a CGridCtrl Row
GTV_HTREENODE CTreeGridCtrl::InsertNode(GTV_HTREENODE hParent, GTV_HTREENODE hInsertAfter, /*OUT*/int& nRowIndex)
{
	if(hParent == GLVI_ROOT)
	{
		hParent = m_hRootNode;
	}

	GTV_HTREENODE hNewItem = NULL;
	GTV_HTREENODE hNextItem = NULL;
	GTV_HTREENODE hPrevItem = hInsertAfter;

	if(hParent != NULL)
	{
		if(!hParent->IsLeaf())
		{
			if(hInsertAfter == GLVI_FIRST)
			{
				// Insert just after the row of the parent
				hPrevItem = NULL;
				// get a pointer to the first child item, because it will be the m_hNextSibling of the new Child 
				hNextItem = hParent->m_arChildNodes[0];
			}
			else if(hInsertAfter == GLVI_LAST)
			{
				// Insert just after the row of the last child
				hPrevItem = hParent->m_arChildNodes[hParent->m_arChildNodes.GetSize() - 1];
			}
		}
		else
		{
			// Insert just after the row of the parent
			hPrevItem = NULL;
		}
		
		hNewItem = CreateNode();
		if(hNewItem != NULL)
		{
			hNewItem->m_hParent = hParent;
			// hPrevItem = NULL implies the parent is a leaf node 
			// or hInsertAfter = GLVI_FIRST when parent is not a leaf node
			if(hPrevItem == NULL)
			{
				// If previous item is NULL, then the newly created item
				// can only have a next sibling, but not a prev sibling
				hNewItem->m_hPrevSibling = NULL;
				hNewItem->m_hNextSibling = hNextItem;

				// Insert just after parent
				//nRowIndex = FindRowIndex(hParent) + 1;
				if (hParent->IsLeaf())
				{
					nRowIndex = FindRowIndex(hParent) + 1;
				}
				else
				{
					CItemRange range;
					FindChildrenRowRange(hParent, range);
					//nRowIndex = range.GetMaxRow() + 1;
					nRowIndex = range.GetMinRow();
				}
			}
			else
			{
				hNewItem->m_hPrevSibling = hPrevItem;
				//hPrevItem->m_hNextSibling = hNewItem;
				
				//nRowIndex = FindRowIndex(hPrevItem) + 1;
				if (hPrevItem->IsLeaf())
				{
					nRowIndex = FindRowIndex(hPrevItem) + 1;
				}
				else
				{
					CItemRange range;
					FindChildrenRowRange(hPrevItem, range);
					nRowIndex = range.GetMaxRow() + 1;
				}

				hPrevItem->m_hNextSibling = hNewItem;
			}

			INT_PTR nNodeIndex = -1;
	
			if(hInsertAfter == GLVI_FIRST)
			{
				// If the new node is the first item
				nNodeIndex = 0;
			}
			else if(hInsertAfter == GLVI_LAST)
			{	// If the new node should be the last item
				nNodeIndex = (int)(hParent->m_arChildNodes.GetSize());
			}
			else if(hPrevItem != NULL)
			{
				INT_PTR size = hParent->m_arChildNodes.GetSize();

				for(INT_PTR i = 0; i < size; i++)
				{
					if(hParent->m_arChildNodes[i] == hPrevItem)
					{
						// Insert after hPrevItem which in this case is equal to hInsertAfter
						nNodeIndex = i + 1;
						break;
					}
				}
			}

			if(nNodeIndex > -1)
			{
				//hNewItem->m_lAttributes = hParent->m_lAttributes + 1;
				GTV_SETATTRIB_TREEITEMLEVEL(hNewItem, (GTV_GETATTRIB_TREEITEMLEVEL(hParent) + 1));
				hParent->m_arChildNodes.InsertAt(nNodeIndex, hNewItem);
			}
			else
			{
				DeleteNode(hNewItem);
				hNewItem = NULL;
				nRowIndex = -1;
			}
		}
	}
	
	/////////////////////////////////// Code moved from InsertItem/////////////////////
	// if Node insertion is successful then insert a new row

	if(GetRowCount() == m_nFixedRows)
	{
		nRowIndex = m_nFixedRows;
	}

	if(hNewItem != NULL)
	{
		GV_ITEMHANDLE nItemHandle = CGridCtrl::InsertRow(_T(""), nRowIndex);
		// The CGridCtrl::InsertItem() return the nRowIndex if the item inserted successfully
		if (nItemHandle == nRowIndex)
		{
			if(GTV_GETATTRIB_COLLAPSED(hParent) || GTV_GETATTRIB_NOTVISIBLE(hParent))
			{
				ShowRow(hNewItem, nRowIndex, FALSE);
			}			
		}

		if(nItemHandle > -1)
		{
			SetCorrespondingRow(hNewItem, (*m_pRowData)[nItemHandle]);

			if(IsEnabledFastRowLookup())
			{
				// Assuming the first column as the column to show the tree hierarchy
				int nTreeCol = m_nTreeCol;		
					
				// Increment the item value of the cells of next following rows 
				for(int i = nRowIndex + 1; i < GetItemCount(); i++)
				{
					CGridCell* pCell = (CGridCell*)GetCell(i, nTreeCol);
					if(pCell != NULL)
					{
						pCell->SetCoords(i, nTreeCol);
					}
				}
			}
		}
		else
		{
			DeleteNode(hNewItem);
			hNewItem = NULL;

			nRowIndex = -1;
		}
	}	

	return hNewItem;
}

void CTreeGridCtrl::SetCorrespondingRow(GTV_HTREENODE hItem, GRID_ROW* pCorrespondingRow)
{
	hItem->m_pCorrespondingRow = pCorrespondingRow;
	
	CTreeCell* pTreeCell = GetTreeCell(hItem);
	if (pTreeCell != NULL)
	{
		//pTreeCell->m_nTreeItemLevel = (int)hItem->m_lAttributes;
		pTreeCell->m_nTreeItemLevel = (int)(GTV_GETATTRIB_TREEITEMLEVEL(hItem));

		// Now set the Tree Item of all the cells in the row
		CTreeCell* pCell = NULL;
		for(int nCol = 0; nCol < hItem->m_pCorrespondingRow->GetSize(); nCol++)
		{
			pCell = TREECELL(hItem->m_pCorrespondingRow->GetAt(nCol));
			pCell->m_hTreeItem = hItem;
		}
	}
}

BOOL CTreeGridCtrl::AttachRows(GTV_HTREENODE hItem, /*[IN OUT]*/int& nInsertAtIndex)
{
	BOOL bRet = FALSE;

	if(hItem != NULL)
	{
		// convert to actual index
		//int nIndex = nInsertAtIndex + nHLVRowOffset;
		int nIndex = nInsertAtIndex;

		if( (nInsertAtIndex > -1) && (nIndex <= (*m_pRowData).GetSize()) )
		{
			if(hItem->m_pCorrespondingRow != NULL)
			{
				(*m_pRowData).InsertAt(nIndex, hItem->m_pCorrespondingRow);

				// Most important thing to remember, row heights array should be addressed as well
				// Assuming the first column as the column to show the tree hierarchy 
				int nTreeColActual = m_nTreeCol;
				CTreeCell* pTreeCell = TREECELL(hItem->m_pCorrespondingRow->GetAt(nTreeColActual));
				if (pTreeCell == NULL)
				{
					return FALSE;
				}
				void* key = (void*)pTreeCell;

				int rValue;
				if(m_mapDetachRowHeights.Lookup(key, rValue))
				{
					(*m_pArrayRowHeights).InsertAt(nIndex, (UINT) rValue);
					m_mapDetachRowHeights.RemoveKey(key);
				}
				else
				{
					ASSERT(FALSE); // this should never happen
					(*m_pArrayRowHeights).InsertAt(nIndex, 30);
				}

				nInsertAtIndex++;
				// Also don't forget to increment the row count
				m_nRows++;
			}

			bRet = TRUE;
			if(!hItem->IsLeaf())
			{
				for(INT_PTR i = 0; i < hItem->m_arChildNodes.GetSize(); i++)
				{
					bRet &= AttachRows(hItem->m_arChildNodes[i], nInsertAtIndex);
				}
			}
		}
	}

	return bRet;
}

BOOL CTreeGridCtrl::DetachRows(GTV_HTREENODE hItem, BOOL bExcludeRoot/*=FALSE*/, CTypedPtrArray<CObArray, GRID_ROW*>* dataDetachedRows/*=NULL*/, int nTreeItemLevelFilter/*=-1*/, CUIntArray* heightDetachedRows/*= NULL*/)
{
	BOOL bRet = FALSE;

	if(hItem != NULL)
	{
		CItemRange range;
		if(FindChildrenRowRange(hItem, range))
		{
			// include the hItem's row as well in the row range if it's not specified to be excluded
			if(!bExcludeRoot)
			{
				//range.m_nMinItem -= 1;
				range.SetMinRow(range.GetMinRow() - 1);
			}
		}
		else
		{
			int nRowIndex = FindRowIndex(hItem);

			//range.m_nMinItem = nRowIndex;
			//range.m_nMaxItem = nRowIndex;
			range.SetMinRow(nRowIndex);
			range.SetMaxRow(nRowIndex);

			if(bExcludeRoot)
			{
				//range.m_nMinItem = -1;
				range.SetMinRow(GetFixedRowCount() - 1);
			}
		}

		//if(range.m_nMinItem > -1)
		if(range.GetMinRow() > GetFixedRowCount() - 1)
		{
			// convert to actual index
			//range.m_nMinItem += nHLVRowOffset;
			//range.m_nMaxItem += nHLVRowOffset;

			//int count = range.m_nMaxItem - range.m_nMinItem + 1;
			int count = range.GetMaxRow() - range.GetMinRow() + 1;

			if(dataDetachedRows != NULL)
			{
				(*dataDetachedRows).RemoveAll();
				if(heightDetachedRows != NULL)
				{
					(*heightDetachedRows).RemoveAll();
				}
			}

			//for(INT_PTR i = range.m_nMinItem; i <= range.m_nMaxItem; i++)
			for(INT_PTR i = range.GetMinRow(); i <= range.GetMaxRow(); i++)
			{
				// Assuming the first column as the column to show the tree hierarchy
				int nTreeColActual = m_nTreeCol;
				CTreeCell* pTreeCell = TREECELL((*m_pRowData)[i]->GetAt(nTreeColActual));
				if (pTreeCell == NULL)
				{
					return FALSE;
				}

				if(dataDetachedRows != NULL)
				{
					if(nTreeItemLevelFilter == -1)
					{
						(*dataDetachedRows).Add((*m_pRowData)[i]);
						if(heightDetachedRows != NULL)
						{
							(*heightDetachedRows).Add((*m_pArrayRowHeights)[i]);
						}
					}
					else
					{
						if(pTreeCell != NULL)
						{
							if(pTreeCell->m_nTreeItemLevel == nTreeItemLevelFilter)
							{
								(*dataDetachedRows).Add((*m_pRowData)[i]);

								if(heightDetachedRows != NULL)
								{
									(*heightDetachedRows).Add((*m_pArrayRowHeights)[i]);
								}
							}
						}
					}
				}

				m_mapDetachRowHeights.SetAt((void*&)pTreeCell, (int&)(*m_pArrayRowHeights)[i]);
			}

			//(*m_pRowData).RemoveAt(range.m_nMinItem, count);
			(*m_pRowData).RemoveAt(range.GetMinRow(), count);
			// Most important thing to remember, row heights array should be adressed as well
			//(*m_pArrayRowHeights).RemoveAt(range.m_nMinItem, count);
			(*m_pArrayRowHeights).RemoveAt(range.GetMinRow(), count);
			// And also don't forget to decrement the row count
			m_nRows -= count;

			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CTreeGridCtrl::UpdateChildLevel(GTV_HTREENODE hItem)
{
	BOOL bRet = TRUE;
	if(hItem != NULL)
	{
		GTV_HTREENODE hChildItem = NULL;
		for(INT_PTR i = 0; i < hItem->m_arChildNodes.GetSize(); i++)
		{
			hChildItem = hItem->m_arChildNodes[i];
			//hChildItem->m_lAttributes = hItem->m_lAttributes + 1;
			GTV_SETATTRIB_TREEITEMLEVEL(hChildItem, (GTV_GETATTRIB_TREEITEMLEVEL(hItem) + 1));
			CTreeCell* pTreeCell = TREECELL(GetTreeCell(hChildItem));
			if(pTreeCell != NULL)
			{
				//pTreeCell->m_nTreeItemLevel = (int)(hChildItem->m_lAttributes);
				pTreeCell->m_nTreeItemLevel = (int)(GTV_GETATTRIB_TREEITEMLEVEL(hChildItem));
			}
			bRet &= UpdateChildLevel(hChildItem);
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CTreeGridCtrl::UpdateChildOrder(GTV_HTREENODE hItem, /*[IN OUT]*/int &nRowIndex)
{
	BOOL bRet = TRUE;

	if(hItem != NULL) 
	{
		if(hItem == m_hRootNode)
		{
			nRowIndex = -1;
		}
		else if(nRowIndex == -1)
		{
			nRowIndex = FindRowIndex(hItem);
		}

		if( (nRowIndex > -1) || (hItem == m_hRootNode) )
		{
			CTreeCell* pTreeCell = NULL;
			if(hItem != m_hRootNode)
			{
				pTreeCell = GetTreeCell(hItem);
			}
			if( (pTreeCell != NULL) || (hItem == m_hRootNode) )
			{
				int nTreeItemLevel = ROOT_TREEITEM_LEVEL;

				if(hItem != m_hRootNode)
				{
					nTreeItemLevel = pTreeCell->m_nTreeItemLevel;
				}

				int nChildIndex = -1;
				INT_PTR nLastChildIndex = hItem->m_arChildNodes.GetSize() - 1;

				for(int nNextIndex = nRowIndex + 1; nNextIndex < m_nRows; nNextIndex++)
				{
					// Assuming the first column as the column to show the tree hierarchy
					int nTreeColActual = m_nTreeCol;
					pTreeCell = TREECELL((*m_pRowData)[nNextIndex]->GetAt(nTreeColActual));
					if(pTreeCell != NULL)
					{
						if(pTreeCell->m_nTreeItemLevel == (nTreeItemLevel + 1))
						{
							//if immediate child
							nRowIndex = nNextIndex;

							// increment nRowIndex
							nChildIndex++;
							hItem->m_arChildNodes[nChildIndex] = pTreeCell->m_hTreeItem;
							if(nChildIndex == 0)
							{
								hItem->m_arChildNodes[nChildIndex]->m_hPrevSibling = NULL;
							}
							else if(nChildIndex > 0)
							{
								hItem->m_arChildNodes[nChildIndex - 1]->m_hNextSibling = hItem->m_arChildNodes[nChildIndex];
								hItem->m_arChildNodes[nChildIndex]->m_hPrevSibling = hItem->m_arChildNodes[nChildIndex - 1];
							}

							if(nChildIndex == nLastChildIndex)
							{
								hItem->m_arChildNodes[nChildIndex]->m_hNextSibling = NULL;
							}

							bRet &= UpdateChildOrder(pTreeCell->m_hTreeItem, nRowIndex);
						}

						if(nChildIndex  == nLastChildIndex)
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

GV_ITEMHANDLE CTreeGridCtrl::PutNode(GTV_HTREENODE hItem, GTV_HTREENODE hNewParent, GTV_HTREENODE hInsertAfter)
{
	GTV_HTREENODE hItemToCopy = NULL;
	GTV_HTREENODE hParent = hNewParent;

	if(hParent == GLVI_ROOT)
	{
		hParent = m_hRootNode;
	}
	
	GTV_HTREENODE hNextItem = NULL;
	GTV_HTREENODE hPrevItem = hInsertAfter;

	GV_ITEMHANDLE nRowIndex = GetFixedRowCount()-1;

	if(hParent != NULL)
	{
		if(!hParent->IsLeaf())
		{
			if(hInsertAfter == GLVI_FIRST)
			{
				// Insert just after the row of the parent
				hPrevItem = NULL;
				// get a pointer to the first child item, because it will be the m_hNextSibling of the new Child 
				hNextItem = hParent->m_arChildNodes[0];
			}
			else if(hInsertAfter == GLVI_LAST)
			{
				// Insert just after the row of the last child
				hPrevItem = hParent->m_arChildNodes[hParent->m_arChildNodes.GetSize() - 1];
			}
		}
		else
		{
			// Insert just after the row of the parent
			hPrevItem = NULL;
		}

		hItemToCopy = hItem;
		if(hItemToCopy != NULL)
		{
			hItemToCopy->m_hParent = hParent;
			// hPrevItem = NULL implies the parent is a leaf node 
			// or hInsertAfter = GLVI_FIRST when parent is not a leaf node
			if(hPrevItem == NULL)
			{				
				// Insert just after parent
				nRowIndex = FindRowIndex(hParent) + 1;

				// If previous item is NULL, then the newly created item
				// can only have a next sibling, but not a prev sibling
				hItemToCopy->m_hPrevSibling = NULL;
				hItemToCopy->m_hNextSibling = hNextItem;

			}
			else
			{				
				CItemRange range;
				if(!hPrevItem->IsLeaf())
				{
					FindChildrenRowRange(hPrevItem, range);
					nRowIndex = range.GetMaxRow() + 1;
				}
				else
				{
					nRowIndex = FindRowIndex(hPrevItem) + 1;
				}

				hItemToCopy->m_hPrevSibling = hPrevItem;				
				hPrevItem->m_hNextSibling = hItemToCopy;
			}

			INT_PTR nNodeIndex = -1;
	
			if(hInsertAfter == GLVI_FIRST)
			{	// If the new node is the first item
				nNodeIndex = 0;
			}
			else if(hInsertAfter == GLVI_LAST)
			{
				// If the new node should be the last item
				nNodeIndex = (int)(hParent->m_arChildNodes.GetSize());
			}
			else if(hPrevItem != NULL)
			{
				INT_PTR size = hParent->m_arChildNodes.GetSize();

				for(INT_PTR i = 0; i < size; i++)
				{
					if(hParent->m_arChildNodes[i] == hPrevItem)
					{
						// Insert after hPrevItem which in this case is equal to hInsertAfter
						nNodeIndex = i + 1;
						break;
					}
				}
			}

			if(nNodeIndex > -1)
			{
				//hItemToCopy->m_lAttributes = hParent->m_lAttributes + 1;
				GTV_SETATTRIB_TREEITEMLEVEL(hItemToCopy, (GTV_GETATTRIB_TREEITEMLEVEL(hParent) + 1));
				hParent->m_arChildNodes.InsertAt(nNodeIndex, hItemToCopy);
			}
			else
			{
				hItemToCopy = NULL;
				nRowIndex = -1;
			}
		}
	}

	return nRowIndex;
}

BOOL CTreeGridCtrl::RemoveNode(GTV_HTREENODE hItem, GTV_HTREENODE hParent)
{
	BOOL bRet = FALSE;
	if(hItem != NULL && hParent != NULL)
	{
		for(INT_PTR i = 0; i < hParent->m_arChildNodes.GetSize(); i++)
		{
			if(hItem == hParent->m_arChildNodes[i])
			{
				// before removing, update the chain info
				if(hItem->m_hNextSibling != NULL)
				{
					hItem->m_hNextSibling->m_hPrevSibling = hItem->m_hPrevSibling;
				}
				if(hItem->m_hPrevSibling != NULL)
				{
					hItem->m_hPrevSibling->m_hNextSibling = hItem->m_hNextSibling;
				}
				
				hParent->m_arChildNodes.RemoveAt(i);
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}

BOOL CTreeGridCtrl::ShowVisibleTreeItemsOnly(CTreeCell* pTreeCell, BOOL bShow)	
{
	BOOL bRet = FALSE;

	GTV_HTREENODE hItem = NULL;
	if(pTreeCell != NULL)
	{
		hItem = pTreeCell->m_hTreeItem;
	}
	if(hItem != NULL)
	{
		bRet = TRUE;

		// now recursively show/hide the child items
		if(!hItem->IsLeaf())
		{
			if(bShow)
			{
				if(pTreeCell->IsExpanded())
				{					
					for(INT_PTR i = 0; i < hItem->m_arChildNodes.GetSize(); i++)
					{
						GTV_HTREENODE hChildItem = hItem->m_arChildNodes[i];
						CTreeCell* pTreeCell = GetTreeCell(hChildItem);
						if (pTreeCell != NULL)
						{
							// The item is made hidden by the user, so do not show the item while expanding
							if((pTreeCell->GetState() & GTVIS_HIDDEN) != GTVIS_HIDDEN)
							{
								bRet &= ShowVisibleTreeItemsOnly(pTreeCell, TRUE);
								int nRowIndex = FindRowIndex(hChildItem);
								if(nRowIndex > -1)
								{
									//CGridCtrl::ShowRow(nRowIndex, TRUE);
									ShowRow(hChildItem, nRowIndex, TRUE);
								}
							}
						}
					}
				}
			}
			else
			{
				for(INT_PTR i = 0; i < hItem->m_arChildNodes.GetSize(); i++)
				{
					GTV_HTREENODE hChildItem = hItem->m_arChildNodes[i];
					CTreeCell* pTreeCell = TREECELL(GetTreeCell(hChildItem));
					if (pTreeCell != NULL)
					{
						// The item is already made hidden by the user, no need to go inside the recursive method of hiding the child items
						if((pTreeCell->GetState() & GTVIS_HIDDEN) != GTVIS_HIDDEN)
						{
							//	yangbin 2017-1-15 该孩子处于展开状态，才需要递归调用让让子孙节点收缩；否则让该孩子节点收缩即可
							if(pTreeCell->IsExpanded())
							{
								bRet &= ShowVisibleTreeItemsOnly(pTreeCell, FALSE);
								
							}
							int nRowIndex = FindRowIndex(hChildItem);
							if(nRowIndex > -1)
							{
								//CGridCtrl::ShowRow(nRowIndex, FALSE);
								ShowRow(hChildItem, nRowIndex, FALSE);
							}
						}
					}
				}
			}
		}
	}

	return bRet;
}

BOOL CTreeGridCtrl::IsRootNode(GTV_HTREENODE hItem)
{
	BOOL bRet = FALSE;
	if(hItem != NULL)
	{
		//if(hItem->m_lAttributes == ROOT_TREEITEM_LEVEL)
		if(GTV_GETATTRIB_TREEITEMLEVEL(hItem) == ROOT_TREEITEM_LEVEL)
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CTreeGridCtrl::IsTopLevelParent(GTV_HTREENODE hItem)
{
	BOOL bRet = FALSE;
	if(hItem != NULL)
	{
		//if(hItem->m_lAttributes == (ROOT_TREEITEM_LEVEL + 1))
		if(GTV_GETATTRIB_TREEITEMLEVEL(hItem) == (ROOT_TREEITEM_LEVEL + 1))
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

//
// Overridden Methods
//

void CTreeGridCtrl::OnDraw(CDC* pDC)
{
	CGridCtrl::OnDraw(pDC);
}

BOOL CTreeGridCtrl::OnTreeHitTest(UINT nFlags, CPoint point)
{
#ifdef GridCtrlCONTROL_USE_TITLETIPS
	// EFW - Bug Fix
	m_TitleTip.Hide();  // hide any title tips
#endif

	BOOL bRet = FALSE;
	//TRACE("CTreeGridCtrl::OnLButtonDown\n");

	if(GetFocus() != this)
	{
		SetFocus();
	}
	m_CurCol = -1;
	m_bLMouseButtonDown   = TRUE;
	m_LeftClickDownPoint = point;
	CCellID originalCell;
	m_LeftClickDownCell = GetCellFromPt(point, TRUE, originalCell);

	int nItem = m_LeftClickDownCell.row;
	int nSubItem = m_LeftClickDownCell.col;

	BOOL bValid = (nItem >= 0 && nItem < m_nRows && nSubItem >= 0 && nSubItem < m_nCols);
	if (bValid)
	{		
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeColActual = m_nTreeCol;	
		if(m_LeftClickDownCell.col == nTreeColActual)
		{
			m_CurRow = m_LeftClickDownCell.row;

			CTreeCell* pCell = TREECELL(GetCell(nItem, nSubItem));
			
			if( (pCell != NULL) && pCell->IsEnabled() )
			{
				if( ((nFlags & MK_SHIFT) != MK_SHIFT) && ((nFlags & MK_CONTROL) != MK_CONTROL) )
				{
					CRect rectCell;
					GetCellRect(nItem, nSubItem, rectCell);
					BOOL bClickedOnBtnImgArea = FALSE;
					if(pCell->TreeBtnHitTest(point, rectCell, bClickedOnBtnImgArea))
					{
						bRet = TRUE;

						if(bClickedOnBtnImgArea)
						{
							if(pCell->IsExpanded())
							{
								if(SendMessageToParent(nItem, nTreeColActual, GTVN_ITEMCOLLAPSING) >= 0)
								{
									CollapseTreeNode(pCell->m_hTreeItem);
									SendMessageToParent(nItem, nTreeColActual, GTVN_ITEMCOLLAPSED);
								}
							}
							else
							{
								if(SendMessageToParent(nItem, nTreeColActual, GTVN_ITEMEXPANDING) >= 0)
								{
									ExpandTreeNode(pCell->m_hTreeItem);
									SendMessageToParent(nItem, nTreeColActual, GTVN_ITEMEXPANDED);
								}
							}
						}
					}
				}
			}
		}
	}
	
	return bRet;
}

void CTreeGridCtrl::OnDeletingColumn(int nColumn)
{	
	// Assuming the first column as the column to show the tree hierarchy
	int nTreeColActual = m_nTreeCol;
	// If the tree column is deleted then destroy the tree info
	if(nColumn == nTreeColActual)
	{
		DeleteNode(m_hRootNode); // destroys all the child info as well
		// just for later use get create an instance of a new root item immediately
		CreateRootNode();
	}
}

void CTreeGridCtrl::OnDeletingRow(GV_ITEMHANDLE /*nRow*/)
{
	// nothing to do here
}

BOOL CTreeGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data, int low, int high)
{
	CGridCtrl* prevThis = m_This;
	m_This = this;

	int nTotalCols = GetColumnCount();
	if (nColumn >= nTotalCols)
		return FALSE;

	if (high == -1)
		high = GetItemCount() - 1;

	int lo = low;
	int hi = high;

	if (hi <= lo)
		return FALSE;

	if (GetVirtualMode())
	{
		return CGridCtrl::SortItems(pfnCompare, nColumn, bAscending, data, low, high);
	}

	LPARAM pMidCell = (LPARAM) GetCell((lo + hi)/2, nColumn);

	// loop through the list until indices cross
	while (lo <= hi)
	{
		// Find the first element that is greater than or equal to the partition element starting from the left Index.
		if (bAscending)
			while (lo < high  && __PFNTREECOMPARE(pfnCompare, bAscending, (LPARAM)GetCell(lo, nColumn), (LPARAM) pMidCell, data) < 0)
				++lo;
		else
			while (lo < high && __PFNTREECOMPARE(pfnCompare, bAscending, (LPARAM)GetCell(lo, nColumn), pMidCell, data) > 0)
				++lo;

		// Find an element that is smaller than or equal to  the partition element starting from the right Index.
		if (bAscending)
			while (hi > low && __PFNTREECOMPARE(pfnCompare, bAscending, (LPARAM)GetCell(hi, nColumn), pMidCell, data) > 0)
				--hi;
		else
			while (hi > low && __PFNTREECOMPARE(pfnCompare, bAscending, (LPARAM)GetCell(hi, nColumn), pMidCell, data) < 0)
				--hi;

		// If the indexes have not crossed, swap if the items are not equal
		if (lo <= hi)
		{
			// swap only if the items are not equal
			if (__PFNTREECOMPARE(pfnCompare, bAscending, (LPARAM)GetCell(lo, nColumn), 
				(LPARAM)GetCell(hi, nColumn), data) != 0)
			{
				for (int col = 0; col < (GetColumnCount()); col++)
				{
					CGridCellBase *pCell = GetCell(lo, col);
					SetCell(lo, col, GetCell(hi, col));
					SetCell(hi, col, pCell);
				}
				UINT nRowHeight = (*m_pArrayRowHeights)[lo];
				(*m_pArrayRowHeights)[lo] = (*m_pArrayRowHeights)[hi];
				(*m_pArrayRowHeights)[hi] = nRowHeight;
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array must now sort the left partition.
	if (low < hi)
		SortItems(pfnCompare, nColumn, bAscending, data, low, hi);

	// If the left index has not reached the right side of array must now sort the right partition.
	if (lo < high)
		SortItems(pfnCompare, nColumn, bAscending, data, lo, high);

	m_This = prevThis;

	return TRUE;
}

BOOL CTreeGridCtrl::SortItems(int nColumn, BOOL bAscending, LPARAM data/*= 0*/)
{
	SetRedraw(FALSE);

	ResetSelectedRange();

	BOOL bRet = SortTreeItems(GetRootNode(), NULL, nColumn, bAscending, data, TRUE);	

	SetRedraw(TRUE);

#ifdef _DEBUG
	int nRowIndex = -1;
	TraverseNodes(m_hRootNode, nRowIndex);
#endif

	return bRet;
}

BOOL CTreeGridCtrl::SortItems(PFNLVCOMPARE pfnCompare, int nColumn, BOOL bAscending, LPARAM data /* = 0 */)
{	
	return SortTreeItems(GetRootNode(), pfnCompare, nColumn, bAscending, data, TRUE);
}

BOOL CTreeGridCtrl::SortTextItems(int nColumn, BOOL bAscending, LPARAM data /* = 0 */)
{
	if(nColumn < 0 || nColumn >= GetColumnCount())
		return FALSE;
	
	return SortTreeItems(GetRootNode(), PFNLVCOMPARE(pfnCellTextCompare), nColumn, bAscending, data, TRUE);
}


//
// Message Handler
//

void CTreeGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bTreeHitTest = OnTreeHitTest(nFlags, point);
	if(!m_bTreeHitTest)
	{
		CGridCtrl::OnLButtonDown(nFlags, point);
	}
	else
	{
		m_bLMouseButtonDown		= FALSE;
		m_LeftClickDownCell.row	= -1;
		m_LeftClickDownCell.col	= -1;
		m_LeftClickDownPoint.x	= -1;
		m_LeftClickDownPoint.y	= -1;

		CWnd::OnLButtonDown(nFlags, point);
	}
}

void CTreeGridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bTreeHitTest)
	{
		CGridCtrl::OnLButtonUp(nFlags, point);
	}
	else
	{
		m_bLMouseButtonDown		= FALSE;
		m_LeftClickDownCell.row	= -1;
		m_LeftClickDownCell.col	= -1;
		m_LeftClickDownPoint.x	= -1;
		m_LeftClickDownPoint.y	= -1;

		CWnd::OnLButtonUp(nFlags, point);
	}

	m_bTreeHitTest = FALSE;
}

void CTreeGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bTreeHitTest = OnTreeHitTest(nFlags, point);
	if(!m_bTreeHitTest)
	{
		CGridCtrl::OnLButtonDblClk(nFlags, point);
	}
	else
	{
		m_bLMouseButtonDown		= FALSE;
		m_LeftClickDownCell.row	= -1;
		m_LeftClickDownCell.col	= -1;
		m_LeftClickDownPoint.x	= -1;
		m_LeftClickDownPoint.y	= -1;

		CWnd::OnLButtonDblClk(nFlags, point);
	}
}

// Show 		 - Show the title tip if needed
// rectTitle	 - The rectangle within which the original 
//				    title is constrained - in client coordinates
// lpszTitleText - The text to be displayed
// xoffset		 - Number of pixel that the text is offset from
//				   left border of the cell
void CTreeGridCtrl::ShowTitleTip(CGridCellBase* pCell, int nRow, int nColumn,
					CRect rectCell, CRect rectTitle, LPCTSTR lpszTitleText, int xoffset /*=0*/,
					 LPRECT lpHoverRect /*=NULL*/,
					 const LOGFONT* lpLogFont /*=NULL*/,
					 COLORREF crTextClr /* CLR_DEFAULT */,
					 COLORREF crBackClr /* CLR_DEFAULT */)
{

	BOOL bDisplayTitleTip = TRUE;

	// Assuming the first column as the column to show the tree hierarchy
	int nTreeColActual = m_nTreeCol;

	if(nColumn == nTreeColActual)
	{		
		CTreeCell* pTreeCtrlCell = TREECELL(pCell);
		if(pTreeCtrlCell != NULL)
		{			
			if(pTreeCtrlCell->m_hTreeItem != NULL)
			{
				CRect rectTreeBtn = rectTitle;
				pTreeCtrlCell->MeasureTreeButtonRect(rectTreeBtn);

				int width = rectTitle.Width();

				int nPadding = 2; //border + gap between image and text
				if((int) (rectTreeBtn.right + pTreeCtrlCell->GetTreeBtnRightPadding() +
						pTreeCtrlCell->GetMargin() + nPadding + xoffset) < rectCell.right )
				{
					rectTitle.left = rectTreeBtn.right;
					rectTitle.right = rectTitle.left + width;
				}
				else
				{
					bDisplayTitleTip = FALSE;
				}
			}
		}
	}

	if(bDisplayTitleTip)
	{
		CGridCtrl::ShowTitleTip(pCell, nRow, nColumn, rectCell, rectTitle, lpszTitleText, xoffset,
					lpHoverRect, lpLogFont, crTextClr, crBackClr);
	}
}

BOOL CTreeGridCtrl::NodeHasChildren(GTV_HTREENODE hNode)
{
	BOOL bHasChildren = FALSE;

	if ( (hNode != NULL) && !(hNode->IsLeaf()) )
	{
		bHasChildren = TRUE;
	}
	
	return bHasChildren;
}

GTV_HTREENODE CTreeGridCtrl::GetChildNode(GTV_HTREENODE hNode)
{
	GTV_HTREENODE hChildNode = NULL;

	if ( (hNode != NULL) && !(hNode->IsLeaf()) )
	{
		hChildNode = hNode->m_arChildNodes[0];
	}

	return hChildNode;
}

GTV_HTREENODE CTreeGridCtrl::GetNextSiblingNode(GTV_HTREENODE hNode)
{
	GTV_HTREENODE hNextSiblingNode = NULL;

	if (hNode != NULL)
	{
		hNextSiblingNode = hNode->m_hNextSibling;
	}

	return hNextSiblingNode;
}

GTV_HTREENODE CTreeGridCtrl::GetPrevSiblingNode(GTV_HTREENODE hNode)
{
	GTV_HTREENODE hPrevSiblingNode = NULL;

	if (hNode != NULL)
	{
		hPrevSiblingNode = hNode->m_hPrevSibling;
	}

	return hPrevSiblingNode;
}

GTV_HTREENODE CTreeGridCtrl::GetParentNode(GTV_HTREENODE hNode)
{
	GTV_HTREENODE hParentNode = NULL;

	if (hNode != NULL)
	{
		hParentNode = hNode->m_hParent;
	}

	return hParentNode;
}

GTV_HTREENODE CTreeGridCtrl::GetRootNode()
{
	return m_hRootNode;
}

GTV_HTREENODE CTreeGridCtrl::GetTreeNode(int nItem)
{
	GTV_HTREENODE hItem = NULL;
	// Assuming the first column as the column to show the tree hierarchy
	int nTreeCol = m_nTreeCol;
	CTreeCell* pTreeCell = TREECELL(GetCell(nItem, nTreeCol));
	if(pTreeCell != NULL)
	{
		hItem = pTreeCell->GetTreeItem();
	}

	return hItem;
}

void CTreeGridCtrl::EnableFastRowLookup(BOOL bEnable/* = TRUE*/)
{
	m_bFastRowLookupMode = bEnable;
}

BOOL CTreeGridCtrl::IsEnabledFastRowLookup()
{
	return m_bFastRowLookupMode;
}

void CTreeGridCtrl::ShowChildNodes(GTV_HTREENODE hParentItem, BOOL bExpanded, int& nRowIndex, BOOL bShow)
{
	CTreeCell* pTreeCell = NULL;

	// Assuming the first column as the column to show the tree hierarchy
	int nTreeCol = m_nTreeCol;
	for(int i = 0; i < hParentItem->m_arChildNodes.GetSize(); i++)
	{
		pTreeCell = TREECELL(GetCell(nRowIndex, nTreeCol));
		if(pTreeCell != NULL)
		{
			GTV_HTREENODE hItem = (hParentItem->m_arChildNodes[i]);
		
			nRowIndex++;
		
			if(!bShow)
			{
				SetCellState(nRowIndex, nTreeCol, GetCellState(nRowIndex, nTreeCol) | GTVIS_HIDDEN);
				//CGridCtrl::ShowRow(nRowIndex, FALSE);
				ShowRow(hItem, nRowIndex, FALSE);
			}
			else
			{
				SetCellState(nRowIndex, nTreeCol, GetCellState(nRowIndex, nTreeCol) & ~GTVIS_HIDDEN);
				//CGridCtrl::ShowRow(nRowIndex, bExpanded);
				ShowRow(hItem, nRowIndex, bExpanded);
			}

			ShowChildNodes(hItem, pTreeCell->IsExpanded(), nRowIndex, bShow);
		}
	}
}

BOOL CTreeGridCtrl::ShowRow(int nRow, BOOL bShow/* = TRUE*/)
{
	BOOL bRet = TRUE;

	if(nRow > -1)
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;
		CTreeCell* pCell = TREECELL(GetCell(nRow, nTreeCol));
		CTreeCell* pCellParent = (pCell->GetTreeItem() != NULL) ? ( GetTreeCell(pCell->GetTreeItem()->m_hParent) ) : NULL;

		BOOL bParentIsExpanded = ( (pCellParent != NULL) && (pCellParent->IsExpanded()) ) ? TRUE : FALSE;

		if(pCell != NULL)
		{
			if(pCell->GetTreeItemLevel() == ROOT_TREEITEM_LEVEL + 1)
			{
				bParentIsExpanded = TRUE;
			}

			if(!bShow)
			{
				SetCellState(nRow, nTreeCol, GetCellState(nRow, nTreeCol) | GTVIS_HIDDEN);
			}
			else
			{
				SetCellState(nRow, nTreeCol, GetCellState(nRow, nTreeCol) & ~GTVIS_HIDDEN);
			}

			//bRet &= CGridCtrl::ShowRow(nRow, (bShow && bParentIsExpanded));
			bRet &= ShowRow(pCell->GetTreeItem(), nRow, (bShow && bParentIsExpanded));
			
			ShowChildNodes(pCell->GetTreeItem(), pCell->IsExpanded(), nRow, bShow);
		}
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;
}

int CTreeGridCtrl::InsertItem(LPCTSTR strHeading, int nItem, BOOL bRefresh)
{
	int nRowIndex = -1;
	
	GTV_HTREENODE hParent = GLVI_ROOT, hInsertAfter = GLVI_LAST;
	CTreeCell* pCell = NULL;
	
	if(nItem == -1)
	{
		hInsertAfter = GLVI_LAST;
	}
	else if(nItem == 0)
	{
		hInsertAfter = GLVI_FIRST;
	}
	else
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;	
		pCell = TREECELL(GetCell(nItem - 1, nTreeCol));
		if(pCell != NULL)
		{
			hInsertAfter = pCell->GetTreeItem();
		}
	}

	InsertNode(hParent, hInsertAfter, nRowIndex);
	if(nRowIndex > -1)
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;
		SetCellText(nRowIndex, nTreeCol, strHeading);

		if(bRefresh)
			Refresh();
	}
	
	return nRowIndex;
}

int CTreeGridCtrl::InsertItem(GV_ITEM* pItem, BOOL bRefresh)
{
	int nRowIndex = -1;
	
	GTV_HTREENODE hParent = GLVI_ROOT, hInsertAfter = GLVI_LAST;
	CTreeCell* pCell = NULL;
	
	int nItem = pItem->row;
	if(nItem == -1)
	{
		hInsertAfter = GLVI_LAST;
	}
	else if(nItem == 0)
	{
		hInsertAfter = GLVI_FIRST;
	}
	else
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;	
		pCell = TREECELL(GetCell(nItem - 1, nTreeCol));
		if(pCell != NULL)
		{
			hInsertAfter = pCell->GetTreeItem();
		}
	}

	InsertNode(hParent, hInsertAfter, nRowIndex);
	if(nRowIndex > -1)
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;

		// Specify at which row the item should be set
		pItem->row = nRowIndex;
		pItem->col = nTreeCol;

		SetItem(pItem);

		if(bRefresh)
			Refresh();
	}

	return nRowIndex;
}

BOOL CTreeGridCtrl::DeleteItem(int nItem)
{
	BOOL bRet = FALSE;

	// Assuming the first column as the column to show the tree hierarchy
	int nTreeCol = m_nTreeCol;
	CTreeCell* pCell = TREECELL(GetCell(nItem, nTreeCol));
	if(pCell != NULL)
	{
		bRet = DeleteItem(pCell->GetTreeItem());
	}

	return bRet;
}

BOOL CTreeGridCtrl::SetItemCount(int nItems)
{
	BOOL bRet = FALSE;

	if(SetChildItemCount(-1, nItems) > -1)
		bRet = TRUE;

	return bRet;
}
	
int CTreeGridCtrl::SetChildItemCount(int nParentItemIndex, int nItems)
{
	int nLastChildIndex = -1;;

	GTV_HTREENODE hParent = NULL;
	if(nParentItemIndex == -1)
	{
		hParent = GetRootNode();
	}
	else
	{
		// Assuming the first column as the column to show the tree hierarchy
		int nTreeCol = m_nTreeCol;
		CTreeCell* pCellParent = TREECELL(GetCell(nParentItemIndex, nTreeCol));
		if(pCellParent != NULL)
		{
			hParent = pCellParent->GetTreeItem();
		}
	}

	if(hParent != NULL)
	{
		// If the parent already has child it must have been bad jumbled up coding
		ASSERT(!(hParent->m_arChildNodes.GetSize()));
		
		GRID_ROWS pNewRows = new GRID_ROW[nItems];
		if(pNewRows != NULL)
		{
			GTV_TREENODESPTR pNodes = CreateNodes(nItems);
			if(pNodes == NULL)
			{
				delete pNewRows;
				pNewRows = NULL;
			}
			else
			{
				// Force recalculation
				m_idTopLeftCell.col = -1;
			
				m_arAllocatedRows.Add(pNewRows);
				m_arAllocatedNodes.Add(pNodes);
			
				// remove all the previous child's
				if(hParent->m_arChildNodes.GetSize() > 0)
				{
					
					INT_PTR nSize = hParent->m_arChildNodes.GetSize();
					for(INT_PTR i = nSize - 1; i >= 0; i--)
					{
						DeleteNode(hParent->m_arChildNodes[i]);
					}
				}

				hParent->m_arChildNodes.SetSize(nItems);
			}
		
			if((pNodes != NULL) && (hParent->m_arChildNodes.GetSize() > 0))
			{
				int i = 0;
				int row = (nParentItemIndex + 1) + i;

				////////// Fill out an array to Insert all the rows in on go//////////////////////
				static CTypedPtrArray<CObArray, GRID_ROW*> rowData;
				rowData.RemoveAll();
				rowData.SetSize(nItems);
				for(int index = 0; index < nItems; index++)
				{
					rowData[index] = &pNewRows[index];
				}
				///////////////////////////////////////////////////////////////////////////////////////
				// Now insert all the rows at once			
				(*m_pRowData).InsertAt(nParentItemIndex + 1, &rowData);

				// Take care of the row heights as well
				(*m_pArrayRowHeights).InsertAt(nParentItemIndex + 1, m_cellDefault.GetHeight(), nItems);

				GTV_HTREENODE hItem = &pNodes[i];
				
				hItem->m_hParent = hParent;
				hParent->m_arChildNodes[i] = &pNodes[i];
				hItem->m_hNextSibling = (nItems > 1) ? &pNodes[1] : NULL;
				hItem->m_hPrevSibling = NULL;
				hItem->m_lParam = NULL;

				/// Create the cells
				(&pNewRows[i])->SetSize(m_nCols);
				for (int col = 0; col < m_nCols; col++)
				{
					GRID_ROW* pRow =  &pNewRows[i];
					if (pRow && !GetVirtualMode())
						pRow->SetAt(col, CreateCell(row, col));
				}
				m_nRows++;
				
				if(GTV_GETATTRIB_COLLAPSED(hParent) || GTV_GETATTRIB_NOTVISIBLE(hParent))
				{
					ShowRow(hItem, row, FALSE);
				}

				///////////////
				//(*m_pArrayRowHeights)[i] = m_cellDefault.GetHeight();

				hItem->m_arChildNodes.RemoveAll();
				//hItem->m_lAttributes = hParent->m_lAttributes + 1;
				hItem->m_lAttributes = 0;
				GTV_SETATTRIB_TREEITEMLEVEL(hItem, (GTV_GETATTRIB_TREEITEMLEVEL(hParent) + 1));
				GTV_SETATTRIB_DELETABLE(hItem, FALSE);

				SetCorrespondingRow(hItem, &pNewRows[i]);

				for(i = 1; i < nItems - 1; i++)
				{
					hItem = &pNodes[i];

					row++;

					hItem->m_hParent = hParent;
					hParent->m_arChildNodes[i] = &pNodes[i];
					hItem->m_hNextSibling = &pNodes[i + 1];
					hItem->m_hPrevSibling = &pNodes[i - 1];
					hItem->m_lParam = NULL;

					/// Create the cells
					(&pNewRows[i])->SetSize(m_nCols);
					for (int col = 0; col < m_nCols; col++)
					{
						GRID_ROW* pRow =  &pNewRows[i];
						if (pRow && !GetVirtualMode())
							pRow->SetAt(col, CreateCell(row, col));
					}
					m_nRows++;

					if(GTV_GETATTRIB_COLLAPSED(hParent) || GTV_GETATTRIB_NOTVISIBLE(hParent))
					{
						ShowRow(hItem, row, FALSE);
					}

					///////////////////
					//(*m_pArrayRowHeights)[i] = m_cellDefault.GetHeight();

					hItem->m_arChildNodes.RemoveAll();
					//hItem->m_lAttributes = hParent->m_lAttributes + 1;
					hItem->m_lAttributes = 0;
					GTV_SETATTRIB_TREEITEMLEVEL(hItem, (GTV_GETATTRIB_TREEITEMLEVEL(hParent) + 1));
					GTV_SETATTRIB_DELETABLE(hItem, FALSE);

					SetCorrespondingRow(hItem, &pNewRows[i]);
				}

				if(i < nItems)
				{
					hItem = &pNodes[i];

					row++;

					hItem->m_hParent = hParent;
					hParent->m_arChildNodes[i] = &pNodes[i];
					hItem->m_hNextSibling = NULL;
					hItem->m_hPrevSibling = &pNodes[i - 1];
					hItem->m_lParam = NULL;
					hItem->m_pCorrespondingRow = &pNewRows[i];

					/// Create the cells
					(&pNewRows[i])->SetSize(m_nCols);
					for (int col = 0; col < m_nCols; col++)
					{
						GRID_ROW* pRow =  &pNewRows[i];
						if (pRow && !GetVirtualMode())
							pRow->SetAt(col, CreateCell(row, col));
					}
					m_nRows++;

					if(GTV_GETATTRIB_COLLAPSED(hParent) || GTV_GETATTRIB_NOTVISIBLE(hParent))
					{
						ShowRow(hItem, row, FALSE);
					}

					////////////////////////////////////////
					//(*m_pArrayRowHeights)[i] = m_cellDefault.GetHeight();

					hItem->m_arChildNodes.RemoveAll();
					//hItem->m_lAttributes = hParent->m_lAttributes + 1;
					hItem->m_lAttributes = 0;
					GTV_SETATTRIB_TREEITEMLEVEL(hItem, (GTV_GETATTRIB_TREEITEMLEVEL(hParent) + 1));
					GTV_SETATTRIB_DELETABLE(hItem, FALSE);

					SetCorrespondingRow(hItem, &pNewRows[i]);
				}

				// Update the last child row index which is to be returned from the method
				nLastChildIndex = nParentItemIndex + nItems;

				if(IsEnabledFastRowLookup())
				{
					// Assuming the first column as the column to show the tree hierarchy
					int nTreeCol = m_nTreeCol;

					// Increment the item value of the cells of next following rows 
					for(int i = nLastChildIndex + 1; i < GetItemCount(); i++)
					{
						CGridCell* pCell = (CGridCell*)GetCell(i, nTreeCol);
						if(pCell != NULL)
						{
							pCell->SetCoords(i, nTreeCol);
						}
					}
				}
			}
		}
	}
	
	return nLastChildIndex;
}

BOOL CTreeGridCtrl::ShowRow(GTV_HTREENODE hItem, int nRow, BOOL bShow)
{
	BOOL bRet = FALSE;
	if(!bShow)
	{
		m_mapHiddenRowHeights.SetAt(hItem, GetRowHeight(nRow));
		bRet = SetRowHeight(nRow, 0);
	}
	else
	{
		if(GetRowHeight(nRow) == 0)
		{
			SetRowHeight(nRow, m_mapHiddenRowHeights[hItem]);
		}
	}
	
	GTV_SETATTRIB_NOTVISIBLE(hItem, !bShow);

	return bRet;
}

COLORREF CTreeGridCtrl::GetHorzTreeLineColor()
{
	COLORREF clrTreeHorzLine = m_clrTreeHorzLine;
	if(clrTreeHorzLine == CLR_DEFAULT)
	{
		// Inverse background color
		/*clrTreeHorzLine = ~GetBKColor();
		clrTreeHorzLine &= 0x00FFFFFFL;*/
		clrTreeHorzLine = RGB(255 - GetRValue(GetBkColor()), 255 - GetGValue(GetBkColor()), 255 - GetBValue(GetBkColor()));
	}

	return clrTreeHorzLine; 
}

COLORREF CTreeGridCtrl::GetVertTreeLineColor()
{
	COLORREF clrTreeVertLine = m_clrTreeVertLine;
	if(clrTreeVertLine == CLR_DEFAULT)
	{
		// Inverse background color
		/*clrTreeHorzLine = ~GetBKColor();
		clrTreeHorzLine &= 0x00FFFFFFL;*/
		clrTreeVertLine = RGB(255 - GetRValue(GetBkColor()), 255 - GetGValue(GetBkColor()), 255 - GetBValue(GetBkColor()));
	}

	return clrTreeVertLine; 
}

BOOL CTreeGridCtrl::SetTreeColumnCellTypeID(enum CellTypeID cellType)
{	
	BOOL bRet = FALSE;

	switch (cellType)
	{
		case CT_DEFAULT:
		{
			m_pRtcTreeColumnDefault = NULL;
			bRet = TRUE;
		}
		break;
		
		case CT_CHECKBOX:
		{
			m_pRtcTreeColumnDefault = RUNTIME_CLASS(CTreeGridCellCheck);
			bRet = TRUE;
		}
		break;		
	}		

	return bRet;
}

BOOL CTreeGridCtrl::SetCellTypeID(int nRow, int nColumn, enum CellTypeID cellType)
{
	int nTreeCol = m_nTreeCol;
	if (!IsValid(nRow, nColumn) || (nColumn != nTreeCol))
		return FALSE;

	BOOL bRet = FALSE;

	switch (cellType)
	{
		case CT_DEFAULT:
		{
			bRet = CGridCtrl::SetCellType(nRow, nColumn, RUNTIME_CLASS(CTreeGridCell));
		}
		break;
		
		case CT_CHECKBOX:
		{
			bRet = CGridCtrl::SetCellType(nRow, nColumn, RUNTIME_CLASS(CTreeGridCellCheck));
		}
		break;		
	}
	
	if (bRet)
	{
		Invalidate();
	}

	return bRet;
}

CellTypeID CTreeGridCtrl::GetCellTypeID(int nRow, int nColumn)
{
	CellTypeID cellType = CT_DEFAULT;
	if(DYNAMIC_DOWNCAST(CTreeGridCellCheck, GetCell(nRow, nColumn)))
		cellType = CT_CHECKBOX;
	
	return cellType;
}

BOOL CTreeGridCtrl::SetColumnCellTypeID(int nColumn, enum CellTypeID cellType)
{
	BOOL bRet  = FALSE;

	if(IsValid(0, nColumn))
	{
		for (int nRow = 0; nRow < GetItemCount(); nRow++)
		{
			bRet |= SetCellTypeID(nRow, nColumn, cellType);
		}
	}

	return bRet;
}

//
// CGridCellBase Creation and Cleanup

// Create a cell in the memory
//
//  nItem	: Specifies the row index of the cell
//  nSubItem	: Specifies the column index of the cell
//
// Returns the pointer of the newly created cell pointer, if failed return NULL
// Creates a new Table cell and performs any necessary initialisation
CGridCellBase* CTreeGridCtrl::CreateCell(int nItem, int nSubItem)
{    
	CRuntimeClass* pRtcDefault = m_pRtcDefault;

	int nTreeCol = m_nTreeCol;
	if((m_pRtcTreeColumnDefault != NULL) && (nSubItem == nTreeCol) && (nItem >= m_nFixedRows))
	{
		m_pRtcDefault = m_pRtcTreeColumnDefault;
	}	

	CGridCellBase* pCell = CGridCtrl::CreateCell(nItem, nSubItem);
	
	m_pRtcDefault = pRtcDefault;    

	return pCell;
}


void CTreeGridCtrl::SetCellText(int row, int col, LPCTSTR pszText)
{
	if(GetCell(row, col))
	{
		GetCell(row, col)->SetText(pszText);
	}
}

//Sets the cell state
void CTreeGridCtrl::SetCellState(int row, int col, DWORD nState)
{
	if(GetCell(row, col))
	{
		GetCell(row, col)->SetState(nState);
	}
}
	
//Gets the cell state
DWORD CTreeGridCtrl::GetCellState(int row, int col)
{
	DWORD nState = 0;
	if(GetCell(row, col))
	{
		nState = GetCell(row, col)->GetState();
	}

	return nState;
}