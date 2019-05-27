// IndividualCapitalFlowView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"

#include "TechAnalysisEvent.h"
#include "DirectionalMovement.h"

#include "IndiDmiView.h"
#include "CalcCapitalFlow.h"


// CIndividualCapitalFlowView

IMPLEMENT_DYNCREATE(CIndiDmiView, CBaseChartCtlView)

CIndiDmiView::CIndiDmiView()
{
	m_pBarSeries = NULL;
	m_pCandlestickSerie = NULL;
}

CIndiDmiView::~CIndiDmiView()
{
}

BEGIN_MESSAGE_MAP(CIndiDmiView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiDmiView 绘图

void CIndiDmiView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualCapitalFlowView 诊断

#ifdef _DEBUG
void CIndiDmiView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiDmiView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiDmiView 消息处理程序



void CIndiDmiView::RedrawChart(CCalculateTask * cse)
{
	// 来自“Yahoo 股价曲线” 和 通过结合股本结构计算的动态市盈率

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	CCalcCapitalFlow * pCse =  (CCalcCapitalFlow *)cse;
	if(pCse->m_capital.size()!=1 || pCse->m_trading.size()!=1)
		return ;

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;

	CRect rect = m_pChartCtrl->GetPlottingRect();
	m_pChartCtrl->SetBackColor(CColor::darkgray);	//	darkgray
	m_pChartCtrl->SetBorderColor(CColor::white);

	CDirectionalMovement & dmi = *(pCse->m_dmi[0]);
	CNetEaseTradeDaily & trading = *(pCse->m_trading[0]);
	CBollingerBands & bollinger = *(pCse->m_bollinger[0]);
	
	if(trading.m_vec_report_date.size()>0 )
	{
		m_pChartCtrl->RemoveAllSeries();
		int nStart = 0, nEnd = trading.m_vec_report_date.size()-1;
		
		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[nStart + i].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			CNetEaseTradeDailyData data = trading.m_vec_reorganize[trading.m_vec_report_date[nStart + i]];
			if(data.high> ymax)  ymax = data.high;
			if(data.low<ymin)	ymin = data.low;
		}

		
		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		pLeftAxis->SetMinMax(ymax- (ymax-ymin)*3/2, ymax);	//	占上半部 ，共 2/3

		/*CChartStandardAxis* pTopAxis =
			m_pChartCtrl->CreateStandardAxis(CChartCtrl::TopAxis);
		pTopAxis->SetMinMax(0, 10);*/

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;
		CChartCandlestickSerie* pSeries = m_pChartCtrl->CreateCandlestickSerie(bSecondHorizAxis, bSecondVertAxis);
		int nWidth = (rect.right - rect.left)/(xmax - xmin);
		if(nWidth <1) nWidth = 1;
		pSeries->SetWidth(nWidth);
		pSeries->SetColor(RGB(0,164,64));
		
		
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[nStart + i].c_str());
			CNetEaseTradeDailyData data = trading.m_vec_reorganize[trading.m_vec_report_date[nStart + i]];
			pSeries->AddPoint(odt.m_dt, data.low, data.high, data.open, data.close);
		}

		m_pCandlestickSerie = pSeries;

		//////////////////////////////////////////////////////////////////////////
		//
		{
			CChartLineSerie* pSeries_up = m_pChartCtrl->CreateLineSerie();
			pSeries_up->SetName("布林上轨线");
			pSeries_up->SetWidth(1);
			pSeries_up->SetColor(CColor::yellow);

			CChartLineSerie* pSeries_middle = m_pChartCtrl->CreateLineSerie();
			pSeries_middle->SetName("布林中轨线");
			pSeries_middle->SetWidth(1);
			pSeries_middle->SetColor(CColor::white);

			CChartLineSerie* pSeries_down = m_pChartCtrl->CreateLineSerie();
			pSeries_down->SetName("布林下轨线");
			pSeries_down->SetWidth(1);
			pSeries_down->SetColor(CColor::purple);
			for(long i=0; i<  nEnd - nStart +1 && i < bollinger.m_vec_index.size(); i++)
			{
				COleDateTime odt ;
				double yy = 0.0;
				odt.ParseDateTime(bollinger.m_vec_index[nStart + i].tradedate.c_str());
				pSeries_up->AddPoint(odt.m_dt, bollinger.m_vec_index[nStart + i].up);
				pSeries_middle->AddPoint(odt.m_dt, bollinger.m_vec_index[nStart + i].middle);
				pSeries_down->AddPoint(odt.m_dt, bollinger.m_vec_index[nStart + i].down);
			}
		}
		

		//////////////////////////////////////////////////////////////////////////
		// 绘制 DMI 
		ymin = 0.0; ymax = 100.0;
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			if(dmi.m_vec_index[nStart + i].positive_di > ymax)  ymax = dmi.m_vec_index[nStart + i].positive_di;
			if(dmi.m_vec_index[nStart + i].negative_di > ymax)  ymax = dmi.m_vec_index[nStart + i].negative_di;
			if(dmi.m_vec_index[nStart + i].ema_dx > ymax)  ymax = dmi.m_vec_index[nStart + i].ema_dx;
		}

		CChartStandardAxis* pRightAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::RightAxis);
		pRightAxis->SetMinMax(ymin, ymax + ymax * 2); // 占底部的 1/3 

		{
			CChartLineSerie* pSeries_negative_di = m_pChartCtrl->CreateLineSerie(false, true);
			pSeries_negative_di->SetName("MDI");
			pSeries_negative_di->SetWidth(1);
			pSeries_negative_di->SetColor(CColor::yellow);

			CChartLineSerie* pSeries_positive_di = m_pChartCtrl->CreateLineSerie(false, true);
			pSeries_positive_di->SetName("PDI");
			pSeries_positive_di->SetWidth(1);
			pSeries_positive_di->SetColor(CColor::white);

			CChartLineSerie* pSeries_ema_dx = m_pChartCtrl->CreateLineSerie(false, true);
			pSeries_ema_dx->SetName("ADX");
			pSeries_ema_dx->SetWidth(1);
			pSeries_ema_dx->SetColor(CColor::purple);

			for(long i=0; i<  nEnd - nStart +1 && i < dmi.m_vec_index.size(); i++)
			{
				COleDateTime odt ;
				double yy = 0.0;
				odt.ParseDateTime(dmi.m_vec_index[nStart + i].tradedate.c_str());
				pSeries_negative_di->AddPoint(odt.m_dt, dmi.m_vec_index[nStart + i].negative_di);
				pSeries_positive_di->AddPoint(odt.m_dt, dmi.m_vec_index[nStart + i].positive_di);
				pSeries_ema_dx->AddPoint(odt.m_dt, dmi.m_vec_index[nStart + i].ema_dx);
			}
		}
		
		m_pChartCtrl->GetLegend()->SetVisible(true);
		m_pChartCtrl->RefreshCtrl();
	}

	//////////////////////////////////////////////////////////////////////////
	// 打上曲线的注释

	/*m_DMGraph.AutoRange();
	double xmin,xmax,ymin,ymax;
	m_DMGraph.GetAutoRange(&xmin, &xmax, &ymin, &ymax);
	double ystep = (ymax - ymin) /25;
	for(int k = 0; k<vec_annotion.size(); k++)
	{
		SetGraphAnnotation(xmin, ymax - ystep*(vec_annotion.size() - k), vec_annotion[k], vec_clr[k]);
	}*/
}


void CIndiDmiView::PostButtonClicked(int nWinDay)
{
	CRect rect = m_pChartCtrl->GetPlottingRect();
	int nWidth;
	if(nWinDay<=0)
	{
		double xmin, xmax;
		m_pChartCtrl->GetBottomAxis()->GetMinMax(xmin, xmax);
		nWidth = (rect.right - rect.left)/(xmax - xmin);
	}
	else
	{
		nWidth = (rect.right - rect.left)/nWinDay;
	}
	if(nWidth <1) nWidth = 1;

	if(m_pBarSeries)
		m_pBarSeries->SetBarWidth(nWidth);

	if(m_pCandlestickSerie)
		m_pCandlestickSerie->SetWidth(nWidth);

}


void CIndiDmiView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	m_LocalDoc.m_nCode = pDoc->m_nCode;	

	m_StartDate = COleDateTime::GetCurrentTime();
	m_StartDate -= 90;
	m_EndDate = COleDateTime::GetCurrentTime();

	RedrawDmGraph();
}


void CIndiDmiView::RedrawDmGraph()
{

	CCalcCapitalFlow * cse =  new CCalcCapitalFlow(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(m_LocalDoc.m_nCode);
	cse->m_vec_request_is_stock.push_back(1);
	cse->m_StartDate = m_StartDate;
	cse->m_EndDate = m_EndDate;
	cse->ScheduleTask(&CCalcCapitalFlow::Execute1);
} 


