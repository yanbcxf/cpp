// EntrepreneurView.cpp : 实现文件
//

#include "stdafx.h"

#include "TechAnalysisEvent.h"

#include "Stock.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "StockDoc.h"
#include "StockView.h"

#include "CsvDocument.h"
#include "CsvView.h"

#include "DatabaseDoc.h"
#include "DatabaseView.h"

#include "AutoDownloadDoc.h"
#include "AutoDownloadView.h"

#include "StockInfoView.h"

#include "IndividualShareDoc.h"
#include "IndividualShareView.h"
#include "IndividualSummaryView.h"
#include "IndividualCostView.h"
#include "IndividualSzsePeView.h"
#include "IndividualVolumeView.h"
#include "IndividualYahooPeView.h"
#include "IndividualNetProfitsView.h"
#include "IndividualCurrentAssetView.h"
#include "IndividualNonCurrentAssetView.h"
#include "IndividualCapitalStructView.h"
#include "IndividualCsvFileView.h"
#include "IndividualCapitalFlowView.h"
#include "IndiCashFlowSummaryView.h"
#include "IndiZombieAnalysisView.h"
#include "IndiFundHoldValueView.h"
#include "IndiMacdView.h"
#include "IndiMainIncomeGrowthView.h"
#include "IndiMcstView.h"
#include "Indi10jqkaBonusView.h"
#include "IndiHexunResearchPapaerView.h"
#include "IndiMarketIndexAnalysisView.h"
#include "IndiCsindexView.h"
#include "IndiGrossProfitRateView.h"
#include "IndiCninfoAnnouncementRoadView.h"
#include "IndiPeAndValueDailyView.h"
#include "IndiIncomestatementForecastView.h"
#include "IndiDmiView.h"
#include "IndiTechnicalAnalysisRoadView.h"
#include "SimulatedAccountListView.h"

#include "BaseChartCtlView.h"

#include "AutoDownloadView.h"

#include "SelfSelectPlateDlg.h"
#include "FindStockByCode.h"
#include "FindStockByPinyin.h"

class CStockInfoViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CStockInfoViewMenuButton)

public:
	CStockInfoViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CStockInfoViewMenuButton, CMFCToolBarMenuButton, 1)

// CStockInfoView

IMPLEMENT_DYNAMIC(CStockInfoView, CDockablePane)

string CStockInfoView::m_strSelectedCode = "";
string CStockInfoView::m_strSelectedItemText = "";

CStockInfoView::CStockInfoView()
{
}

CStockInfoView::~CStockInfoView()
{
}


BEGIN_MESSAGE_MAP(CStockInfoView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_COMMAND(ID_STOCKINFO_OPEN, &CStockInfoView::OnStockinfoOpen)
	ON_COMMAND(ID_INDIVIDUAL_SUMMARY, &CStockInfoView::OnIndividualSummary)
	ON_COMMAND(ID_OVERALL_COST, &CStockInfoView::OnOverallCost)
	ON_COMMAND(ID_INDIVIDUAL_SZSE_PE, &CStockInfoView::OnIndividualSzsePe)
	ON_COMMAND(ID_INDIVIDUAL_VOLUME, &CStockInfoView::OnIndividualVolume)
	ON_COMMAND(ID_INDIVIDUAL_NET_PROFITS, &CStockInfoView::OnIndividualNetProfits)
	ON_COMMAND(ID_BALANCE_CURRENT_ASSET, &CStockInfoView::OnBalanceCurrentAsset)
	ON_COMMAND(ID_BALANCE_NON_CURRENT_ASSET, &CStockInfoView::OnBalanceNonCurrentAsset)
	ON_COMMAND(ID_DOWNLOAD_INDIVIDUAL, &CStockInfoView::OnDownloadIndividual)
	ON_COMMAND(ID_INDIVIDUAL_CAPITAL_STRUCT, &CStockInfoView::OnIndividualCapitalStruct)
	ON_COMMAND(ID_INDIVIDUAL_CSV_INCOME, &CStockInfoView::OnIndividualCsvIncome)
	ON_COMMAND(ID_INDIVIDUAL_CSV_TRADING, &CStockInfoView::OnIndividualCsvTrading)
	ON_COMMAND(ID_INDIVIDUAL_CSV_BALANCE, &CStockInfoView::OnIndividualCsvBalance)
	ON_COMMAND(ID_INDIVIDUAL_CAPITAL_FLOW, &CStockInfoView::OnIndividualCapitalFlow)
	ON_COMMAND(ID_INDIVIDUAL_CAPITALFLOW_VALUE, &CStockInfoView::OnIndividualCapitalflowValue)
	ON_COMMAND(ID_CASHFLOW_SUMMARY, &CStockInfoView::OnCashflowSummary)
	ON_COMMAND(ID_INDIVIDUAL_CSV_CASHFLOWS, &CStockInfoView::OnIndividualCsvCashflows)
	ON_COMMAND(ID_INDIVIDUAL_ADD_TO_SELF_SELECT, &CStockInfoView::OnIndividualAddToSelfSelect)
	ON_COMMAND(ID_INDIVIDUAL_CSV_FUND_HOLD, &CStockInfoView::OnIndividualCsvFundHold)
	ON_COMMAND(ID_FIND_STOCK_BY_CODE, &CStockInfoView::OnFindStockByCode)
	ON_COMMAND(ID_INDIVIDUAL_ZOMBIE_ANALYSIS, &CStockInfoView::OnIndividualZombieAnalysis)
	ON_COMMAND(ID_INDIVIDUAL_FUND_HOLD_VALUE_ANALYSIS, &CStockInfoView::OnIndividualFundHoldValueAnalysis)
	ON_COMMAND(ID_INDIVIDUAL_MACD, &CStockInfoView::OnIndividualMacd)
	ON_COMMAND(ID_INDI_MAIN_INCOME_GROWTH, &CStockInfoView::OnIndiMainIncomeGrowth)
	ON_COMMAND(ID_INDIVIDUAL_MCST, &CStockInfoView::OnIndividualMcst)
	ON_COMMAND(ID_INDI_10JQKA_BONUS, &CStockInfoView::OnIndi10jqkaBonus)
	ON_COMMAND(ID_INDI_HEXUN_RESEARCH_PAPER_EPS, &CStockInfoView::OnIndiHexunResearchPaperEps)
	ON_COMMAND(ID_INDI_MARKET_VALUE_INDEX_ANALYSIS, &CStockInfoView::OnIndiMarketValueIndexAnalysis)
	ON_COMMAND(ID_INDI_CSINDEX_ANALYSIS, &CStockInfoView::OnIndiCsindexAnalysis)
	ON_COMMAND(ID_INDI_GROSS_PROFIT_RATE, &CStockInfoView::OnIndiGrossProfitRate)
	ON_COMMAND(ID_INDI_ADD_INSURANCE_ROADMAP, &CStockInfoView::OnIndiAddInsuranceRoadmap)
	ON_COMMAND(ID_INDI_IMPORTANT_EVENT_ROADMAP, &CStockInfoView::OnIndiImportantEventRoadmap)
	ON_COMMAND(ID_INDI_QUARTER_REPORT_ROADMAP, &CStockInfoView::OnIndiQuarterReportRoadmap)
	ON_COMMAND(ID_INDI_TRADE_ROADMAP, &CStockInfoView::OnIndiTradeRoadmap)
	ON_COMMAND(ID_INDI_SPECIAL_DISPOSAL_ROADMAP, &CStockInfoView::OnIndiSpecialDisposalRoadmap)
	ON_COMMAND(ID_INDI_SUPPLEMENT_CORRECTION_ROADMAP, &CStockInfoView::OnIndiSupplementCorrectionRoadmap)
	ON_COMMAND(ID_INDI_PE_AND_VALUE_DAILY, &CStockInfoView::OnIndiPeAndValueDaily)
	ON_COMMAND(ID_INDI_ORIGINAL_ISSUE_ROADMAP, &CStockInfoView::OnIndiOriginalIssueRoadmap)
	ON_COMMAND(ID_INDI_INCOMESTATEMENT_FORECAST, &CStockInfoView::OnIndiIncomestatementForecast)
	ON_COMMAND(ID_INDI_DMI, &CStockInfoView::OnIndiDmi)
	ON_COMMAND(ID_INDI_TECH_ANALYSIS_ROADMAP, &CStockInfoView::OnIndiTechAnalysisRoadmap)
	ON_COMMAND(ID_INDI_SIMULATED_ACCOUNT_QUERY, &CStockInfoView::OnIndiSimulatedAccountQuery)
END_MESSAGE_MAP()



// CStockInfoView 消息处理程序


BOOL CStockInfoView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	m_strSelectedCode = "";
	m_strSelectedItemText = "";
	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )>0)
		{
			string::size_type pos1 = firstMatch[0].find(")");
			m_strSelectedCode = firstMatch[0].substr(1, pos1-1);
			m_strSelectedItemText = text.GetBuffer();
		}
	}

	/*先获取当前打开的 MDI 视图，让他来处理命令消息*/
	if(((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive())
	{
		CView* pCurView=((CMainFrame*)(AfxGetApp()->m_pMainWnd))->MDIGetActive()->GetActiveView();//<--This Line
		if(pCurView && m_strSelectedCode.empty()==FALSE )
		{
			/*if(pCurView->GetDocument()->IsKindOf(RUNTIME_CLASS(CIndividualShareDoc)))
				((CIndividualShareDoc *)pCurView->GetDocument())->m_nCode = m_strSelectedCode;*/

			if(pCurView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)==TRUE)
				return TRUE;

		}

	}
	
	return CDockablePane::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


int CStockInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS \
		| WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_SHOWSELALWAYS  /*在失去焦点时也显示当前选中的结点*/;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("未能创建创业板视图\n");
		return -1;      // 未能创建
	}

	// 加载图像:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_STOCKINFO);
	m_wndToolBar.LoadToolBar(IDR_STOCKINFO, 0, 0, TRUE /* 已锁定*/);

	// OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_STOCKINFO);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CStockInfoViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CStockInfoViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CStockInfoViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		// pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}
	return 0;
}

void CStockInfoView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	//FillClassView();
	AdjustLayout();
}

void CStockInfoView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码

	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_STOCKINFO);

	CMenu* pSumMenu = menu.GetSubMenu(0);
	
	//	删除某些暂时不开放的菜单
	/*pSumMenu->RemoveMenu(ID_INDI_PE_AND_VALUE_DAILY, MF_BYCOMMAND); */

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}


bool CStockInfoView::FocusOn(int nCode)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);
	
	bool bFind = false;
	char tmp[10];
	sprintf_s(tmp,10,"(%06d)", nCode);
	string strCode = tmp;
	HTREEITEM hTreeItem =  pWndTree->GetRootItem();
	HTREEITEM hSelectItem = hTreeItem;
	HTREEITEM hFirstItem = hTreeItem;
	int nSelectedPos = pWndTree->GetCount();
	
	//hTreeItem = pWndTree->GetChildItem(hTreeItem);
	for(int i=0; i<nSelectedPos; i++)
	{
		string str = pWndTree->GetItemText(hTreeItem).GetBuffer();
		if(str.find(strCode)!=string::npos)
		{
			if(i+3<nSelectedPos)
				nSelectedPos = i+3;
			hSelectItem = hTreeItem;
			bFind = true;
		}
		hTreeItem = pWndTree->GetNextSiblingItem(hTreeItem);
	}
	
	if(bFind)
	{
		//	多次选择，以便可以实现跳转显示
		if(hFirstItem!=NULL)
			pWndTree->SelectItem(hFirstItem);
		if(hTreeItem!=NULL)
			pWndTree->SelectItem(hTreeItem);
		if(hSelectItem!=NULL)
			pWndTree->SelectItem(hSelectItem);
	}
	return bFind;
	//pWndTree->SetScrollTime();
}

BOOL CStockInfoView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(pMsg->message==WM_CHAR )
	{
		TCHAR ch = pMsg->wParam;

		if(/*(ch >='A' && ch <='Z') ||*/ (ch >='a' && ch <='z'))
		{
			CFindStockByPinyin dlg;
			dlg.m_strPinyin = ch; 
			dlg.DoModal();

			if(dlg.m_strPinyin.GetLength()>6)
			{
				CString strCode = dlg.m_strPinyin.Mid(5).Left(6);
				CBaseMessageControl::FocusStockOnLeftPane(strCode.GetBuffer());
			}
			return TRUE;
		}

	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

void CStockInfoView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDockablePane::OnPaint()

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}


void CStockInfoView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}



void CStockInfoView::FillClassView()
{
	/*HTREEITEM hRoot = m_wndClassView.InsertItem(_T("FakeApp 类"), 0, 0);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);*/

	m_wndClassView.DeleteAllItems();

	for(int i=0; i< m_pStockInfo->m_nRowNum; i++)
	{
		CString str;
		str.Format("%s(%06d)",m_pStockInfo->m_vec_abbreviation[i].c_str(),m_pStockInfo->m_vec_a_code[i] );
		HTREEITEM hClass = m_wndClassView.InsertItem(str, 1, 1/*, hRoot*/);
	}
	
	//m_wndClassView.Expand(hRoot, TVE_EXPAND);

}

void CStockInfoView::OnStockinfoOpen()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());
		
		CIndividualYahooPeView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);	
		CString title;
		title.Format("每日股票及动态PE-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualSummary()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualSummaryView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);	
		pNewDoc->SetTitle(text);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}



void CStockInfoView::OnOverallCost()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCostView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("营业总成本-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualSzsePe()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualSzsePeView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("深市股票PE-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualVolume()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualVolumeView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("每日成交额-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualNetProfits()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualNetProfitsView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("净利润构成-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnBalanceCurrentAsset()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCurrentAssetView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("流动资产-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnBalanceNonCurrentAsset()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualNonCurrentAssetView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("非流动资产-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnDownloadIndividual()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CAutoDownloadView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("个股数据下载-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCapitalStruct()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCapitalStructView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format("股本结构分析-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCsvIncome()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCsvFileView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_INCOME); //strCarMunber);
		CString title;
		title.Format(" 利润表-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCsvTrading()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		AfxMessageBox("暂停该功能");		
		return ;

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCsvFileView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_TRADING); //strCarMunber);
		CString title;
		title.Format(" 近期交易数据-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCsvBalance()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCsvFileView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_BALANCE); //strCarMunber);
		CString title;
		title.Format(" 资产负债表-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCapitalFlow()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCapitalFlowView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format(" 资金流向-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCapitalflowValue()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCapitalFlowView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format(" 资金流向与流通市值-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnCashflowSummary()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiCashFlowSummaryView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr); //strCarMunber);
		CString title;
		title.Format(" 现金流量小计-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualCsvCashflows()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCsvFileView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_CASHFLOWS); //strCarMunber);
		CString title;
		title.Format(" 现金流量表-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualAddToSelfSelect()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string  stockName = text.GetBuffer();
		string::size_type pos = stockName.find(firstMatch[0]);
		stockName = stockName.substr(0,pos);

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CSelfSelectPlateDlg dlg;
		dlg.DoModal();

		if(dlg.m_nPlateCode> 0)
		{
			CStockPlateData treeNode;
			treeNode.code = Int2String(code, "%06d");
			treeNode.name = stockName;
			treeNode.company_number = 1;
			treeNode.is_stock = 1;

			CStockPlateData treeNodeParent;
			treeNodeParent.code = Int2String(dlg.m_nPlateCode, "%08d");
			treeNodeParent.is_stock = 0;

			JMutexAutoLock lock(((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_mutex);
			CStockPlateTree * pPlates = ((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_pTree;
			CStockPlateTree * pNode = pPlates->SearchSpecialNode(treeNodeParent);
			if(pNode)
			{
				pNode->insert(treeNode);
			}
			((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->ImportToDatabase();

			StockInfo_Reqeust req;
			req.code = -ID_READ_STOCKPLATE_SELF_SELECT;
			req.hwnd = NULL;
			((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_front(req);
		}
	}
}

void CStockInfoView::OnIndividualCsvFundHold()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndividualCsvFileView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_FUND_HOLD); //strCarMunber);
		CString title;
		title.Format(" 基金持有CSV-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnFindStockByCode()
{
	// TODO: 在此添加命令处理程序代码
	CFindStockByCode dlg;
	dlg.SetStockInfo(m_pStockInfo);

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	if(m_pStockInfo==&pApp->m_CEntrepreneurship)
		dlg.m_strCaption = "【创业板】股票代码查询";
	if(m_pStockInfo==&pApp->m_CSmallMidEnterprise)
		dlg.m_strCaption = "【中小板】股票代码查询";
	if(m_pStockInfo==&pApp->m_CShenzhenAshares)
		dlg.m_strCaption = "【深圳A股】股票代码查询";
	if(m_pStockInfo==&pApp->m_CShanghaiStock)
		dlg.m_strCaption = "【上海A股】股票代码查询";

	dlg.DoModal();

	if(dlg.m_strCode.GetLength()>6)
	{
		int code = atoi(dlg.m_strCode.Left(6));
		if(((CMainFrame *)AfxGetMainWnd())->m_wndEntrepreneurView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndEntrepreneurView.ShowPane(TRUE, FALSE, TRUE);

		else if(((CMainFrame *)AfxGetMainWnd())->m_wndSmallMidEnterpriseView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndSmallMidEnterpriseView.ShowPane(TRUE, FALSE, TRUE);

		else if(((CMainFrame *)AfxGetMainWnd())->m_wndShenzhenAsharesView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndShenzhenAsharesView.ShowPane(TRUE, FALSE, TRUE);

		else if(((CMainFrame *)AfxGetMainWnd())->m_wndShanghaiStockView.FocusOn(code)==true)
			((CMainFrame *)AfxGetMainWnd())->m_wndShanghaiStockView.ShowPane(TRUE, FALSE, TRUE);
	}
}

void CStockInfoView::OnIndividualZombieAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiZombieAnalysisView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_FUND_HOLD); //strCarMunber);
		CString title;
		title.Format("僵尸指标分析-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualFundHoldValueAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiFundHoldValueView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, ID_INDIVIDUAL_CSV_FUND_HOLD); //strCarMunber);
		CString title;
		title.Format(" 基金持有市值跟踪-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualMacd()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiMacdView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("MACD分析-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiMainIncomeGrowth()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiMainIncomeGrowthView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("主营收入成长构成-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndividualMcst()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiMcstView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("MCST 成本价分析-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndi10jqkaBonus()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndi10jqkaBonusView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("历年分红送股-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiHexunResearchPaperEps()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiHexunResearchPapaerView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("和讯研报EPS分布图-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiMarketValueIndexAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiMarketIndexAnalysisView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("指数分析-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiCsindexAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	
	if (m_strSelectedCode.empty() == FALSE)
	{
		
		CIndiCsindexView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		CString title;
		title.Format("中证公司数据分析-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}



void CStockInfoView::OnIndiGrossProfitRate()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;


	if (m_strSelectedCode.empty() == FALSE)
	{

		CIndiGrossProfitRateView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		CString title;
		title.Format("季度毛利率分析-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiAddInsuranceRoadmap()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "增发";
		CString title;
		title.Format("个股增发路线图-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiImportantEventRoadmap()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "其它重大事项";
		CString title;
		title.Format("其它重大事项-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiQuarterReportRoadmap()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "度报告";
		CString title;
		title.Format("季度（半）年度报告-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiTradeRoadmap()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "交易";
		CString title;
		title.Format("交易-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiSpecialDisposalRoadmap()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "特别处理及退市";
		CString title;
		title.Format("特别处理及退市-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiSupplementCorrectionRoadmap()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "补充及更正";
		CString title;
		title.Format("补充及更正-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiPeAndValueDaily()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiPeAndValueDailyView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		CString title;
		title.Format("每日总市值及动态PE-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiOriginalIssueRoadmap()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiCninfoAnnouncementRoadView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "首次公开发行及上市";
		CString title;
		title.Format("首次公开发行及上市-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiIncomestatementForecast()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	if (m_strSelectedCode.empty() == FALSE)
	{
		CIndiIncomestatementForecastView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(m_strSelectedCode, -1); //strCarMunber);
		pNewDoc->m_strInput = "今（明）年业绩预测";
		CString title;
		title.Format("今（明）年业绩预测-%s", m_strSelectedItemText.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiDmi()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiDmiView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("DMI 分析-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiTechAnalysisRoadmap()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CIndiTechnicalAnalysisRoadView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1); //strCarMunber);
		CString title;
		title.Format("技术分析事件图-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}

void CStockInfoView::OnIndiSimulatedAccountQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndClassView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_wndClassView.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		CSimulatedAccountListView	MonitorListView;

		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);
		CDocument * pNewDoc = new CIndividualShareDoc(codeStr, -1, 1); //strCarMunber);
		CString title;
		title.Format("模拟交易-%s", text);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	
	}
}
