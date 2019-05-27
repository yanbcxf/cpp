// IndiCashFlowSummaryView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndiCashFlowSummaryView.h"


// CIndiCashFlowSummaryView

IMPLEMENT_DYNCREATE(CIndiCashFlowSummaryView, CBaseChartCtlView)

CIndiCashFlowSummaryView::CIndiCashFlowSummaryView()
{
	
}

CIndiCashFlowSummaryView::~CIndiCashFlowSummaryView()
{
}

BEGIN_MESSAGE_MAP(CIndiCashFlowSummaryView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiCashFlowSummaryView 绘图

void CIndiCashFlowSummaryView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiCashFlowSummaryView 诊断

#ifdef _DEBUG
void CIndiCashFlowSummaryView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiCashFlowSummaryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiCashFlowSummaryView 消息处理程序

void CIndiCashFlowSummaryView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();
	//JMutexAutoLock lock(pApp->m_mutexStockinfo);
	StockInfo_Reqeust req;
	req.code = atoi(pDoc->m_nCode.c_str());
	req.hwnd = m_hWnd;
	pApp->m_listStockinfoReq.push_front(req);
}



void CIndiCashFlowSummaryView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	


	CRect rect;
	m_pChartCtrl->GetClientRect(&rect);
	m_pChartCtrl->RemoveAllSeries();

	CCninfoCashFlows cashflow;
	CBussinessUtils::GetCashFlow(atoi(pDoc->m_nCode.c_str()), cashflow);
	if(cashflow.m_vec_row.size()>0)
	{
		int nStart = 0, nEnd = cashflow.m_vec_row.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i<cashflow.m_vec_row.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(cashflow.m_vec_row[i][_T("截止日期")].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i=cashflow.m_vec_row.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(cashflow.m_vec_row[i][_T("截止日期")].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 绘制价格曲线

		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		
		CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("经营流量净额");

		CChartLineSerie* pSeries1 = m_pChartCtrl->CreateLineSerie();
		pSeries1->SetName("投资流量净额");

		CChartLineSerie* pSeries2 = m_pChartCtrl->CreateLineSerie();
		pSeries2->SetName("筹资流量净额");

		CChartLineSerie* pSeries3 = m_pChartCtrl->CreateLineSerie();
		pSeries3->SetName("期末现金余额");
		pSeries3->SetWidth(2);
	

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(cashflow.m_vec_row[nStart + i][_T("截止日期")].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;
			int mon = odt.GetMonth();

			double net_cash_flow = String2Double(cashflow.m_vec_row[nStart + i][_T("经营活动产生的现金流量净额")])/10000.0;
			if(net_cash_flow> ymax)  ymax = net_cash_flow;
			if(net_cash_flow<ymin)	ymin = net_cash_flow;
			pSeries->AddPoint(odt.m_dt, net_cash_flow);
			if(mon==12)
			{
				//	期初清零，向后推迟一日，每年的元旦
				pSeries->AddPoint(odt.m_dt + 1.0, 0.0);
			}

			net_cash_flow = String2Double(cashflow.m_vec_row[nStart + i][_T("投资活动产生的现金流量净额")])/10000.0;
			if(net_cash_flow> ymax)  ymax = net_cash_flow;
			if(net_cash_flow<ymin)	ymin = net_cash_flow;
			pSeries1->AddPoint(odt.m_dt, net_cash_flow);
			if(mon==12)
			{
				//	期初清零，向后推迟一日，每年的元旦
				pSeries1->AddPoint(odt.m_dt + 1.0, 0.0);
			}

			net_cash_flow = String2Double(cashflow.m_vec_row[nStart + i][_T("筹资活动产生的现金流量净额")])/10000.0;
			if(net_cash_flow> ymax)  ymax = net_cash_flow;
			if(net_cash_flow<ymin)	ymin = net_cash_flow;
			pSeries2->AddPoint(odt.m_dt, net_cash_flow);
			if(mon==12)
			{
				//	期初清零
				pSeries2->AddPoint(odt.m_dt + 1.0, 0.0);
			}

			net_cash_flow = String2Double(cashflow.m_vec_row[nStart + i][_T("期末现金及现金等价物余额")])/10000.0;
			if(net_cash_flow> ymax)  ymax = net_cash_flow;
			if(net_cash_flow<ymin)	ymin = net_cash_flow;
			pSeries3->AddPoint(odt.m_dt, net_cash_flow);
		}

		pBottomAxis->SetMinMax(xmin, xmax);
		pLeftAxis->SetMinMax(ymin, ymax);
	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();
}