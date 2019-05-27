

#include "BaseChartCtlView.h"

// CIndiCashFlowSummaryView 视图

class CIndiCashFlowSummaryView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CIndiCashFlowSummaryView)

protected:
public:
	CIndiCashFlowSummaryView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndiCashFlowSummaryView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	void RedrawDmGraph();
	virtual void OnInitialUpdate();

protected:
	DECLARE_MESSAGE_MAP()


};


