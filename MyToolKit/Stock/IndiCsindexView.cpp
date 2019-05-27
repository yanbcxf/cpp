// IndiCsindexView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndiCsindexView.h"
#include "CalcCsindex.h"

#include "ChildSplitterFrm.h"
#include "StockInfoView.h"

// CIndiCsindexView

IMPLEMENT_DYNCREATE(CIndiCsindexView, CBaseChartCtlView)

CIndiCsindexView::CIndiCsindexView()
{

}

CIndiCsindexView::~CIndiCsindexView()
{
}

BEGIN_MESSAGE_MAP(CIndiCsindexView, CBaseChartCtlView)
	ON_COMMAND(ID_INDI_CSINDEX_ANALYSIS, &CIndiCsindexView::OnIndiCsindexAnalysis)
END_MESSAGE_MAP()


// CIndiCsindexView 绘图

void CIndiCsindexView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiCsindexView 诊断

#ifdef _DEBUG
void CIndiCsindexView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiCsindexView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiCsindexView 消息处理程序



void CIndiCsindexView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_pChartCtrl->GetLegend()->SetVisible(true);

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	m_LocalDoc.m_nCode = pDoc->m_nCode;	
	RedrawDmGraph();
}


void CIndiCsindexView::OnIndiCsindexAnalysis()
{
	// TODO: 在此添加命令处理程序代码

	m_LocalDoc.m_nCode = CStockInfoView::m_strSelectedCode;
	RedrawDmGraph();
}

void CIndiCsindexView::RedrawDmGraph()
{

	CCalcCsindex * cse =  new CCalcCsindex(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(m_LocalDoc.m_nCode);
	cse->m_vec_request_is_stock.push_back(1);
	cse->m_StartDate = m_StartDate;
	cse->m_EndDate = m_EndDate;
	cse->ScheduleTask(&CCalcCsindex::Execute1);
} 

void CIndiCsindexView::PostFocusChanged()
{
	((CChildSplitterFrame*)GetParentFrame())->m_wndSplitter.RefreshSplitBars();
}
