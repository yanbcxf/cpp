// IndiMarketIndexAnalysisView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CalcMarketValue.h"
#include "IndiMarketIndexAnalysisView.h"


// CIndiMarketIndexAnalysisView

IMPLEMENT_DYNCREATE(CIndiMarketIndexAnalysisView, CBaseChartCtlView)

CIndiMarketIndexAnalysisView::CIndiMarketIndexAnalysisView()
{

}

CIndiMarketIndexAnalysisView::~CIndiMarketIndexAnalysisView()
{
}

BEGIN_MESSAGE_MAP(CIndiMarketIndexAnalysisView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiMarketIndexAnalysisView 绘图

void CIndiMarketIndexAnalysisView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiMarketIndexAnalysisView 诊断

#ifdef _DEBUG
void CIndiMarketIndexAnalysisView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiMarketIndexAnalysisView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiMarketIndexAnalysisView 消息处理程序


void CIndiMarketIndexAnalysisView::RedrawDmGraph(CCalculateTask * cse)
{
	// 来自“Yahoo 股价曲线” 和 通过结合股本结构计算的动态市盈率

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;

	CRect rect;
	m_pChartCtrl->GetClientRect(&rect);
	m_pChartCtrl->RemoveAllSeries();

	CCalcMarketValue * pData = (CCalcMarketValue *)cse;

	if(pData->m_vec_trade_date.size()>0)
	{

		double dRatio = pData->m_vec_market_index[0];
		dRatio = dRatio / pData->m_vec_stock_index[0];

		int nStart = 0, nEnd = pData->m_vec_trade_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i< pData->m_vec_trade_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pData->m_vec_trade_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i= pData->m_vec_trade_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pData->m_vec_trade_date[i].c_str());
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

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			if(pData->m_vec_market_index[nStart + i]> ymax)  ymax = pData->m_vec_market_index[nStart + i];
			if(pData->m_vec_stock_index[nStart + i] * dRatio> ymax)  ymax = pData->m_vec_stock_index[nStart + i]* dRatio;
			
			if(pData->m_vec_corrected_stock_index[nStart + i] * dRatio> ymax )  
				ymax = pData->m_vec_corrected_stock_index[nStart + i] * dRatio;

		}

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		pLeftAxis->SetMinMax(0, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("上证指数");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_market_index[nStart + i] );
		}
		pSeries->CreateBalloonLabel( RandomFromRange(0,  nEnd - nStart +1), "上证指数")->SetBackgroundColor(pSeries->GetColor());
		
		pSeries->EnableMouseNotifications(true,false);
		pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("个股指数");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_stock_index[nStart + i] * dRatio );
		}
		
		pSeries->CreateBalloonLabel( RandomFromRange(0,  nEnd - nStart +1), "个股指数")->SetBackgroundColor(pSeries->GetColor());
		pSeries->EnableMouseNotifications(true,false);
		pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("个股指数（修改大盘波动）");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_corrected_stock_index[nStart + i] * dRatio );
		}
		pSeries->CreateBalloonLabel( RandomFromRange(0,  nEnd - nStart +1), "个股指数（修改大盘波动）")->SetBackgroundColor(pSeries->GetColor());
		pSeries->EnableMouseNotifications(true,false);
		pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));

	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);

	//////////////////////////////////////////////////////////////////////////
	// 打上曲线的注释

}

void CIndiMarketIndexAnalysisView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMarketValue * cse =  new CCalcMarketValue(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(pDoc->m_nCode);	
	cse->ScheduleTask(&CCalcMarketValue::Execute1);
}



void CIndiMarketIndexAnalysisView::ProcessXmlWParam(string msgType,string code,string model,\
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
