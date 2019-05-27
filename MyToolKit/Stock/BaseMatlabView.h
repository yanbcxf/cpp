#pragma once

#include "BaseMessageView.h"

// CBaseMatlabView 视图

class CBaseMatlabView : public CBaseMessageView
{
	DECLARE_DYNCREATE(CBaseMatlabView)

protected:
public:
	CBaseMatlabView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaseMatlabView();

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

protected:
	HWND	m_plotH;
	int		m_windowInMatlab;				//	matlab 中窗口的标识;	
	bool	m_bCalculating;	
	static int g_windowInMatlab;			//	用来产生matlab 中窗口的唯一标识
public:
	virtual void OnInitialUpdate();

	virtual void RedrawGridCtrl(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void Startup(){};
	virtual void PostFocusChanged();
	afx_msg void OnDestroy();
};


