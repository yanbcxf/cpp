#pragma once
#include "BaseChartCtlView.h"


// CIndiCsindexView 视图

class CIndiPeAndValueDailyView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CIndiPeAndValueDailyView)

protected:
public:
	CIndiPeAndValueDailyView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndiPeAndValueDailyView();

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

	afx_msg void OnIndiPeAndValueDaily();

	virtual void RedrawDmGraph();

	void PostFocusChanged();

protected:
	CIndividualShareDoc m_LocalDoc;
};


