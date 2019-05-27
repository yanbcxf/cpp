// PlateChangeRateSummaryView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CalcMarketValueSimple.h"
#include "PlateChangeRateSummaryView.h"


// CPlateChangeRateSummaryView

IMPLEMENT_DYNCREATE(CPlateChangeRateSummaryView, CBaseGridCtlView)

CPlateChangeRateSummaryView::CPlateChangeRateSummaryView()
{

}

CPlateChangeRateSummaryView::~CPlateChangeRateSummaryView()
{
}

BEGIN_MESSAGE_MAP(CPlateChangeRateSummaryView, CBaseGridCtlView)
END_MESSAGE_MAP()


// CPlateChangeRateSummaryView 绘图

void CPlateChangeRateSummaryView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateChangeRateSummaryView 诊断

#ifdef _DEBUG
void CPlateChangeRateSummaryView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateChangeRateSummaryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateChangeRateSummaryView 消息处理程序


void CPlateChangeRateSummaryView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMarketValueSimple * cse =  new CCalcMarketValueSimple(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcMarketValueSimple::Execute1);
}
