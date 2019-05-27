// PlateCorrectedMarketValueView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "PlateCorrectedMarketValueView.h"
#include "CalcMarketValue.h"


// CPlateCorrectedMarketValueView

IMPLEMENT_DYNCREATE(CPlateCorrectedMarketValueView, CBaseChartCtlView)

CPlateCorrectedMarketValueView::CPlateCorrectedMarketValueView()
{

}

CPlateCorrectedMarketValueView::~CPlateCorrectedMarketValueView()
{
}

BEGIN_MESSAGE_MAP(CPlateCorrectedMarketValueView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateCorrectedMarketValueView 绘图

void CPlateCorrectedMarketValueView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateCorrectedMarketValueView 诊断

#ifdef _DEBUG
void CPlateCorrectedMarketValueView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateCorrectedMarketValueView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateCorrectedMarketValueView 消息处理程序


void CPlateCorrectedMarketValueView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	RedrawDmGraph();
}

void CPlateCorrectedMarketValueView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMarketValue * cse =  new CCalcMarketValue(m_hWnd, WM_USER_LOG_1);

	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, cse);

	cse->ScheduleTask(&CCalcMarketValue::Execute1);
}

void CPlateCorrectedMarketValueView::ProcessXmlWParam(string msgType,string code,string model,\
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


void CPlateCorrectedMarketValueView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcMarketValue * pCse =  (CCalcMarketValue *)cse;
	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	//	绘制 “扣除大盘后的市值指数”曲线
	int nLastCode = -1;
	CChartLineSerie* pSeries = NULL;
	for(int m=0; m< pCse->m_vec_code.size(); m++)
	{	
		if(nLastCode!=pCse->m_vec_code[m])
		{
			nLastCode = pCse->m_vec_code[m];

			pSeries = m_pChartCtrl->CreateLineSerie();
			pSeries->SetWidth(1);
			char tmp[32];
			sprintf_s(tmp, 32, "%06d", pCse->m_vec_code[m]);

			pSeries->SetName( tmp /* abbreviation.c_str()*/);

			pSeries->EnableMouseNotifications(true,false);
			pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));
		}

		string report_date = pCse->m_vec_trade_date[m];
		COleDateTime odt ;
		odt.ParseDateTime(report_date.c_str());
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			if(odt.m_dt < m_StartDate || odt.m_dt > m_EndDate)
				continue;
		}

		// 以开盘价作为“基准”
		double ratio = pCse->m_vec_corrected_stock_index[m] ;
		if(odt.m_dt> xmax) xmax = odt.m_dt;
		if(odt.m_dt< xmin) xmin = odt.m_dt;

		if(ratio > ymax) ymax = ratio;
		if(ratio < ymin) ymin = ratio;

		pSeries->AddPoint(odt.m_dt, ratio);
	}

	if(ymax>0 && xmax > xmin)
	{
		pBottomAxis->SetMinMax(xmin, xmax);
		pLeftAxis->SetMinMax(ymin, ymax);

		m_pChartCtrl->GetLegend()->SetVisible(true);
		m_pChartCtrl->RefreshCtrl();
	}

	m_pChartCtrl->EnableRefresh(true);
}
