// IndiMainIncomeGrowthView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "IndiMainIncomeGrowthView.h"
#include "CalcMainIncomeGrowth.h"

// CIndiMainIncomeGrowthView

IMPLEMENT_DYNCREATE(CIndiMainIncomeGrowthView, CBaseChartCtlView)

CIndiMainIncomeGrowthView::CIndiMainIncomeGrowthView()
{

}

CIndiMainIncomeGrowthView::~CIndiMainIncomeGrowthView()
{
}

BEGIN_MESSAGE_MAP(CIndiMainIncomeGrowthView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CIndiMainIncomeGrowthView 绘图

void CIndiMainIncomeGrowthView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndiMainIncomeGrowthView 诊断

#ifdef _DEBUG
void CIndiMainIncomeGrowthView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiMainIncomeGrowthView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndiMainIncomeGrowthView 消息处理程序



void CIndiMainIncomeGrowthView::OnInitialUpdate()
{
	CBaseChartCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcMainIncomeGrowth * cse =  new CCalcMainIncomeGrowth(m_hWnd, WM_USER_LOG_1);

	cse->m_vec_request_code.push_back(pDoc->m_nCode);	
	cse->ScheduleTask(&CCalcMainIncomeGrowth::Execute1);

}



void CIndiMainIncomeGrowthView::ProcessXmlWParam(string msgType,string code,string model,\
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



void CIndiMainIncomeGrowthView::RedrawDmGraph(CCalculateTask * pcse)
{
	// 来自“Yahoo 股价曲线” 和 通过结合股本结构计算的动态市盈率

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	HRESULT hr;
	vector<string>   vec_annotion;
	vector<COLORREF> vec_clr;

	CRect rect;
	m_pChartCtrl->GetClientRect(&rect);
	m_pChartCtrl->RemoveAllSeries();

	CCalcMainIncomeGrowth * cse = (CCalcMainIncomeGrowth *)pcse;

	if(cse->m_vec_report_date.size()>0)
	{
		int nStart = 0, nEnd = cse->m_vec_report_date.size()-1;
		if(m_StartDate> 0 && m_EndDate> m_StartDate)
		{
			//	寻找“起始”的记录
			for(int i=0 ; i< cse->m_vec_report_date.size(); i++)
			{
				COleDateTime odt ;
				odt.ParseDateTime(cse->m_vec_report_date[i].c_str());
				if(odt.m_dt >= m_StartDate)
				{
					nStart = i;
					break;
				}
			}

			//	寻找“结束”的记录
			for(int i= cse->m_vec_report_date.size() - 1; i>=0; i--)
			{
				COleDateTime odt ;
				odt.ParseDateTime(cse->m_vec_report_date[i].c_str());
				if(odt.m_dt<= m_EndDate)
				{
					nEnd = i;
					break;
				}
			}
		}

		//	绘制坐标轴

		double xmin, xmax, ymin, ymax;
		ymin = 99999.0; ymax = 0.0;

		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(cse->m_vec_report_date[nStart + i].c_str());
			if(i==0)	xmin = odt.m_dt;
			if(i== nEnd - nStart) xmax = odt.m_dt;

			if(cse->m_vec_income[nStart + i]> ymax)  ymax = cse->m_vec_income[nStart + i];
		}

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);
		pLeftAxis->SetMinMax(ymin, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;


		//	汇总所有的产品名称
		map<string, string>  mapCompositionName;
		int idx = 0;
		for(long i=0; i<  nEnd - nStart +1; i++)
		{
			string compositionName = cse->m_vec_composition_name[nStart + i];
			mapCompositionName[compositionName]=cse->m_vec_composition_name[nStart + i] 
				+ ", " + Double2String(cse->m_vec_proportion_of_total_income[nStart + i]) + "%";
		}

		//	汇总各个的产品曲线
		map<string, string>::iterator it = mapCompositionName.begin();
		for(; it!=mapCompositionName.end(); it++)
		{
			string compositionName = it->first;
			CChartLineSerie* pSeries = m_pChartCtrl->CreateLineSerie();
			pSeries->SetWidth(1);
			pSeries->SetName(it->second.c_str());

			pSeries->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));

			bool isFirst = true;
			for(long i=0; i<  nEnd - nStart +1; i++)
			{
				if(cse->m_vec_composition_name[nStart + i]== compositionName)
				{
					isFirst = false;
					COleDateTime odt ;
					double yy = 0.0;
					odt.ParseDateTime(cse->m_vec_report_date[nStart + i].c_str());
					pSeries->AddPoint(odt.m_dt, cse->m_vec_income[nStart + i]);

					int mon = odt.GetMonth();
					if(mon==12)
					{
						//	期初清零，向后推迟一日，每年的元旦
						pSeries->AddPoint(odt.m_dt + 1.0, 0.0);
					}
				}
				if(isFirst == true)
				{
					/*在曲线的最开始年份补充 0 点位*/
					COleDateTime odt ;
					odt.ParseDateTime(cse->m_vec_report_date[nStart + i].c_str());
					int mon = odt.GetMonth();
					if(mon==12)
					{
						pSeries->AddPoint(odt.m_dt + 1.0, 0.0);
					}
				}
			}
		}

	}

	m_pChartCtrl->GetLegend()->SetVisible(true);
	m_pChartCtrl->RefreshCtrl();
}