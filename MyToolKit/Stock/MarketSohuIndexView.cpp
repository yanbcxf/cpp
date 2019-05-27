// MarketSohuIndexView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MarketSohuIndexView.h"
#include "CalcSohuIndexDaily.h"

// CMarketSohuIndexView

IMPLEMENT_DYNCREATE(CMarketSohuIndexView, CBaseChartCtlView)

CMarketSohuIndexView::CMarketSohuIndexView()
{

}

CMarketSohuIndexView::~CMarketSohuIndexView()
{
}

BEGIN_MESSAGE_MAP(CMarketSohuIndexView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CMarketSohuIndexView 绘图

void CMarketSohuIndexView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CMarketSohuIndexView 诊断

#ifdef _DEBUG
void CMarketSohuIndexView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CMarketSohuIndexView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMarketSohuIndexView 消息处理程序



void CMarketSohuIndexView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CMarketSohuIndexView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcSohuIndexDaily * cse =  new CCalcSohuIndexDaily(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back("1");
	cse->m_vec_request_code.push_back("300");
	cse->m_vec_request_code.push_back("399001");

	cse->ScheduleTask(&CCalcSohuIndexDaily::Execute1);
}

void CMarketSohuIndexView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CMarketSohuIndexView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcSohuIndexDaily * pCse =  (CCalcSohuIndexDaily *)cse;
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

		map<int, CChartLineSerie*>	mapCloseLineSerie;
		map<int, CChartLineSerie*>	mapTurnoverLineSerie;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->m_vec_trade_date[nStart + i].c_str());

			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			/* 绘制不同类型的股指的收盘价 */
			if(mapCloseLineSerie.count(pCse->m_vec_code[nStart + i])==0)
			{
				mapCloseLineSerie[pCse->m_vec_code[nStart + i]] = m_pChartCtrl->CreateLineSerie();
			}
			CChartLineSerie* pSeries = mapCloseLineSerie[pCse->m_vec_code[nStart + i]];
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_close[nStart + i]);

			if(pCse->m_vec_close[nStart + i]> ymax)  ymax = pCse->m_vec_close[nStart + i];
			if(pCse->m_vec_close[nStart + i]< ymin)  ymin = pCse->m_vec_close[nStart + i];

			/* 绘制不同类型的股指的成交金额(亿) */
			if(mapTurnoverLineSerie.count(pCse->m_vec_code[nStart + i])==0)
			{
				mapTurnoverLineSerie[pCse->m_vec_code[nStart + i]] = m_pChartCtrl->CreateLineSerie();
			}
			pSeries = mapTurnoverLineSerie[pCse->m_vec_code[nStart + i]];
			pSeries->AddPoint(odt.m_dt, pCse->m_vec_turnover[nStart + i]);

			if(pCse->m_vec_turnover[nStart + i]> ymax)  ymax = pCse->m_vec_turnover[nStart + i];
			if(pCse->m_vec_turnover[nStart + i]< ymin)  ymin = pCse->m_vec_turnover[nStart + i];
		}

		//	给各条曲线打上“股指”名称
		map<int, CChartLineSerie*>::iterator it = mapCloseLineSerie.begin();
		for(; it!=mapCloseLineSerie.end(); it++)
		{
			int cnt = it->second->GetPointsCount();
			if(it->first==1)
			{
				//mapCloseLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), "上证指数");
				mapCloseLineSerie[it->first]->SetName("上证指数");
				mapCloseLineSerie[it->first]->SetColor(RGB(255,0,0));
				mapCloseLineSerie[it->first]->SetWidth(2);
			}
			if(it->first==300)
			{
				//mapCloseLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), "沪深300指数");
				mapCloseLineSerie[it->first]->SetName("沪深300指数");
				mapCloseLineSerie[it->first]->SetColor(RGB(0,255,0));
				mapCloseLineSerie[it->first]->SetWidth(2);
			}
			if(it->first==399001)
			{			
				//mapCloseLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), "深证成指");
				mapCloseLineSerie[it->first]->SetName("深证成指");
				mapCloseLineSerie[it->first]->SetColor(RGB(0,0, 255));
				mapCloseLineSerie[it->first]->SetWidth(2);
			}
		}

		it = mapTurnoverLineSerie.begin();
		for(; it!=mapTurnoverLineSerie.end(); it++)
		{
			int cnt = it->second->GetPointsCount();
			if(it->first==1)
			{
				//mapTurnoverLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), "上证成交金额（亿元）");
				mapTurnoverLineSerie[it->first]->SetName("上证成交亿元");
				mapTurnoverLineSerie[it->first]->SetColor(RGB(255,0,0));
				mapTurnoverLineSerie[it->first]->SetPenStyle(PS_DASHDOT);
			}
			if(it->first==300)
			{
				//mapTurnoverLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), "沪深300成交金额（亿元）");
				mapTurnoverLineSerie[it->first]->SetName("沪深300成交亿元");
				mapTurnoverLineSerie[it->first]->SetColor(RGB(0,255,0));
				mapTurnoverLineSerie[it->first]->SetPenStyle(PS_DASHDOT);
			}
			if(it->first==399001)
			{
				//mapTurnoverLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), "深证成交金额（亿元）");
				mapTurnoverLineSerie[it->first]->SetName("深证成交亿元");
				mapTurnoverLineSerie[it->first]->SetColor(RGB(0,0, 255));
				mapTurnoverLineSerie[it->first]->SetPenStyle(PS_DASHDOT);
			}
		}


		//////////////////////////////////////////////////////////////////////////

		if(ymax>0 && xmax > xmin)
		{
			pBottomAxis->SetMinMax(xmin, xmax);
			pLeftAxis->SetMinMax(0, ymax+100);

			m_pChartCtrl->GetLegend()->SetVisible(TRUE);
			m_pChartCtrl->RefreshCtrl();
		}

	}
	m_pChartCtrl->EnableRefresh(true);

}

