// Dijkstra.cpp : Implementation of CGraphCtrl

#include "stdafx.h"
#include "GraphCtrl.h"
#include "Node.h"
#include "../GridCtrl_src/MemDC.h"
//#include <atlbase.h>
//#include <atlctl.h>
//#include <atlcom.h>
//#include <comutil.h>
//#include <comdef.h>

#define MAXDOUBLE   0xfffffffffffffffe

/////////////////////////////////////////////////////////////////////////////
// CGraphCtrl

IMPLEMENT_DYNCREATE(CGraphCtrl, CWnd)

BEGIN_MESSAGE_MAP(CGraphCtrl, CWnd)
	//EFW - Added ON_WM_RBUTTONUP
	//{{AFX_MSG_MAP(CGraphCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_GRAPH_ADD_EDGE, &CGraphCtrl::OnGraphAddEdge)
	ON_COMMAND(ID_GRAPH_ADD_NODE, &CGraphCtrl::OnGraphAddNode)
	ON_COMMAND(ID_GRAPH_DELETE, &CGraphCtrl::OnGraphDelete)
	ON_COMMAND(ID_GRAPH_UPDATE, &CGraphCtrl::OnGraphUpdate)
	ON_COMMAND(ID_GRAPH_MOVE, &CGraphCtrl::OnGraphMove)
	ON_COMMAND(ID_GRAPH_TIPS, &CGraphCtrl::OnGraphTips)
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CGraphCtrl::OnPaint()
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

void CGraphCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO : Add Code for message handler. Call DefWindowProc if necessary.
	return;
}


void CGraphCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LastLButtonDownPosition.x = point.x;
	m_LastLButtonDownPosition.y = point.y;

	// CBalloonTip::Hide(m_pBalloonTip);
	m_toolTipsCtrl.Activate(FALSE);
	
	if (m_mode_addNodes)
		OnAddNode(point.x, point.y);
	else if (m_mode_addEdges)
		OnAddEdge(point.x, point.y);
	else if (m_mode_update)
		OnUpdate(point.x, point.y);
	else if (m_mode_delete)
		OnDelete(point.x, point.y);
	else if (m_mode_move)
		OnMove(point.x, point.y);
	else if (m_mode_tips)
		OnTips(point.x, point.y);
	return;
}

void CGraphCtrl::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// TODO : Add Code for message handler. Call DefWindowProc if necessary.
	return;
}

void CGraphCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	return;
}

void CGraphCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO : Add Code for message handler. Call DefWindowProc if necessary.
	return;
}

void CGraphCtrl::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	CPoint pt = point;
	CMenu menu;
	CMenu* popupMenu = NULL;

	ScreenToClient(&pt);

	//加载菜单
	menu.LoadMenu(IDR_POPUP_COMPONENT);

	//子菜单项
	//右键点击 弹出此子菜单项
	popupMenu = menu.GetSubMenu(1);
	popupMenu->CheckMenuItem(ID_GRAPH_ADD_NODE, MF_BYCOMMAND | m_mode_addNodes ? MF_CHECKED : MF_UNCHECKED);
	popupMenu->CheckMenuItem(ID_GRAPH_ADD_EDGE, MF_BYCOMMAND | m_mode_addEdges ? MF_CHECKED : MF_UNCHECKED);
	popupMenu->CheckMenuItem(ID_GRAPH_UPDATE, MF_BYCOMMAND | m_mode_update ? MF_CHECKED : MF_UNCHECKED);
	popupMenu->CheckMenuItem(ID_GRAPH_DELETE, MF_BYCOMMAND | m_mode_delete ? MF_CHECKED : MF_UNCHECKED);
	popupMenu->CheckMenuItem(ID_GRAPH_MOVE, MF_BYCOMMAND | m_mode_move ? MF_CHECKED : MF_UNCHECKED);
	popupMenu->CheckMenuItem(ID_GRAPH_TIPS, MF_BYCOMMAND | m_mode_tips ? MF_CHECKED : MF_UNCHECKED);

	popupMenu->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
}


int CGraphCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


void CGraphCtrl::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	// https://www.xuebuyuan.com/837800.html
	/* 使用 DDX_Control 创建时，CGraphCtrl 控件窗口创建后会调用本函数 */

	if (::IsWindow(m_hWnd)) {
		EnableToolTips(TRUE);
		m_toolTipsCtrl.Create(this);
		m_toolTipsCtrl.Activate(TRUE);

		/* 设定宽度，\r\n和空格就会同时起作用，只是空格是在一行宽度超过设定宽度时起作用的 */
		m_toolTipsCtrl.SetMaxTipWidth(120);
		m_toolTipsCtrl.SetMargin(CRect(3, 3, 3, 3));

		m_toolTipsCtrl.AddTool(this, "图形控件"); //为此控件添加tip
	}

	CWnd::PreSubclassWindow();
}



BOOL CGraphCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (NULL != m_toolTipsCtrl.GetSafeHwnd())
	{
		m_toolTipsCtrl.RelayEvent(pMsg);
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CGraphCtrl::ShowBalloonTip(long x, long y, string tips) {

	m_toolTipsCtrl.Activate(TRUE);
	m_toolTipsCtrl.UpdateTipText(tips.c_str(), this);
	CToolInfo       sTinfo;                // 提示信息
	m_toolTipsCtrl.GetToolInfo(sTinfo, this);
	sTinfo.uFlags = TTF_TRACK;     // 显示方式设置
	m_toolTipsCtrl.SetToolInfo(&sTinfo);

	CPoint pt;
	pt.x = x; pt.y = y;
	ClientToScreen(&pt);

	// 下面是关键两步
	m_toolTipsCtrl.SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x, pt.y));
	m_toolTipsCtrl.SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&sTinfo); // tips显示
}

// creates the control - use like any other window create control
BOOL CGraphCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	ASSERT(pParentWnd->GetSafeHwnd());

	if (!CWnd::Create(GRAPHCTRL_CLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	//Initialise(); - called in PreSubclassWnd

	// The number of rows and columns will only be non-zero if the constructor
	// was called with non-zero initialising parameters. If this window was created
	// using a dialog template then the number of rows and columns will be 0 (which
	// means that the code below will not be needed - which is lucky 'cause it ain't
	// gonna get called in a dialog-template-type-situation.

	return TRUE;
}

void CGraphCtrl::DrawNode(CDC* pDC, CNode node) {

	if (node.m_bottom.empty() && node.m_middle.empty()) {
		pDC->Ellipse(node.m_p.x - m_node_radius, node.m_p.y - m_node_radius, node.m_p.x + m_node_radius, node.m_p.y + m_node_radius);
		pDC->TextOut(node.m_p.x - 5, node.m_p.y - 8, node.m_top.c_str(), node.m_top.length());
	}
	else {
		pDC->Ellipse(node.m_p.x - m_node_radius, node.m_p.y - m_node_radius, node.m_p.x + m_node_radius, node.m_p.y + m_node_radius);

		pDC->TextOut(node.m_p.x - 3 * node.m_top.length(), node.m_p.y - 8 - 14, node.m_top.c_str(), node.m_top.length());
		pDC->TextOut(node.m_p.x - 3 * node.m_middle.length(), node.m_p.y - 8 , node.m_middle.c_str(), node.m_middle.length());
		pDC->TextOut(node.m_p.x - 3 * node.m_bottom.length(), node.m_p.y - 8 + 15, node.m_bottom.c_str(), node.m_bottom.length());

		
		pDC->MoveTo(node.m_p.x - 22, node.m_p.y - 8);
		pDC->LineTo(node.m_p.x + 22, node.m_p.y - 8);
		pDC->MoveTo(node.m_p.x - 22, node.m_p.y + 7);
		pDC->LineTo(node.m_p.x + 22, node.m_p.y + 7);
	}

}

void CGraphCtrl::OnDraw(CDC* pDC)
{
	//RECT& rc = *(RECT*)di.prcBounds;
	CRect rc;
	GetClientRect(rc);
	
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
		DrawNode(pDC, *kl);
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

		/* 绘制箭头 */
		// pDC->Ellipse((*kll).m_secondPct.x-5, (*kll).m_secondPct.y-5, (*kll).m_secondPct.x+5, (*kll).m_secondPct.y+5);
		pDC->MoveTo((*kll).m_secondPct.x, (*kll).m_secondPct.y);
		pDC->LineTo((*kll).m_arrow1.x, (*kll).m_arrow1.y);

		pDC->MoveTo((*kll).m_secondPct.x, (*kll).m_secondPct.y);
		pDC->LineTo((*kll).m_arrow2.x, (*kll).m_arrow2.y);

		// calcul middle pint
		POINT po;
		po.x = ((*kll).m_firstPct.x+(*kll).m_secondPct.x)/2;
		po.y = ((*kll).m_firstPct.y+(*kll).m_secondPct.y)/2;
				
		/* 绘制边上的标注 */
		pDC->TextOut(po.x - 13, po.y - 13, (*kll).m_top.c_str(), (*kll).m_top.length());
		pDC->TextOut(po.x + 4, po.y + 4, (*kll).m_bottom.c_str(), (*kll).m_bottom.length());

		if((*kll).m_red)
			pDC->SelectObject(temp);
	}
	pDC->SelectObject(OldFont);
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	DeleteObject(pen);
	DeleteObject(brush);

}

void CGraphCtrl::ReleaseAll()
{
	if(m_lmfont)
		::DeleteObject(m_lmfont);
}

void CGraphCtrl::initGraph() {
	g.m_edges.clear();
	g.m_nodes.clear();
	/* 默认的 节点半径  */
	m_node_radius = 15;
}

void CGraphCtrl::AddNode(long x, long y, string top, string middle, string bottom, string tips) {
	CNode node;
	node.m_p.x = x;
	node.m_p.y = y;
	node.m_top = top;
	node.m_middle = middle;
	node.m_bottom = bottom;
	node.m_tips = tips;
	
	g.m_nodes.push_back(node);

	/* 当有多行时，放大节点半径  */
	if (!node.m_bottom.empty() || !node.m_middle.empty())
		m_node_radius = 25;
}


void  CGraphCtrl::AddEdge(int from, int to, string top, string bottom, string tips) {
	CEdge ed;
	ed.m_firstNode = from;
	ed.m_secondNode = to;

	double x1 = g.m_nodes[from].m_p.x;
	double y1 = g.m_nodes[from].m_p.y;
	double x2 = g.m_nodes[to].m_p.x;
	double y2 = g.m_nodes[to].m_p.y;
	double d = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	double x = (x2 - x1) / d;
	double y = (y2 - y1) / d;
	
	/* 确定边线的起始和结束点 （扣除节点的半径） */
	ed.m_firstPct.x = x1 + x * m_node_radius;
	ed.m_firstPct.y = y1 + y * m_node_radius;
	ed.m_secondPct.x = x2 - x * m_node_radius;
	ed.m_secondPct.y = y2 - y * m_node_radius;

	/* 确定箭头的 绘制点 */
	x = -x;
	y = -y;
	double arrow1_x = x * cos(3.14159 / 6) - y * sin(3.14159 / 4);
	double arrow1_y = x * sin(3.14159 / 6) + y * cos(3.14159 / 4);
	double arrow2_x = x * cos(-3.14159 / 6) - y * sin(-3.14159 / 4);
	double arrow2_y = x * sin(-3.14159 / 6) + y * cos(-3.14159 / 4);

	ed.m_arrow1.x = ed.m_secondPct.x + arrow1_x * 10;
	ed.m_arrow1.y = ed.m_secondPct.y + arrow1_y * 10;
	ed.m_arrow2.x = ed.m_secondPct.x + arrow2_x * 10;
	ed.m_arrow2.y = ed.m_secondPct.y + arrow2_y * 10;

	ed.m_top = top;
	ed.m_bottom = bottom;
	ed.m_tips = tips;
	g.m_edges.push_back(ed);
}


void CGraphCtrl::Refresh()
{
	if (GetSafeHwnd())
		Invalidate();
}



// used in getedge to find graphically the node
int CGraphCtrl::GetNode(long x, long y)
{
	VTYPE_NODE::iterator kl;
	for(int i = 0; i< g.m_nodes.size(); i++)
	{
		CNode kl = g.m_nodes[i];
		if (x< kl.m_p.x + m_node_radius && x>kl.m_p.x - m_node_radius && y<kl.m_p.y + m_node_radius && y>kl.m_p.y - m_node_radius)
			return i;
	}
	return -1;
}

/**
* 计算点在直线的垂足.
*/
POINT footpointOfLine(const POINT& pt, const POINT& v1, const POINT& v2) {

	double a, b, c;		// 直线一般式方程的系数
	a = v2.y - v1.y;
	b = v1.x - v2.x;
	c = v2.x * v1.y - v1.x * v2.y;

	POINT r;
	r.x = (b * b * pt.x - a * b * pt.y - a * c) / (a * a + b * b);
	r.y = (a * a * pt.y - a * b * pt.x - b * c) / (a * a + b * b);
	return r;
}

int CGraphCtrl::GetEdge(long x, long y)
{
	vector<int> edges;
	for (int i = 0; i < g.m_edges.size(); i++)
	{
		CEdge e = g.m_edges[i];
		POINT min, max;
		min.x = e.m_firstPct.x < e.m_secondPct.x ? e.m_firstPct.x : e.m_secondPct.x;
		min.y = e.m_firstPct.y < e.m_secondPct.y ? e.m_firstPct.y : e.m_secondPct.y;
		max.x = e.m_firstPct.x > e.m_secondPct.x ? e.m_firstPct.x : e.m_secondPct.x;
		max.y = e.m_firstPct.y > e.m_secondPct.y ? e.m_firstPct.y : e.m_secondPct.y;

		/* 对于水平或者垂直的 边线 ，扩大搜索区域 */
		if (max.x - min.x < 30) {
			min.x = (max.x + min.x) / 2 - 15;
			max.x = (max.x + min.x) / 2 + 15;
		}
		if (max.y - min.y < 30) {
			min.y = (max.y + min.y) / 2 - 15;
			max.y = (max.y + min.y) / 2 + 15;
		}

		if (x > min.x && x < max.x && y > min.y && y < max.y)
			edges.push_back(i);
	}
	if (edges.size() == 0)
		return -1;

	double d = 100000000;
	int idx = -1;
	for (int i = 0; i < edges.size(); i++) {
		POINT pt;
		pt.x = x; pt.y = y;
		pt = footpointOfLine(pt, g.m_edges[edges[i]].m_firstPct, g.m_edges[edges[i]].m_secondPct);
		double ds = sqrt((pt.x - x) * (pt.x - x) + (pt.y - y) * (pt.y - y));
		if (ds < d) {
			d = ds;
			idx = edges[i];
		}
	}
	return idx;
}


bool CGraphCtrl::ExistEdge(CNode u, CNode v)
{
	VTYPE_EDGE::iterator kl;
	/*for(kl=g.m_edges.begin(); kl<g.m_edges.end(); kl++)
	{
		if(((*kl).m_firstNode == u.m_NodeNr && (*kl).m_secondNode == v.m_NodeNr))
			return true;
	}*/
	return false;
}


void CGraphCtrl::OnGraphAddEdge()
{
	// TODO: 在此添加命令处理程序代码
	m_mode_addEdges = true;
	m_mode_addNodes = false;
	m_mode_update = false;
	m_mode_delete = false;
	m_mode_move = false;
	m_mode_tips = false;
}


void CGraphCtrl::OnGraphAddNode()
{
	// TODO: 在此添加命令处理程序代码
	m_mode_addEdges = false;
	m_mode_addNodes = true;
	m_mode_update = false;
	m_mode_delete = false;
	m_mode_move = false;
	m_mode_tips = false;
}


void CGraphCtrl::OnGraphDelete()
{
	// TODO: 在此添加命令处理程序代码
	m_mode_addEdges = false;
	m_mode_addNodes = false;
	m_mode_update = false;
	m_mode_delete = true;
	m_mode_move = false;
	m_mode_tips = false;
}


void CGraphCtrl::OnGraphUpdate()
{
	// TODO: 在此添加命令处理程序代码
	m_mode_addEdges = false;
	m_mode_addNodes = false;
	m_mode_update = true;
	m_mode_delete = false;
	m_mode_move = false;
	m_mode_tips = false;
}

void CGraphCtrl::OnGraphMove()
{
	// TODO: 在此添加命令处理程序代码
	m_mode_addEdges = false;
	m_mode_addNodes = false;
	m_mode_update = false;
	m_mode_delete = false;
	m_mode_move = true;
	m_mode_tips = false;
}

void CGraphCtrl::OnGraphTips()
{
	// TODO: 在此添加命令处理程序代码
	m_mode_addEdges = false;
	m_mode_addNodes = false;
	m_mode_update = false;
	m_mode_delete = false;
	m_mode_move = false;
	m_mode_tips = true;
}

void CGraphCtrl::OnAddNode(long x, long y)
{
	long n = GetNode(x, y);
	if (n >= 0) return;

	/* 该位置不存节点，则插入新节点 */
	NM_GRAPH_ADD_NODE_STRUCT nmgv;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom = GetDlgCtrlID();
	nmgv.hdr.code = NM_GRAPH_ADD_NODE;

	nmgv.x = x;
	nmgv.y = y;

	CWnd *pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
}

void CGraphCtrl::OnAddEdge(long x, long y)
{
	MSG msg;
	HDC hdc = ::GetDC(m_hWnd);
	SetROP2(hdc, R2_NOTXORPEN);
	HPEN pen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	HPEN oldpen;
	//POINT Apct;
	long pox = -1, poy = -1;
	oldpen = (HPEN)SelectObject(hdc, pen);
	bool TrackFinished = false;

	long firstnode = GetNode(x, y);
	long secondnode;

	// while mouse not up try to find the nodes between which it will draw the edge
	while (!TrackFinished)
	{
		GetMessage(&msg, m_hWnd, 0x200, 0x204);
		if (msg.message == WM_MOUSEMOVE)
		{
			int poxT = ((int)LOWORD(msg.lParam));
			int poyT = ((int)HIWORD(msg.lParam));
			if (pox >= 0 || poy >= 0) {
				/* 擦除旧线 */
				MoveToEx(hdc, x, y, NULL);
				LineTo(hdc, pox, poy);
			}
			
			/* 画新线 */
			MoveToEx(hdc, x, y, NULL);
			LineTo(hdc, poxT, poyT);
			short fwKeys = short(UINT(msg.wParam));
			pox = poxT;
			poy = poyT;
		}
		if (msg.message == WM_LBUTTONUP)
		{
			int poxT = ((int)LOWORD(msg.lParam));
			int poyT = ((int)HIWORD(msg.lParam));
			secondnode = GetNode(poxT, poyT);
			TrackFinished = true;
			if (firstnode >= 0 && secondnode >= 0 && firstnode != secondnode)
			{
				NM_GRAPH_ADD_EDGE_STRUCT ed;
				ed.hdr.hwndFrom = m_hWnd;
				ed.hdr.idFrom = GetDlgCtrlID();
				ed.hdr.code = NM_GRAPH_ADD_EDGE;

				ed.m_firstNode = firstnode;
				ed.m_firstPct.x = x;
				ed.m_firstPct.y = y;
				ed.m_secondNode = secondnode;
				ed.m_secondPct.x = poxT;
				ed.m_secondPct.y = poyT;

				CWnd *pOwner = GetOwner();
				if (pOwner && IsWindow(pOwner->m_hWnd))
					pOwner->SendMessage(WM_NOTIFY, ed.hdr.idFrom, (LPARAM)&ed);
			}
			else {
				Refresh();
			}
		}
	}

	SelectObject(hdc, oldpen);
	SetROP2(hdc, R2_COPYPEN);
	DeleteObject(pen);
	::ReleaseDC(m_hWnd, hdc);

	// Refresh();
}

void CGraphCtrl::DisplayBalloon(int x, int y, const CString & szMessage)
{
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = 20;
	lf.lfWeight = FW_BOLD;
	lf.lfUnderline = FALSE;
	::strcpy(lf.lfFaceName, _T("Arial"));

	CPoint pt;
	pt.x = x; pt.y = y;
	ClientToScreen(&pt);

	HDC hdc = ::GetDC(this->m_hWnd);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	int nCharWidth = tm.tmAveCharWidth;
	int nCharHeight = tm.tmHeight;

	CRect ClientRect;
	GetClientRect(&ClientRect);

	m_pBalloonTip = CBalloonTip::Show(pt, &ClientRect, nCharWidth, nCharHeight,
		szMessage, lf, 5, TRUE);
}


void CGraphCtrl::OnUpdate(long x, long y) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT nmgv;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom = GetDlgCtrlID();
	
	nmgv.idx = GetNode(x, y);
	if (nmgv.idx >= 0) {
		/* 编辑 节点 */
		nmgv.hdr.code = NM_GRAPH_EDIT_NODE;
		CWnd *pOwner = GetOwner();
		if (pOwner && IsWindow(pOwner->m_hWnd))
			pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
	}
	else {
		/* 编辑 边 */
		nmgv.idx = GetEdge(x, y);
		if (nmgv.idx >= 0) {
			nmgv.hdr.code = NM_GRAPH_EDIT_EDGE;
			CWnd *pOwner = GetOwner();
			if (pOwner && IsWindow(pOwner->m_hWnd))
				pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);

		}
	}

}

void CGraphCtrl::OnDelete(long x, long y) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT nmgv;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom = GetDlgCtrlID();
	nmgv.hdr.code = NM_GRAPH_DEL_NODE;

	nmgv.idx = GetNode(x, y);
	if (nmgv.idx >= 0) {
		/* 删除 节点 */
		CWnd *pOwner = GetOwner();
		if (pOwner && IsWindow(pOwner->m_hWnd))
			pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
	}
	else {
		/* 删除 边 */
		nmgv.idx = GetEdge(x, y);
		if (nmgv.idx >= 0) {
			nmgv.hdr.code = NM_GRAPH_DEL_EDGE;
			CWnd *pOwner = GetOwner();
			if (pOwner && IsWindow(pOwner->m_hWnd))
				pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
		}
	}
}

void CGraphCtrl::OnMove(long x, long y) {
	MSG msg;
	HDC hdc = ::GetDC(m_hWnd);
	SetROP2(hdc, R2_NOTXORPEN);
	HPEN pen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
	HPEN oldpen;
	//POINT Apct;
	CNode curNode;
	oldpen = (HPEN)SelectObject(hdc, pen);
	CDC dc;
	dc.Attach(hdc);

	bool TrackFinished = false;

	long firstnode = GetNode(x, y);
	if (firstnode < 0) {
		TrackFinished = true;
	}
	else {
		curNode = g.m_nodes[firstnode];
	}

	// while mouse not up try to find the nodes between which it will draw the edge
	while (!TrackFinished)
	{
		GetMessage(&msg, m_hWnd, 0x200, 0x204);
		if (msg.message == WM_MOUSEMOVE)
		{
			int poxT = ((int)LOWORD(msg.lParam));
			int poyT = ((int)HIWORD(msg.lParam));
			// 擦除旧节点
			DrawNode(&dc, curNode);

			/* 画新节点 */
			curNode.m_p.x = poxT;
			curNode.m_p.y = poyT;
			DrawNode(&dc, curNode);
			short fwKeys = short(UINT(msg.wParam));
		}
		if (msg.message == WM_LBUTTONUP)
		{
			int poxT = ((int)LOWORD(msg.lParam));
			int poyT = ((int)HIWORD(msg.lParam));
			
			TrackFinished = true;

			NM_GRAPH_DEL_EDIT_MOVE_STRUCT nmgv;
			nmgv.hdr.hwndFrom = m_hWnd;
			nmgv.hdr.idFrom = GetDlgCtrlID();
			nmgv.hdr.code = NM_GRAPH_MOVE_NODE;

			nmgv.idx = firstnode;
			nmgv.x = poxT;
			nmgv.y = poyT;

			if (nmgv.idx >= 0) {
				CWnd *pOwner = GetOwner();
				if (pOwner && IsWindow(pOwner->m_hWnd))
					pOwner->SendMessage(WM_NOTIFY, nmgv.hdr.idFrom, (LPARAM)&nmgv);
			}
		}
	}

	SelectObject(hdc, oldpen);
	SetROP2(hdc, R2_COPYPEN);
	DeleteObject(pen);
	::ReleaseDC(m_hWnd, hdc);
}

void CGraphCtrl::OnTips(long x, long y) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT nmgv;
	nmgv.hdr.hwndFrom = m_hWnd;
	nmgv.hdr.idFrom = GetDlgCtrlID();

	nmgv.idx = GetNode(x, y);
	if (nmgv.idx >= 0) {
		/* 节点 提示消息 */
		
		// DisplayBalloon(x, y, "12345678\n  tttttttttttttt\n hhhhhhjjhhj\n kkkkkkk\n Node");
		if(g.m_nodes[nmgv.idx].m_tips.length() > 0)
			ShowBalloonTip(x, y, g.m_nodes[nmgv.idx].m_tips);
	}
	else {
		/* 边 提示消息 */
		nmgv.idx = GetEdge(x, y);
		if (nmgv.idx >= 0) {
			
			// DisplayBalloon(x, y, "12345678\n  yyyyyyyyyyyyyy\n kkkkkkkk\n ffffffffff\n Edge");
			if (g.m_edges[nmgv.idx].m_tips.length() > 0)
				ShowBalloonTip(x, y, g.m_edges[nmgv.idx].m_tips);
		}
	}

}