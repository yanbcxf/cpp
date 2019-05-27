#pragma once

#include "IndividualDmGraphView.h"
// CIndividualCostView 视图

class CIndividualCostView : public CIndividualDmGraphView
{
	DECLARE_DYNCREATE(CIndividualCostView)

protected:
public:
	CIndividualCostView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualCostView();

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

protected:
	DECLARE_MESSAGE_MAP()

private:
	void OverallCostsCurve();

};


