#pragma once


// CAutoDownloadView 视图

class CAutoDownloadView : public CEditView
{
	DECLARE_DYNCREATE(CAutoDownloadView)

protected:
public:
	CAutoDownloadView();           // 动态创建所使用的受保护的构造函数
	virtual ~CAutoDownloadView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAutoDownloadList();

	afx_msg void OnDownloadStockinfoShenzhenAshare();
	afx_msg void OnDownloadStockinfoSmallandmid();
	afx_msg void OnDownloadStockinfoEnterpreneur();
	afx_msg void OnDownloadTradingdailyEnterprenuer();
	afx_msg void OnDownloadFinanceEnterprenuer();

	afx_msg LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDownloadStockinfoShanghai();
	afx_msg void OnDownloadTradingdailyShenzhenAshare();
	afx_msg void OnDownloadTradingdailySmallAndMid();
	afx_msg void OnDownloadTradingdailyShanghaistock();
	afx_msg void OnUpdateDownloadTradingdailyShanghaistock(CCmdUI *pCmdUI);

private:
	void StartupDownloadThread(int downloadType);
	void OnUpdateCmdUIDownload(CCmdUI *pCmdUI);

private:
	//	记录是否处在“下载中......”的线程句柄
	HANDLE m_bDownloading;

	//	下载完成后是否自动关闭
	bool	m_bAutoClose;

	CFont m_editFont;

public:
	afx_msg void OnUpdateDownloadTradingdailyShenzhenAshare(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadTradingdailySmallAndMid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadTradingdailyEnterprenuer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadFinanceEnterprenuer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadFinanceShanghai(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadFinanceSmallAndMid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadFinanceShenzhenAshare(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadCapitialstructShanghai(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadStockinfoShenzhenAshare(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadStockinfoSmallandmid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadStockinfoEnterpreneur(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadStockinfoShanghai(CCmdUI *pCmdUI);
	afx_msg void OnDownloadFinanceShenzhenAshare();
	afx_msg void OnDownloadFinanceSmallAndMid();
	afx_msg void OnDownloadFinanceShanghai();
	afx_msg void OnDownloadCapitialstructShanghai();
	afx_msg void OnDownloadCapitialstructShenzhen();
	afx_msg void OnUpdateDownloadCapitialstructShenzhen(CCmdUI *pCmdUI);
	virtual void OnInitialUpdate();
	afx_msg void OnDownloadSouhuStockPlate();
	afx_msg void OnUpdateDownloadSouhuStockPlate(CCmdUI *pCmdUI);
	afx_msg void OnDownloadCapitalflowShenzhenAshare();
	afx_msg void OnDownloadCapitalflowSmallAndMid();
	afx_msg void OnDownloadCapitalflowEnterprenuer();
	afx_msg void OnDownloadCapitalflowShanghaistock();
	afx_msg void OnUpdateDownloadCapitalflowShenzhenAshare(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadCapitalflowSmallAndMid(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadCapitalflowEnterprenuer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDownloadCapitalflowShanghaistock(CCmdUI *pCmdUI);
	afx_msg void OnDownloadPerformanceForecast();
	afx_msg void OnUpdateDownloadPerformanceForecast(CCmdUI *pCmdUI);
	afx_msg void OnDownloadIncomeComposition();
	afx_msg void OnUpdateDownloadIncomeComposition(CCmdUI *pCmdUI);
	afx_msg void OnDownloadFundHoldShenzhenAshare();
	afx_msg void OnUpdateDownloadFundHoldShenzhenAshare(CCmdUI *pCmdUI);
	afx_msg void OnDownloadFundHoldEnterprenuer();
	afx_msg void OnUpdateDownloadFundHoldEnterprenuer(CCmdUI *pCmdUI);
	afx_msg void OnDownloadFundHoldSmallAndMid();
	afx_msg void OnUpdateDownloadFundHoldSmallAndMid(CCmdUI *pCmdUI);
	afx_msg void OnDownloadFundHoldShanghai();
	afx_msg void OnUpdateDownloadFundHoldShanghai(CCmdUI *pCmdUI);
	afx_msg void OnDownloadHexunResearchPaper();
	afx_msg void OnUpdateDownloadHexunResearchPaper(CCmdUI *pCmdUI);
	afx_msg void OnDownloadHusheng300Index();
	afx_msg void OnUpdateDownloadHusheng300Index(CCmdUI *pCmdUI);
	afx_msg void OnDownloadCffexFutureDaily();
	afx_msg void OnUpdateDownloadCffexFutureDaily(CCmdUI *pCmdUI);
	afx_msg void OnDownloadStatsgovHgyd();
	afx_msg void OnUpdateDownloadStatsgovHgyd(CCmdUI *pCmdUI);
	afx_msg void OnDownloadStatsgovHgjd();
	afx_msg void OnUpdateDownloadStatsgovHgjd(CCmdUI *pCmdUI);
	afx_msg void OnDownloadStatsgovHgnd();
	afx_msg void OnUpdateDownloadStatsgovHgnd(CCmdUI *pCmdUI);
	afx_msg void OnDownloadStatsgovFsyd();
	afx_msg void OnUpdateDownloadStatsgovFsyd(CCmdUI *pCmdUI);
	afx_msg void OnDownloadStatsgovFsjd();
	afx_msg void OnUpdateDownloadStatsgovFsjd(CCmdUI *pCmdUI);
	afx_msg void OnDownloadStatsgovFsnd();
	afx_msg void OnUpdateDownloadStatsgovFsnd(CCmdUI *pCmdUI);
	afx_msg void OnDownloadCninfoAnnouncement();
	afx_msg void OnUpdateDownloadCninfoAnnouncement(CCmdUI *pCmdUI);
};


