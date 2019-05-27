// MarketCffexHs300FutureView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MarketCffexHs300FutureView.h"
#include "CalcCffexFutureHs300.h"

// CMarketCffexHs300FutureView

IMPLEMENT_DYNCREATE(CMarketCffexHs300FutureView, CBaseChartCtlView)

CMarketCffexHs300FutureView::CMarketCffexHs300FutureView()
{

}

CMarketCffexHs300FutureView::~CMarketCffexHs300FutureView()
{
}

BEGIN_MESSAGE_MAP(CMarketCffexHs300FutureView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CMarketCffexHs300FutureView 绘图

void CMarketCffexHs300FutureView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CMarketCffexHs300FutureView 诊断

#ifdef _DEBUG
void CMarketCffexHs300FutureView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CMarketCffexHs300FutureView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMarketCffexHs300FutureView 消息处理程序



void CMarketCffexHs300FutureView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CMarketCffexHs300FutureView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcCffexFutureHs300 * cse =  new CCalcCffexFutureHs300(m_hWnd, WM_USER_LOG_1);

	cse->ScheduleTask(&CCalcCffexFutureHs300::Execute1);
}

void CMarketCffexHs300FutureView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CMarketCffexHs300FutureView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcCffexFutureHs300 * pCse =  (CCalcCffexFutureHs300 *)cse;
	m_pChartCtrl->RemoveAllSeries();

	
	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	if(pCse->m_vec_trade_date.size()>0)
	{
		int nStart = 0, nEnd = pCse->m_vec_trade_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i< pCse->m_vec_trade_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pCse->m_vec_trade_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i= pCse->m_vec_trade_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pCse->m_vec_trade_date[i].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		//pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		//pLeftAxis->SetMinMax(0, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		map<string, CChartLineSerie*>	mapLineSerie;
		string	lastTradeDate = "";

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->m_vec_trade_date[nStart + i].c_str());

			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			if(pCse->m_vec_trade_date[nStart + i]!=lastTradeDate)
			{
				lastTradeDate = pCse->m_vec_trade_date[nStart + i];

				/*绘制沪深300指数*/
				if(mapLineSerie.count("hs300")==0)
				{
					mapLineSerie["hs300"] = m_pChartCtrl->CreateLineSerie();
					mapLineSerie["hs300"]->SetWidth(3);
				}
				CChartLineSerie* pSeries = mapLineSerie["hs300"];
				pSeries->AddPoint(odt.m_dt, pCse->m_vec_hs300[nStart + i]);

				if(pCse->m_vec_hs300[nStart + i]> ymax)  ymax = pCse->m_vec_hs300[nStart + i];
				if(pCse->m_vec_hs300[nStart + i]< ymin)  ymin = pCse->m_vec_hs300[nStart + i];
			}


			/*绘制不同时期的股指期货*/
			if(mapLineSerie.count(pCse->m_vec_agreement_code[nStart + i])==0)
			{
				mapLineSerie[pCse->m_vec_agreement_code[nStart + i]] = m_pChartCtrl->CreateLineSerie();
			}
			CChartLineSerie* pSeries = mapLineSerie[pCse->m_vec_agreement_code[nStart + i]];
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_settlement[nStart + i]);
			if(pCse->m_vec_settlement[nStart + i]> ymax)  ymax = pCse->m_vec_settlement[nStart + i];
			if(pCse->m_vec_settlement[nStart + i]< ymin)  ymin = pCse->m_vec_settlement[nStart + i];
		}

		//	给各条曲线打上“股指期货”名称
		map<string, CChartLineSerie*>::iterator it = mapLineSerie.begin();
		for(; it!=mapLineSerie.end(); it++)
		{
			int cnt = it->second->GetPointsCount();
			mapLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), it->first);
		}


		//////////////////////////////////////////////////////////////////////////

		if(ymax>0 && xmax > xmin)
		{
			pBottomAxis->SetMinMax(xmin, xmax);
			pLeftAxis->SetMinMax(ymin-100, ymax+100);

			m_pChartCtrl->GetLegend()->SetVisible(false);
			m_pChartCtrl->RefreshCtrl();
		}

	}	

	m_pChartCtrl->EnableRefresh(true);

}

