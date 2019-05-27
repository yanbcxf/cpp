#pragma once

#include "BaseMessageFormView.h"

// CPlateIndustryDynamicPeView 视图

class CIndiIncomestatementForecastView : public CBaseMessageFormView
{
	DECLARE_DYNCREATE(CIndiIncomestatementForecastView)

protected:
public:
	CIndiIncomestatementForecastView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndiIncomestatementForecastView();

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
	//virtual void RedrawDmGraph(CCalculateTask * cse);
	virtual void OnInitialUpdate();

	string ExtractKeyword(string title);

	virtual void RedrawChart(CCalculateTask * cse);
	
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);

};


