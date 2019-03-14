
// FfplaySdl2Test.h : main header file for the FfplaySdl2Test application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFfplaySdl2TestApp:
// See FfplaySdl2Test.cpp for the implementation of this class
//

class CFfplaySdl2TestApp : public CWinAppEx
{
public:
	CFfplaySdl2TestApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFfplaySdl2TestApp theApp;
