// IndiMacdView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndiMacdView.h"
#include "CalcMacdViewData.h"

// CIndiMacdView

IMPLEMENT_DYNCREATE(CIndiMacdView, CBaseChartCtlView)

CIndiMacdView::CIndiMacdView()
{

}

CIndiMacdView::~CIndiMacdView()
{
}

BEGIN_MESSAGE_MAP(CIndiMacdView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiMacdView 绘图

void CIndiMacdView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiMacdView 诊断

#ifdef _DEBUG
void CIndiMacdView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiMacdView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiMacdView 消息处理程序


void CIndiMacdView::RedrawDmGraph(CCalculateTask * cse)
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

	CCalcMacdViewData * pData = (CCalcMacdViewData *)cse;
	
	if(pData->m_vec_trade_date.size()>0)
	{
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
			
			// if(pData->m_vec_total_value[nStart + i]> ymax)  ymax = pData->m_vec_total_value[nStart + i];
			if(pData->m_vec_ema12[nStart + i]> ymax)  ymax = pData->m_vec_ema12[nStart + i];
			if(pData->m_vec_ema26[nStart + i]> ymax)  ymax = pData->m_vec_ema26[nStart + i];
			if(pData->m_vec_dif[nStart + i]> ymax)  ymax = pData->m_vec_dif[nStart + i];
			if(pData->m_vec_dea[nStart + i]> ymax)  ymax = pData->m_vec_dea[nStart + i];

			// if(pData->m_vec_total_value[nStart + i] <  ymin)  ymin = pData->m_vec_total_value[nStart + i];
			if(pData->m_vec_ema12[nStart + i] < ymin)  ymin = pData->m_vec_ema12[nStart + i];
			if(pData->m_vec_ema26[nStart + i] < ymin)  ymin = pData->m_vec_ema26[nStart + i];
			if(pData->m_vec_dif[nStart + i] < ymin)  ymin = pData->m_vec_dif[nStart + i];
			if(pData->m_vec_dea[nStart + i] < ymin)  ymin = pData->m_vec_dea[nStart + i];
			
		}

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		pLeftAxis->SetMinMax(ymin, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////
		
		/*CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("市值");
	
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_total_value[nStart + i]);
		}*/

		//////////////////////////////////////////////////////////////////////////
		CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("EMA12");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_ema12[nStart + i]);
		}

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("EMA26");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_ema26[nStart + i]);
		}

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetColor(RGB(47, 39, 135));
		pSeries->SetName("离差值(DIF)");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_dif[nStart + i]);
		}

		//////////////////////////////////////////////////////////////////////////
		pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetColor(RGB(168, 168, 0));
		pSeries->SetName("9日离差平均值(DEA)");
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_dea[nStart + i]);
		}

	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);
	//////////////////////////////////////////////////////////////////////////
	// 打上曲线的注释

}

void CIndiMacdView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMacdViewData * cse =  new CCalcMacdViewData(m_hWnd, WM_USER_LOG_1);

	cse->m_nCode = 	atoi(pDoc->m_nCode.c_str());	
	cse->ScheduleTask(&CCalcMacdViewData::Execute1);
}



void CIndiMacdView::ProcessXmlWParam(string msgType,string code,string model,\
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