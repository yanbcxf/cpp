// MarketCsdcInvestorStatView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MarketCsdcInvestorStatView.h"
#include "CalcCsdcInvestorStat.h"

// CMarketCsdcInvestorStatView

IMPLEMENT_DYNCREATE(CMarketCsdcInvestorStatView, CBaseChartCtlView)

CMarketCsdcInvestorStatView::CMarketCsdcInvestorStatView()
{

}

CMarketCsdcInvestorStatView::~CMarketCsdcInvestorStatView()
{
}

BEGIN_MESSAGE_MAP(CMarketCsdcInvestorStatView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CMarketCsdcInvestorStatView 绘图

void CMarketCsdcInvestorStatView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CMarketCsdcInvestorStatView 诊断

#ifdef _DEBUG
void CMarketCsdcInvestorStatView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CMarketCsdcInvestorStatView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMarketCsdcInvestorStatView 消息处理程序



void CMarketCsdcInvestorStatView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CMarketCsdcInvestorStatView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcCsdcInvestorStat * cse =  new CCalcCsdcInvestorStat(m_hWnd, WM_USER_LOG_1);

	cse->ScheduleTask(&CCalcCsdcInvestorStat::Execute1);
}

void CMarketCsdcInvestorStatView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CMarketCsdcInvestorStatView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcCsdcInvestorStat * pCse =  (CCalcCsdcInvestorStat *)cse;
	m_pChartCtrl->RemoveAllSeries();


	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	if(pCse->begin_date.size()>0)
	{
		int nStart = 0, nEnd = pCse->begin_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i< pCse->begin_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pCse->begin_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i= pCse->begin_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pCse->begin_date[i].c_str());
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
		pLeftAxis->GetLabel()->SetText("投资者数（万）");
		//pLeftAxis->SetMinMax(0, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		map<string, CChartLineSerie*>	mapLineSerie;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->end_date[nStart + i].c_str());

			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;
			
			/*绘制不同的投资者统计曲线 */
			if(mapLineSerie.count("final_a_natural_investor")==0)
				mapLineSerie["final_a_natural_investor"] = m_pChartCtrl->CreateLineSerie();

			if(mapLineSerie.count("final_a_legal_investor")==0)
				mapLineSerie["final_a_legal_investor"] = m_pChartCtrl->CreateLineSerie();

			if(mapLineSerie.count("final_a_position_investor")==0)
				mapLineSerie["final_a_position_investor"] = m_pChartCtrl->CreateLineSerie();

			if(mapLineSerie.count("period_a_trading_investor")==0)
				mapLineSerie["period_a_trading_investor"] = m_pChartCtrl->CreateLineSerie();
			
			mapLineSerie["final_a_natural_investor"]->AddPoint(odt.m_dt, pCse->final_a_natural_investor[nStart + i]);
			mapLineSerie["final_a_legal_investor"]->AddPoint(odt.m_dt, pCse->final_a_legal_investor[nStart + i]);
			mapLineSerie["final_a_position_investor"]->AddPoint(odt.m_dt, pCse->final_a_position_investor[nStart + i]);
			mapLineSerie["period_a_trading_investor"]->AddPoint(odt.m_dt, pCse->period_a_trading_investor[nStart + i]);

			if(pCse->final_a_natural_investor[nStart + i]> ymax)  ymax = pCse->final_a_natural_investor[nStart + i];
			if(pCse->final_a_natural_investor[nStart + i]< ymin)  ymin = pCse->final_a_natural_investor[nStart + i];

			if(pCse->final_a_legal_investor[nStart + i]> ymax)  ymax = pCse->final_a_legal_investor[nStart + i];
			if(pCse->final_a_legal_investor[nStart + i]< ymin)  ymin = pCse->final_a_legal_investor[nStart + i];

			if(pCse->final_a_position_investor[nStart + i]> ymax)  ymax = pCse->final_a_position_investor[nStart + i];
			if(pCse->final_a_position_investor[nStart + i]< ymin)  ymin = pCse->final_a_position_investor[nStart + i];

			if(pCse->period_a_trading_investor[nStart + i]> ymax)  ymax = pCse->period_a_trading_investor[nStart + i];
			if(pCse->period_a_trading_investor[nStart + i]< ymin)  ymin = pCse->period_a_trading_investor[nStart + i];
		}

		//	给各条曲线打上“股指期货”名称
		map<string, CChartLineSerie*>::iterator it = mapLineSerie.begin();
		for(; it!=mapLineSerie.end(); it++)
		{
			int cnt = it->second->GetPointsCount();
			string curveName;
			if(it->first=="final_a_natural_investor") curveName = "A股自然人";
			if(it->first=="final_a_legal_investor") curveName = "A股法人";
			if(it->first=="final_a_position_investor") curveName = "A股持仓者";
			if(it->first=="period_a_trading_investor") curveName = "本周参与交易者";

			//mapLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), curveName);
			mapLineSerie[it->first]->SetName(curveName.c_str());

		}


		//////////////////////////////////////////////////////////////////////////

		if(ymax>0 && xmax > xmin)
		{
			pBottomAxis->SetMinMax(xmin, xmax);
			pLeftAxis->SetMinMax(0, ymax+1000);

			m_pChartCtrl->GetLegend()->SetVisible(true);
			m_pChartCtrl->RefreshCtrl();
		}

	}

	m_pChartCtrl->EnableRefresh(true);

}
