// PlateIndustryDynamicPeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "PlateCsindexDynamicPeView.h"

#include "CalcCsindex.h"


// CPlateIndustryDynamicPeView

IMPLEMENT_DYNCREATE(CPlateCsindexDynamicPeView, CBaseChartCtlView)

CPlateCsindexDynamicPeView::CPlateCsindexDynamicPeView()
{

}

CPlateCsindexDynamicPeView::~CPlateCsindexDynamicPeView()
{
}

BEGIN_MESSAGE_MAP(CPlateCsindexDynamicPeView, CBaseChartCtlView)
END_MESSAGE_MAP()


// CPlateIndustryDynamicPeView 绘图

void CPlateCsindexDynamicPeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateIndustryDynamicPeView 诊断

#ifdef _DEBUG
void CPlateCsindexDynamicPeView::AssertValid() const
{
	CBaseChartCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlateCsindexDynamicPeView::Dump(CDumpContext& dc) const
{
	CBaseChartCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateIndustryDynamicPeView 消息处理程序



void CPlateCsindexDynamicPeView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	m_MessageCtrl.SetStockRangeForCalculateTask(pDoc, NULL, CBaseMessageControl::Search_Child);

	RedrawDmGraph();
}

void CPlateCsindexDynamicPeView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcCsindex * cse =  new CCalcCsindex(m_hWnd, WM_USER_LOG_1);
	cse->m_vec_request_code = pDoc->m_vec_request_code;
	cse->m_vec_request_is_stock = pDoc->m_vec_request_is_stock;
	cse->m_str_request_zb_code = pDoc->m_strInput;

	cse->ScheduleTask(&CCalcCsindex::Execute1);
}



void CPlateCsindexDynamicPeView::ProccessChartMenu()
{
	//	弹出菜单
	/*CMenu menu;
	menu.LoadMenu(IDR_IndividualSummary);		
	CMenu *pMenuPopup = menu.GetSubMenu(4);
	ASSERT(pMenuPopup);

	CPoint point;
	GetCursorPos(&point);
	pMenuPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,point.x,point.y,this,NULL);
	menu.DestroyMenu();*/

	/*if(m_strSelectedCurveName.empty()==false)
	{
		CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
		
		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), m_strSelectedCurveName.c_str(), firstMatch )>0)
		{
			string::size_type pos1 = firstMatch[0].find(")");
			string codeStr = firstMatch[0].substr(1, pos1-1);

			vector<string>::iterator iter;
			vector<int>::iterator iter1;
			for( iter=pDoc->m_vec_request_code.begin(), iter1=pDoc->m_vec_request_is_stock.begin() ; 
				iter!=pDoc->m_vec_request_code.end(); )
			{
				if( *iter == codeStr)
				{
					iter = pDoc->m_vec_request_code.erase(iter);
					iter1 = pDoc->m_vec_request_is_stock.erase(iter1);
				}
				else
				{
					iter ++ ;
					iter1 ++;
				}
			}
		}
	
		RedrawDmGraph();
	}*/
}



//	绘制 “中证公司动态市盈率PE”曲线
/*
void CPlateCsindexDynamicPeView::RedrawDmGraph(CCalculateTask * cse)
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	ASSERT_VALID(pDoc);	

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalcCsindex * pCse =  (CCalcCsindex *)cse;
	m_pChartCtrl->RemoveAllSeries();

	CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
	CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	
	string nLastCode = "-1";
	int nIsStock = -1;
	CChartLineSerie* pSeries = NULL;
	for(int m=0; m< pCse->m_vec_code.size(); m++)
	{	
		if(nLastCode!=pCse->m_vec_code[m] || nIsStock!=pCse->m_vec_is_stock[m])
		{
			nLastCode = pCse->m_vec_code[m];
			nIsStock = pCse->m_vec_is_stock[m];

			pSeries = m_pChartCtrl->CreateLineSerie();
			char tmp[128];
			if(pCse->m_vec_is_stock[m])
			{
				sprintf_s(tmp, 128, "%s(%s)", pCse->m_vec_abbreviation[m].c_str(), pCse->m_vec_code[m].c_str());
				pSeries->SetWidth(1);
			}
			else
			{
				sprintf_s(tmp, 128, "%s(%s)", pCse->m_vec_abbreviation[m].c_str(), pCse->m_vec_code[m].c_str());
				pSeries->SetWidth(2);
			}
			pSeries->SetName( tmp);

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

		double ratio;
		if(pDoc->m_strInput == "DynamicPe") 
		{
			ratio = pCse->m_vec_dynamic_pe[m];
		}
		else if(pDoc->m_strInput == "StaticPe")
		{
			ratio = pCse->m_vec_static_pe[m];
		}
		else if(pDoc->m_strInput == "Pb")
		{
			ratio = pCse->m_vec_pb[m];
		}
		else if(pDoc->m_strInput == "Company_Number")
		{
			ratio = pCse->m_vec_company_number[m];
		}
		else if(pDoc->m_strInput == "Dividend_Yield_Ratio")
		{
			ratio = pCse->m_vec_dividend_yield_ratio[m];
		}
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
	}

	m_pChartCtrl->EnableRefresh(true);
}
*/