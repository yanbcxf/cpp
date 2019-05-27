// IndiZombieAnalysisView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndiZombieAnalysisView.h"


// CIndiZombieAnalysisView

IMPLEMENT_DYNCREATE(CIndiZombieAnalysisView, CBaseChartCtlView)

CIndiZombieAnalysisView::CIndiZombieAnalysisView()
{

}

CIndiZombieAnalysisView::~CIndiZombieAnalysisView()
{
}

BEGIN_MESSAGE_MAP(CIndiZombieAnalysisView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiZombieAnalysisView 绘图

void CIndiZombieAnalysisView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiZombieAnalysisView 诊断

#ifdef _DEBUG
void CIndiZombieAnalysisView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiZombieAnalysisView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiZombieAnalysisView 消息处理程序



void CIndiZombieAnalysisView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	CRect rect = m_pChartCtrl->GetPlottingRect();

	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;


	CCninfoBalance balance;
	CCninfoIncomeStatement incomeStatement;
	CCninfoCashFlows cashFlows;
	CBussinessUtils::GetBalance(atoi(pDoc->m_nCode.c_str()), balance);
	CBussinessUtils::GetIncomestatement(atoi(pDoc->m_nCode.c_str()), incomeStatement);
	CBussinessUtils::GetCashFlow(atoi(pDoc->m_nCode.c_str()), cashFlows);

	if(balance.m_vec_row.size()>0 )
	{
		int nStart = 0, nEnd = balance.m_vec_row.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i<balance.m_vec_row.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(balance.m_vec_row[i][_T("截止日期")].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i=balance.m_vec_row.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(balance.m_vec_row[i][_T("截止日期")].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;

		//	绘制 “未分配利润(累计)”曲线
		CChartLineSerie* pSeries = NULL;
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("每股未分配利润(元)");
		pSeries->SetWidth(1);
		pSeries->SetColor(RGB(0,164,64));

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(balance.m_vec_row[nStart + i][_T("截止日期")].c_str());

			double ratio = String2Double(balance.m_vec_row[nStart + i][_T("未分配利润")]);
			ratio = ratio / String2Double(balance.m_vec_row[nStart + i][_T("实收资本（或股本）")]);
			if(i==0)	
				xmin = odt.m_dt;
			if(i== nEnd - nStart) 
				xmax = odt.m_dt;

			if(ratio > ymax) ymax = ratio;
			if(ratio < ymin) ymin = ratio;

			pSeries->AddPoint(odt.m_dt, ratio);
		}

		//	绘制 “每股净资产”曲线
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("每股净资产(元)");
		pSeries->SetWidth(1);
		pSeries->SetColor(RGB(69,45,72));

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(balance.m_vec_row[nStart + i][_T("截止日期")].c_str());

			double ratio = String2Double(balance.m_vec_row[nStart + i][_T("资产总计")]);
			ratio -= String2Double(balance.m_vec_row[nStart + i][_T("负债合计")]);
			ratio = ratio / String2Double(balance.m_vec_row[nStart + i][_T("实收资本（或股本）")]);

			if(ratio > ymax) ymax = ratio;
			if(ratio < ymin) ymin = ratio;

			pSeries->AddPoint(odt.m_dt, ratio);
		}


		pBottomAxis->SetMinMax(xmin, xmax);
		pLeftAxis->SetMinMax(ymin, ymax);

		//	绘制 “每股收益”曲线
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("每股收益(元)");
		pSeries->SetWidth(1);
		pSeries->SetColor(RGB(0,64,164));

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(balance.m_vec_row[nStart + i][_T("截止日期")].c_str());

			double ratio = String2Double(incomeStatement.m_vec_row[nStart + i][_T("归属于母公司所有者的净利润")]);
			ratio = ratio / String2Double(balance.m_vec_row[nStart + i][_T("实收资本（或股本）")]);
		
			if(ratio > ymax) ymax = ratio;
			if(ratio < ymin) ymin = ratio;

			pSeries->AddPoint(odt.m_dt, ratio);
		}


		pBottomAxis->SetMinMax(xmin, xmax);
		pLeftAxis->SetMinMax(ymin, ymax);
	}

	

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();
}


void CIndiZombieAnalysisView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	StockInfo_Reqeust req;
	req.code = atoi(pDoc->m_nCode.c_str());
	req.hwnd = m_hWnd;
	pApp->m_listStockinfoReq.push_front(req);

}