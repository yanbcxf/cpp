
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "StockInfoView.h"
#include "StockPlatesView.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	CPropertiesWnd    m_wndProperties;

	COutputWnd        m_wndOutput;
	CComboBoxExt	  m_comboBox;	//	股票代码输入框

public:
	CSplashScreenEx	* m_pWndSplashScreen;	//	下载股票基础资料的启动窗口


public:
	CStockInfoView m_wndEntrepreneurView;
	CStockInfoView m_wndSmallMidEnterpriseView;   
	CStockInfoView m_wndShenzhenAsharesView;
	CStockInfoView m_wndShanghaiStockView;

	//	行业分类、地域板块、概念板块
	CStockPlatesView m_wndHangyePlatesView;
	CStockPlatesView m_wndDiyuPlatesView;
	CStockPlatesView m_wndGainianPlatesView;

	//	自选板块
	CStockPlatesView m_wndSelfSelectPlatesView;

	//	中证公司行业分类
	CStockPlatesView m_wndCsindexPlatesView;

	//	中证公司 证监会行业分类
	CStockPlatesView m_wndCsindexCsrcPlatesView;

	//	深圳股票指数
	CStockPlatesView m_wndShenzhenIndexPlatesView;


// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);

	afx_msg LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	/*afx_msg LRESULT OnUserLogWParam_2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserLogWParam_3(WPARAM wParam, LPARAM lParam);*/
	afx_msg LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT	OnNotifyIcon(WPARAM wParam,LPARAM lParam);
	void ToTray();
	void DeleteTray();

	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);


public:
	afx_msg void OnSouhuPlate();
	afx_msg void OnAllStockList();
	//afx_msg void OnShanghaiStockList();
	//afx_msg void OnShenzhenAStockList();
	//afx_msg void OnSmallMidStockList();
	//afx_msg void OnEntreprStockList();
	afx_msg void OnAllStockDownload();
	afx_msg void OnZombieStockList();
	afx_msg void OnMainProductQuery();
	afx_msg void OnPeStockList();
	afx_msg void OnMacdStockList();
	afx_msg void OnHexunResearchPaperQuery();
	afx_msg void OnMainIncomeYearGrowthQuery();
	afx_msg void OnMainProfitYearGrowthQuery();
	afx_msg void OnRecentBonusCashQuery();
	afx_msg void OnThisYearBonusPlanQuery();
	afx_msg void OnMcstStockList();
	afx_msg void OnAddIssuanceStockList();
	afx_msg void OnNewmat11Test();
	afx_msg void OnMarketExchangeFundInSettlement();
	afx_msg void OnMarketMarginGuaranteeFund();
	afx_msg void OnMarketCffexHs300Future();
	afx_msg void OnMarketIndex();
	afx_msg void OnMarketInvestorStat();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnStatsgovHgydMenu(UINT nID);
	afx_msg void OnFinanceSalesFromOperations(UINT nID);
	afx_msg void OnCninfoAnnouncementQuery();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPeGrossList();
	afx_msg void OnCninfoPdfQuery();
	afx_msg void OnShanghaiIndex();
	afx_msg void OnCninfoIncentivePlanQuery();
	afx_msg void OnHushenChangeRateQuery();
	afx_msg void OnSmallMiddleIndex();
	afx_msg void OnCninfoGetPlanQuery();
	afx_msg void OnEnterprenuerIndex();
	afx_msg void OnCninfoShareHoldingQuery();
	afx_msg void OnCninfoShareHoldingStatistics();
	afx_msg void OnCandlesticksJapanQuery();
	afx_msg void OnMacdQuery();
	afx_msg void OnBollQuery();
};


