
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Stock.h"

#include "MainFrm.h"
#include "BaseMessageElementsParser.h"
#include "CalculateStockInfoEx.h"
#include "FindStockByPinyin.h"

#include "IndividualShareDoc.h"
#include "BaseHtmlView.h"
#include "AllStockListView.h"
#include "ZombieStockListView.h"
#include "AutoDownloadView.h"
#include "MainProductQueryView.h"
#include "PeStockListView.h"
#include "MacdStockListView.h"
#include "HexunResearchPaperListView.h"
#include "AllMainIncomeYearGrowthStockListView.h"
#include "AllMainProfitYearGrowthStockListView.h"
#include "AllRecentBonusCashStockListView.h"
#include "AllThisYearBonusListView.h"
#include "AllMcstStockListView.h"
#include "AllAddIssuanceListView.h"
#include "CninfoAnnouncementListView.h"
#include "CninfoAnnouncePDFListView.h"
#include "CninfoIncentivePlanListView.h"
#include "CninfoGetLandListView.h"
#include "CninfoShareHoldingListView.h"
#include "CninfoShareHoldingStatisticsView.h"
#include "PlateShanghaiIndexView.h"

#include "MarketExchageFundView.h"
#include "MarketMarginGuaranteeFundView.h"
#include "MarketCffexHs300FutureView.h"
#include "MarketSohuIndexView.h"
#include "MarketCsdcInvestorStatView.h"

#include "StockHomePageView.h"
#include "StatsGovCnChartView.h"
#include "StatsGovCnGridView.h"

#include "ShanghaiIndexView.h"

#include "Newmat11TestView.h"

#include "SimpleInputBox.h"
#include "StatsGovMenuDlg.h"
#include "StockGroupDlg.h"

#include "CalcFinanceDownload.h"
#include "CalcForecastDownload.h"
#include "CalcCapiStructDownload.h"
#include "CalcPeStockList.h"
#include "CalcMacdListData.h"
#include "CalcHexunResearchPaperQuery.h"
#include "CalcSohuMainIncomeCompoDownload.h"
#include "CalcCsiPeDailyDownload.h"
#include "CalcCsiCsrcPeDailyDownload.h"
#include "CalcSipfSettlementFundDownload.h"
#include "CalcSseMarginTradingDownload.h"
#include "CalcSzseMarginTradingDownload.h"
#include "CalcCsdcInvestorStatDownload.h"
#include "CalcCsdcSecurityIssueStatDownload.h"
#include "CalcUpdateStockInfo.h"
#include "CalcSzseIndexDailyDownload.h"

#include "CandlesticksJapanListView.h"
#include "MacdListView.h"
#include "BollListView.h"

#include "AutoDownloadDoc.h"
#include "AutoDownloadView.h"


extern UINT InitialReadStockInfoThreadFunc(LPVOID param);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)

	ON_MESSAGE(WM_USER_LOG_1, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_2, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_3, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_CapitalFlow163, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_FinanceReport, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_Forecast, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_CapitalStruct, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_ResearchPaper, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_Income_Composition, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_Cfi_Bonus, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_Cninfo, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_StatsGovCn, OnUserLogWParam_1)
	ON_MESSAGE(WM_USER_LOG_CSDC, OnUserLogWParam_1)

	ON_MESSAGE(WM_NOTIFY_FROM_TRAY,OnNotifyIcon)

	ON_MESSAGE(WM_USER_XML,	OnUserXmlWParam)
	ON_COMMAND(ID_SOUHU_PLATE, &CMainFrame::OnSouhuPlate)
	ON_COMMAND(ID_ALL_STOCK_LIST, &CMainFrame::OnAllStockList)
	/*ON_COMMAND(ID_SHANGHAI_STOCK_LIST, &CMainFrame::OnShanghaiStockList)
	ON_COMMAND(ID_SHENZHEN_A_STOCK_LIST, &CMainFrame::OnShenzhenAStockList)
	ON_COMMAND(ID_SMALL_MID_STOCK_LIST, &CMainFrame::OnSmallMidStockList)
	ON_COMMAND(ID_ENTREPR_STOCK_LIST, &CMainFrame::OnEntreprStockList)*/
	ON_COMMAND(ID_ALL_STOCK_DOWNLOAD, &CMainFrame::OnAllStockDownload)
	ON_COMMAND(ID_ZOMBIE_STOCK_LIST, &CMainFrame::OnZombieStockList)
	ON_COMMAND(ID_MAIN_PRODUCT_QUERY, &CMainFrame::OnMainProductQuery)
	ON_COMMAND(ID_PE_STOCK_LIST, &CMainFrame::OnPeStockList)
	ON_COMMAND(ID_MACD_STOCK_LIST, &CMainFrame::OnMacdStockList)
	ON_COMMAND(ID_HEXUN_RESEARCH_PAPER_QUERY, &CMainFrame::OnHexunResearchPaperQuery)
	ON_COMMAND(ID_MAIN_INCOME_YEAR_GROWTH_QUERY, &CMainFrame::OnMainIncomeYearGrowthQuery)
	ON_COMMAND(ID_MAIN_PROFIT_YEAR_GROWTH_QUERY, &CMainFrame::OnMainProfitYearGrowthQuery)
	ON_COMMAND(ID_RECENT_BONUS_CASH_QUERY, &CMainFrame::OnRecentBonusCashQuery)
	ON_COMMAND(ID_THIS_YEAR_BONUS_PLAN_QUERY, &CMainFrame::OnThisYearBonusPlanQuery)
	ON_COMMAND(ID_MCST_STOCK_LIST, &CMainFrame::OnMcstStockList)
	ON_COMMAND(ID_ADD_ISSUANCE_STOCK_LIST, &CMainFrame::OnAddIssuanceStockList)
	ON_COMMAND(ID_NEWMAT11_TEST, &CMainFrame::OnNewmat11Test)
	ON_COMMAND(ID_MARKET_EXCHANGE_FUND_IN_SETTLEMENT, &CMainFrame::OnMarketExchangeFundInSettlement)
	ON_COMMAND(ID_MARKET_MARGIN_GUARANTEE_FUND, &CMainFrame::OnMarketMarginGuaranteeFund)
	ON_COMMAND(ID_MARKET_CFFEX_HS300_FUTURE, &CMainFrame::OnMarketCffexHs300Future)
	ON_COMMAND(ID_MARKET_INDEX, &CMainFrame::OnMarketIndex)
	ON_COMMAND(ID_MARKET_INVESTOR_STAT, &CMainFrame::OnMarketInvestorStat)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_COMMAND_RANGE(ID_STATSGOV_HGYD_MENU, ID_STATSGOV_HGYD_MENU, &CMainFrame::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGJD_MENU, ID_STATSGOV_HGJD_MENU, &CMainFrame::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_HGND_MENU, ID_STATSGOV_HGND_MENU, &CMainFrame::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_FSND_MENU, ID_STATSGOV_FSND_MENU, &CMainFrame::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_FSYD_MENU, ID_STATSGOV_FSYD_MENU, &CMainFrame::OnStatsgovHgydMenu)
	ON_COMMAND_RANGE(ID_STATSGOV_FSJD_MENU, ID_STATSGOV_FSJD_MENU, &CMainFrame::OnStatsgovHgydMenu)

	/*ON_COMMAND(ID_STATSGOV_HGJD_MENU, &CMainFrame::OnStatsgovHgjdMenu)
	ON_COMMAND(ID_STATSGOV_HGND_MENU, &CMainFrame::OnStatsgovHgndMenu)
	ON_COMMAND(ID_STATSGOV_FSND_MENU, &CMainFrame::OnStatsgovFsndMenu)
	ON_COMMAND(ID_STATSGOV_FSYD_MENU, &CMainFrame::OnStatsgovFsydMenu)
	ON_COMMAND(ID_STATSGOV_FSJD_MENU, &CMainFrame::OnStatsgovFsjdMenu)*/
	ON_COMMAND_RANGE(ID_FINANCE_SALES_FROM_OPERATIONS, ID_FINANCE_SALES_FROM_OPERATIONS, &CMainFrame::OnFinanceSalesFromOperations)
	ON_COMMAND_RANGE(ID_FINANCE_COST_OF_OPERATIONS, ID_FINANCE_COST_OF_OPERATIONS, &CMainFrame::OnFinanceSalesFromOperations)

	ON_COMMAND(ID_CNINFO_ANNOUNCEMENT_QUERY, &CMainFrame::OnCninfoAnnouncementQuery)
	ON_COMMAND(ID_PE_GROSS_LIST, &CMainFrame::OnPeGrossList)
	ON_COMMAND(ID_CNINFO_PDF_QUERY, &CMainFrame::OnCninfoPdfQuery)
	ON_COMMAND(ID_SHANGHAI_INDEX, &CMainFrame::OnShanghaiIndex)
	ON_COMMAND(ID_CNINFO_INCENTIVE_PLAN_QUERY, &CMainFrame::OnCninfoIncentivePlanQuery)
	ON_COMMAND(ID_HUSHEN_CHANGE_RATE_QUERY, &CMainFrame::OnHushenChangeRateQuery)
	ON_COMMAND(ID_SMALL_MIDDLE_INDEX, &CMainFrame::OnSmallMiddleIndex)
	ON_COMMAND(ID_CNINFO_GET_PLAN_QUERY, &CMainFrame::OnCninfoGetPlanQuery)
	ON_COMMAND(ID_ENTERPRENUER_INDEX, &CMainFrame::OnEnterprenuerIndex)
	ON_COMMAND(ID_CNINFO_SHARE_HOLDING_QUERY, &CMainFrame::OnCninfoShareHoldingQuery)
	ON_COMMAND(ID_CNINFO_SHARE_HOLDING_STATISTICS, &CMainFrame::OnCninfoShareHoldingStatistics)
	ON_COMMAND(ID_CANDLESTICKS_JAPAN_QUERY, &CMainFrame::OnCandlesticksJapanQuery)
	ON_COMMAND(ID_MACD_QUERY, &CMainFrame::OnMacdQuery)
	ON_COMMAND(ID_BOLL_QUERY, &CMainFrame::OnBollQuery)
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	/*显示启动画面，并且开始下载股票基础资料*/
	m_pWndSplashScreen = new CSplashScreenEx();
	m_pWndSplashScreen->Create(this,"A 股分析系统欢迎您",0,CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
	m_pWndSplashScreen->SetBitmap(IDB_SPLASH,255,0,255);
	m_pWndSplashScreen->SetTextFont("Impact",100,CSS_TEXT_NORMAL);
	m_pWndSplashScreen->SetTextRect(CRect(125,60,291,104));
	m_pWndSplashScreen->SetTextColor(RGB(255,255,255));
	m_pWndSplashScreen->SetTextFormat(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	m_pWndSplashScreen->Show();

	/*Sleep(1000);
	pSplash->SetText("You can display infos");

	pSplash->Hide();*/

	

	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	//	股票
	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndEntrepreneurView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSmallMidEnterpriseView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndShenzhenAsharesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndShanghaiStockView.EnableDocking(CBRS_ALIGN_ANY);
	//	板块
	m_wndHangyePlatesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDiyuPlatesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndGainianPlatesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSelfSelectPlatesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCsindexPlatesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCsindexCsrcPlatesView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndShenzhenIndexPlatesView.EnableDocking(CBRS_ALIGN_ANY);

	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	//m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndEntrepreneurView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndSmallMidEnterpriseView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndShenzhenAsharesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndShanghaiStockView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);

	m_wndHangyePlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndDiyuPlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndGainianPlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndSelfSelectPlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndCsindexPlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndCsindexCsrcPlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndShenzhenIndexPlatesView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	/*m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);*/


	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	//	取消菜单折叠
	//CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建创业板视图
	m_wndEntrepreneurView.SetStockInfo(&((CStockApp *)AfxGetApp())->m_CEntrepreneurship);
	CString strEntrepreneurView;
	bNameValid = strEntrepreneurView.LoadString(IDS_ENTREPRENEUR_VIEW);
	ASSERT(bNameValid);
	if (!m_wndEntrepreneurView.Create(strEntrepreneurView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_ENTREPRENEUR, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“创业板视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建中小板视图
	m_wndSmallMidEnterpriseView.SetStockInfo(&((CStockApp *)AfxGetApp())->m_CSmallMidEnterprise);
	CString strSmallMidEnterpriseView;
	bNameValid = strSmallMidEnterpriseView.LoadString(IDS_SMALLANDMID_VIEW);
	ASSERT(bNameValid);
	if (!m_wndSmallMidEnterpriseView.Create(strSmallMidEnterpriseView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SMALLMIDENTERPRISE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“中小板视图”窗口\n");
		return FALSE; 
	}

	// 创建深圳A股视图
	m_wndShenzhenAsharesView.SetStockInfo(&((CStockApp *)AfxGetApp())->m_CShenzhenAshares);
	CString strShenzhenAsharesView;
	bNameValid = strShenzhenAsharesView.LoadString(IDS_SHENZHENASHARE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndShenzhenAsharesView.Create(strShenzhenAsharesView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SHENZHENASHARES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“深圳A股视图”窗口\n");
		return FALSE; 
	}

	// 创建上海证券交易所股票视图
	m_wndShanghaiStockView.SetStockInfo(&((CStockApp *)AfxGetApp())->m_CShanghaiStock);
	CString strShanghaiStockView;
	bNameValid = strShanghaiStockView.LoadString(IDS_SHANGHAISTOCK_VIEW);
	ASSERT(bNameValid);
	if (!m_wndShanghaiStockView.Create(strShanghaiStockView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SHANGHAISTOCK, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}

	// 创建板块视图
	m_wndHangyePlatesView.SetPlateCode(ID_PLATE_TYPE_SOHU, 1631);
	CString strHangyePlatesView;
	bNameValid = strHangyePlatesView.LoadString(IDS_SOUHU_HANGYE_PLATE);
	ASSERT(bNameValid);
	if (!m_wndHangyePlatesView.Create(strHangyePlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_HANGYE_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}

	m_wndDiyuPlatesView.SetPlateCode(ID_PLATE_TYPE_SOHU,1632);
	CString strDiyuPlatesView;
	bNameValid = strDiyuPlatesView.LoadString(IDS_SOUHU_DIYU_PLATE);
	ASSERT(bNameValid);
	if (!m_wndDiyuPlatesView.Create(strDiyuPlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_DIYU_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}

	m_wndGainianPlatesView.SetPlateCode(ID_PLATE_TYPE_SOHU,1630);
	CString strGainianPlatesView;
	bNameValid = strGainianPlatesView.LoadString(IDS_SOUHU_GAINIAN_PLATE);
	ASSERT(bNameValid);
	if (!m_wndGainianPlatesView.Create(strGainianPlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_GAINIAN_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}

	m_wndCsindexPlatesView.SetPlateCode(ID_PLATE_TYPE_CSINDEX,0);
	CString strCsindexPlatesView;
	bNameValid = strCsindexPlatesView.LoadString(IDS_CSINDEX_PLATE);
	ASSERT(bNameValid);
	if (!m_wndCsindexPlatesView.Create(strCsindexPlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_CSINDEX_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}

	m_wndCsindexCsrcPlatesView.SetPlateCode(ID_PLATE_TYPE_CSINDEX_CSRC,0);
	CString strCsindexCsrcPlatesView;
	bNameValid = strCsindexCsrcPlatesView.LoadString(IDS_CSINDEX_CSRC_PLATE);
	ASSERT(bNameValid);
	if (!m_wndCsindexCsrcPlatesView.Create(strCsindexCsrcPlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_CSINDEX_CSRC_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}


	m_wndShenzhenIndexPlatesView.SetPlateCode(ID_PLATE_TYPE_SHENZHEN_INDEXINFO,0);
	CString strShenzhenIndexPlatesView;
	bNameValid = strShenzhenIndexPlatesView.LoadString(IDS_SHENZHEN_INDEXINFO_PLATE);
	ASSERT(bNameValid);
	if (!m_wndShenzhenIndexPlatesView.Create(strShenzhenIndexPlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_CSINDEX_CSRC_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}

	//	列出 root 根目录下的所有 自选板块组
	m_wndSelfSelectPlatesView.SetPlateCode(ID_PLATE_TYPE_SELF_SELECT, 0);
	CString strSelfSelectPlatesView;
	bNameValid = strSelfSelectPlatesView.LoadString(IDS_SELF_SELECT_PLATE);
	ASSERT(bNameValid);
	if (!m_wndSelfSelectPlatesView.Create(strSelfSelectPlatesView, this, CRect(0, 0, 200, 200), TRUE,\
		ID_VIEW_SELF_SELECT_PLATE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“上海证券交易所股票视图”窗口\n");
		return FALSE; 
	}


	// 创建类视图
	/*CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“类视图”窗口\n");
		return FALSE; 
	}*/

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	/*CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; 
	}*/

	SetDockingWindowIcons(theApp.m_bHiColorIcons);

	ToTray();
	//ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	/*HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);*/

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	/*HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);*/

	HICON hEntrepreneurViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndEntrepreneurView.SetIcon(hEntrepreneurViewIcon, FALSE);

	HICON hSmallMidEnterpriseViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndSmallMidEnterpriseView.SetIcon(hSmallMidEnterpriseViewIcon, FALSE);

	HICON hShenzhenAShareViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndShenzhenAsharesView.SetIcon(hShenzhenAShareViewIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
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

	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}



LRESULT CMainFrame::OnUserLogWParam_1(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;
	int		logType = (int)lParam;

	if (CMessageString::IsStringValid(pString))
	{
		m_wndOutput.AppendLog(pString->GetBuffer(), logType);
		delete pString;
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}



LRESULT CMainFrame::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;

	stringstream ss;
	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		CBaseMessageElementsParser parser(this);
		if(parser.Parse(pString->GetBuffer(), pString->GetLength())==false)
		{
			ss.str("");
			ss << "CMainFrame::OnUserXmlWParam, xml parser 解析错误";
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);			
		}

		delete pString;
		
		if(parser.msg_type=="ReadFinished")
		{
			if(atoi(parser.param1.c_str())==-ID_READ_STOCKINFO_ENTERPRENUER)
			{
				//	股票信息从数据库读入完毕，收到通知，开始更新展示树
				m_wndEntrepreneurView.FillClassView();

				//////////////////////////////////////////////////////////////////////////
				// 启动下载深圳股票每日行情信息

				/*CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = 60000;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_3;
				pThread = AfxBeginThread(AutoDownloadThreadFunc, req);*/

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 Souhu 股票每日行情信息 (创业版)
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_TRADINGDAILY_ENTERPRENUER;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_3;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
				
				//////////////////////////////////////////////////////////////////////////
				// 启动下载 163 网易 股票每日资金流向 (创业版)
				CDownloadRequest * req1 = new CDownloadRequest;
				req1->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_ENTERPRENUER;
				req1->m_hwnd = NULL;
				req1->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				

				//////////////////////////////////////////////////////////////////////////
				//	启动读入【创业板】的五年内的财务报表
				/*time_t curtime = time (NULL);
				struct tm * tm1;
				tm1 = gmtime (&curtime);
				char tmp[256] = { 0 };
				snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + tm1->tm_year -1, tm1->tm_mon+1, tm1->tm_mday);	

				CEntrepreneurship entrepreneur;
				CBussinessUtils::GetEntrepreneurship(entrepreneur);
				for(int k=0; k<entrepreneur.m_vec_code.size(); k++)
				{
					StockInfo_Reqeust req;
					req.code = entrepreneur.m_vec_code[k];
					req.hwnd = NULL;
					req.logType = WM_USER_LOG_2;
					req.strAfterDate = string(tmp);
					((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req);
				}*/
				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的财务报表股票
				{
					CCalcFinanceDownload * cse =  new CCalcFinanceDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcFinanceDownload::NeedDownloadFinanceReport1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的业绩预告股票
				{
					CCalcForecastDownload * cse =  new CCalcForecastDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcForecastDownload::NeedDownloadPerformanceForecast1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的股票结构股票
				{
					CCalcCapiStructDownload * cse =  new CCalcCapiStructDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcCapiStructDownload::NeedDownload1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的主营业务构成的股票
				{
					CCalcSohuMainIncomeCompoDownload * cse =  new CCalcSohuMainIncomeCompoDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcSohuMainIncomeCompoDownload::Execute1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的中国投资者保护基金的银证交易金数据的交易日期
				{
					CCalcSipfSettlementFundDownload * cse =  new CCalcSipfSettlementFundDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcSipfSettlementFundDownload::NeedDownload1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的上海证券交易所的融资融券数据的交易日期
				{
					CCalcSseMarginTradingDownload * cse =  new CCalcSseMarginTradingDownload(m_hWnd, WM_USER_LOG_Cfi_Bonus);
					cse->ScheduleTask(&CCalcSseMarginTradingDownload::NeedDownload1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的深圳证券交易所的融资融券数据的交易日期
				{
					CCalcSzseMarginTradingDownload * cse =  new CCalcSzseMarginTradingDownload(m_hWnd, WM_USER_LOG_Cfi_Bonus);
					cse->ScheduleTask(&CCalcSzseMarginTradingDownload::NeedDownload1);
				}

				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的中国结算数据的周数
				{
					CCalcCsdcInvestorStatDownload * cse =  new CCalcCsdcInvestorStatDownload(m_hWnd, WM_USER_LOG_CSDC);
					cse->ScheduleTask(&CCalcCsdcInvestorStatDownload::NeedDownload1);
				}

				{
					CCalcCsdcSecurityIssueStatDownload * cse =  new CCalcCsdcSecurityIssueStatDownload(m_hWnd, WM_USER_LOG_CSDC);
					cse->ScheduleTask(&CCalcCsdcSecurityIssueStatDownload::NeedDownload1);
				}
			}
			else if(atoi(parser.param1.c_str())==-ID_READ_STOCKINFO_SMALLMID)
			{
				m_wndSmallMidEnterpriseView.FillClassView();

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 163 网易 股票每日资金流向 (中小版)
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);

				
			}
			else if(atoi(parser.param1.c_str())==-ID_READ_STOCKINFO_SHENZHENA)
			{
				m_wndShenzhenAsharesView.FillClassView();

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 163 网易 股票每日资金流向 (深圳 A 股版)
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);

				
			}
			else if(atoi(parser.param1.c_str())==-ID_READ_STOCKINFO_SHANGHAISTOCK)
			{
				m_wndShanghaiStockView.FillClassView();

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 163 网易 股票每日资金流向 （上海股票）
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_SHANGHAISTOCK;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 搜狐 股票业绩预告 (所有股票)  ,改为根据现有下载情况，进行选择性下载
				/*{
					CDownloadRequest * req2 = new CDownloadRequest;
					req2->m_nDownloadType = ID_DOWNLOAD_PERFORMANCE_FORECAST;
					req2->m_hwnd = NULL;
					req2->m_nLogType = WM_USER_LOG_Forecast;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req2);
				}*/
								

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 和讯 研究报告 (所有股票)
				CDownloadRequest * req3 = new CDownloadRequest;
				req3->m_nDownloadType = ID_DOWNLOAD_HEXUN_RESEARCH_PAPER;
				req3->m_hwnd = NULL;
				req3->m_nLogType = WM_USER_LOG_ResearchPaper;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req3);
				
				//////////////////////////////////////////////////////////////////////////
				// 启动下载 中财网 分红、配售股票 (所有股票)
				/*{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CFI_BONUS_SHARE;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cfi_Bonus;
					//req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);	
				}*/

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 巨潮网 分红、配售股票 (所有股票)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CNINFO_BONUS_SHARE;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cfi_Bonus;
					//req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);	
				}

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 同花顺 分红融资 (所有股票)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_10JQKA_BONUS_SHARE;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_CapitalStruct;
					//req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 巨潮网公告 (所有股票)
				/*{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CNINFO_ANNOUNCEMENT;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cninfo;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}*/

				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_SHANGHAI;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cninfo;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}

				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_SHENZHEN_ASHARE;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cninfo;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}

				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_SMALLANDMID;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cninfo;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}

				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CNINFO_ANNOUNCEMENT_ENTERPRENEUR;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cninfo;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}


				//////////////////////////////////////////////////////////////////////////
				// 启动下载 国家统计局数据 "分省年度"
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_STATSGOV_FSND;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_StatsGovCn;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}

				//////////////////////////////////////////////////////////////////////////
				// 启动下载 深圳交易所的 指数样本股信息
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_SHENZHEN_INDEX_YANGBEN;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_1;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
				}


			}
			else if(atoi(parser.param1.c_str()) == -ID_READ_STOCKPLATE_SOHU)
			{

				//////////////////////////////////////////////////////////////////////////
				//	启动 更新 stockinfo 中的 分类信息
				{
					CCalcUpdateStockInfo * cse =  new CCalcUpdateStockInfo(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcUpdateStockInfo::Execute1);
				}


				// 板块信息 生成树结构 OK， 展示停靠栏
				m_wndHangyePlatesView.FillClassView();
				m_wndDiyuPlatesView.FillClassView();
				m_wndGainianPlatesView.FillClassView();

				//	启动下载搜狐板块信息，为明天做数据准备
				if(1)
				{
					CDocTemplate * pDocTemplate = NULL;
					CView * pView = NULL;
					CAutoDownloadView	MonitorListView;

					pView = &MonitorListView;				
					pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

					pView = NULL;
					ASSERT_VALID(pDocTemplate);
					CIndividualShareDoc * pNewDoc = new CIndividualShareDoc("-1"); //strCarMunber);
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_CFFEX_FUTURE_DAILY);
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_SOUHU_STOCK_PLATE);
					//pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_CNINFO_ANNOUNCEMENT);
					
					CString title;
					title.Format("自动下载-%s", "股指期货、搜狐板块");
					pNewDoc->SetTitle(title);
					CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
					wnd->ShowWindow( SW_SHOWMAXIMIZED);
					pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
					pDocTemplate->AddDocument(pNewDoc);	
				}

				//	启动下载中国国家统计局数据
				/*if(1)
				{
					CDocTemplate * pDocTemplate = NULL;
					CView * pView = NULL;
					CAutoDownloadView	MonitorListView;

					pView = &MonitorListView;				
					pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

					pView = NULL;
					ASSERT_VALID(pDocTemplate);
					CIndividualShareDoc * pNewDoc = new CIndividualShareDoc("-1");
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_STATSGOV_FSND);
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_STATSGOV_FSJD);
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_STATSGOV_FSYD);

					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_STATSGOV_HGYD);
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_STATSGOV_HGJD);
					pNewDoc->m_vecCommandCode.push_back(ID_DOWNLOAD_STATSGOV_HGND);

					

					CString title;
					title.Format("自动下载-%s", "国家统计局");
					pNewDoc->SetTitle(title);
					CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);
					wnd->ShowWindow(SW_SHOWNORMAL);
					pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
					pDocTemplate->AddDocument(pNewDoc);	
				}*/
				
			}
			else if(atoi(parser.param1.c_str()) == -ID_READ_STOCKPLATE_SELF_SELECT)
			{
				// 自选板块信息 生成树结构 OK， 展示停靠栏
				m_wndSelfSelectPlatesView.FillClassView();
			}
			else if(atoi(parser.param1.c_str()) == -ID_READ_STOCKPLATE_CSINDEX)
			{
				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的中证指数公司市盈率等数据的交易日期
				{
					CCalcCsiPeDailyDownload * cse =  new CCalcCsiPeDailyDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcCsiPeDailyDownload::NeedDownload1);
				}

				// 自选板块信息 生成树结构 OK， 展示停靠栏
				m_wndCsindexPlatesView.FillClassView();

				
			}
			else if(atoi(parser.param1.c_str()) == -ID_READ_STOCKPLATE_CSINDEX_CSRC)
			{
				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的中证指数公司市盈率等数据的交易日期
				{
					CCalcCsiCsrcPeDailyDownload * cse =  new CCalcCsiCsrcPeDailyDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcCsiCsrcPeDailyDownload::NeedDownload1);
				}

				// 自选板块信息 生成树结构 OK， 展示停靠栏
				m_wndCsindexCsrcPlatesView.FillClassView();
			}
			else if(atoi(parser.param1.c_str()) == -ID_READ_SHENZHEN_INDEXINFO)
			{
				//////////////////////////////////////////////////////////////////////////
				//	启动计算需要下载的深圳交易所的 股票指数 等数据的交易日期
				{
					CCalcSzseIndexDailyDownload * cse =  new CCalcSzseIndexDailyDownload(m_hWnd, WM_USER_LOG_1);
					cse->ScheduleTask(&CCalcSzseIndexDailyDownload::NeedDownload1);
				}

				// 自选板块信息 生成树结构 OK， 展示停靠栏
				m_wndShenzhenIndexPlatesView.FillClassView();
			}
		}
		
		if(parser.msg_type=="DownloadFinished")
		{
			
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_SHANGHAI_COMPOSITE_INDEX)
			{
				m_pWndSplashScreen->SetText("沪深300指数下载中...");

				// 启动下载 沪深300指数
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_HUSHENG300_INDEX;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			else if(atoi(parser.param1.c_str())==ID_DOWNLOAD_HUSHENG300_INDEX)
			{
				m_pWndSplashScreen->SetText("中小板综合指数下载中...");

				// 启动下载 中小板综合指数
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_SMALL_MIDDLE_INDEX;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			else if(atoi(parser.param1.c_str())==ID_DOWNLOAD_SMALL_MIDDLE_INDEX)
			{
				m_pWndSplashScreen->SetText("创业板综合指数下载中...");

				// 启动下载 创业板综合指数
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_ENTERPRENUER_INDEX;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			else if(atoi(parser.param1.c_str())==ID_DOWNLOAD_ENTERPRENUER_INDEX)
			{
				m_pWndSplashScreen->SetText("深证成指下载中...");

				// 启动下载 深证成指
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_SHENZHENG_COMPOSITE_INDEX;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			else if(atoi(parser.param1.c_str())==ID_DOWNLOAD_SHENZHENG_COMPOSITE_INDEX)
			{
				m_pWndSplashScreen->SetText("深圳A股下载中...");

				// 启动下载 深圳A股 股票基础资料
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STOCKINFO_SHENZHEN_ASHARE;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			else if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STOCKINFO_SHENZHEN_ASHARE)
			{
				m_pWndSplashScreen->SetText("深圳中小版下载中...");

				// 启动下载 中小版 股票基础资料
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STOCKINFO_SMALLANDMID;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STOCKINFO_SMALLANDMID)
			{
				m_pWndSplashScreen->SetText("深圳创业版下载中...");

				// 启动下载 创业版 股票基础资料
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STOCKINFO_ENTERPRENEUR;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
				
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STOCKINFO_ENTERPRENEUR)
			{
				m_pWndSplashScreen->SetText("上海A股下载中...");

				// 启动下载 上海 A 股 股票基础资料
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STOCKINFO_SHANGHAI;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_1;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
				
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STOCKINFO_SHANGHAI)
			{
				if(parser.param2=="NewData")
				{
					m_pWndSplashScreen->SetText("巨潮网基础资料下载中...");

					// 启动下载 巨潮网 股票基础资料
					CDownloadRequest * req = new CDownloadRequest;
					req->m_nDownloadType = ID_DOWNLOAD_CNINFO_STOCKINFO;
					req->m_hwnd = NULL;
					req->m_nLogType = WM_USER_LOG_1;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
				}
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_CNINFO_STOCKINFO ||
				(atoi(parser.param1.c_str())==ID_DOWNLOAD_STOCKINFO_SHANGHAI && parser.param2=="NoMoreData" ))
			{

				m_pWndSplashScreen->SetText("股票基础资料读入中...");

				//	基础数据下载完毕,重新载入 数据日志
				((CStockApp *)AfxGetApp())->m_StockDataLog.FetchFromDB();

				// 【深圳A股】信息下载完成，重新要求读入，并刷新板块展示树
				StockInfo_Reqeust req_read;
				req_read.code = -ID_READ_STOCKINFO_SHENZHENA;
				req_read.hwnd = NULL;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				// 【中小板】信息下载完成，重新要求读入，并刷新板块展示树
				req_read.code = -ID_READ_STOCKINFO_SMALLMID;
				req_read.hwnd = NULL;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				// 【创业板】信息下载完成，重新要求读入，并刷新板块展示树
				req_read.code = -ID_READ_STOCKINFO_ENTERPRENUER;
				req_read.hwnd = NULL;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				// 【上海股票】信息下载完成，重新要求读入，并刷新板块展示树
				req_read.code = -ID_READ_STOCKINFO_SHANGHAISTOCK;
				req_read.hwnd = NULL;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				// 先在 主线程读入 以上股票信息
				InitialReadStockInfoThreadFunc(false);

				m_pWndSplashScreen->SetText("板块基础资料读入中...");

				//	启动读入 股票信息、板块信息、自选板块， 中证行业分类板块, 深圳股票指数板块 
				req_read.code = -ID_READ_STOCKPLATE_SELF_SELECT;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STOCKPLATE_CSINDEX;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STOCKPLATE_CSINDEX_CSRC;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STOCKPLATE_SOHU;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				// 先在 主线程读入 以上股票信息
				InitialReadStockInfoThreadFunc(false);

				m_pWndSplashScreen->SetText("深证股指板块读入中...");

				req_read.code = -ID_READ_SHENZHEN_INDEXINFO;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				// 先在 主线程读入 以上板块信息
				InitialReadStockInfoThreadFunc(false);

				// 再启动读入线程
				((CStockApp *)AfxGetApp())->StartWorkerThread(InitialReadStockInfoThreadFunc,(LPVOID)TRUE);

				//	隐藏开机提示框
				m_pWndSplashScreen->Hide();
				m_pWndSplashScreen = NULL;

				// 主窗口已初始化，因此显示它并对其进行更新
				ShowWindow(SW_SHOWMAXIMIZED);
				UpdateWindow();

				//	启动读入 国家统计局菜单
				
				req_read.code = -ID_READ_STATSGOV_HGYD;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STATSGOV_HGJD;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STATSGOV_HGND;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STATSGOV_FSND;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STATSGOV_FSYD;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);

				req_read.code = -ID_READ_STATSGOV_FSJD;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_back(req_read);


				//	跳出“系统首页” 
				CDocTemplate * pDocTemplate = NULL;
				CView * pView = NULL;
				CStockHomePageView	MonitorListView;

				pView = &MonitorListView;				
				pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

				pView = NULL;
				ASSERT_VALID(pDocTemplate);
				CIndividualShareDoc * pNewDoc = new CIndividualShareDoc("-1"); //strCarMunber);
			
				CString title;
				title.Format("首页（HomePage）%s", "");
				pNewDoc->SetTitle(title);
				CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
				wnd->ShowWindow(SW_SHOWMAXIMIZED);
				pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
				pDocTemplate->AddDocument(pNewDoc);	
			}

			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_TRADINGDAILY_ENTERPRENUER)
			{
				// 启动下载 Yahoo 股票每日行情信息 (中小版)
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_TRADINGDAILY_SMALL_AND_MID;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_3;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_TRADINGDAILY_SMALL_AND_MID)
			{
				// 启动下载 Yahoo 股票每日行情信息 (深圳 A 股版)
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_TRADINGDAILY_SHENZHEN_ASHARE;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_3;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_TRADINGDAILY_SHENZHEN_ASHARE)
			{
				// 启动下载 Yahoo 股票每日行情信息 （上海股票）
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_TRADINGDAILY_SHANGHAISTOCK;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_3;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}


			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_CAPITALFLOW_ENTERPRENUER)
			{
				// 启动下载 163 网易 股票每日资金流向 (中小版)
				/*CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);*/
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_CAPITALFLOW_SMALL_AND_MID)
			{
				// 启动下载 163 网易 股票每日资金流向 (深圳 A 股版)
				/*CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);*/
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_CAPITALFLOW_SHENZHEN_ASHARE)
			{
				// 启动下载 163 网易 股票每日资金流向 （上海股票）
				/*CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_CAPITALFLOW_SHANGHAISTOCK;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_CapitalFlow163;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);*/
			}

			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_SOUHU_STOCK_PLATE)
			{
				// 搜狐板块信息下载完成，重新要求读入，并刷新板块展示树
				StockInfo_Reqeust req;
				req.code = -ID_READ_STOCKPLATE_SOHU;
				req.hwnd = NULL;
				((CStockApp *)AfxGetApp())->m_listStockinfoReq.push_front(req);
			}

			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STATSGOV_FSND)
			{
				// 启动下载 国家统计局 "分省月度"
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STATSGOV_FSYD;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_StatsGovCn;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STATSGOV_FSYD)
			{
				// 启动下载 国家统计局 "分省季度"
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STATSGOV_FSJD;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_StatsGovCn;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STATSGOV_FSJD)
			{
				// 启动下载 国家统计局 "全国月度"
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STATSGOV_HGYD;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_StatsGovCn;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STATSGOV_HGYD)
			{
				// 启动下载 国家统计局 "全国季度"
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STATSGOV_HGJD;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_StatsGovCn;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			if(atoi(parser.param1.c_str())==ID_DOWNLOAD_STATSGOV_HGJD)
			{
				// 启动下载 国家统计局 "全国年度"
				CDownloadRequest * req = new CDownloadRequest;
				req->m_nDownloadType = ID_DOWNLOAD_STATSGOV_HGND;
				req->m_hwnd = NULL;
				req->m_nLogType = WM_USER_LOG_StatsGovCn;
				((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req);
			}
			
		}
		if(parser.msg_type=="DatabaseImported")
		{
			if(parser.model=="CSouhuTradingDaily")
			{
				/*CCalculateStockInfoEx * cse =  new CCalculateStockInfoEx(NULL, WM_USER_LOG_1);
				cse->m_vec_code[0] = atoi(parser.code.c_str());
				cse->m_vec_calculate_date[0] = parser.param1;
				cse->m_vec_close[0] = String2Double(parser.param2);
				cse->ScheduleTask(&CCalculateStockInfoEx::ImportToDatabase1);*/

				/*CStockApp * pApp = (CStockApp *)AfxGetApp();
				if(pApp->m_mapClosePrice.count(atoi(parser.code.c_str()))==0)
					pApp->m_mapClosePrice[atoi(parser.code.c_str())]= String2Double(parser.param2);*/
			}

		}

		if(parser.msg_type=="TaskFinished")
		{
			stringstream ss;
			ss << parser.model << "【" << (parser.param1) << "】 任务计算完成. ";

			if(parser.model=="CCalcForecastDownload")
			{
				CCalcForecastDownload * cse = (CCalcForecastDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{

					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_PERFORMANCE_FORECAST_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Forecast;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}
			}
			else if(parser.model=="CCalcFinanceDownload")
			{
				CCalcFinanceDownload * cse = (CCalcFinanceDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_FINANCE_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_FinanceReport;

					CDownloadRequest * req2 = new CDownloadRequest;
					req2->m_nDownloadType = ID_DOWNLOAD_FINANCE_SELECTED;
					req2->m_hwnd = NULL;
					req2->m_nLogType = WM_USER_LOG_FinanceReport;

					CDownloadRequest * req3 = new CDownloadRequest;
					req3->m_nDownloadType = ID_DOWNLOAD_FINANCE_SELECTED;
					req3->m_hwnd = NULL;
					req3->m_nLogType = WM_USER_LOG_FinanceReport;

					for(int i=0; i< cse->m_vec_code.size(); i++)
					{
						CDownloadRequest * req = NULL;
						if(i%3==0)	req = req1;
						else if(i%3==1) req = req2;
						else  req = req3;

						req->m_vecCode.push_back(cse->m_vec_code[i]);
						req->m_vec_cninfo_orgid.push_back(cse->m_vec_cninfo_orgid[i]);
					}
					
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req2);
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req3);

					delete cse;
				}
			}
			else if(parser.model=="CCalcCapiStructDownload")
			{
				CCalcCapiStructDownload * cse = (CCalcCapiStructDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					//	同花顺股本结构 分成 5 个线程同时下载
					CDownloadRequest * req1 = new CDownloadRequest;
					CDownloadRequest * req2 = new CDownloadRequest;
					CDownloadRequest * req3 = new CDownloadRequest;
					CDownloadRequest * req4 = new CDownloadRequest;
					CDownloadRequest * req5 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED;
					req2->m_nDownloadType = ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED;
					req3->m_nDownloadType = ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED;
					req4->m_nDownloadType = ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED;
					req5->m_nDownloadType = ID_DOWNLOAD_CAPITIALSTRUCT_SELECTED;

					req1->m_hwnd = NULL;
					req2->m_hwnd = NULL;
					req3->m_hwnd = NULL;
					req4->m_hwnd = NULL;
					req5->m_hwnd = NULL;

					req1->m_nLogType = WM_USER_LOG_CapitalStruct;
					req2->m_nLogType = WM_USER_LOG_CapitalStruct;
					req3->m_nLogType = WM_USER_LOG_CapitalStruct;
					req4->m_nLogType = WM_USER_LOG_CapitalStruct;
					req5->m_nLogType = WM_USER_LOG_CapitalStruct;

					for(int k = 0; k< cse->m_vec_code.size(); k++)
					{
						if(k%5==0)	req1->m_vecCode.insert(req1->m_vecCode.end(), cse->m_vec_code[k]);
						if(k%5==1)	req2->m_vecCode.insert(req2->m_vecCode.end(), cse->m_vec_code[k]);
						if(k%5==2)	req3->m_vecCode.insert(req3->m_vecCode.end(), cse->m_vec_code[k]);
						if(k%5==3)	req4->m_vecCode.insert(req4->m_vecCode.end(), cse->m_vec_code[k]);
						if(k%5==4)	req5->m_vecCode.insert(req5->m_vecCode.end(), cse->m_vec_code[k]);
					}
					//req1->m_vecCode = cse->m_vec_code;

					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req2);
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req3);
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req4);
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req5);

					delete cse;
				}
			}
			else if(parser.model=="CCalcSohuMainIncomeCompoDownload")
			{
				CCalcSohuMainIncomeCompoDownload * cse = (CCalcSohuMainIncomeCompoDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_INCOME_COMPOSITION_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Income_Composition;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}

				
			}
			else if(parser.model=="CCalcCsiPeDailyDownload")
			{
				CCalcCsiPeDailyDownload * cse = (CCalcCsiPeDailyDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CSINDEX_PE_DAILY_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_2;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}
			}
			else if(parser.model=="CCalcCsiCsrcPeDailyDownload")
			{
				CCalcCsiCsrcPeDailyDownload * cse = (CCalcCsiCsrcPeDailyDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CSINDEX_CSRC_PE_DAILY_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_2;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}
			}
			else if(parser.model=="CCalcSipfSettlementFundDownload")
			{
				CCalcSipfSettlementFundDownload * cse = (CCalcSipfSettlementFundDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_SIPF_SETTLEMENT_FUND_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cfi_Bonus;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}

			}
			else if(parser.model=="CCalcSseMarginTradingDownload")
			{
				CCalcSseMarginTradingDownload * cse = (CCalcSseMarginTradingDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_SSE_MARGIN_TRADING_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cfi_Bonus;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);	

					delete cse;
				}

			}
			else if(parser.model=="CCalcSzseMarginTradingDownload")
			{
				CCalcSzseMarginTradingDownload * cse = (CCalcSzseMarginTradingDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_SZSE_MARGIN_TRADING_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_Cfi_Bonus;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}

			}
			else if(parser.model=="CCalcCsdcInvestorStatDownload")
			{
				CCalcCsdcInvestorStatDownload * cse = (CCalcCsdcInvestorStatDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CSDC_INVESTOR_STAT_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_CSDC;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}

			}
			else if(parser.model=="CCalcCsdcSecurityIssueStatDownload")
			{
				CCalcCsdcSecurityIssueStatDownload * cse = (CCalcCsdcSecurityIssueStatDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_CSDC_SECURITY_ISSUE_STAT_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_CSDC;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);	

					delete cse;
				}

			}
			else if(parser.model=="CCalcSzseIndexDailyDownload")
			{
				CCalcSzseIndexDailyDownload * cse = (CCalcSzseIndexDailyDownload *)CCalculateTask::FetchTask(atol(parser.param1.c_str()));
				if(cse!=NULL)
				{
					CDownloadRequest * req1 = new CDownloadRequest;
					req1->m_nDownloadType = ID_DOWNLOAD_SZSE_INDEX_DAILY_SELECTED;
					req1->m_hwnd = NULL;
					req1->m_nLogType = WM_USER_LOG_2;
					req1->m_vecCode = cse->m_vec_code;
					((CStockApp *)AfxGetApp())->StartWorkerThread(AutoDownloadThreadFunc,req1);

					delete cse;
				}
			}
			
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		}

	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	最小化到托盘代码

void CMainFrame::ToTray()
{ 
	NOTIFYICONDATA nid;
	nid.cbSize=sizeof(NOTIFYICONDATA);
	nid.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nid.uID = IDR_MAINFRAME;
	nid.hWnd=m_hWnd;
	lstrcpy(nid.szTip,_T("A股分析"));
	nid.uCallbackMessage= WM_NOTIFY_FROM_TRAY;
	nid.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD,&nid);
}

LRESULT CMainFrame::OnNotifyIcon(WPARAM wParam,LPARAM IParam)
{
	if ((IParam == WM_LBUTTONDOWN) || (IParam == WM_RBUTTONDOWN))
	{ 
		//ModifyStyleEx( WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
		ModifyStyleEx(0,WS_EX_TOPMOST);
		ShowWindow(SW_SHOW);
		this->UpdateWindow();
		//Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);//消除托盘图标
	}
	return 0;
}

void CMainFrame::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd=this->m_hWnd;
	nid.uID=IDR_MAINFRAME;
	nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage= WM_NOTIFY_FROM_TRAY;			//自定义的消息名称
	nid.hIcon= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	strcpy(nid.szTip,"A股分析");				//信息提示条为“计划任务提醒”
	Shell_NotifyIcon(NIM_DELETE,&nid);			//在托盘区删除图标
}


void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(nID)
	{
	case SC_MINIMIZE:
		ShowWindow(SW_HIDE);
		break;
	default:
		CMDIFrameWndEx::OnSysCommand(nID, lParam);
		break;
	}

}

void CMainFrame::OnDestroy()
{
	CMDIFrameWndEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	DeleteTray();
}



//////////////////////////////////////////////////////////////////////////

void CMainFrame::OnSouhuPlate()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CBaseHtmlView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc(); //strCarMunber);
	CString title;
	title.Format("板块分类-%s", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnAllStockList()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("所有股票列表-%s", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}


void CMainFrame::OnAllStockDownload()
{
	// TODO: 在此添加命令处理程序代码
	
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAutoDownloadView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CIndividualShareDoc * pNewDoc = new CIndividualShareDoc(); //strCarMunber);
	pNewDoc->m_nCommandCode = -1;
	pNewDoc->m_nCode = "-1";
	pNewDoc->m_vecCommandCode.clear();

	CString title;
	title.Format("资料下载-%s", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnZombieStockList()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CZombieStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("僵尸股票排序-%s", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMainProductQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMainProductQueryView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CSimpleInputBox dlg;
	dlg.m_strCaption = _T("输入主营产品模糊名称");
	if(IDOK==dlg.DoModal() && dlg.m_strInput.IsEmpty()==false )
	{
		CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
		((CIndividualShareDoc *)pNewDoc)->m_strInput = dlg.m_strInput.GetBuffer();
		CString title;
		title.Format("主营产品查询-%s",  dlg.m_strInput);
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	

	}

	
}

void CMainFrame::OnPeStockList()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CPeStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CIndividualShareDoc * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	pNewDoc->m_strInput = "pe";
	CString title;
	title.Format("PE排序 - 2012年开始计算");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	

}

void CMainFrame::OnMacdStockList()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMacdStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("MACD 买卖分析");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnHexunResearchPaperQuery()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CHexunResearchPaperListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("和讯研报查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMainIncomeYearGrowthQuery()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllMainIncomeYearGrowthStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("主营收入逐年增长排序", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMainProfitYearGrowthQuery()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllMainProfitYearGrowthStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("主营利润逐年增长排序", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnRecentBonusCashQuery()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllRecentBonusCashStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("近期分红收益排序", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnThisYearBonusPlanQuery()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllThisYearBonusListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("今年分红预案查询", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMcstStockList()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllMcstStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("股票成本价 MCST 排序", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnAddIssuanceStockList()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CAllAddIssuanceListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("股票增发盈亏汇总", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnNewmat11Test()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CNewmat11TestView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("Newmat11 测试", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMarketExchangeFundInSettlement()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMarketExchageFundView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("证券交易结算资金", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMarketMarginGuaranteeFund()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMarketMarginGuaranteeFundView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("融资融券担保资金", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMarketCffexHs300Future()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMarketCffexHs300FutureView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("沪深300指数及股指期货", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMarketIndex()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMarketSohuIndexView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("大盘指数", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnMarketInvestorStat()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMarketCsdcInvestorStatView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("投资者情况统计", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnStatsgovHgydMenu(UINT nID)
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CStatsGovMenuDlg dlg;
	if(nID==ID_STATSGOV_HGYD_MENU)		dlg.m_str_db_code = "hgyd";
	if(nID==ID_STATSGOV_HGJD_MENU)		dlg.m_str_db_code = "hgjd";
	if(nID==ID_STATSGOV_HGND_MENU)		dlg.m_str_db_code = "hgnd";
	if(nID==ID_STATSGOV_FSYD_MENU)		dlg.m_str_db_code = "fsyd";
	if(nID==ID_STATSGOV_FSJD_MENU)		dlg.m_str_db_code = "fsjd";
	if(nID==ID_STATSGOV_FSND_MENU)		dlg.m_str_db_code = "fsnd";
		
	if(IDOK==dlg.DoModal() && dlg.m_vec_zb_code.size() > 0)
	{
		if(dlg.m_nOutput==0)
		{
			CStatsGovCnGridView /*CStatsGovCnChartView*/	MonitorListView;
			pView = &MonitorListView;				
			pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);
		}
		else
		{
			CStatsGovCnChartView	MonitorListView;
			pView = &MonitorListView;				
			pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);
		}

		CDocument * pNewDoc = new CStatsGovCnDoc(); //strCarMunber);
		((CStatsGovCnDoc *)pNewDoc)->m_str_db_code = dlg.m_str_db_code;
		((CStatsGovCnDoc *)pNewDoc)->m_vec_zb_code = dlg.m_vec_zb_code;
		((CStatsGovCnDoc *)pNewDoc)->m_vec_reg_code = dlg.m_vec_reg_code;
		((CStatsGovCnDoc *)pNewDoc)->m_nOutput = dlg.m_nOutput;

		CString title;
		title.Format("经济分析",  "");
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	

	}

}


void CMainFrame::OnFinanceSalesFromOperations(UINT nID)
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;


	CStockGroupDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		CStatsGovCnChartView	MonitorListView;
		pView = &MonitorListView;				
		pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		CStatsGovCnDoc * pNewDoc = new CStatsGovCnDoc(); //strCarMunber);


		if(nID==ID_FINANCE_SALES_FROM_OPERATIONS)		
		{
			pNewDoc->m_str_db_code = "incomestatement";
		}
		if(nID==ID_FINANCE_COST_OF_OPERATIONS)
		{
			pNewDoc->m_str_db_code = "incomestatement";
		}
	
		pNewDoc->m_vec_zb_code.clear();
		if(nID==ID_FINANCE_SALES_FROM_OPERATIONS)
			pNewDoc->m_vec_zb_code.push_back("colume_2");
		if(nID==ID_FINANCE_COST_OF_OPERATIONS)
			pNewDoc->m_vec_zb_code.push_back("colume_9");

		pNewDoc->m_vec_csrc_gate_code.clear();
		for(int i = 0 ; i<dlg.m_vecSelectedItemCsrcGate.size(); i++)
		{
			vector<string> firstMatch;
			string strPattern = string("\\([A-Za-z\\d]+\\)");
			if(Pcre2Grep(strPattern.c_str(), dlg.m_vecSelectedItemCsrcGate[i].c_str(), firstMatch )>0)
			{
				string::size_type pos1 = firstMatch[0].find(")");
				string codeStr = firstMatch[0].substr(1, pos1-1);
				pNewDoc->m_vec_csrc_gate_code.push_back(codeStr);
			}
		}
		
		pNewDoc->m_vec_csrc_big_code.clear();
		for(int i = 0 ; i<dlg.m_vecSelectedItemCsrcBig.size(); i++)
		{
			vector<string> firstMatch;
			string strPattern = string("\\([A-Za-z\\d]+\\)");
			if(Pcre2Grep(strPattern.c_str(), dlg.m_vecSelectedItemCsrcBig[i].c_str(), firstMatch )>0)
			{
				string::size_type pos1 = firstMatch[0].find(")");
				string codeStr = firstMatch[0].substr(1, pos1-1);
				pNewDoc->m_vec_csrc_big_code.push_back(codeStr);
			}
		}

		pNewDoc->m_vec_reg_code.clear();
		for(int i = 0 ; i<dlg.m_vecSelectedItemReg.size(); i++)
		{
			vector<string> firstMatch;
			string strPattern = string("\\([A-Za-z\\d]+\\)");
			if(Pcre2Grep(strPattern.c_str(), dlg.m_vecSelectedItemReg[i].c_str(), firstMatch )>0)
			{
				string::size_type pos1 = firstMatch[0].find(")");
				string codeStr = firstMatch[0].substr(1, pos1-1);
				pNewDoc->m_vec_reg_code.push_back(codeStr);
			}
		}

		CString title;
		title.Format("经济分析",  "");
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
		pDocTemplate->AddDocument(pNewDoc);	

	}
}

void CMainFrame::OnCninfoAnnouncementQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCninfoAnnouncementListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("巨潮网公告查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}


void CMainFrame::OnPeGrossList()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CPeStockListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CIndividualShareDoc * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	pNewDoc->m_strInput = "pe_gross";
	CString title;
	title.Format("毛利润 PE排序 - 2012年开始");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	

}

void CMainFrame::OnCninfoPdfQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCninfoAnnouncePDFListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("巨潮网 PDF 分析");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnShanghaiIndex()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CShanghaiIndexView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("1", ID_SHANGHAI_INDEX); //strCarMunber);
	CString title;
	title.Format("上证指数");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnCninfoIncentivePlanQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCninfoIncentivePlanListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("巨潮网股权激励查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnHushenChangeRateQuery()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CPlateShanghaiIndexView	 MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);
	CIndividualShareDoc * pNewDoc = new CIndividualShareDoc("", 1, 1); //strCarMunber);

	/*所有股票的概况列表*/
	CEntrepreneurship Entrepreneurship ;
	CBussinessUtils::GetEntrepreneurship(Entrepreneurship);
	for(int i=0; i< Entrepreneurship.m_nRowNum; i++)
	{
		pNewDoc->m_vec_request_code.push_back(Int2String(Entrepreneurship.m_vec_code[i], "%06d"));
	}

	CSmallMidEnterprise midsmall ;
	CBussinessUtils::GetSmallMidEnterprise(midsmall);
	for(int i=0; i< midsmall.m_nRowNum; i++)
	{
		pNewDoc->m_vec_request_code.push_back(Int2String(midsmall.m_vec_code[i], "%06d"));
	}

	CShenzhenAshares shenzhenA ;
	CBussinessUtils::GetShenzhenAshares(shenzhenA);
	for(int i=0; i< shenzhenA.m_nRowNum; i++)
	{
		pNewDoc->m_vec_request_code.push_back(Int2String(shenzhenA.m_vec_code[i], "%06d"));
	}

	CShanghaiStockInfo shanghai ;
	CBussinessUtils::GetShanghaiStockInfo(shanghai);
	for(int i=0; i< shanghai.m_nRowNum; i++)
	{
		pNewDoc->m_vec_request_code.push_back(Int2String(shanghai.m_vec_code[i], "%06d"));
	}

	pNewDoc->m_nIs_Stock = 1;
	pNewDoc->m_strInput = "Table";
	CString title;
	title.Format("沪深股市涨跌查询-%s", "");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnSmallMiddleIndex()
{

	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CShanghaiIndexView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("399101", ID_SMALL_MIDDLE_INDEX); //strCarMunber);
	CString title;
	title.Format("中小板综指");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnCninfoGetPlanQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCninfoGetLandListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("巨潮网竞得土地查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnEnterprenuerIndex()
{
	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CShanghaiIndexView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("399102", ID_ENTERPRENUER_INDEX); //strCarMunber);
	CString title;
	title.Format("创业板综指");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnCninfoShareHoldingQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCninfoShareHoldingListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("前10名股东持股查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnCninfoShareHoldingStatistics()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCninfoShareHoldingStatisticsView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("前10名股东持股统计");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);	
}

void CMainFrame::OnCandlesticksJapanQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CCandlesticksJapanListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("蜡烛图查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);
}

void CMainFrame::OnMacdQuery()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CMacdListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("MACD查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);
}

void CMainFrame::OnBollQuery()
{
	// TODO: 在此添加命令处理程序代码


	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	CBollListView	MonitorListView;

	pView = &MonitorListView;				
	pDocTemplate = ((CStockApp *)AfxGetApp())->GetCurrDocTemplate(pView);

	pView = NULL;
	ASSERT_VALID(pDocTemplate);

	CDocument * pNewDoc = new CIndividualShareDoc("-1", ID_ALL_STOCK_LIST); //strCarMunber);
	CString title;
	title.Format("BOLL查询");
	pNewDoc->SetTitle(title);
	CFrameWnd * wnd= pDocTemplate->CreateNewFrame(pNewDoc,NULL);//create view
	wnd->ShowWindow(SW_SHOWMAXIMIZED);
	pDocTemplate->InitialUpdateFrame(wnd,pNewDoc,TRUE);
	pDocTemplate->AddDocument(pNewDoc);
}
