#pragma once

#include "./GridCtrl_src/GridCtrl.h"

// CCsvView 视图

class CCsvView : public CView
{
	DECLARE_DYNCREATE(CCsvView)

protected:
	CCsvView();           // 动态创建所使用的受保护的构造函数
	virtual ~CCsvView();

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
	CGridCtrl* m_pGridCtrl;
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


