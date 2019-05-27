#pragma once

#include "BaseChartCtlView.h"
// CPlateMainIncomeGrowthView 视图

class CPlateMainIncomeGrowthView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CPlateMainIncomeGrowthView)

protected:
public:
	CPlateMainIncomeGrowthView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPlateMainIncomeGrowthView();

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
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

public:
	void RedrawDmGraph();
	virtual void RedrawDmGraph(CCalculateTask * cse);
	virtual void OnInitialUpdate();
};


