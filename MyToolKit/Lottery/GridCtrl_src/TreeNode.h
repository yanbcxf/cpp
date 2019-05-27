//
// TreeNode.h : header file
//

#if !defined(__TreeNode_H__)
#define __TreeNode_H__

#include <afxtempl.h>

class CTreeNode;

// Tree node structure
typedef CTreeNode		GTV_TREENODE;
// Pointer to an array of Tree nodes
typedef GTV_TREENODE*		GTV_TREENODESPTR;

// Handle to a Tree node 
typedef GTV_TREENODE*		GTV_HTREENODE;

// Tree node handles array
typedef CArray<GTV_HTREENODE, GTV_HTREENODE&> GTV_HTREENODES;

#ifndef GTV_LOW_LPARAM
#define GTV_LOW_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GTV_HI_LPARAM
#define GTV_HI_LPARAM(lp)                         ((int)(short)HIWORD(lp))
#endif

#if defined(TREEGRIDCTRL)

	// definitions available from Application side Only
	enum GTV_ATTRIBS
	{
		GTVA_NONDELETABLE	= 0x0001, // Specifies if node is non deletable. NonDeletable is used as attrib (instead of deletable) for convenience, becoz usually a node is deletable
		GTVA_COLLAPSED		= 0x0002, // Specifies if node is at collapsed state hiding the child items
		GTVA_NOTVISIBLE	= 0x0004, // Specifies if node is hidden becoz of ShowRow or parent is collapsed during insertion, this attribute will be set
		GTVA_HASNOBUTTON	= 0x0008  // Specifies if node is shown without button. HasNoButton is used as attrib (instead of has button) for convenience, becoz usually a node will show button if it has children
	};

	#define GTV_SETATTRIB_TREEITEMLEVEL(hItem, level)	{int hw = GTV_HI_LPARAM(hItem->m_lAttributes); hItem->m_lAttributes = MAKELPARAM(level, hw); }
	#define GTV_GETATTRIB_TREEITEMLEVEL(hItem)			GTV_LOW_LPARAM(hItem->m_lAttributes)

	#define GTV_SETATTRIB_DELETABLE(hItem, deletable)	\
		{int hw = GTV_HI_LPARAM(hItem->m_lAttributes); \
		if(!deletable) hw |= GTVA_NONDELETABLE; else hw &= ~GTVA_NONDELETABLE; \
		int lw = GTV_LOW_LPARAM(hItem->m_lAttributes);  \
		hItem->m_lAttributes = MAKELPARAM(lw, hw);}
	
	#define GTV_GETATTRIB_DELETABLE(hItem) ((GTV_HI_LPARAM(hItem->m_lAttributes) & GTVA_NONDELETABLE ) != GTVA_NONDELETABLE)

	
	#define GTV_SETATTRIB_COLLAPSED(hItem, collapsed)	\
		{int hw = GTV_HI_LPARAM(hItem->m_lAttributes); \
		if(collapsed) hw |= GTVA_COLLAPSED; else hw &= ~GTVA_COLLAPSED; \
		int lw = GTV_LOW_LPARAM(hItem->m_lAttributes);  \
		hItem->m_lAttributes = MAKELPARAM(lw, hw);}
	
	#define GTV_GETATTRIB_COLLAPSED(hItem) ((GTV_HI_LPARAM(hItem->m_lAttributes) & GTVA_COLLAPSED) == GTVA_COLLAPSED)


	#define GTV_SETATTRIB_NOTVISIBLE(hItem, notvisible)	\
		{int hw = GTV_HI_LPARAM(hItem->m_lAttributes); \
		if(notvisible) hw |= GTVA_NOTVISIBLE; else hw &= ~GTVA_NOTVISIBLE; \
		int lw = GTV_LOW_LPARAM(hItem->m_lAttributes);  \
		hItem->m_lAttributes = MAKELPARAM(lw, hw);}
	
	#define GTV_GETATTRIB_NOTVISIBLE(hItem) ((GTV_HI_LPARAM(hItem->m_lAttributes) & GTVA_NOTVISIBLE) == GTVA_NOTVISIBLE)
	
	
	#define GTV_SETATTRIB_HASBUTTON(hItem, hasbutton)	\
		{int hw = GTV_HI_LPARAM(hItem->m_lAttributes); \
		if(!hasbutton) hw |= GTVA_HASNOBUTTON; else hw &= ~GTVA_HASNOBUTTON; \
		int lw = GTV_LOW_LPARAM(hItem->m_lAttributes);  \
		hItem->m_lAttributes = MAKELPARAM(lw, hw);}
	
	#define GTV_GETATTRIB_HASBUTTON(hItem) ((GTV_HI_LPARAM(hItem->m_lAttributes) & GTVA_HASNOBUTTON) != GTVA_HASNOBUTTON)

#endif /* _ESIMFCCONTROLS_ */

// CTreeNode class definition
class CTreeNode
{	
	friend class CTreeGridCtrl;
	friend class CTreeCell;

public:

	// Hold the user data
	LPARAM m_lParam;

	// Hold the parent node handle
	GTV_HTREENODE  m_hParent;

	// Hold the next sibling node handle
	GTV_HTREENODE  m_hNextSibling;

	// Hold the previous sibling node handle
	GTV_HTREENODE  m_hPrevSibling;

	// Hold the array of child nodes handle
	GTV_HTREENODES m_arChildNodes;

	// Determine whether the node have a parent
	//
	// Return TRUE if the node hava a parent, otherwise FALSE
	BOOL IsLeaf()
	{
		return m_arChildNodes.GetSize() ? FALSE : TRUE;
	}
	
private:
	// Pointer of the LISTCTRL_ROW which hold the corresponding row information
	GRID_ROW* m_pCorrespondingRow;

	// Internal data carrying the node attributes, managed by CTreeGridCtrl
	// The tree item level value is contained in LOWORD
	// The flag whether the node should be skipped from deleting during DeleteNode is stored in HIWORD	
	LPARAM m_lAttributes;
};

#endif // !defined(__TreeNode_H__)
