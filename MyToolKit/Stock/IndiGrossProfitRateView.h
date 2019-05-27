#pragma once
#include "BaseChartCtlView.h"


// CIndiCsindexView 视图

class CIndiGrossProfitRateView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CIndiGrossProfitRateView)

protected:
public:
	CIndiGrossProfitRateView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndiGrossProfitRateView();

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
	//void RedrawDmGraph(CCalculateTask * cse);

public:
	virtual void OnInitialUpdate();

	afx_msg void OnIndiGrossProfitRate();

	virtual void RedrawDmGraph();

	void PostFocusChanged();

protected:
	CIndividualShareDoc m_LocalDoc;
};


