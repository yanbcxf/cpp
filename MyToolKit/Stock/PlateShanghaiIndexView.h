#pragma once
#include "BaseMatlabView.h"

// CShanghaiIndexView 视图

class CPlateShanghaiIndexView : public CBaseMatlabView
{
	DECLARE_DYNCREATE(CPlateShanghaiIndexView)

protected:
public:
	CPlateShanghaiIndexView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPlateShanghaiIndexView();

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
	virtual void Startup();
	afx_msg void OnSetStockRange();
	afx_msg void OnUpdateShanghaiIndex(CCmdUI *pCmdUI);
};


