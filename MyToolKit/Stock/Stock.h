
// Stock.h : Stock 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CStockApp:
// 有关此类的实现，请参阅 Stock.cpp
//

class CMvDocTemplate : public CMultiDocTemplate
{
public:
	CMvDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

	virtual ~CMvDocTemplate(){}

	CRuntimeClass * GetCurrView(){return m_pViewClass;}
};

class CStockApp : public CWinAppEx
{
public:
	CStockApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	CDocTemplate* GetCurrDocTemplate(CView * pView);

	BOOL TestRequestExit();

	CWinThread * StartWorkerThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam);

	// 深圳股票信息
	CEntrepreneurship m_CEntrepreneurship;
	CSmallMidEnterprise m_CSmallMidEnterprise;
	CShenzhenAshares m_CShenzhenAshares;

	// 上海股票信息
	CShanghaiStockInfo m_CShanghaiStock;

	//	搜狐股票板块
	CStockPlateTree *		m_pCSouhuStockPlates;
	vector<CStockPlateData>	m_vecStockPlateArea;
	vector<CStockPlateData>	m_vecStockPlateConcept;
	vector<CStockPlateData>	m_vecStockPlateIndustry;

	//	自定义股票板块
	CSelfSelectStockPlates * m_pCSelfSelectStockPlates;

	//	深圳股票指数板块
	CShenzhenIndexList	 m_ShenzhenIndexList;

	//	中国证券指数公司板块
	CStockPlateTree *  m_pCsindexStockPlates;

	//	中国证券指数 证监会行业板块
	CStockPlateTree *  m_pCsindexCsrcStockPlates;

	//	国家统计局 ，月度数据板块
	CStockPlateTree *  m_pCStatsGovHgydPlates;

	//	国家统计局， 季度数据板块
	CStockPlateTree *  m_pCStatsGovHgjdPlates;

	//	国家统计局， 年度数据板块
	CStockPlateTree *  m_pCStatsGovHgndPlates;

	//	国家统计局 ，分省月度数据板块
	CStockPlateTree *  m_pCStatsGovFsydPlates;

	//	国家统计局， 分省季度数据板块
	CStockPlateTree *  m_pCStatsGovFsjdPlates;

	//	国家统计局， 分省年度数据板块
	CStockPlateTree *  m_pCStatsGovFsndPlates;

	//	最晚的交易日的深圳股票行情
	CShenzhenMarketQuotation m_CShenzhenMarketQuotation;

	//	特定股票的数据池
	map<int, CNetEaseTradeDaily>	m_mapTradingDaily;
	map<int, CCninfoCashFlows>	m_mapCashFlows;
	map<int, CCninfoBalance>		m_mapBalance;
	map<int, CCninfoIncomeStatement>	m_mapIncomeStatement;
	map<int, CCapitalStructOf10jqka>	m_mapCapitalStruct;
	map<int, CShenzhenMarketQuotation>	m_mapShenzhenQuotation;
	map<int, CCapitalFlows163>	m_mapCapitalFlows;
	map<int, CSohuPerformanceForecast>	m_CSohuPerformanceForecast;
	map<int, CSohuFundHold>		m_mapFundHold;

	//	所有股票的 数据更新记录
	CStockDataLog	m_StockDataLog;

	// map<int, double>	m_mapClosePrice;

	//	统一管理“后台的工作线程”
	vector<CWinThread *>  m_vec_pThread;

	HANDLE	m_bAppExit;

	
	list<StockInfo_Reqeust>		m_listStockinfoReq;	
		
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	
	virtual int ExitInstance();

public:
	void PostCalculateTask(void * clientData, CalculateTaskFunc * proc);
	void* FetchCalculateResult(int taskHandle);
	virtual BOOL OnIdle(LONG lCount);

protected:
	TCHAR  theAppPath[MAX_PATH];
};

extern CStockApp theApp;
