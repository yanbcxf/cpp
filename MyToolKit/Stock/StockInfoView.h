#pragma once

#include "ViewTree.h"

// CStockInfoView


class CEntrepreneurToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CStockInfoView : public CDockablePane
{
	DECLARE_DYNAMIC(CStockInfoView)

public:
	CStockInfoView();
	virtual ~CStockInfoView();

	void AdjustLayout();
	void SetStockInfo(CStockInfo * pStockInfo){ m_pStockInfo = pStockInfo; }
	void FillClassView();
	bool FocusOn(int nCode);

	static string	m_strSelectedCode;
	static string	m_strSelectedItemText;

protected:
	CEntrepreneurToolBar m_wndToolBar;
	CViewTree m_wndClassView;

	CStockInfo * m_pStockInfo;

	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnStockinfoOpen();
	afx_msg void OnIndividualSummary();
	afx_msg void OnOverallCost();
	afx_msg void OnIndividualSzsePe();
	afx_msg void OnIndividualVolume();
	afx_msg void OnIndividualNetProfits();
	afx_msg void OnBalanceCurrentAsset();
	afx_msg void OnBalanceNonCurrentAsset();
	afx_msg void OnDownloadIndividual();
	afx_msg void OnIndividualCapitalStruct();
	afx_msg void OnIndividualCsvIncome();
	afx_msg void OnIndividualCsvTrading();
	afx_msg void OnIndividualCsvBalance();
	afx_msg void OnIndividualCapitalFlow();
	afx_msg void OnIndividualCapitalflowValue();
	afx_msg void OnCashflowSummary();
	afx_msg void OnIndividualCsvCashflows();
	afx_msg void OnIndividualAddToSelfSelect();
	afx_msg void OnIndividualCsvFundHold();
	afx_msg void OnFindStockByCode();
	afx_msg void OnIndividualZombieAnalysis();
	afx_msg void OnIndividualFundHoldValueAnalysis();
	afx_msg void OnIndividualMacd();
	afx_msg void OnIndiMainIncomeGrowth();
	afx_msg void OnIndividualMcst();
	afx_msg void OnIndi10jqkaBonus();
	afx_msg void OnIndiHexunResearchPaperEps();
	afx_msg void OnIndiMarketValueIndexAnalysis();
	afx_msg void OnIndiCsindexAnalysis();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnIndiGrossProfitRate();
	afx_msg void OnIndiAddInsuranceRoadmap();
	afx_msg void OnIndiImportantEventRoadmap();
	afx_msg void OnIndiQuarterReportRoadmap();
	afx_msg void OnIndiTradeRoadmap();
	afx_msg void OnIndiSpecialDisposalRoadmap();
	afx_msg void OnIndiSupplementCorrectionRoadmap();
	afx_msg void OnIndiPeAndValueDaily();
	afx_msg void OnIndiOriginalIssueRoadmap();
	afx_msg void OnIndiIncomestatementForecast();
	afx_msg void OnIndiDmi();
	afx_msg void OnIndiTechAnalysisRoadmap();
	afx_msg void OnIndiSimulatedAccountQuery();
};


