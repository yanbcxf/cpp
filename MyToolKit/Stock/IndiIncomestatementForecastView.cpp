// PlateIndustryDynamicPeView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndiIncomestatementForecastView.h"
#include "BaseMessageFormView.h"
#include "CalcIncomestatementForecast.h"

#include "BaseChartCtlView.h"

// CPlateIndustryDynamicPeView

IMPLEMENT_DYNCREATE(CIndiIncomestatementForecastView, CBaseMessageFormView)

CIndiIncomestatementForecastView::CIndiIncomestatementForecastView()
{

}

CIndiIncomestatementForecastView::~CIndiIncomestatementForecastView()
{
}

BEGIN_MESSAGE_MAP(CIndiIncomestatementForecastView, CBaseMessageFormView)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
END_MESSAGE_MAP()


// CIndiAddIssuranceRoadView 绘图

void CIndiIncomestatementForecastView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CPlateIndustryDynamicPeView 诊断

#ifdef _DEBUG
void CIndiIncomestatementForecastView::AssertValid() const
{
	CBaseMessageFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndiIncomestatementForecastView::Dump(CDumpContext& dc) const
{
	CBaseMessageFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlateIndustryDynamicPeView 消息处理程序



void CIndiIncomestatementForecastView::OnInitialUpdate()
{
	CBaseMessageFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	

	RedrawDmGraph();
}

void CIndiIncomestatementForecastView::RedrawDmGraph()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcIncomestatementForecast * cse =  new CCalcIncomestatementForecast(m_hWnd, WM_USER_LOG_1);
	cse->m_vec_request_code.push_back( pDoc->m_nCode );
	cse->m_str_request_zb_code = pDoc->m_strInput;

	cse->ScheduleTask(&CCalcIncomestatementForecast::Execute1);
}



string CIndiIncomestatementForecastView::ExtractKeyword(string title)
{
	string strTip1 = "";
	if(title.find("预案")!=string::npos )	return "预案";
	if(title.find("证监会受理")!=string::npos )	return "证监会受理";
	if(title.find("证监会核准")!=string::npos )	return "证监会核准";
	if(title.find("核准通过")!=string::npos )	return "核准通过";
	if(title.find("审核通过")!=string::npos )	return "审核通过";
	if(title.find("申请中止")!=string::npos )	return "申请中止";
	if(title.find("停牌")!=string::npos )	return "停牌";
	if(title.find("股票激励")!=string::npos )	return "股票激励";
	if(title.find("合同")!=string::npos )	return "合同";
	if(title.find("中标")!=string::npos )	return "中标";
	
	return "";
						
}


void CIndiIncomestatementForecastView::RedrawChart(CCalculateTask * cse)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	CCalculateTask * pCse =  (CCalculateTask *)cse;

	//////////////////////////////////////////////////////////////////////////
	//	对现在数据进行汇总，以便表格显示

	map<string , map<string, double>>	map_Cell;
	map<string, string>		map_Row;
	vector<string>		vec_Col;
	vector<string>		vec_Col_Unit;
	vector<string>		vec_Row;
	vector<string>		vec_Row_Code;
	for(int i=0; i< pCse->zb_name.size(); i++)
	{
		map_Cell[pCse->zb_name[i]][pCse->sj_code[i]] = pCse->data[i];
		//map_Col[pCse->sj_code[i]] = pCse->sj_code[i];
		map_Row[pCse->zb_code[i]] = pCse->zb_name[i];

		vector<string>::iterator it = vec_Col.begin();
		for(; it!=vec_Col.end(); it++)
		{
			if(*it==pCse->sj_code[i])
				break;
		}
		if(it==vec_Col.end())
		{
			vec_Col.insert(vec_Col.end(), pCse->sj_code[i]);

			if(pCse->unit[i].empty())
				vec_Col_Unit.insert(vec_Col_Unit.end(), pCse->sj_code[i]);
			else
				vec_Col_Unit.insert(vec_Col_Unit.end(), pCse->sj_code[i] + "(" + pCse->unit[i] + ")");
		}
	}

	/* 按“列名称” 反向排序, 以便显示时最近的时间在最左边 */
	std::stable_sort(vec_Col.begin(), vec_Col.end()); 
	std::stable_sort(vec_Col_Unit.begin(), vec_Col_Unit.end()); 
	std::reverse(vec_Col.begin(), vec_Col.end());
	std::reverse(vec_Col_Unit.begin(), vec_Col_Unit.end()); 
	
	map<string,string>::iterator it = map_Row.begin();
	for(; it!=map_Row.end(); it++)
	{
		vec_Row.push_back(it->second);
		vec_Row_Code.push_back(it->first);
	}

	if( map_Cell.size()>0)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_Grid.SetRowCount(vec_Row.size() + 1);
			m_Grid.SetColumnCount(vec_Col.size() + 3);		//	额外增加三列 ： 序号/代码/名称
			m_Grid.SetFixedRowCount(1);
			m_Grid.SetFixedColumnCount(3);
			m_Grid.SetHeaderSort(TRUE);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}

		for (int row = 0; row < m_Grid.GetRowCount(); row++)
		{
			for (int col = 0; col < m_Grid.GetColumnCount(); col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;
				Item.col = col;
				string val;

				if (row < 1) {
					Item.nFormat = DT_LEFT|DT_WORDBREAK;

					if(col==0)		
						val = pCse->title_name;
					else if(col==1)
					{
						val = "编码";
					}
					else if(col==2)
					{
						val = "名称";
					}
					else
					{
						val = vec_Col_Unit[col-3];
						/*if(vec_Col_Unit[col-3].empty()==false)
							val += "(" + vec_Col_Unit[col-3] + ")";*/

					}
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					if(col <= 2 )
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					if( col> 2  || col==0)
					{
						if (!m_Grid.SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
							return;
					}

					if(col==0)
					{
						val = Int2String(row);
					}
					else if(col==1)
					{
						val = vec_Row_Code[row-1];
					}
					else if(col==2)
					{
						val = vec_Row[row-1];
					}
					else
					{
						string zb_name = vec_Row[row-1];
						string sj_code = vec_Col[col-3];

						if(map_Cell[zb_name].count(sj_code) >0)
						{
							double db = map_Cell[zb_name][sj_code];
							if(_isnan(db)) val = "";
							else
							{
								val = String2Currency(Double2String(db));
								Item.mask    |= (GVIF_FGCLR);
								if(db >0)
								{
									//Item.crFgClr = RGB(255, 64, 64);
									Item.crFgClr = RGB(0, 0, 0);
								}
								else
									Item.crFgClr = RGB(0, 100, 0);

								/*if(vec_Col_Unit[col-3].find("(预)")!=string::npos)
								{
									Item.crBkClr = RGB( 236, 196,  196);
									Item.mask |= GVIF_BKCLR;
								}*/

								if(sj_code.find("12-31")!=string::npos)
								{
									// 年度报表，则改变背景色
									Item.crBkClr = RGB(196, 236, 196);
									Item.mask |= GVIF_BKCLR;
								}


								
							}
						}
						else 
							val = "";
					}

					Item.strText.Format(_T("%s"), val.c_str());

				}
				m_Grid.SetItem(&Item);
			}
			int k = 1;
		}

		m_Grid.AutoSize();
		//m_pGridCtrl->SetColumnWidth(2, 80);
	}

	//////////////////////////////////////////////////////////////////////////
	//	设置标题
	
	m_ChartCtrl.EnableRefresh(false);
	m_ChartCtrl.SetBackColor(RGB(166, 166, 166));
	CRect rect = m_ChartCtrl.GetPlottingRect();


	//	设置标题
	CString strTitle;
	strTitle.Format("(%s) %s", pCse->title_code.c_str(), pCse->title_name.c_str());
	/*m_strTitle = strTitle.GetBuffer();
	m_pStaticRightTip->SetWindowText(strTitle);*/

	m_ChartCtrl.RemoveAllSeries();

	

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	if(pCse->sj_code.size()>0)
	{

		CChartDateTimeAxis* pBottomAxis = m_ChartCtrl.CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");

		CChartStandardAxis* pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		map<string, CChartLineSerie*>	mapLineSerie;

		for(long i=0; i<  pCse->sj_code.size(); i++)
		{
			COleDateTime odt ;
			double yy = 0.0;
			odt.ParseDateTime(pCse->sj_code[i].c_str());

			/*绘制不同的统计曲线 */
			string serieName = pCse->zb_name[i] ;
			if(mapLineSerie.count(serieName)==0)
			{
				mapLineSerie[serieName] =m_ChartCtrl.CreateLineSerie();
				/*mapLineSerie[serieName]->EnableMouseNotifications(true,false);
				mapLineSerie[serieName]->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));*/
			}

			/* isnan 判断 */
			if(pCse->data[i] == pCse->data[i])
				mapLineSerie[serieName]->AddPoint(odt.m_dt, pCse->data[i]);

			/*if(pCse->data[i]> ymax)  ymax = pCse->data[i];
			if(pCse->data[i]< ymin)  ymin = pCse->data[i];*/

			
		}

		//	给各条曲线打上名称
		map<string, CChartLineSerie*>::iterator it = mapLineSerie.begin();
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

	m_ChartCtrl.GetLegend()->SetVisible(true);
	m_ChartCtrl.EnableRefresh(true);
}



void CIndiIncomestatementForecastView::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	//AfxMessageBox(str);
	//////////////////////////////////////////////////////////////////////////
	//	股票停靠栏显示当前选择的 股票

	if(pItem->iRow>0)
	{
		m_ChartCtrl.EnableRefresh(false);
		CString str = m_Grid.GetItemText(pItem->iRow, 2);

		double ymax, ymin;
		m_ChartCtrl.GoToFirstSerie();
		CChartLineSerie*  pSerie;
		while((pSerie=(CChartLineSerie *)m_ChartCtrl.GetNextSerie()))
		{
			string strName = pSerie->GetName();
			if(strName!=str.GetBuffer())
			{
				pSerie->SetVisible(false);
			}
			else
			{
				pSerie->SetVisible(true);
				pSerie->GetSerieYMinMax(ymin,ymax);
			}
		}

		if(ymax > ymin)
		{
			double span = ymax - ymin;
			ymax += span * 0.03;
			ymin -= span * 0.03;
			m_ChartCtrl.GetLeftAxis()->SetMinMax(ymin, ymax);
		}
		m_ChartCtrl.EnableRefresh(true);

	}
}
