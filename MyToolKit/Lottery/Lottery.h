
// Lottery.h: Lottery 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CLotteryApp:
// 有关此类的实现，请参阅 Lottery.cpp
//

class CMvDocTemplate : public CMultiDocTemplate
{
public:
	CMvDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

	virtual ~CMvDocTemplate() {}

	CRuntimeClass * GetCurrView() { return m_pViewClass; }
};

class CLotteryApp : public CWinAppEx
{
public:
	CLotteryApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	BOOL TestRequestExit();
	CWinThread * StartWorkerThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam);

	CDocTemplate* GetCurrDocTemplate(CView * pView);
	CDocument * SearchDoc(int idx);

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	HANDLE	m_bAppExit;

	vector<LotteryInfo>	m_vec_lottery_info;
	BOOL m_bRunning;
};

extern CLotteryApp theApp;
