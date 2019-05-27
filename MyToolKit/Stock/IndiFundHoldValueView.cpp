// IndiFundHoldValueView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndiFundHoldValueView.h"


// CIndiFundHoldValueView

IMPLEMENT_DYNCREATE(CIndiFundHoldValueView, CBaseChartCtlView)

CIndiFundHoldValueView::CIndiFundHoldValueView()
{

}

CIndiFundHoldValueView::~CIndiFundHoldValueView()
{
}

BEGIN_MESSAGE_MAP(CIndiFundHoldValueView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiFundHoldValueView 绘图

void CIndiFundHoldValueView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiFundHoldValueView 诊断

#ifdef _DEBUG
void CIndiFundHoldValueView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiFundHoldValueView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiFundHoldValueView 消息处理程序



void CIndiFundHoldValueView::RedrawDmGraph()
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


	CSohuFundHold fundHold;
	CCapitalFlows163 capitalFlow;
	CNetEaseTradeDaily trading ;
	CBussinessUtils::GetSohuTradingDaily(atoi(pDoc->m_nCode.c_str()), trading);
	CBussinessUtils::GetFundHold(atoi(pDoc->m_nCode.c_str()), fundHold);
	CBussinessUtils::GetCapitalFlows163(atoi(pDoc->m_nCode.c_str()), capitalFlow);

	//	汇总某个季度的 基金持有总市值
	map<double, double>	 map_fundhold;
	for(int i=0; i<fundHold.m_vec_report_date.size(); i++ )
	{
		COleDateTime odt ;
		odt.ParseDateTime(fundHold.m_vec_report_date[i].c_str());

		if(map_fundhold.count(odt.m_dt)==0)
			map_fundhold[odt.m_dt] = fundHold.m_vec_number_hold[i];
		else
		{
			map_fundhold[odt.m_dt] += fundHold.m_vec_number_hold[i];
		}
	}
	
	
	if(capitalFlow.m_vec_report_date.size()>0 )
	{
		int nStart = 0, nEnd = capitalFlow.m_vec_report_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i<capitalFlow.m_vec_report_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(capitalFlow.m_vec_report_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i=capitalFlow.m_vec_report_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(capitalFlow.m_vec_report_date[i].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;
		xmin = 99999.0; xmax = 0.0;

		//	绘制 “基金持有市值”曲线
		CChartLineSerie* pSeries = NULL;
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("基金持有量跟踪(股)");
		pSeries->SetWidth(1);
		pSeries->SetColor(RGB(0,164,64));


		double currValue;
		double currBase = 0.0;
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(capitalFlow.m_vec_report_date[nStart + i].c_str());

			double tmpBase = 0.0;
			double tmpValue = 0.0;
			map<double, double>::iterator it = map_fundhold.begin();
			for(; it!=map_fundhold.end(); it++)
			{
				double dd = it->first;
				if(odt.m_dt >= dd)
				{
					tmpBase = dd;
					tmpValue = it->second;
				}
				else
					break;
			}
			if(tmpBase< 1.0)
				continue;

			if(tmpBase!=currBase)
			{
				currBase = tmpBase;
				currValue = tmpValue;
			}

			if(trading.m_vec_reorganize.count(capitalFlow.m_vec_report_date[nStart + i])>0)
			{
				CNetEaseTradeDailyData data = trading.m_vec_reorganize[capitalFlow.m_vec_report_date[nStart + i]];
				currValue += (capitalFlow.m_vec_main_net_flow[nStart + i] * 10000.0)/data.close;
			}
			
			
			if(odt.m_dt < xmin)	
				xmin = odt.m_dt;
			if(odt.m_dt > xmax) 
				xmax = odt.m_dt;

			if(currValue > ymax) ymax = currValue;
			if(currValue < ymin) ymin = currValue;

			pSeries->AddPoint(odt.m_dt, currValue);
		}

		// 
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("基金持有量(股)");
		pSeries->SetWidth(1);
		pSeries->SetColor(RGB(200,0,64));

		map<double, double>::iterator it = map_fundhold.begin();
		for(; it!=map_fundhold.end(); it++)
		{
			double dd = it->first;
			pSeries->AddPoint(dd, it->second);
		}

		if(pSeries->GetPointsCount()>0)
		{
			pBottomAxis->SetMinMax(xmin, xmax);
			pLeftAxis->SetMinMax(ymin, ymax);
		}
	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();
}
