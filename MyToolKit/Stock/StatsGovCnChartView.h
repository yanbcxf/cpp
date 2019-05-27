#pragma once
#include "BaseChartCtlView.h"

// CStatsGovCnChartView 视图

class CStatsGovCnChartView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CStatsGovCnChartView)

protected:
public:
	CStatsGovCnChartView();           // 动态创建所使用的受保护的构造函数
	virtual ~CStatsGovCnChartView();

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

protected:
	virtual void ProccessChartMenu();


public:
	void RedrawDmGraph();
	
	virtual void OnInitialUpdate();

	void PostFocusChanged();

protected:
	CStatsGovCnDoc  m_LocalDoc;
public:
	afx_msg void OnFinanceSalesFromOperations(UINT nID);
	afx_msg void OnStatsgovHgydMenu(UINT nID);

};


