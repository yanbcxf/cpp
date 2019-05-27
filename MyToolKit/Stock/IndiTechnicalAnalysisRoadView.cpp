// PlateIndustryDynamicPeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndiTechnicalAnalysisRoadView.h"
#include "BaseMessageFormView.h"
#include "CalcTechnicalAnalysisRoadMap.h"

#include "BaseChartCtlView.h"

// CPlateIndustryDynamicPeView

IMPLEMENT_DYNCREATE(CIndiTechnicalAnalysisRoadView, CBaseMessageFormView)

CIndiTechnicalAnalysisRoadView::CIndiTechnicalAnalysisRoadView()
{

}

CIndiTechnicalAnalysisRoadView::~CIndiTechnicalAnalysisRoadView()
{
}

BEGIN_MESSAGE_MAP(CIndiTechnicalAnalysisRoadView, CBaseMessageFormView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
END_MESSAGE_MAP()


// CIndiAddIssuranceRoadView 绘图

void CIndiTechnicalAnalysisRoadView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateIndustryDynamicPeView 诊断

#ifdef _DEBUG
void CIndiTechnicalAnalysisRoadView::AssertValid() const
{
	CBaseMessageFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiTechnicalAnalysisRoadView::Dump(CDumpContext& dc) const
{
	CBaseMessageFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateIndustryDynamicPeView 消息处理程序



void CIndiTechnicalAnalysisRoadView::OnInitialUpdate()
{
	CBaseMessageFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	

	RedrawDmGraph();
}

void CIndiTechnicalAnalysisRoadView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcTechnicalAnalysisRoadMap * cse =  new CCalcTechnicalAnalysisRoadMap(m_hWnd, WM_USER_LOG_1);
	cse->m_vec_request_code.push_back( pDoc->m_nCode );
	cse->m_str_request_zb_code = pDoc->m_strInput;

	cse->ScheduleTask(&CCalcTechnicalAnalysisRoadMap::Execute1);
}



string CIndiTechnicalAnalysisRoadView::ExtractKeyword(string title)
{
	string strTip1 = "";
	if(title.find("预案")!=string::npos )	return "预案";
	if(title.find("证监会受理")!=string::npos )	return "证监会受理";
	
	
	return "";
						
}


void CIndiTechnicalAnalysisRoadView::RedrawChart(CCalculateTask * cse)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();



	CCalculateTask * pCse =  (CCalculateTask *)cse;

	//////////////////////////////////////////////////////////////////////////
	// 
	list<CTipRecord>		lstTip;

	map<string,list<CTipRecord>>::iterator it_t = pCse->tip.begin();
	for(; it_t!=pCse->tip.end(); it_t++)
	{
		lstTip.insert(lstTip.end(), it_t->second.begin(), it_t->second.end());
	}

	lstTip.reverse();

	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

	m_Grid.SetRowCount(lstTip.size() + 1);
	m_Grid.SetColumnCount(4);

	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnSelection(FALSE);
	m_Grid.SetFixedRowSelection(FALSE);
	m_Grid.EnableColumnHide();
	m_Grid.SetEditable(FALSE);

	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.col = 0;
	Item.strText.Format(_T("%s"), "日期");
	m_Grid.SetItem(&Item);

	Item.col = 1;
	Item.strText.Format(_T("%s"), "指标");
	m_Grid.SetItem(&Item);

	Item.col = 2;
	Item.strText.Format(_T("%s"), "事件");
	m_Grid.SetItem(&Item);

	Item.col = 3;
	Item.strText.Format(_T("%s"), "备注");
	m_Grid.SetItem(&Item);

	int		bgColor = 0;
	string	last_report_date = "";

	list<CTipRecord>::iterator it_tip = lstTip.begin();
	for(int nRow = 0 ; it_tip != lstTip.end(); it_tip++, nRow++)
	{

		if(it_tip->report_date!=last_report_date)
		{
			last_report_date = it_tip->report_date;
			if(bgColor==0) bgColor = 1;
			else bgColor = 0;
		}

		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT | GVIF_BKCLR;
		Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		Item.row = nRow + 1;
		Item.col = 0;
		if(bgColor==0)
			Item.crBkClr = RGB(196, 236, 196);
		else 
			Item.crBkClr = RGB(255, 255, 255);
		Item.strText.Format(_T("%s"), it_tip->report_date.c_str());
		m_Grid.SetItem(&Item);

		Item.col = 1;
		Item.strText.Format(_T("%s"), it_tip->id.c_str());		//	技术分析 指标名称
		m_Grid.SetItem(&Item);

		Item.col = 2;
		Item.strText.Format(_T("%s"), it_tip->title.c_str());	//	事件基本描述
		if (!m_Grid.SetCellType(nRow + 1, 2, RUNTIME_CLASS(CGridURLCell)))
			return ;
		Item.nFormat &= ~DT_RIGHT;
		Item.nFormat |= DT_CENTER;
		m_Grid.SetItem(&Item);

		Item.col = 3;	
		Item.strText.Format(_T("%s"), it_tip->url.c_str());		//	扩展描述
		m_Grid.SetItem(&Item);
	}


	m_Grid.AutoSize();
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	m_Grid.SetColumnWidth(2, 160);


	//////////////////////////////////////////////////////////////////////////
	//	设置标题
	
	m_ChartCtrl.EnableRefresh(false);
	CRect rect = m_ChartCtrl.GetPlottingRect();

	m_ChartCtrl.RemoveAllSeries();

	//m_mapTip = pCse->tip;

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	if(pCse->sj_code.size()>0)
	{

		CChartDateTimeAxis* pBottomAxis = m_ChartCtrl.CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		//pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		map<string, CChartCandlestickSerie*>	mapLineSerie;

		for(long i=0; i<  pCse->sj_code.size(); i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->sj_code[i].c_str());

			/*if(i==0)	xmin = odt.m_dt;
			if(i== pCse->sj_code.size()-1) xmax = odt.m_dt;*/

			/*绘制不同的统计曲线 */
			string serieName = pCse->zb_name[i] + " " + pCse->unit[i];
			if(mapLineSerie.count(serieName)==0)
			{
				mapLineSerie[serieName] = m_ChartCtrl.CreateCandlestickSerie();
				mapLineSerie[serieName]->EnableMouseNotifications(false,false);
				mapLineSerie[serieName]->SetColor(RGB(0,164,64));
			}

			/* isnan 判断 */
			if(pCse->data[i] == pCse->data[i])
			{
				double  close = pCse->data[i];
				double  open = pCse->data1[i];
				double  high = pCse->data2[i];
				double	low = pCse->data3[i];
				//mapLineSerie[serieName]->AddPoint(odt.m_dt, pCse->data[i]);
				mapLineSerie[serieName]->AddPoint(odt.m_dt, low, high, open, close);

			}			
		}

		//	给各条曲线打上名称
		map<string, CChartCandlestickSerie*>::iterator it = mapLineSerie.begin();
		for(; it!=mapLineSerie.end(); it++)
		{
			int cnt = it->second->GetPointsCount();
			string curveName;

			//mapLineSerie[it->first]->CreateBalloonLabel( RandomFromRange(0,  cnt-1), it->first);
			mapLineSerie[it->first]->SetName( it->first.c_str());

		}


		//////////////////////////////////////////////////////////////////////////

		ymin = 99999.0; ymax = 0.0;
		xmax = 0; xmin = 99999.0;
		m_ChartCtrl.GoToFirstSerie();
		CChartSerie*  pSerie;
		while((pSerie=m_ChartCtrl.GetNextSerie()))
		{
			string strName = pSerie->GetName();
			double yymin, yymax;
			pSerie->GetSerieYMinMax(yymin, yymax);
			if(yymax> ymax)  ymax = yymax;
			if(yymin< ymin)  ymin = yymin;

			double xxmin, xxmax;
			pSerie->GetSerieXMinMax(xxmin, xxmax);
			if(xxmax> xmax)  xmax = xxmax;
			if(xxmin< xmin)  xmin = xxmin;
		}
		pBottomAxis->SetMinMax(xmin, xmax);
		if(ymin>=0)
			pLeftAxis->SetMinMax(0, ymax * 1.05);
		else
			pLeftAxis->SetMinMax(ymin * 1.05, ymax * 1.05);

	}	

	m_ChartCtrl.EnableRefresh(true);
}

void CIndiTechnicalAnalysisRoadView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	//AfxMessageBox(str);
	//////////////////////////////////////////////////////////////////////////
	//	股票停靠栏显示当前选择的 股票

	if(pItem->iRow>0)
	{
		CString str = m_Grid.GetItemText(pItem->iRow, 0);

		COleDateTime odt ;
		odt.ParseDateTime(str);

		double xmax = odt.m_dt + 60;
		double xmin = odt.m_dt - 60;

		m_ChartCtrl.GetBottomAxis()->SetAutomatic(false);
		m_ChartCtrl.GetBottomAxis()->SetMinMax(xmin, xmax);
		m_ChartCtrl.GetBottomAxis()->EnableScrollBar(true);

		CRect rect = m_ChartCtrl.GetPlottingRect();
		int nWidth = (rect.right - rect.left)/120;
		if(nWidth <1) nWidth = 1;

		double ymax, ymin;
		m_ChartCtrl.GoToFirstSerie();
		CChartCandlestickSerie*  pSerie;
		while((pSerie=(CChartCandlestickSerie *)m_ChartCtrl.GetNextSerie()))
		{
			pSerie->SetWidth(nWidth);
			pSerie->GetYMinMaxForXMinMax(xmin, xmax, ymin, ymax);
		}

		if(ymax > ymin)
		{
			double span = ymax - ymin;
			ymax += span * 0.3;
			ymin -= span * 0.3;
			m_ChartCtrl.GetLeftAxis()->SetMinMax(ymin, ymax);
		}

	}
}
