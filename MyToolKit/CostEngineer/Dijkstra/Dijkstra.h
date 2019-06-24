// Dijkstra.h : Declaration of the CDijkstra

#ifndef __DIJKSTRA_H_
#define __DIJKSTRA_H_

#include "resource.h"       // main symbols
#include "STDAFX.h"
#include "Graph.h"
#include "Node.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CDijkstra
class CDijkstra : public CWnd
{
	DECLARE_DYNCREATE(CDijkstra)
public:
	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	CDijkstra()
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

		m_AddEdges = false;
		m_AddNodes = true;

		m_bDoubleBuffer = true;
	}

	~CDijkstra()
	{
		ReleaseAll();
	}


// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

protected:
	BOOL	m_bDoubleBuffer;

// IDijkstra
public:
	void ReleaseGraph();
	void RefreshDone(long node1, long node2);
	bool ExistEdge(CNode u, CNode v);
	double GetEdgeVal(CNode u, CNode v);
	void Relax(CNode u, CNode v, double w);
	CNode ExtractMin(VTYPE_NODE& Q);
	HFONT m_lmfont;
	void InitializeSource(CGraph& g, CNode s);
	void ShortestPath(/*[in]*/ long node1, /*[in]*/ long node2);
	CGraph g;
	long GetNode(long x, long y);
	void AddEdge(long x, long y);
	void StartAddEdges();
	void StartAddNodes();
	void Refresh();
	void AddNode(long x, long y);
	void ReleaseAll();
	POINT m_LastLButtonDownPosition;
	bool m_AddNodes;
	bool m_AddEdges;
	//STDMETHOD(Initialize)(/*[in]*/ long nrvert, /*[in]*/ short bidir);

	afx_msg void OnPaint();
	afx_msg void OnDraw(CDC* pDC);
	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	afx_msg void OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam);

	afx_msg void OnSize(UINT nType, int cx, int cy)
	{
		RECT rc;
		::GetClientRect(m_hWnd, &rc);
		if(::IsWindow(m_hWnd))
		{
			::MoveWindow(m_hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, true);
			::ShowWindow(m_hWnd, SW_SHOW);
		}
		return ;
	}
	afx_msg void OnMouseMove(UINT nFlags, CPoint point)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		return ;
	}

	DECLARE_MESSAGE_MAP()
};

#endif //__DIJKSTRA_H_
