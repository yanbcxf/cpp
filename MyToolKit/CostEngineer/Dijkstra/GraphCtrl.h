// Dijkstra.h : Declaration of the CGraphCtrl

#ifndef __GRAPHCTRL_H_
#define __GRAPHCTRL_H_

#include "resource.h"       // main symbols
#include "STDAFX.h"
#include "Graph.h"
#include "Node.h"	// Added by ClassView

#define GRAPHCTRL_CLASSNAME    _T("MFCGraphCtrl")  // Window class name

#define NM_GRAPH_ADD_NODE                0L      
#define NM_GRAPH_DEL_NODE                1L
#define NM_GRAPH_EDIT_NODE               2L

#define NM_GRAPH_ADD_EDGE                3L
#define NM_GRAPH_DEL_EDGE                4L
#define NM_GRAPH_EDIT_EDGE               5L

typedef struct tagNM_GRAPH_DEL_EDIT {
	NMHDR hdr;
	int   idx;
} NM_GRAPH_DEL_EDIT_STRUCT;

typedef struct tagNM_GRAPH_ADD_NODE {
	NMHDR hdr;
	LONG  x;
	LONG  y;
} NM_GRAPH_ADD_NODE_STRUCT;

typedef struct tagNM_GRAPH_ADD_EDGE {
	NMHDR hdr;
	
	long m_secondNode;	// the second node number
	long m_firstNode;	// the first node number
	POINT m_secondPct;	// graphical elements for drawing the edges
	POINT m_firstPct;
} NM_GRAPH_ADD_EDGE_STRUCT;




/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl
class CGraphCtrl : public CWnd
{
	DECLARE_DYNCREATE(CGraphCtrl)
public:
	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	CGraphCtrl()
	{
		if(m_lmfont)
			::DeleteObject(m_lmfont);
		LOGFONT	m_FontUse;
		memset(&m_FontUse, 0, sizeof(LOGFONT));
		m_FontUse.lfHeight = 15;
		m_FontUse.lfWeight = FW_NORMAL;
		m_FontUse.lfOutPrecision=OUT_TT_PRECIS;
		m_FontUse.lfClipPrecision=CLIP_CHARACTER_PRECIS;
		m_FontUse.lfQuality=PROOF_QUALITY;
		m_FontUse.lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
		memcpy(m_FontUse.lfFaceName, "Arial", 5);
		m_FontUse.lfFaceName[6] = '\0';
		m_lmfont = ::CreateFontIndirect(&m_FontUse);

		m_mode_addNodes = true;
		m_mode_addEdges = false;
		m_mode_update = false;
		m_mode_delete = false;

		m_bDoubleBuffer = true;
	}

	~CGraphCtrl()
	{
		ReleaseAll();
	}


// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

protected:
	BOOL	m_bDoubleBuffer;
	bool	m_mode_addNodes;
	bool	m_mode_addEdges;
	bool	m_mode_update;
	bool	m_mode_delete;
	POINT m_LastLButtonDownPosition;

	HFONT m_lmfont;
	CGraph g;

protected:
	void OnAddEdge(long x, long y);
	void OnAddNode(long x, long y);
	void OnUpdate(long x, long y);
	void OnDelete(long x, long y);


// IDijkstra
public:
	void initGraph();
	bool ExistEdge(CNode u, CNode v);
	double GetEdgeVal(CNode u, CNode v);
	void Relax(CNode u, CNode v, double w);
		
	int GetNode(long x, long y);

	void AddEdge(int from, int to);
	void AddNode(long x, long y);

	void Refresh();
	void ReleaseAll();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnDraw(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnGraphAddEdge();
	afx_msg void OnGraphAddNode();
	afx_msg void OnGraphDelete();
	afx_msg void OnGraphUpdate();
	
};

#endif //__DIJKSTRA_H_
