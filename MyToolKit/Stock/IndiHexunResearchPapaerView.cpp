// IndiHexunResearchPapaerView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CalcHexunResearchPaperQuery.h"
#include "IndiHexunResearchPapaerView.h"


// CIndiHexunResearchPapaerView

IMPLEMENT_DYNCREATE(CIndiHexunResearchPapaerView, CBaseChartCtlView)

CIndiHexunResearchPapaerView::CIndiHexunResearchPapaerView()
{

}

CIndiHexunResearchPapaerView::~CIndiHexunResearchPapaerView()
{
}

BEGIN_MESSAGE_MAP(CIndiHexunResearchPapaerView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiHexunResearchPapaerView 绘图

void CIndiHexunResearchPapaerView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiHexunResearchPapaerView 诊断

#ifdef _DEBUG
void CIndiHexunResearchPapaerView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiHexunResearchPapaerView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiHexunResearchPapaerView 消息处理程序




void CIndiHexunResearchPapaerView::RedrawDmGraph(CCalculateTask * cse)
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

	CCalcHexunResearchPaperQuery * pData = (CCalcHexunResearchPaperQuery *)cse;

	if(pData->m_vec_report_date.size()>0)
	{
		int nStart = 0, nEnd = pData->m_vec_report_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i< pData->m_vec_report_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pData->m_vec_report_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i= pData->m_vec_report_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(pData->m_vec_report_date[i].c_str());
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
		int nFirstYear = 3000;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_report_date[nStart + i].c_str());
			if(i==0)	xmax = odt.m_dt;
			if(i== nEnd - nStart) xmin = odt.m_dt;

			if(pData->m_vec_first_eps[nStart + i]> ymax)  ymax = pData->m_vec_first_eps[nStart + i];
			if(pData->m_vec_second_eps[nStart + i]> ymax)  ymax = pData->m_vec_second_eps[nStart + i];
			if(pData->m_vec_third_eps[nStart + i]> ymax)  ymax = pData->m_vec_third_eps[nStart + i];

			if(pData->m_vec_initial_year[nStart + i]>0 && pData->m_vec_initial_year[nStart + i]< nFirstYear)
			{
				nFirstYear = pData->m_vec_initial_year[nStart + i];
			}
		}

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		pBottomAxis->SetMinMax(xmin-5, xmax + 5);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		pLeftAxis->SetMinMax(0, ymax + 0.2);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		/*CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
		pSeries->SetName("收盘价");

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_trade_date[nStart + i].c_str());
			pSeries->AddPoint(odt.m_dt, pData->m_vec_price[nStart + i]);
		}*/

		//////////////////////////////////////////////////////////////////////////
		CChartPointsSerie* pSeries[20] = { NULL };
		
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			if(pData->m_vec_initial_year[nStart + i]==0)
				continue;

			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pData->m_vec_report_date[nStart + i].c_str());

			/*研报预测的第一年*/
			if(pData->m_vec_first_eps[nStart + i]>0)
			{
				int idx = pData->m_vec_initial_year[nStart + i] - nFirstYear;
				if(pSeries[idx]==NULL)
				{
					pSeries[idx] = m_pChartCtrl->CreatePointsSerie();
					pSeries[idx]->SetPointType(CChartPointsSerie::ptEllipse);
					pSeries[idx]->SetPointSize(9, 9);
					pSeries[idx]->SetName(Int2String(nFirstYear + idx).c_str());
				}
				pSeries[idx]->AddPoint(odt.m_dt, pData->m_vec_first_eps[nStart + i]);
			}

			/*研报预测的第二年*/
			if(pData->m_vec_second_eps[nStart + i]>0)
			{
				int idx = pData->m_vec_initial_year[nStart + i] - nFirstYear + 1;
				if(pSeries[idx]==NULL)
				{
					pSeries[idx] = m_pChartCtrl->CreatePointsSerie();
					pSeries[idx]->SetPointType(CChartPointsSerie::ptEllipse);
					pSeries[idx]->SetPointSize(9, 9);
					pSeries[idx]->SetName(Int2String(nFirstYear + idx).c_str());
				}
				pSeries[idx]->AddPoint(odt.m_dt, pData->m_vec_second_eps[nStart + i]);
			}

			/*研报预测的第三年*/
			if(pData->m_vec_third_eps[nStart + i]>0)
			{
				int idx = pData->m_vec_initial_year[nStart + i] - nFirstYear + 2;
				if(pSeries[idx]==NULL)
				{
					pSeries[idx] = m_pChartCtrl->CreatePointsSerie();
					pSeries[idx]->SetPointType(CChartPointsSerie::ptEllipse);
					pSeries[idx]->SetPointSize(9, 9);
					pSeries[idx]->SetName(Int2String(nFirstYear + idx).c_str());
				}
				pSeries[idx]->AddPoint(odt.m_dt, pData->m_vec_third_eps[nStart + i]);
			}
			
		}
	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();

	m_pChartCtrl->EnableRefresh(true);

	//////////////////////////////////////////////////////////////////////////
	// 打上曲线的注释

}

void CIndiHexunResearchPapaerView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcHexunResearchPaperQuery * cse =  new CCalcHexunResearchPaperQuery(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(pDoc->m_nCode);	
	cse->ScheduleTask(&CCalcHexunResearchPaperQuery::Execute1);
}



void CIndiHexunResearchPapaerView::ProcessXmlWParam(string msgType,string code,string model,\
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