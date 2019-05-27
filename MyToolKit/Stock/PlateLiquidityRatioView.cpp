// PlateLiquidityRatioView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "PlateLiquidityRatioView.h"
#include "CalculateStockInfoEx.h"

// CPlateLiquidityRatioView

IMPLEMENT_DYNCREATE(CPlateLiquidityRatioView, CBaseChartCtlView)

CPlateLiquidityRatioView::CPlateLiquidityRatioView()
{

}

CPlateLiquidityRatioView::~CPlateLiquidityRatioView()
{
}

BEGIN_MESSAGE_MAP(CPlateLiquidityRatioView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateLiquidityRatioView 绘图

void CPlateLiquidityRatioView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateLiquidityRatioView 诊断

#ifdef _DEBUG
void CPlateLiquidityRatioView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateLiquidityRatioView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateLiquidityRatioView 消息处理程序


void CPlateLiquidityRatioView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateLiquidityRatioView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalculateStockInfoEx::RecentLiquidityRatio1);
}

void CPlateLiquidityRatioView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CPlateLiquidityRatioView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalculateStockInfoEx * pCse =  (CCalculateStockInfoEx *)cse;
	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	for(int i =0; i<pCse->m_vec_request_code.size(); i++)
	{
		//	逐个股票数据进行过滤
		string abbreviation;
		map<string, double>	vec_liquidity;
		for(int m=0; m< pCse->m_vec_code.size(); m++)
		{
			if(pCse->m_vec_code[m]==atoi(pCse->m_vec_request_code[i].c_str()))
			{
				vec_liquidity[pCse->m_vec_report_date[m]] = pCse->m_vec_liquidity_ratio[m];

				if(abbreviation.empty())
				{
					abbreviation = pCse->m_vec_abbreviation[m];
				}
			}
		}

		//	绘制 “流动比率”曲线
		CChartLineSerie* pSeries = NULL;
		if(vec_liquidity.size()>0)
		{
			pSeries = m_pChartCtrl->CreateLineSerie();
			char tmp[32];
			sprintf_s(tmp, 32, "%s(%s)", abbreviation.c_str(), pCse->m_vec_request_code[i].c_str());
			pSeries->SetName(tmp);
			pSeries->SetWidth(1);

			pSeries->EnableMouseNotifications(true,false);
			pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));
		}
		map<string, double>::iterator it = vec_liquidity.begin();
		for(; it!=vec_liquidity.end(); it++)
		{
			string report_date = it->first;
			COleDateTime odt ;
			odt.ParseDateTime(report_date.c_str());
			if(m_StartDate> 0 && m_EndDate> m_StartDate)
			{
				if(odt.m_dt < m_StartDate || odt.m_dt > m_EndDate)
					continue;
			}

			double liquidity = it->second;
			if(odt.m_dt> xmax) xmax = odt.m_dt;
			if(odt.m_dt< xmin) xmin = odt.m_dt;
			
			if(liquidity > ymax) ymax = liquidity;
			if(liquidity < ymin) ymin = liquidity;

			pSeries->AddPoint(odt.m_dt, liquidity);
		}
	}
	pBottomAxis->SetMinMax(xmin, xmax);
	pLeftAxis->SetMinMax(ymin, ymax);

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
}
