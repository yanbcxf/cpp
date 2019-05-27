// BaseChartCtlView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "BaseChartCtlView.h"

#include "TipDialog.h"
#include "StockRangeSet.h"

//////////////////////////////////////////////////////////////////////////
//

void CBaseChartCtlViewMouseListener::OnMouseEventSeries(CChartMouseListener::MouseEvent mouseEvent, CPoint point,
						CChartSerieBase<SChartXYPoint>* pSerie, unsigned uPointIndex) 
{
	if(mouseEvent == CChartMouseListener::LButtonDoubleClick)
	{
		m_pView->m_nSelectedCurve = pSerie->GetSerieId();

		string name = pSerie->GetName();

		string strTip = m_pView->m_strTitle + "  " + name;
		m_pView->m_pStaticRightTip->SetWindowText(strTip.c_str());

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), name.c_str(), firstMatch )>0)
		{
			string::size_type pos1 = firstMatch[0].find(")");
			string codeStr = firstMatch[0].substr(1, pos1-1);
			m_pView->m_MessageCtrl.FocusStockOnLeftPane(codeStr.c_str());
		}
		else
			m_pView->m_MessageCtrl.FocusStockOnLeftPane(name.c_str());

		//	寻找该点附近是否有“气球提示”
		int uPt;
		if((uPt = pSerie->SearchPointWithCustomLabel(uPointIndex))>=0)
		{
			//AfxMessageBox("有气球提示");
			SChartXYPoint pt  = pSerie->GetPoint(uPt);
			COleDateTime dt;
			dt.m_dt = pt.GetX();

			CTipDialog dlg;
			COleDateTime odt1 = dt;
			COleDateTime odt2 = COleDateTime::GetCurrentTime();
			if(uPt<pSerie->GetPointsCount()-1)
				odt2 =  pSerie->GetPoint(uPt + 1).GetX();
			while(odt1 < odt2)
			{
				string strKey = pSerie->GetName() + "_" + odt1.Format("%Y-%m-%d").GetBuffer();
				list<CTipRecord>  lstTmp = m_pView->m_mapTip[strKey];
				dlg.m_lstTip.insert(dlg.m_lstTip.end(),  lstTmp.begin(), lstTmp.end());
				odt1 += 1.0;
			}
			dlg.DoModal();
		}
	}

	else if(mouseEvent == CChartMouseListener::LButtonDown)
	{
		//pSerie->SetWidth(pSerie->GetWidth() + 3);
		pSerie->EnableShadow(true);
	}
	else if(mouseEvent == CChartMouseListener::LButtonUp)
	{
		pSerie->EnableShadow(false);
	}
}


//////////////////////////////////////////////////////////////////////////
// CBaseChartCtlView

IMPLEMENT_DYNCREATE(CBaseChartCtlView, CBaseMessageView)

CBaseChartCtlView::CBaseChartCtlView()
{
	m_pChartCtrl = NULL;

	m_pButtonAll = NULL;
	m_pButton6Month = NULL;
	m_pButton1Year =  NULL;
	m_pButton3Year = NULL;
	m_pButtonMenu = NULL;

	m_pStaticRightTip = NULL;
	//m_pStaticLeftTip = NULL;

	m_bPagination = true;

	m_StartDate = -1.0;
	m_EndDate = -1.0;
}

CBaseChartCtlView::~CBaseChartCtlView()
{
	if (m_pChartCtrl)
		delete m_pChartCtrl;

	if (m_pButtonAll)
		delete m_pButtonAll;
	if (m_pButton6Month)
		delete m_pButton6Month;
	if (m_pButton1Year)
		delete m_pButton1Year;
	if (m_pButton3Year)
		delete m_pButton3Year;
	if (m_pButtonMenu)
		delete m_pButtonMenu;

	/*if(m_pStaticLeftTip)
		delete m_pStaticLeftTip;*/
	if(m_pStaticRightTip)
		delete m_pStaticRightTip;
}

BEGIN_MESSAGE_MAP(CBaseChartCtlView, CBaseMessageView)
	ON_WM_SIZE()
	ON_COMMAND(ID_SET_STOCK_RANGE, &CBaseChartCtlView::OnSetStockRange)
	ON_WM_CREATE()

	ON_BN_CLICKED(IDC_BTN_CHART_ALL, OnBtnAll)
	ON_BN_CLICKED(IDC_BTN_CHART_6MONTH, OnBtn6Month)
	ON_BN_CLICKED(IDC_BTN_CHART_1YEAR, OnBtn1Year)
	ON_BN_CLICKED(IDC_BTN_CHART_3YEAR, OnBtn3Year)
	ON_BN_CLICKED(IDC_BTN_CHART_MENU, OnBtnChartMenu)
	ON_STN_DBLCLK(IDC_STATIC_CHART_RIGHT_TIP, OnStnDblclickSaticRightTip)
	/*IDC_STATIC_CHART_RIGHT_TIP*/
END_MESSAGE_MAP()


// CBaseChartCtlView 绘图

void CBaseChartCtlView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CBaseChartCtlView 诊断

#ifdef _DEBUG
void CBaseChartCtlView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseChartCtlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaseChartCtlView 消息处理程序

void CBaseChartCtlView::OnSize(UINT nType, int cx, int cy)
{
	CBaseMessageView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (m_pChartCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		rect.top += m_bPagination ? 30 : 0;
		m_pChartCtrl->MoveWindow(rect);
	}

	if (m_bPagination && m_pStaticRightTip->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		int left = 101 + 4 * 71; 
		rect.left = left;
		rect.right = rect.right;
		rect.top = 2;
		rect.bottom = 28;
		m_pStaticRightTip->MoveWindow(rect);
	}

}

void CBaseChartCtlView::OnInitialUpdate()
{
	CBaseMessageView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	

}




void CBaseChartCtlView::OnSetStockRange()
{
	// TODO: 在此添加命令处理程序代码
	CStockRangeSet aboutDlg;
	if(m_StartDate>0 && m_EndDate> m_StartDate)
	{
		aboutDlg.m_StartDate.m_dt = m_StartDate;
		aboutDlg.m_End_Date.m_dt = m_EndDate;
	}
	aboutDlg.DoModal();

	m_StartDate = aboutDlg.m_StartDate;
	m_EndDate = aboutDlg.m_End_Date;
	//m_strViewType = aboutDlg.m_strViewType;

	RedrawDmGraph();
	
}

/*void CBaseChartCtlView::ProcessXmlWParam(string msgType, string code,string model,\
											  string param1,string param2,string param3)
{
	RedrawDmGraph();
}
*/



void CBaseChartCtlView::ProcessXmlWParam(string msgType,string code,string model,\
											string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawChart(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
	else if(msgType=="ReadFinished")
		RedrawDmGraph();
}



int CBaseChartCtlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseMessageView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (m_pChartCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pChartCtrl = new CChartCtrl;
		if (!m_pChartCtrl) return -1;

		// Create the Gridctrl window
		CRect rect;
		GetClientRect(rect);
		rect.top += m_bPagination ? 30 : 0;
		m_pChartCtrl->Create( this, rect, IDC_CHART_CTL);

		m_pChartCtrl->SetBackColor(RGB(220,220,220));
		m_pChartCtrl->GetLegend()->SetVisible(false);
	}

	if(1)
	{
		LOGFONT   logfont;   
		lstrcpy((LPSTR)logfont.lfFaceName,(LPSTR)"楷体_GB2312");   
		logfont.lfWeight=FW_THIN;   
		logfont.lfWidth=6;   
		logfont.lfHeight=12;   
		logfont.lfEscapement=0;   
		logfont.lfUnderline=FALSE;   
		logfont.lfItalic=FALSE;   
		logfont.lfStrikeOut=FALSE;   
		logfont.lfCharSet=GB2312_CHARSET;       //以上参数好像一个都不能少
		m_editFont.CreateFontIndirect(&logfont); 

		m_pChartCtrl->SetFont(&m_editFont);
	}


	int left = 0;
	short	shBtnColor = 30;
	if(m_pButtonMenu==NULL && m_bPagination)
	{
		CRect rect;
		GetClientRect(rect);
		//int left = (rect.right - rect.left)/8 * 2; 
		m_pButtonMenu = new CButtonST();
		m_pButtonMenu->Create("删除曲线", WS_CHILD | BS_DEFPUSHBUTTON,CRect(left,2, left + 100,28),this,IDC_BTN_CHART_MENU);
		m_pButtonMenu->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_pButtonMenu->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
		m_pButtonMenu->SetTooltipText(_T("删除曲线"));

		m_pButtonMenu->SetFont(&m_editFont); // 设置新字体
		m_pButtonMenu->ShowWindow(SW_SHOWNORMAL);
	}


	left +=101;
	if(m_pButtonAll == NULL && m_bPagination)
	{
		CRect rect;
		GetClientRect(rect);
		//int left = (rect.right - rect.left)/8 * 4; 
		m_pButtonAll = new CButtonST();
		m_pButtonAll->Create("全部", WS_CHILD | BS_DEFPUSHBUTTON,CRect(left,2, left + 70,28),this,IDC_BTN_CHART_ALL);
		m_pButtonAll->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_pButtonAll->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));
		m_pButtonAll->SetTooltipText(_T("全部"));

		m_pButtonAll->SetFont(&m_editFont); // 设置新字体
		m_pButtonAll->ShowWindow(SW_SHOWNORMAL);
	}

	

	left +=71;
	if(m_pButton6Month == NULL && m_bPagination)
	{
		CRect rect;
		GetClientRect(rect);
		//int left = (rect.right - rect.left)/8 * 6; 
		m_pButton6Month = new CButtonST();
		m_pButton6Month->Create("最近6个月", WS_CHILD | BS_DEFPUSHBUTTON,CRect(left,2, left + 70,28),this,IDC_BTN_CHART_6MONTH);
		m_pButton6Month->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_pButton6Month->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

		m_pButton6Month->SetFont(&m_editFont); // 设置新字体
		m_pButton6Month->ShowWindow(SW_SHOWNORMAL);
	}

	left +=71;
	if(m_pButton1Year == NULL && m_bPagination)
	{
		CRect rect;
		GetClientRect(rect);
		//int left = (rect.right - rect.left)/8 * 7; 
		m_pButton1Year = new CButtonST();
		m_pButton1Year->Create("最近1年", WS_CHILD | BS_DEFPUSHBUTTON,CRect(left,2, left + 70,28),this,IDC_BTN_CHART_1YEAR);
		m_pButton1Year->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_pButton1Year->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

		m_pButton1Year->SetFont(&m_editFont); // 设置新字体
		m_pButton1Year->ShowWindow(SW_SHOWNORMAL);
	}

	left +=71;
	if(m_pButton3Year == NULL && m_bPagination)
	{
		CRect rect;
		GetClientRect(rect);
		//int left = (rect.right - rect.left)/8 * 5; 
		m_pButton3Year = new CButtonST();
		m_pButton3Year->Create("最近3年", WS_CHILD | BS_DEFPUSHBUTTON,CRect(left,2, left + 70,28),this,IDC_BTN_CHART_3YEAR);
		m_pButton3Year->OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_pButton3Year->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

		m_pButton3Year->SetFont(&m_editFont); // 设置新字体
		m_pButton3Year->ShowWindow(SW_SHOWNORMAL);
	}

	left +=71;
	if(m_pStaticRightTip==NULL && m_bPagination)
	{
		CRect rect;
		GetClientRect(rect);
		//int left = (rect.right - rect.left)/8 * 2; 
		m_pStaticRightTip = new CStatic();
		m_pStaticRightTip->Create("", WS_CHILD | SS_CENTERIMAGE | ES_CENTER, CRect(left,2, rect.right, 28),this,IDC_STATIC_CHART_RIGHT_TIP);

		m_pStaticRightTip->SetFont(&m_editFont); // 设置新字体
		m_pStaticRightTip->ShowWindow(SW_SHOWNORMAL);

		/*BOOL ModifyStyleEx( 
			DWORD dwRemove, 
			DWORD dwAdd, 
			UINT nFlags=0 
			); */
		m_pStaticRightTip->ModifyStyle(0,SS_NOTIFY);   //Add
	}

	return 0;
}

// 查找在当前显示区域的曲线
vector<string>  CBaseChartCtlView::GetVisibleSeries()
{
	vector<string> vecSeries;
	int cnt = m_pChartCtrl->GetSeriesCount();
	if(cnt>0)
	{
		CRect plottingRect = m_pChartCtrl->GetPlottingRect();
		m_pChartCtrl->GoToFirstSerie();
		CChartSerie*  pSerie;
		while((pSerie=m_pChartCtrl->GetNextSerie()))
		{
			unsigned uFirst=0, uLast=0;
			if (!pSerie->GetVisiblePoints1(uFirst,uLast))
				continue;

			if (uFirst>0)
				uFirst--;
			if (uLast<pSerie->GetPointsCount()-1)
				uLast++;
			if (uLast-uFirst < 1)
				continue;

			for(int k = uFirst; k< uLast; k++)
			{
				CPoint screenPt =pSerie->GetPointScreenCoord(k);
				if(plottingRect.PtInRect(screenPt))
				{
					vecSeries.push_back(pSerie->GetName());
					break;
				}
			}
		}
	}
	
	return vecSeries;
}

void CBaseChartCtlView::OnBtnChartMenu()
{
	ProccessChartMenu();

	if(m_nSelectedCurve >= 0)
	{
		m_pChartCtrl->EnableRefresh(false);

		//	删除选中的曲线
		m_pChartCtrl->RemoveSerie(m_nSelectedCurve);

		// 重新确定 Y 轴的显示区域
		double ymin, ymax, xmin, xmax;
		ymin = 99999.0; ymax = 0.0;
		xmax = 0; xmin = 99999.0;
		m_pChartCtrl->GoToFirstSerie();
		CChartSerie*  pSerie;
		while((pSerie=m_pChartCtrl->GetNextSerie()))
		{
			double yymin, yymax;
			pSerie->GetSerieYMinMax(yymin, yymax);
			if(yymax> ymax)  ymax = yymax;
			if(yymin< ymin)  ymin = yymin;

		}
		m_pChartCtrl->GetLeftAxis()->SetMinMax(0, ymax * 1.10);
		m_pChartCtrl->EnableRefresh(true);
	}

	
}

void CBaseChartCtlView::OnBtnAll()
{

	if(m_pChartCtrl->GetBottomAxis())
	{
		m_pChartCtrl->GetBottomAxis()->SetAutomatic(true);
		m_pChartCtrl->GetBottomAxis()->EnableScrollBar(false);
	}
	
	PostButtonClicked(0);
	m_pChartCtrl->RefreshCtrl();
}



void CBaseChartCtlView::OnBtn6Month()
{
	COleDateTime odt ;
	char tmp[256] = { 0 };
	time_t time_end = CBussinessUtils::CurrentTradingDate(0);
	time_t time_begin = time_end - 180 * 24 * 3600;

	/*struct tm * curr_tm = gmtime (&time_end);*/
	struct tm curr_tm;
	localtime_s(&curr_tm, &time_end);
	snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	odt.ParseDateTime(tmp);
	double xmax = odt.m_dt;

	/*curr_tm = gmtime (&time_begin);*/
	localtime_s(&curr_tm, &time_begin);
	snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	odt.ParseDateTime(tmp);
	double xmin = odt.m_dt;

	if(m_pChartCtrl->GetBottomAxis())
	{
		m_pChartCtrl->GetBottomAxis()->SetAutomatic(false);
		m_pChartCtrl->GetBottomAxis()->SetMinMax(xmin, xmax);
		m_pChartCtrl->GetBottomAxis()->EnableScrollBar(true);
	}
		
	PostButtonClicked(183);
	m_pChartCtrl->RefreshCtrl();

}


void CBaseChartCtlView::OnBtn1Year()
{
	COleDateTime odt ;
	char tmp[256] = { 0 };
	time_t time_end = CBussinessUtils::CurrentTradingDate(0);
	time_t time_begin = time_end - 365 * 24 * 3600;

	/*struct tm * curr_tm = gmtime (&time_end);*/
	struct tm curr_tm;
	localtime_s(&curr_tm, &time_end);
	snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	odt.ParseDateTime(tmp);
	double xmax = odt.m_dt;

	/*curr_tm = gmtime (&time_begin);*/
	localtime_s(&curr_tm, &time_begin);
	snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	odt.ParseDateTime(tmp);
	double xmin = odt.m_dt;

	if(m_pChartCtrl->GetBottomAxis())
	{
		m_pChartCtrl->GetBottomAxis()->SetAutomatic(false);
		m_pChartCtrl->GetBottomAxis()->SetMinMax(xmin, xmax);
		m_pChartCtrl->GetBottomAxis()->EnableScrollBar(true);
	}
	
	PostButtonClicked(365);
	m_pChartCtrl->RefreshCtrl();

}



void CBaseChartCtlView::OnBtn3Year()
{
	//m_pChartCtrl->GetBottomAxis()->SetMinMax();

	COleDateTime odt ;
	char tmp[256] = { 0 };
	time_t time_end = CBussinessUtils::CurrentTradingDate(0);
	time_t time_begin = time_end - 90 * 24 * 3600;

	/*struct tm * curr_tm = gmtime (&time_end);*/
	struct tm curr_tm;
	localtime_s(&curr_tm, &time_end);
	snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	odt.ParseDateTime(tmp);
	double xmax = odt.m_dt;

	/*curr_tm = gmtime (&time_begin);*/
	localtime_s(&curr_tm, &time_begin);
	snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	odt.ParseDateTime(tmp);
	double xmin = odt.m_dt;

	m_pChartCtrl->GetBottomAxis()->SetAutomatic(false);
	m_pChartCtrl->GetBottomAxis()->SetMinMax(xmin, xmax);
	m_pChartCtrl->GetBottomAxis()->EnableScrollBar(true);

	PostButtonClicked(90);
	m_pChartCtrl->RefreshCtrl();

}

void CBaseChartCtlView::OnStnDblclickSaticRightTip()
{
	CString strName;
	m_pStaticRightTip->GetWindowText(strName);
	string name = strName.GetBuffer();

	vector<string> firstMatch;
	string strPattern = string("\\([A-Za-z\\d]+\\)");
	if(Pcre2Grep(strPattern.c_str(), name.c_str(), firstMatch )>0)
	{
		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		m_MessageCtrl.FocusStockOnLeftPane(codeStr.c_str());
	}
	else
		m_MessageCtrl.FocusStockOnLeftPane(name.c_str());
}

void CBaseChartCtlView::RedrawChart(CCalculateTask * cse)
{
	CStockApp * pApp = (CStockApp *)AfxGetApp();

	m_nSelectedCurve= -1;

	m_pChartCtrl->EnableRefresh(false);

	CRect rect = m_pChartCtrl->GetPlottingRect();

	CCalculateTask * pCse =  (CCalculateTask *)cse;

	//	设置标题
	CString strTitle;
	strTitle.Format("(%s) %s", pCse->title_code.c_str(), pCse->title_name.c_str());
	m_strTitle = strTitle.GetBuffer();
	m_pStaticRightTip->SetWindowText(strTitle);

	m_pChartCtrl->RemoveAllSeries();

	m_mapTip = pCse->tip;

	double xmin, xmax, ymin, ymax;
	xmax = 0; xmin = 99999.0;
	ymin = 99999.0; ymax = 0.0;

	if(pCse->sj_code.size()>0)
	{

		CChartDateTimeAxis* pBottomAxis = m_pChartCtrl->CreateDateTimeAxis(CChartCtrl::BottomAxis);
		pBottomAxis->SetTickLabelFormat(false, "%Y%m%d");
		//pBottomAxis->SetMinMax(xmin, xmax);

		CChartStandardAxis* pLeftAxis = m_pChartCtrl->CreateStandardAxis(CChartCtrl::LeftAxis);

		//pLeftAxis->GetLabel()->SetText("投资者数（万）");
		//pLeftAxis->SetMinMax(0, ymax);

		bool bSecondHorizAxis = false;
		bool bSecondVertAxis = false;

		//////////////////////////////////////////////////////////////////////////

		map<string, CChartLineSerie*>	mapLineSerie;

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
				mapLineSerie[serieName] = m_pChartCtrl->CreateLineSerie();
				mapLineSerie[serieName]->EnableMouseNotifications(true,false);
				mapLineSerie[serieName]->RegisterMouseListener(new CBaseChartCtlViewMouseListener(this));
			}

			/* isnan 判断 */
			if(pCse->data[i] == pCse->data[i])
				mapLineSerie[serieName]->AddPoint(odt.m_dt, pCse->data[i]);

			/*if(pCse->data[i]> ymax)  ymax = pCse->data[i];
			if(pCse->data[i]< ymin)  ymin = pCse->data[i];*/

			/* 添加某些点的“气球提示”,周末或者停牌期的公告体现在前一个交易日 */
			COleDateTime odt1 = odt;
			COleDateTime odt2 = COleDateTime::GetCurrentTime();
			if(i<pCse->sj_code.size()-1)
				odt2.ParseDateTime(pCse->sj_code[i+1].c_str());
			string strTip = pCse->sj_code[i];
			while(odt1 < odt2)
			{
				string sjcode = odt1.Format("%Y-%m-%d");
				string strKey = serieName + "_" + sjcode;
				if(m_mapTip.count(strKey)>0)
				{
					TipContent(serieName, sjcode, strTip);
					mapLineSerie[serieName]->CreateBalloonLabel( mapLineSerie[serieName]->GetPointsCount() -1,strTip);
				}
				odt1.m_dt += 1.0; 
			}
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
		m_pChartCtrl->GoToFirstSerie();
		CChartSerie*  pSerie;
		while((pSerie=m_pChartCtrl->GetNextSerie()))
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
		
		/*if(ymax>0 && xmax > xmin)
		{
			pBottomAxis->SetMinMax(xmin, xmax);
			pLeftAxis->SetMinMax(0, ymax * 1.10);

			m_pChartCtrl->SetBackColor(RGB(220,220,220));
			m_pChartCtrl->GetLegend()->SetVisible(false);
			m_pChartCtrl->RefreshCtrl();
		}*/

	}	

	m_pChartCtrl->EnableRefresh(true);
}