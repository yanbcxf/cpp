#pragma once

#include "BaseChartCtlView.h"
// CIndividualCapitalFlowView 视图

class CIndiDmiView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CIndiDmiView)

protected:
public:
	CIndiDmiView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndiDmiView();

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

public:
	void RedrawDmGraph();
	void PostButtonClicked(int nWinDay);

	virtual void RedrawChart(CCalculateTask * cse);

private:
	CChartCandlestickSerie * m_pCandlestickSerie;
	CChartBarSerie*			m_pBarSeries;

	void ExportFromDatabase(string strCode, double startTime = 0, double endTime = 0);

public:
	virtual void OnInitialUpdate();

protected:
	CIndividualShareDoc m_LocalDoc;
};


