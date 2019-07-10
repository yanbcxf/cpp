// BaseMessageFormView.cpp : 实现文件
//

#include "stdafx.h"
#include "CostEngineer.h"
#include "BaseMessageFormView.h"


// CBaseMessageFormView

IMPLEMENT_DYNCREATE(CBaseMessageFormView, CFormView)

CBaseMessageFormView::CBaseMessageFormView()
	: CFormView(CBaseMessageFormView::IDD)
{
	m_MessageCtrl.SetOwnerView(this);
	m_display_mode = DisplayModes::Grid_Grid;
	m_upper_percent = 3;
	m_down_percent = 4;
}

CBaseMessageFormView::~CBaseMessageFormView()
{
}

void CBaseMessageFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON1, m_btn1);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_GRID1, m_Grid1);
	DDX_Control(pDX, IDC_DIJKSTRA, m_Graph);
}

BEGIN_MESSAGE_MAP(CBaseMessageFormView, CFormView)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_XML, OnUserXmlWParam)
	
	ON_WM_ACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID1, OnGridDblClick1)
	ON_NOTIFY(NM_CLICK, IDC_GRID1, OnGridClick1)
	ON_NOTIFY(NM_GRAPH_ADD_NODE, IDC_DIJKSTRA, OnAddNodeToGraph)
	ON_NOTIFY(NM_GRAPH_EDIT_NODE, IDC_DIJKSTRA, OnEditNodeInGraph)
	ON_NOTIFY(NM_GRAPH_DEL_NODE, IDC_DIJKSTRA, OnDelNodeInGraph)
	ON_NOTIFY(NM_GRAPH_MOVE_NODE, IDC_DIJKSTRA, OnMoveNodeInGraph)

	ON_NOTIFY(NM_GRAPH_ADD_EDGE, IDC_DIJKSTRA, OnAddEdgeToGraph)
	ON_NOTIFY(NM_GRAPH_EDIT_EDGE, IDC_DIJKSTRA, OnEditEdgeInGraph)
	ON_NOTIFY(NM_GRAPH_DEL_EDGE, IDC_DIJKSTRA, OnDelEdgeInGraph)
END_MESSAGE_MAP()


// CBaseMessageFormView 诊断

#ifdef _DEBUG
void CBaseMessageFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseMessageFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaseMessageFormView 消息处理程序

void CBaseMessageFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	ReLayout();
}

void CBaseMessageFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ReLayout();
}

void CBaseMessageFormView::ReLayout()
{
	if (m_display_mode == DisplayModes::None) {
		m_Grid.ShowWindow(false);
		m_Grid1.ShowWindow(false);
		m_Graph.ShowWindow(false);
	}

	else if (m_display_mode == DisplayModes::Grid_Grid)
	{
		m_Grid.ShowWindow(true);
		m_Grid1.ShowWindow(true);
		m_Graph.ShowWindow(false);

		CRect rect;
		GetClientRect(&rect);
		rect.bottom = rect.top + (rect.bottom - rect.top) * m_upper_percent / (m_upper_percent + m_down_percent) - 2;
		m_Grid.MoveWindow(rect);
		m_Grid.SetEditable(FALSE);

		GetClientRect(&rect);
		rect.top = rect.bottom - (rect.bottom - rect.top) * m_down_percent / (m_upper_percent + m_down_percent) + 2;
		m_Grid1.MoveWindow(rect);
		m_Grid1.SetEditable(FALSE);
	}
	
	else if (m_display_mode == DisplayModes::Grid) {
		m_Grid.ShowWindow(true);
		m_Grid1.ShowWindow(false);
		m_Graph.ShowWindow(false);

		CRect rect;
		GetClientRect(&rect);
		m_Grid.MoveWindow(rect);
	}
	else if (m_display_mode == DisplayModes::Dijkstra) {
		m_Grid.ShowWindow(false);
		m_Grid1.ShowWindow(false);
		m_Graph.ShowWindow(true);

		CRect rect;
		GetClientRect(&rect);
		m_Graph.MoveWindow(rect);
	}

	else if (m_display_mode == DisplayModes::Grid_Dijkstra)
	{
		m_Grid.ShowWindow(true);
		m_Grid1.ShowWindow(false);
		m_Graph.ShowWindow(true);

		CRect rect;
		GetClientRect(&rect);
		rect.bottom = rect.top + (rect.bottom - rect.top) * m_upper_percent / (m_upper_percent + m_down_percent) - 2;
		m_Grid.MoveWindow(rect);
		m_Grid.SetEditable(FALSE);

		GetClientRect(&rect);
		rect.top = rect.bottom - (rect.bottom - rect.top) * m_down_percent / (m_upper_percent + m_down_percent) + 2;
		m_Graph.MoveWindow(rect);
	}
}



LRESULT CBaseMessageFormView::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	return m_MessageCtrl.OnUserXmlWParam(wParam,lParam);
}


LRESULT CBaseMessageFormView::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	return m_MessageCtrl.OnUserLogWParam_1(wParam, lParam);
}


void CBaseMessageFormView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码

	/*当获取焦点时，自动在左边的股票各个板块中寻找对应项*/

	CString strTitle;
	GetParent()->GetWindowText(strTitle);

	m_MessageCtrl.OnSetFocus(strTitle);

	//////////////////////////////////////////////////////////////////////////
	PostFocusChanged();

}

void CBaseMessageFormView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码


	PostFocusChanged();
}




void CBaseMessageFormView::ProcessXmlWParam(string msgType,string code,string model,\
										 string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawChart(cse);
		if(cse!=NULL)
			delete cse;
		//sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
	else if(msgType=="ReadFinished")
		RedrawDmGraph();
}


BOOL CBaseMessageFormView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类


	return CFormView::PreTranslateMessage(pMsg);
}



void CBaseMessageFormView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	CString str;
	str.Format(_T("Context menu called on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	//AfxMessageBox(str);
	
	if (pItem->iRow>0 && pItem->iColumn <1)
		PostGridDblClick(0, pItem->iRow);
}

//这是当CTREECTRL控件点击时NM_CLICK的处理函数
void CBaseMessageFormView::OnGridClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNMHDR;
	//Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	CString str;
	str.Format(_T("Context menu called on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	// TODO:  在此添加控件通知处理程序代码
	if (pItem->iRow > 0 && pItem->iColumn >= 0)
		PostGridClick(0, pItem->iRow, pItem->iColumn);

	*pResult = 0;
}

void CBaseMessageFormView::OnGridDblClick1(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	CString str;
	str.Format(_T("Context menu called on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	//AfxMessageBox(str);

	if (pItem->iRow>0 && pItem->iColumn <1)
		PostGridDblClick(1, pItem->iRow);
}

//这是当CTREECTRL控件点击时NM_CLICK的处理函数
void CBaseMessageFormView::OnGridClick1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNMHDR;
	//Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	CString str;
	str.Format(_T("Context menu called on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	// TODO:  在此添加控件通知处理程序代码
	if (pItem->iRow > 0 && pItem->iColumn >= 0)
		PostGridClick(1, pItem->iRow, pItem->iColumn);

	*pResult = 0;
}

void CBaseMessageFormView::OnAddNodeToGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_ADD_NODE_STRUCT* pItem = (NM_GRAPH_ADD_NODE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Node");
	PostAddNodeToGraph(pItem->x, pItem->y);
	*pResult = 0;
}

void CBaseMessageFormView::OnAddEdgeToGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_ADD_EDGE_STRUCT* pItem = (NM_GRAPH_ADD_EDGE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Edge");
	PostAddEdgeToGraph(pItem->m_firstNode, pItem->m_secondNode);
	*pResult = 0;
}

void CBaseMessageFormView::OnDelNodeInGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT* pItem = (NM_GRAPH_DEL_EDIT_MOVE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Node");
	PostDelNodeInGraph(pItem->idx);
	*pResult = 0;
}

void CBaseMessageFormView::OnEditNodeInGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT* pItem = (NM_GRAPH_DEL_EDIT_MOVE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Edge");
	PostEditNodeInGraph(pItem->idx);
	*pResult = 0;
}

void CBaseMessageFormView::OnMoveNodeInGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT* pItem = (NM_GRAPH_DEL_EDIT_MOVE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Edge");
	PostMoveNodeInGraph(pItem->idx, pItem->x, pItem->y);
	*pResult = 0;
}

void CBaseMessageFormView::OnEditEdgeInGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT* pItem = (NM_GRAPH_DEL_EDIT_MOVE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Edge");
	PostEditEdgeInGraph(pItem->idx);
	*pResult = 0;
}

void CBaseMessageFormView::OnDelEdgeInGraph(NMHDR *pNMHDR, LRESULT *pResult) {
	NM_GRAPH_DEL_EDIT_MOVE_STRUCT* pItem = (NM_GRAPH_DEL_EDIT_MOVE_STRUCT*)pNMHDR;
	// AfxMessageBox("Add Node");
	PostDelEdgeInGraph(pItem->idx);
	*pResult = 0;
}