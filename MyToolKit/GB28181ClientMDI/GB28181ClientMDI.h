
// GB28181ClientMDI.h : GB28181ClientMDI 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CGB28181ClientMDIApp:
// 有关此类的实现，请参阅 GB28181ClientMDI.cpp
//

class CMvDocTemplate : public CMultiDocTemplate
{
public:
	CMvDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

	virtual ~CMvDocTemplate(){}

	CRuntimeClass * GetCurrView(){return m_pViewClass;}
};

class CGB28181ClientMDIApp : public CWinAppEx
{
public:
	CGB28181ClientMDIApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	CDocTemplate* GetCurrDocTemplate(CView * pView);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGB28181ClientMDIApp theApp;
