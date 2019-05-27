// MarketExchageFundView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MarketExchageFundView.h"
#include "CalcMarketExchangeFund.h"


// CMarketExchageFundView

IMPLEMENT_DYNCREATE(CMarketExchageFundView, CBaseChartCtlView)

CMarketExchageFundView::CMarketExchageFundView()
{

}

CMarketExchageFundView::~CMarketExchageFundView()
{
}

BEGIN_MESSAGE_MAP(CMarketExchageFundView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CMarketExchageFundView 绘图

void CMarketExchageFundView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CMarketExchageFundView 诊断

#ifdef _DEBUG
void CMarketExchageFundView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CMarketExchageFundView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMarketExchageFundView 消息处理程序




void CMarketExchageFundView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CMarketExchageFundView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMarketExchangeFund * cse =  new CCalcMarketExchangeFund(m_hWnd, WM_USER_LOG_1);

	cse->ScheduleTask(&CCalcMarketExchangeFund::Execute1);
}

void CMarketExchageFundView::ProcessXmlWParam(string msgType,string code,string model,\
												  string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawDmGraph(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
}


void CMarketExchageFundView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcMarketExchangeFund * pCse =  (CCalcMarketExchangeFund *)cse;
	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	if(pCse->m_vec_begin_date.size()>0)
	{
		int nStart = 0, nEnd = pCse->m_vec_begin_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i< pCse->m_vec_begin_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pCse->m_vec_begin_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i= pCse->m_vec_begin_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pCse->m_vec_begin_date[i].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 绘制价格曲线

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->m_vec_begin_date[nStart + i].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			if(pCse->m_vec_begin_index[nStart + i]> ymax)  ymax = pCse->m_vec_begin_index[nStart + i];
			if(pCse->m_vec_end_index[nStart + i]> ymax)  ymax = pCse->m_vec_end_index[nStart + i];
			if(pCse->m_vec_final_fund[nStart + i]> ymax)  ymax = pCse->m_vec_final_fund[nStart + i];
			if(pCse->m_vec_net_fund[nStart + i]> ymax)  ymax = pCse->m_vec_net_fund[nStart + i];
			
			//if(pCse->m_vec_output_fund[nStart + i] * -1 <  ymin)  ymin = pCse->m_vec_output_fund[nStart + i] * -1;
		/*	if(pCse->m_vec_end_index[nStart + i] < ymin)  ymin = pCse->m_vec_end_index[nStart + i];
			if(pCse->m_vec_ema26[nStart + i] < ymin)  ymin = pCse->m_vec_ema26[nStart + i];
			if(pCse->m_vec_dif[nStart + i] < ymin)  ymin = pCse->m_vec_dif[nStart + i];
			if(pCse->m_vec_dea[nStart + i] < ymin)  ymin = pCse->m_vec_dea[nStart + i];*/

		}

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		//pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		//pLeftAxis->SetMinMax(0, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("上证综指 * 10 ");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->m_vec_begin_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_begin_index[nStart + i] * 10);

			odt.ParseDateTime(pCse->m_vec_end_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_end_index[nStart + i] * 10);

		}
	

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("结算资金（亿元）");

		for(long i=0; i<  nEnd - nStart + 1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->m_vec_end_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_final_fund[nStart + i]);
		}
		
		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("本周成交金额（亿元）");

		for(long i=0; i<  nEnd - nStart + 1; i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(pCse->m_vec_end_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_sum_turnover[nStart + i]/10000.0);

			if(pCse->m_vec_sum_turnover[nStart + i]/10000.0 > ymax)  ymax = pCse->m_vec_sum_turnover[nStart + i]/10000.0;
		}
		

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("银证转账累计额（亿元）");

		double sum_net_fund = 0;
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			sum_net_fund += pCse->m_vec_net_fund[nStart + i];
			odt.ParseDateTime(pCse->m_vec_end_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, sum_net_fund);

			if(sum_net_fund > ymax)  ymax = sum_net_fund;
			if(sum_net_fund < ymin)  ymin = sum_net_fund;
		}
		
		//////////////////////////////////////////////////////////////////////////

		if(ymax>0 && xmax > xmin)
		{
			pBottomAxis->SetMinMax(xmin, xmax);
			pLeftAxis->SetMinMax(ymin, ymax);

			m_pChartCtrl->GetLegend()->SetVisible(true);
			m_pChartCtrl->RefreshCtrl();
		}

	}
	m_pChartCtrl->EnableRefresh(true);	

}
