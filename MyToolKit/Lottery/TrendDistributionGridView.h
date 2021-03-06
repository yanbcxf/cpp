#pragma once
#include "basegridctlview.h"


// CTrendDistributionGridView 视图

class CTrendDistributionGridView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CTrendDistributionGridView)

protected:
public:
	CTrendDistributionGridView();           // 动态创建所使用的受保护的构造函数
	virtual ~CTrendDistributionGridView();

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
	void RedrawGridCtrl(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType, string code, string model, string param1, string param2, string param3);

public:
	virtual void OnInitialUpdate();

public:
	
	afx_msg void OnLotteryRangeSet();
	afx_msg void OnCumulativeNumberGraph();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnInheritNumberGrid();
	afx_msg void OnTheoryInheritNumberGrid();
};


