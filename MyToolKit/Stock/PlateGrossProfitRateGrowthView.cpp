// PlateChangeRateSummaryView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CalcGrossProfit.h"
#include "PlateGrossProfitRateGrowthView.h"


// CPlateChangeRateSummaryView

IMPLEMENT_DYNCREATE(CPlateGrossProfitRateGrowthView, CBaseGridCtlView)

CPlateGrossProfitRateGrowthView::CPlateGrossProfitRateGrowthView()
{

}

CPlateGrossProfitRateGrowthView::~CPlateGrossProfitRateGrowthView()
{
}

BEGIN_MESSAGE_MAP(CPlateGrossProfitRateGrowthView, CBaseGridCtlView)
END_MESSAGE_MAP()


// CPlateChangeRateSummaryView 绘图

void CPlateGrossProfitRateGrowthView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateChangeRateSummaryView 诊断

#ifdef _DEBUG
void CPlateGrossProfitRateGrowthView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateGrossProfitRateGrowthView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateChangeRateSummaryView 消息处理程序


void CPlateGrossProfitRateGrowthView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcGrossProfit * cse =  new CCalcGrossProfit(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->m_str_request_zb_code = pDoc->m_strInput;			//	 "profit_rate_quarter"

	cse->ScheduleTask(&CCalcGrossProfit::Execute1);
}
