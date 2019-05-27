#pragma once

#include "BaseChartCtlView.h"
// CIndividualCapitalFlowView 视图

class CIndividualCapitalFlowView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CIndividualCapitalFlowView)

protected:
public:
	CIndividualCapitalFlowView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualCapitalFlowView();

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

public:
	virtual void OnInitialUpdate();

protected:
	CIndividualShareDoc m_LocalDoc;

public:
	vector<CTechAnalysisEventData>	m_tech_analysis_event;
};




class CIndividualCapitalFlowViewMouseListener: public CChartSeriesMouseListener<SChartXYPoint>
{
public:

	CIndividualCapitalFlowViewMouseListener(CIndividualCapitalFlowView * pView)
	{
		m_pView = pView;
	}

	void OnMouseEventSeries(CChartMouseListener::MouseEvent mouseEvent, CPoint point,
		CChartSerieBase<SChartXYPoint>* pSerie, unsigned uPointIndex);

private:
	CIndividualCapitalFlowView * m_pView;

};
