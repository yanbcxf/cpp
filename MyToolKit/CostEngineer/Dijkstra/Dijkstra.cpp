// Dijkstra.cpp : Implementation of CDijkstra

#include "stdafx.h"
#include "Dijkstra.h"
#include "Node.h"
#include "../GridCtrl_src/MemDC.h"
//#include <atlbase.h>
//#include <atlctl.h>
//#include <atlcom.h>
//#include <comutil.h>
//#include <comdef.h>

#define MAXDOUBLE   0xfffffffffffffffe

/////////////////////////////////////////////////////////////////////////////
// CDijkstra

IMPLEMENT_DYNCREATE(CDijkstra, CWnd)

BEGIN_MESSAGE_MAP(CDijkstra, CWnd)
	//EFW - Added ON_WM_RBUTTONUP
	//{{AFX_MSG_MAP(CDijkstra)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CDijkstra::OnPaint()
{
	CPaintDC dc(this);      // device context for painting

	if (m_bDoubleBuffer)    // Use a memory DC to remove flicker
	{
		CMemDC1 MemDC(&dc);
		OnDraw(&MemDC);
	}
	else                    // Draw raw - this helps in debugging vis problems.
		OnDraw(&dc);
}

void CDijkstra::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO : Add Code for message handler. Call DefWindowProc if necessary.
	return;
}

void CDijkstra::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LastLButtonDownPosition.x = point.x;
	m_LastLButtonDownPosition.y = point.y;
	// short fwKeys = short(UINT(wParam));
	if (m_AddNodes)
		AddNode(point.x, point.y);
	else if (m_AddEdges)
		AddEdge(point.x, point.y);
	// Fire_MouseDown(1,fwKeys,px,py);
	return;
}

void CDijkstra::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// TODO : Add Code for message handler. Call DefWindowProc if necessary.
	return;
}

// creates the control - use like any other window create control
BOOL CDijkstra::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	ASSERT(pParentWnd->GetSafeHwnd());

	if (!CWnd::Create(GRIDCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	//Initialise(); - called in PreSubclassWnd

	// The number of rows and columns will only be non-zero if the constructor
	// was called with non-zero initialising parameters. If this window was created
	// using a dialog template then the number of rows and columns will be 0 (which
	// means that the code below will not be needed - which is lucky 'cause it ain't
	// gonna get called in a dialog-template-type-situation.

	return TRUE;
}

void CDijkstra::OnDraw(CDC* pDC)
{
	//RECT& rc = *(RECT*)di.prcBounds;
	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	pDC->Rectangle(rc.left, rc.top, rc.right, rc.bottom);
	HPEN pen=CreatePen(PS_SOLID,0,RGB(0,0,0));
	HPEN penred=CreatePen(PS_SOLID,2,RGB(255,0,0));
	HBRUSH brush=CreateSolidBrush(RGB(0,0,0));
	HPEN oldpen;
	HPEN oldbrush;
	oldpen=(HPEN)pDC->SelectObject(pen);

	HFONT OldFont = (HFONT)pDC->SelectObject(m_lmfont);

	// draw the nodes and the text on the nodes
	long nr = 0;
	VTYPE_NODE::iterator kl;
	for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
	{
		char s[10];
		//ltoa((*kl).m_NodeNr, s, 10);
		pDC->Ellipse((*kl).m_p.x-10, (*kl).m_p.y-10, (*kl).m_p.x+10, (*kl).m_p.y+10);
		if(nr<9)
			pDC->TextOut((*kl).m_p.x-5, (*kl).m_p.y-8, s, 1);
		else
			pDC->TextOut((*kl).m_p.x-5, (*kl).m_p.y-8, s, 2);
		nr++;
	}	
	oldbrush=(HPEN)pDC->SelectObject(brush);

	// draw the edges and the text on the edges
	VTYPE_EDGE::iterator kll;
	for(kll=g.m_edges.begin(); kll<g.m_edges.end(); kll++)
	{
		HPEN temp;
		if((*kll).m_red)
			temp=(HPEN)pDC->SelectObject(penred);
		pDC->MoveTo((*kll).m_firstPct.x, (*kll).m_firstPct.y);
		pDC->LineTo((*kll).m_secondPct.x, (*kll).m_secondPct.y);
		pDC->Ellipse((*kll).m_secondPct.x-5, (*kll).m_secondPct.y-5, (*kll).m_secondPct.x+5, (*kll).m_secondPct.y+5);
		// calcul middle pint
		POINT po;
		po.x = ((*kll).m_firstPct.x+(*kll).m_secondPct.x)/2;
		po.y = ((*kll).m_firstPct.y+(*kll).m_secondPct.y)/2;
		char s[5];
		ltoa((*kll).m_cost, s, 10);
		pDC->TextOut(po.x, po.y, s, 1);
		if((*kll).m_red)
			pDC->SelectObject(temp);
	}
	pDC->SelectObject(OldFont);
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	DeleteObject(pen);
	DeleteObject(brush);

}

void CDijkstra::ReleaseAll()
{
	if(m_lmfont)
		::DeleteObject(m_lmfont);
}

void CDijkstra::AddNode(long x, long y)
{
	CNode node;
	node.m_p.x = x;
	node.m_p.y = y;
	
	g.m_nodes.push_back(node);
	Refresh();
}

void CDijkstra::Refresh()
{
	HDC dc = ::GetDC(m_hWnd);
	HPEN pen=CreatePen(PS_SOLID,0,RGB(0,0,0));
	HPEN penred=CreatePen(PS_SOLID,2,RGB(255,0,0));
	HBRUSH brush=CreateSolidBrush(RGB(0,0,0));
	HPEN oldpen;
	HPEN oldbrush;
	oldpen=(HPEN)SelectObject(dc,pen);

	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);

	HFONT OldFont = (HFONT)::SelectObject(dc, m_lmfont);

	long nr = 0;
	VTYPE_NODE::iterator kl;
	for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
	{
		char s[5];
		ltoa((*kl).m_NodeNr, s, 10);
		Ellipse(dc, (*kl).m_p.x-10, (*kl).m_p.y-10, (*kl).m_p.x+10, (*kl).m_p.y+10);
		if(nr<9)
			TextOut(dc, (*kl).m_p.x-5, (*kl).m_p.y-8, s, 1);
		else
			TextOut(dc, (*kl).m_p.x-8, (*kl).m_p.y-8, s, 2);
		nr++;
	}

	oldbrush=(HPEN)SelectObject(dc,brush);
	VTYPE_EDGE::iterator kll;
	for(kll=g.m_edges.begin(); kll<g.m_edges.end(); kll++)
	{
		HPEN temp;
		if((*kll).m_red)
			temp=(HPEN)SelectObject(dc,penred);
		MoveToEx(dc, (*kll).m_firstPct.x, (*kll).m_firstPct.y, NULL);
		LineTo(dc, (*kll).m_secondPct.x, (*kll).m_secondPct.y);
		Ellipse(dc, (*kll).m_secondPct.x-5, (*kll).m_secondPct.y-5, (*kll).m_secondPct.x+5, (*kll).m_secondPct.y+5);
		// calcul middle pint
		POINT po;
		po.x = ((*kll).m_firstPct.x+(*kll).m_secondPct.x)/2;
		po.y = ((*kll).m_firstPct.y+(*kll).m_secondPct.y)/2;
		char s[5];
		ltoa((*kll).m_cost, s, 10);
		TextOut(dc, po.x, po.y, s, 1);
		if((*kll).m_red)
			SelectObject(dc,temp);
	}
	::SelectObject(dc, OldFont);

	SelectObject(dc,oldpen);
	SelectObject(dc,oldbrush);
	DeleteObject(pen);
	DeleteObject(brush);
	::ReleaseDC(m_hWnd, dc);
}

void CDijkstra::StartAddNodes()
{
	m_AddEdges = false;
	m_AddNodes = true;
}

void CDijkstra::StartAddEdges()
{
	m_AddNodes = false;
	m_AddEdges = true;
}

void CDijkstra::AddEdge(long x, long y)
{
	MSG msg;
	HDC hdc = ::GetDC(m_hWnd);
	SetROP2(hdc,R2_NOTXORPEN);
	HPEN pen=CreatePen(PS_SOLID,0,RGB(0,0,0));
	HPEN oldpen;
	//POINT Apct;
	long pox, poy;
	oldpen=(HPEN)SelectObject(hdc,pen);
	bool TrackFinished = false;

	long firstnode = GetNode(x, y);
	long secondnode;

	// while mouse not up try to find the nodes between which it will draw the edge
	while(!TrackFinished)
	{
		GetMessage(&msg,m_hWnd,0x200,0x204);
        if(msg.message==WM_MOUSEMOVE)
        { 
    		int poxT = ((int)LOWORD(msg.lParam));
    		int poyT = ((int)HIWORD(msg.lParam));
        	MoveToEx(hdc,x,y,NULL);
        	LineTo(hdc,pox,poy);
        	MoveToEx(hdc,x,y,NULL);
        	LineTo(hdc,poxT,poyT);
        	short fwKeys = short(UINT(msg.wParam));
    		pox = poxT;
    		poy = poyT;
        }
		if(msg.message==WM_LBUTTONUP)
		{
    		int poxT = ((int)LOWORD(msg.lParam));
    		int poyT = ((int)HIWORD(msg.lParam));
			secondnode = GetNode(poxT, poyT);
			TrackFinished = true;
			if(firstnode!=0 && secondnode!=0)
			{
				CEdge ed;
				ed.m_firstNode = firstnode;
				ed.m_firstPct.x = x;
				ed.m_firstPct.y = y;
				ed.m_secondNode = secondnode;
				ed.m_secondPct.x = poxT;
				ed.m_secondPct.y = poyT;
				g.m_edges.push_back(ed);
			}
		}
	}

    SelectObject(hdc,oldpen);
	SetROP2(hdc,R2_COPYPEN);
	DeleteObject(pen);
	::ReleaseDC(m_hWnd, hdc);
	m_AddNodes = false;
	Refresh();
}

// used in getedge to find graphically the node
long CDijkstra::GetNode(long x, long y)
{
	VTYPE_NODE::iterator kl;
	for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
	{
		if(x<(*kl).m_p.x+20 && x>(*kl).m_p.x-20 && y<(*kl).m_p.y+20 && y>(*kl).m_p.y-20)
			return (*kl).m_NodeNr;
	}
	return 0;
}


// The Dijkstra's algorithm
void CDijkstra::ShortestPath(long node1, long node2)
{
	ReleaseGraph();
	// init d and pi
	InitializeSource(g, g.m_nodes[node1-1]);
	// Set S empty
	VTYPE_NODE S;
	// Put nodes in Q
	VTYPE_NODE Q;
	VTYPE_NODE::iterator kl;
	for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
	{
		CNode node = (*kl).Copy();
		Q.push_back(node);
	}
	// Algorithm
	while(Q.size())
	{
		CNode nod = ExtractMin(Q); // the minim value for the shortest path up to this step
		S.push_back(nod);
		// each vertex v which is a neighbour of u
		VTYPE_NODE::iterator kl;
		for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
		{
			if(ExistEdge(nod, (*kl)))
			{
				bool gasit = false;
				VTYPE_NODE::iterator kll;
				for(kll=Q.begin(); kll<Q.end(); kll++)
				{
					if((*kll).m_NodeNr == (*kl).m_NodeNr)
						gasit = true;
				}
				if(gasit)
					Relax(nod, (*kl), GetEdgeVal(nod, (*kl)));
			}
		}
	}

	/*_bstr_t ss;
	VTYPE_NODE_P::iterator kll;
	for(kll=g.pi.begin(); kll<g.pi.end(); kll++)
	{
		char s[5];
		long x = (*kll);
		ltoa(x, s, 10);
		ss = ss+(LPCSTR)s;
		ss = ss+(LPCSTR)"  ";
	}*/
	
	RefreshDone(node1, node2);
	//::MessageBox(this->m_hWndCD, (LPCSTR)ss, "Result", MB_OK);

	return;
}

void CDijkstra::InitializeSource(CGraph& g, CNode s)
{
	VTYPE_NODE::iterator kl;
	for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
	{
		long d, pi;
		d = (long)MAXLONG;
		if((*kl).m_NodeNr == s.m_NodeNr)
			d = 0;
		g.d.push_back(d);
		pi = 0;
		g.pi.push_back(pi);
	}
}

CNode CDijkstra::ExtractMin(VTYPE_NODE& Q)
{
	CNode node;
	double min = (double)MAXDOUBLE;
	long elem;
	VTYPE_NODE::iterator kl;
	for(kl=Q.begin(); kl<Q.end(); kl++)
	{
		long xx = g.d[(*kl).m_NodeNr-1];
		if(g.d[(*kl).m_NodeNr-1]<min)
		{
			min = g.d[(*kl).m_NodeNr-1];
			elem = (*kl).m_NodeNr;
		}
	}
	node = g.m_nodes[elem-1].Copy();
	for(kl=Q.begin(); kl<Q.end(); kl++)
	{
		if(elem == (*kl).m_NodeNr)
		{
			Q.erase(kl);
			break;
		}
	}
	return node;
}

void CDijkstra::Relax(CNode u, CNode v, double w)
{
	if(g.d[v.m_NodeNr-1]>g.d[u.m_NodeNr-1]+w)
	{
		g.d[v.m_NodeNr-1] = g.d[u.m_NodeNr-1]+w;
		g.pi[v.m_NodeNr-1] = u.m_NodeNr;
	}
}

double CDijkstra::GetEdgeVal(CNode u, CNode v)
{
	VTYPE_EDGE::iterator kl;
	for(kl=g.m_edges.begin(); kl<g.m_edges.end(); kl++)
	{
		if(((*kl).m_firstNode == u.m_NodeNr && (*kl).m_secondNode == v.m_NodeNr))/* ||
			((*kl).m_firstNode == v.m_NodeNr && (*kl).m_secondNode == u.m_NodeNr))*/
			return (*kl).m_cost;
	}
	return 0;
}

bool CDijkstra::ExistEdge(CNode u, CNode v)
{
	VTYPE_EDGE::iterator kl;
	for(kl=g.m_edges.begin(); kl<g.m_edges.end(); kl++)
	{
		if(((*kl).m_firstNode == u.m_NodeNr && (*kl).m_secondNode == v.m_NodeNr))/* ||
			((*kl).m_firstNode == v.m_NodeNr && (*kl).m_secondNode == u.m_NodeNr))*/
			return true;
	}
	return false;
}

void CDijkstra::RefreshDone(long node1, long node2)
{
	// CALCULATE RED
	long curnode = node2;
	while(curnode!=node1 && curnode != 0 /*nu exista conexiune*/)
	{
		long nextnode = g.pi[curnode-1];
		VTYPE_EDGE::iterator kk;
		for(kk=g.m_edges.begin(); kk<g.m_edges.end(); kk++)
		{
			if((*kk).m_firstNode == nextnode && (*kk).m_secondNode == curnode)
			{
				(*kk).m_red = true;
			}
		}
		curnode = nextnode;
	}

	if(curnode == 0)
		::MessageBox(this->m_hWnd, "There is no path!", "Result", MB_OK);
	else
	{
		// DRAW
		HDC dc = ::GetDC(m_hWnd);
		HPEN pen=CreatePen(PS_SOLID,0,RGB(0,0,0));
		HPEN penred=CreatePen(PS_SOLID,2,RGB(255,0,0));
		HBRUSH brush=CreateSolidBrush(RGB(0,0,0));
		HPEN oldpen;
		HPEN oldbrush;
		oldpen=(HPEN)SelectObject(dc,pen);

		RECT rc;
		::GetClientRect(m_hWnd, &rc);
		Rectangle(dc, rc.left, rc.top, rc.right, rc.bottom);

		HFONT OldFont = (HFONT)::SelectObject(dc, m_lmfont);

		long nr = 0;
		VTYPE_NODE::iterator kl;
		for(kl=g.m_nodes.begin(); kl<g.m_nodes.end(); kl++)
		{
			char s[5];
			ltoa((*kl).m_NodeNr, s, 10);;
			Ellipse(dc, (*kl).m_p.x-10, (*kl).m_p.y-10, (*kl).m_p.x+10, (*kl).m_p.y+10);
			if(nr<9)
				TextOut(dc, (*kl).m_p.x-5, (*kl).m_p.y-8, s, 1);
			else
				TextOut(dc, (*kl).m_p.x-8, (*kl).m_p.y-8, s, 2);
			nr++;
		}

		oldbrush=(HPEN)SelectObject(dc,brush);
		VTYPE_EDGE::iterator kll;
		for(kll=g.m_edges.begin(); kll<g.m_edges.end(); kll++)
		{
			HPEN temp;
			if((*kll).m_red)
				temp=(HPEN)SelectObject(dc,penred);
			MoveToEx(dc, (*kll).m_firstPct.x, (*kll).m_firstPct.y, NULL);
			LineTo(dc, (*kll).m_secondPct.x, (*kll).m_secondPct.y);
			Ellipse(dc, (*kll).m_secondPct.x-5, (*kll).m_secondPct.y-5, (*kll).m_secondPct.x+5, (*kll).m_secondPct.y+5);
			POINT po;
			po.x = ((*kll).m_firstPct.x+(*kll).m_secondPct.x)/2;
			po.y = ((*kll).m_firstPct.y+(*kll).m_secondPct.y)/2;
			char s[5];
			ltoa((*kll).m_cost, s, 10);
			TextOut(dc, po.x, po.y, s, 1);
			if((*kll).m_red)
				SelectObject(dc,temp);
		}
		::SelectObject(dc, OldFont);
		SelectObject(dc,oldpen);
		SelectObject(dc,oldbrush);
		DeleteObject(pen);
		DeleteObject(brush);
		::ReleaseDC(m_hWnd, dc);
	}
}

void CDijkstra::ReleaseGraph()
{
	g.d.clear();
	g.pi.clear();
	VTYPE_EDGE::iterator kll;
	for(kll=g.m_edges.begin(); kll<g.m_edges.end(); kll++)
	{
		(*kll).m_red = false;
	}
	Refresh();
}

