#pragma once

#include "BaseChartCtlView.h"

// CPlateIndustryDynamicPeView 视图

class CPlateCsindexDynamicPeView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CPlateCsindexDynamicPeView)

protected:
public:
	CPlateCsindexDynamicPeView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPlateCsindexDynamicPeView();

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

	virtual void ProccessChartMenu();
};


