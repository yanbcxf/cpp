
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口

class COutputList : public CListBox /*CMultiLineListBox*/
{
// 构造
public:
	COutputList();

// 实现
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();

	void AppendLog(LPCTSTR lpszItem, int logLevel);

// 属性
protected:
	CFont m_Font;

	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputBuild;
	COutputList m_wndOutputDebug;
	COutputList m_wndOutputTradingDaily;			//	每日交易行情
	COutputList m_wndOutputFinanceReport;			//	财务报表
	COutputList m_wndOutputCapitalFlow163;			//	每日资金流向	
	COutputList m_wndOutputForecast;				//	业绩预告
	COutputList m_wndOutputCapitalStruct;			//	同花顺 股本结构
	COutputList m_wndOutputResearchPaper;			//	和讯 研报
	COutputList m_wndOutputIncomeComposition;		//	主营业务构成
	COutputList m_wndOutputCfiBonus;				//	中财网配股增发、投资基金的银证交易金额、融资融券
	COutputList m_wndOutputCninfo;					//	巨潮网公告
	COutputList m_wndOutputStatsGovCn;				//	国家统计局
	COutputList m_wndOutputCSDC;					//	中国结算

protected:
	void FillBuildWindow();
	void FillDebugWindow();
	void FillFindWindow();


	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

