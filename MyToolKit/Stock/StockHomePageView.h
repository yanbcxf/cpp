#pragma once
#include "basegridctlview.h"

// CStockHomePageView 视图

class CStockHomePageView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CStockHomePageView)

protected:
public:
	CStockHomePageView();           // 动态创建所使用的受保护的构造函数
	virtual ~CStockHomePageView();

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
	virtual void OnInitialUpdate();

private:
	void RedrawGridCtrl();
public:
	afx_msg void OnClose();
};


