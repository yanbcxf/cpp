#pragma once
#include "BaseChartCtlView.h"

// CPlateNetFixedAssestsGrowthView

class CPlateNetFixedAssestsGrowthView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CPlateNetFixedAssestsGrowthView)

protected:
public:
	CPlateNetFixedAssestsGrowthView();
	virtual ~CPlateNetFixedAssestsGrowthView();


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


