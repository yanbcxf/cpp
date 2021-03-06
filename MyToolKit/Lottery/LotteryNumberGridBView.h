#pragma once
#include "basegridctlview.h"

// CLotteryNumberGridView 视图

class CLotteryNumberGridBView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CLotteryNumberGridBView)

protected:
public:
	CLotteryNumberGridBView();           // 动态创建所使用的受保护的构造函数
	virtual ~CLotteryNumberGridBView();

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
	void Startup();
	virtual void OnInitialUpdate();
	virtual void PostGridDblClick(int nRow);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLotteryRangeSet();
	afx_msg void OnRegionNumberSet();
};


