// IndividualCapitalFlowView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"

#include "TechAnalysisEvent.h"
#include "DirectionalMovement.h"
#include "IndividualCapitalFlowView.h"
#include "CalcCapitalFlow.h"

#include "TipDialog.h"

//////////////////////////////////////////////////////////////////////////
//

void CIndividualCapitalFlowViewMouseListener::OnMouseEventSeries(CChartMouseListener::MouseEvent mouseEvent, CPoint point,
														CChartSerieBase<SChartXYPoint>* pSerie, unsigned uPointIndex) 
{
	if(mouseEvent == CChartMouseListener::LButtonDoubleClick)
	{
		SChartXYPoint pt  = pSerie->GetPoint(uPointIndex);
		COleDateTime dt;
		dt.m_dt = pt.GetX();

		list<CTipRecord>  lstTmp;
		for(int i=0; i< m_pView->m_tech_analysis_event.size(); i++)
		{
			if(m_pView->m_tech_analysis_event[i].index_name.find("VOL")==string::npos)
			{
				CTipRecord tip;
				COleDateTime dt1;
				dt1.ParseDateTime(m_pView->m_tech_analysis_event[i].tradedate.c_str());
				if(dt1.m_dt >= dt.m_dt && lstTmp.size() < 10)
				{
					tip.report_date = m_pView->m_tech_analysis_event[i].tradedate;
					tip.id = m_pView->m_tech_analysis_event[i].index_name;
					tip.title = m_pView->m_tech_analysis_event[i].index_name;
					tip.url = m_pView->m_tech_analysis_event[i].basic_desc;

					lstTmp.push_back(tip);
				}
			}			
		}

		if(lstTmp.size()>0)
		{
			CTipDialog dlg;

			dlg.m_lstTip.insert(dlg.m_lstTip.end(),  lstTmp.begin(), lstTmp.end());

			dlg.DoModal();
		}
		
	}
	else if(mouseEvent == CChartMouseListener::LButtonDown)
	{
		
		pSerie->EnableShadow(true);
	}
	else if(mouseEvent == CChartMouseListener::LButtonUp)
	{
		pSerie->EnableShadow(false);
	}
}





//////////////////////////////////////////////////////////////////////////
// CIndividualCapitalFlowView

IMPLEMENT_DYNCREATE(CIndividualCapitalFlowView, CBaseChartCtlView)

CIndividualCapitalFlowView::CIndividualCapitalFlowView()
{
	m_pBarSeries = NULL;
	m_pCandlestickSerie = NULL;
}

CIndividualCapitalFlowView::~CIndividualCapitalFlowView()
{
}

BEGIN_MESSAGE_MAP(CIndividualCapitalFlowView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndividualCapitalFlowView 绘图

void CIndividualCapitalFlowView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualCapitalFlowView 诊断

#ifdef _DEBUG
void CIndividualCapitalFlowView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualCapitalFlowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualCapitalFlowView 消息处理程序



void CIndividualCapitalFlowView::RedrawChart(CCalculateTask * cse)
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

	CCapitalFlows163 & capital = *(pCse->m_capital[0]);
	CNetEaseTradeDaily & trading = *(pCse->m_trading[0]);
	CBollingerBands & bollinger = *(pCse->m_bollinger[0]);
	
	if(trading.m_vec_report_date.size()>0 )
	{
		m_pChartCtrl->RemoveAllSeries();
		int nStart = 0, nEnd = trading.m_vec_report_date.size()-1;
		
		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;

		for(long i=0; i< trading.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[i].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			CNetEaseTradeDailyData data = trading.m_vec_reorganize[trading.m_vec_report_date[i]];
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
		
		
		for(long i=0; i<  trading.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(trading.m_vec_report_date[i].c_str());
			CNetEaseTradeDailyData data = trading.m_vec_reorganize[trading.m_vec_report_date[i]];
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
			for(long i=0; i < bollinger.m_vec_index.size(); i++)
			{
				COleDateTime odt ;
				double yy = 0.0;
				odt.ParseDateTime(bollinger.m_vec_index[i].tradedate.c_str());
				pSeries_up->AddPoint(odt.m_dt, bollinger.m_vec_index[i].up);
				pSeries_middle->AddPoint(odt.m_dt, bollinger.m_vec_index[i].middle);
				pSeries_down->AddPoint(odt.m_dt, bollinger.m_vec_index[i].down);
			}
		}
		

		//////////////////////////////////////////////////////////////////////////
		// 绘制资金
		ymin = 0.0; ymax = 0.0;
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			if(capital.m_vec_flow_in[nStart + i]> ymax)  ymax = capital.m_vec_flow_in[nStart + i];
			if(capital.m_vec_flow_out[nStart + i]>ymin)		ymin = capital.m_vec_flow_out[nStart + i];
		}
		
		CChartStandardAxis* pRightAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::RightAxis);
		pRightAxis->SetMinMax(-ymin -10, ymax + (ymax + ymin) * 2); // 占底部的 1/3 

		// 绘制资金的总进出
		CChartBarSerie* pBarSeries = m_pChartCtrl->CreateBarSerie(false, true);
		pBarSeries->SetBaseLine(false, 0);
		pBarSeries->SetGroupId(1);
		pBarSeries->SetBarWidth(nWidth);

		for(long i=0; i < capital.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(capital.m_vec_report_date[i].c_str());
			pBarSeries->AddPoint(odt.m_dt, capital.m_vec_flow_in[i]);
		}
		
		pBarSeries = m_pChartCtrl->CreateBarSerie(false, true);
		pBarSeries->SetBaseLine(false, 0);
		pBarSeries->SetGroupId(2);
		pBarSeries->SetBarWidth(nWidth);
		for(long i=0; i < capital.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(capital.m_vec_report_date[i].c_str());
			pBarSeries->AddPoint(odt.m_dt, -capital.m_vec_flow_out[i]);
		}

		// 绘制其中主力资金的进出
		pBarSeries = m_pChartCtrl->CreateBarSerie(false, true);
		pBarSeries->SetBaseLine(false, 0);
		pBarSeries->SetGroupId(3);
		pBarSeries->SetBarWidth(nWidth);

		for(long i=0; i < capital.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(capital.m_vec_report_date[i].c_str());
			pBarSeries->AddPoint(odt.m_dt, capital.m_vec_main_flow_in[i]);
		}

		pBarSeries = m_pChartCtrl->CreateBarSerie(false, true);
		pBarSeries->SetBaseLine(false, 0);
		pBarSeries->SetGroupId(4);
		pBarSeries->SetBarWidth(nWidth);
		for(long i=0;   i < capital.m_vec_report_date.size(); i++)
		{
			COleDateTime odt ;
			odt.ParseDateTime(capital.m_vec_report_date[nStart + i].c_str());
			pBarSeries->AddPoint(odt.m_dt, -capital.m_vec_main_flow_out[nStart + i]);
		}

		m_pBarSeries = pBarSeries;

		//////////////////////////////////////////////////////////////////////////
		// 打上曲线的注释

		CTechAnalysisEvent & event = *(pCse->m_tech_analysis_event[0]);

		map<string, CChartPointsSerie *>	mapPointsSerie;

		if(event.m_vec_index.size()>0)
		{
			m_tech_analysis_event = event.m_vec_index;
			for(int i=0; i< m_tech_analysis_event.size(); i++)
			{
				if(m_tech_analysis_event[i].index_name.find("VOL")==string::npos)
				{
					if(mapPointsSerie.count(m_tech_analysis_event[i].index_name)==0)
					{
						CChartPointsSerie * pSeries = m_pChartCtrl->CreatePointsSerie(false , true);
						pSeries->SetPointType(CChartPointsSerie::ptEllipse);
						pSeries->SetPointSize(6,6);

						if(m_tech_analysis_event[i].index_name.find("BOLL")!=string::npos)
							pSeries->SetColor(CColor::red);
						else
							pSeries->SetColor(CColor::green);

						mapPointsSerie[m_tech_analysis_event[i].index_name] = pSeries;

						pSeries->EnableMouseNotifications(true,false);
						pSeries->RegisterMouseListener(new CIndividualCapitalFlowViewMouseListener(this));
					}

					CChartPointsSerie * pSeries = mapPointsSerie[m_tech_analysis_event[i].index_name];
					COleDateTime odt ;
					odt.ParseDateTime(m_tech_analysis_event[i].tradedate.c_str());
					pSeries->AddPoint(odt.m_dt, -ymin - 5);
				}	
			}
		}
				
		m_pChartCtrl->GetLegend()->SetVisible(false);
		m_pChartCtrl->RefreshCtrl();
	}
}


void CIndividualCapitalFlowView::PostButtonClicked(int nWinDay)
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


void CIndividualCapitalFlowView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	m_LocalDoc.m_nCode = pDoc->m_nCode;	

	m_StartDate = COleDateTime::GetCurrentTime();
	m_StartDate -= 270;
	m_EndDate = COleDateTime::GetCurrentTime();

	RedrawDmGraph();
}


void CIndividualCapitalFlowView::RedrawDmGraph()
{

	CCalcCapitalFlow * cse =  new CCalcCapitalFlow(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(m_LocalDoc.m_nCode);
	cse->m_vec_request_is_stock.push_back(1);
	cse->m_StartDate = m_StartDate;
	cse->m_EndDate = m_EndDate;
	cse->ScheduleTask(&CCalcCapitalFlow::Execute1);
} 


