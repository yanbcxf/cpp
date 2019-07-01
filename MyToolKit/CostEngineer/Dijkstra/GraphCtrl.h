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
#define NM_GRAPH_MOVE_NODE               3L

#define NM_GRAPH_ADD_EDGE                4L
#define NM_GRAPH_DEL_EDGE                5L
#define NM_GRAPH_EDIT_EDGE               6L

typedef struct tagNM_GRAPH_DEL_EDIT {
	NMHDR hdr;
	int   idx;
	LONG  x;			//	对于 移动中的 节点的 x 和 y 坐标
	LONG  y;
} NM_GRAPH_DEL_EDIT_MOVE_STRUCT;

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
		m_mode_move = false;
		m_mode_tips = false;

		m_bDoubleBuffer = true;
		m_pBalloonTip = NULL;
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
	bool	m_mode_move;
	bool	m_mode_tips;

	POINT	m_LastLButtonDownPosition;
	HFONT	m_lmfont;
	int		m_node_radius;
	CGraph g;

	/* 多个屏幕时，不好用，不用了 */
	CBalloonTip* m_pBalloonTip;

	/* https://www.codeproject.com/Articles/3655/CPPToolTip-v2-1 */
	CToolTipCtrl m_toolTipsCtrl;

protected:
	void DisplayBalloon(int x, int y, const CString & szMessage);
	void OnAddEdge(long x, long y);
	void OnAddNode(long x, long y);
	void OnUpdate(long x, long y);
	void OnDelete(long x, long y);
	void OnMove(long x, long y);
	void OnTips(long x, long y);

	void ShowBalloonTip(long x, long y, string tips);

// IDijkstra
public:
	void initGraph();
	bool ExistEdge(CNode u, CNode v);

		
	int	GetNode(long x, long y);
	int	GetEdge(long x, long y);

	/* 先调用 AddNode 插入节点 ，然后调用 AddEdge 插入边，以便正确确定 节点的绘制半径 */
	void AddNode(long x, long y, string top,  string middle = "", string bottom = "", string tips = "");
	void AddEdge(int from, int to, string top , string bottom = "", string tips = "");

	void Refresh();
	void ReleaseAll();
	void DrawNode(CDC* pDC, CNode node);

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
	afx_msg void OnGraphMove();
	afx_msg void OnGraphTips();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif //__DIJKSTRA_H_
