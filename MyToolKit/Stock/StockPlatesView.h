#pragma once

#include "ViewTree.h"

// CStockPlatesView

class CStockPlatesToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CStockPlatesView : public CDockablePane
{
	DECLARE_DYNAMIC(CStockPlatesView)

public:
	CStockPlatesView();
	virtual ~CStockPlatesView();

	void AdjustLayout();
	void FillClassView();
	
	/* 找到第一个节点，直接返回 */
	HTREEITEM  TreeTravesal(HTREEITEM hStart, string strCode);

	/* 遍历所有的节点，查找所有匹配的节点 */
	void	   TreeTravesal(HTREEITEM hStart, string strCode, vector<HTREEITEM> & vecHtreeitem);

	bool FocusOn(int nCode, int is_stock);

	void FindAllOnTree(int nCode, int is_stock);
	bool FocusOnNext();
	void SetFindTarget(int nCode, int is_stock)
	{
		m_nCodeFound = nCode;
		m_nIsStockFound = is_stock;

		/* -1 : 还未在树中查找， >=0 ：已经查找 */
		m_nNextFound = -1;
	}

	void SetPlateCode(int nPlateType, int nPlateCode)
	{  
		m_nPlateType = nPlateType;
		m_nPlateCode = nPlateCode; 
	}

protected:
	CStockPlatesToolBar m_wndToolBar;
	CViewTree m_wndClassView;

	/* 保存找到的所有匹配节点 */
	vector<HTREEITEM>	m_vecFound;		
	int	m_nNextFound;
	int	m_nCodeFound;
	int m_nIsStockFound;

	/* 板块类型 */
	int m_nPlateCode;
	int m_nPlateType;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnPaint();
	afx_msg void OnPlateStockList();
	afx_msg void OnPlateGrossProfit();
	afx_msg void OnPlateAddStock();
	afx_msg void OnPlateDelStock();
	afx_msg void OnUpdatePlateAddStock(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlateDelStock(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePlateAddGroup(CCmdUI *pCmdUI);
	afx_msg void OnPlateLiquidityRatio();
	afx_msg void OnPlateNetprofitsRatio();
	afx_msg void OnTotalAssetsTurnoverRatio();
	afx_msg void OnFixedAssetsTurnoverRatio();
	afx_msg void OnCurrentAssetsTurnoverRatio();
	afx_msg void OnInventoryTurnoverRatio();
	afx_msg void OnDebtToAssetsRatio();
	afx_msg void OnDebtToCapitalRatio();
	afx_msg void OnFundHoldRatio();
	afx_msg void OnTotalAssetsGrowth();

	afx_msg void OnNetFixedAssetsGrowth();
	afx_msg void OnMainIncomeGrowth();
	afx_msg void OnPlateMainIncomeYearGrowth();
	afx_msg void OnPlateResearchPaperQuery();
	afx_msg void OnPlateZombieStockList();
	afx_msg void OnPlatePeStockList();
	afx_msg void OnPlateMainProfitYearGrowth();
	afx_msg void OnPlateClearStock();
	afx_msg void OnUpdatePlateClearStock(CCmdUI *pCmdUI);
	afx_msg void OnPlateBonusPlanQuery();
	afx_msg void OnPlateMcstStockList();
	afx_msg void OnPlateAddIssuanceStockList();
	afx_msg void OnPlateCorrectedMarketValue();
	afx_msg void OnPlateIndustryDynamicPe();
	afx_msg void OnPlateIndustryStaticPe();
	afx_msg void OnPlateIndustryPb();
	afx_msg void OnPlateIndustryCompanyNumber();
	afx_msg void OnPlateDividendYieldRatio();
	afx_msg void OnPlateCsiDynamicPeStockList();
	afx_msg void OnPlateDealDetailDaily();
	afx_msg void OnPlateTurnoverRateDaily();
	afx_msg void OnFocusNextOnPlate();
	afx_msg void OnPlateChangeRateSummary();
	afx_msg void OnPlateGrossProfitRateQuarterGrowth();
	afx_msg void OnPlateGrossProfitRateChangeForLast();
	afx_msg void OnPlateGrossProfitChangeForAverage();
	afx_msg void OnPlateGrossProfitRate4QuarterGrowth();
	afx_msg void OnPlateCninfoAnnouncementQuery();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPlateCsiStaticPeStockList();
	afx_msg void OnPlateCninfoPdfQuery();
	afx_msg void OnPlateShanghaiIndex();
	afx_msg void OnPlateCninfoGetLandQuery();
	afx_msg void OnPlateCninfoIncentivePlanQuery();
	afx_msg void OnPlateCninfoShareHoldingQuery();
};


