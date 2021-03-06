#pragma once
#include "basegridctlview.h"

// CLotteryNumberGridView 视图

class CLotteryNumberGridView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CLotteryNumberGridView)

protected:
public:
	CLotteryNumberGridView();           // 动态创建所使用的受保护的构造函数
	virtual ~CLotteryNumberGridView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

private:
	void RedrawGridCtrl(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType, string code, string model, string param1, string param2, string param3);
	int GoForwardByOffset(vector<LotteryNumber> & vecln, int periods, int offset, int & index);
	void AssignmentAnalysis(string aa, int analysis_periods);

public:
	virtual void OnInitialUpdate();
	virtual void PostGridDblClick(int nRow);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLotteryRangeSet();
	afx_msg void OnVarAnalysis();
	afx_msg void OnStdevAnalysis();
	afx_msg void OnDevsqAnalysis();
	afx_msg void OnKurtAnalysis();
	afx_msg void OnSkewAnalysis();
	afx_msg void OnSumAnalysis();
	afx_msg void OnOddEvenAnalysis();
	afx_msg void OnConsecutiveNumberAnalysis();
	afx_msg void OnFredParamSet();
	afx_msg void OnRegionNumberSet();
	afx_msg void OnSaveAsExcel();
	afx_msg void OnAdjacentNumberAnalysis();
	afx_msg void OnRegionAnalysis();
	afx_msg void OnAcAnalysis();
	afx_msg void OnGroupQtyAnalysis();
	afx_msg void OnFringeNoQtyAnalysis();
	afx_msg void OnFantasticAnalysis();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnRun();
	afx_msg void OnBasicCharacterSetup();
	afx_msg void OnRegionCharacterSetup();
	afx_msg void OnStatisticsCharacterSetup();
	afx_msg void OnAdvancedCharacterSetup();
	afx_msg void OnConsecutiveCharacterSetup();
	afx_msg void OnTailCharacterSetup();
	afx_msg void OnLotteryNumberList();
	afx_msg void OnTrendDistributionView();
	afx_msg void OnLeakageAwardQuery();
	afx_msg void OnWinLotteryQuery();
	afx_msg void OnSystemFilterSetup();
	afx_msg void OnDataSourceSetup();
	afx_msg void OnUpdateRun(CCmdUI *pCmdUI);

	void LeakageAwardProcess(int number[]);
	void AssignmentFilterLog(map<int, Assignment> mapPai, string type, LotteryNumber ln);
	void WinLotteryProcess(int number[]);
	afx_msg void OnPaiSetup();
	afx_msg void OnLnPaiSetup();
	afx_msg void OnSwingPowerSetup();
	afx_msg void OnLnSwingPowerSetup();
	afx_msg void OnBernoulliSetup();
	afx_msg void OnPotentialSetup();
	afx_msg void OnPaiAnalysis();
	afx_msg void OnFilterSourceSetup();
	afx_msg void OnLnPaiAnalysis();
	afx_msg void OnSwingPowerAnalysis();
	afx_msg void OnLnSwingPowerAnalysis();
	afx_msg void OnBernoulliAnalysis();
	afx_msg void OnPotentialAnalysis();
};


