// IndiCsindexView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndiGrossProfitRateView.h"
#include "CalcGrossProfit.h"

#include "ChildSplitterFrm.h"
#include "StockInfoView.h"

// CIndiCsindexView

IMPLEMENT_DYNCREATE(CIndiGrossProfitRateView, CBaseChartCtlView)

CIndiGrossProfitRateView::CIndiGrossProfitRateView()
{

}

CIndiGrossProfitRateView::~CIndiGrossProfitRateView()
{
}

BEGIN_MESSAGE_MAP(CIndiGrossProfitRateView, CBaseChartCtlView)
	ON_COMMAND(ID_INDI_GROSS_PROFIT_RATE, &CIndiGrossProfitRateView::OnIndiGrossProfitRate)
END_MESSAGE_MAP()


// CIndiCsindexView 绘图

void CIndiGrossProfitRateView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiCsindexView 诊断

#ifdef _DEBUG
void CIndiGrossProfitRateView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiGrossProfitRateView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiCsindexView 消息处理程序



void CIndiGrossProfitRateView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_pChartCtrl->GetLegend()->SetVisible(true);

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	m_LocalDoc.m_nCode = pDoc->m_nCode;	
	RedrawDmGraph();
}


void CIndiGrossProfitRateView::OnIndiGrossProfitRate()
{
	// TODO: 在此添加命令处理程序代码

	m_LocalDoc.m_nCode = CStockInfoView::m_strSelectedCode;
	RedrawDmGraph();
}

void CIndiGrossProfitRateView::RedrawDmGraph()
{

	CCalcGrossProfit * cse =  new CCalcGrossProfit(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(m_LocalDoc.m_nCode);
	cse->m_vec_request_is_stock.push_back(1);
	cse->m_StartDate = m_StartDate;
	cse->m_EndDate = m_EndDate;
	cse->ScheduleTask(&CCalcGrossProfit::Execute1);
} 

void CIndiGrossProfitRateView::PostFocusChanged()
{
	((CChildSplitterFrame*)GetParentFrame())->m_wndSplitter.RefreshSplitBars();
}
