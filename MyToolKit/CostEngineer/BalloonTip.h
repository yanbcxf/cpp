/*******************************************************************************
File:        BalloonTip.h

Description: This file contains the module for creating a a balloon tip which can
             be shown anywhere on a parent window
             

Created: Nov 1, 2001
Author:  Prateek Kaul
e-mail:  kaulpr@yahoo.com

Compiler with version number : Visual C++ 6.0


Copyright (c) 2001, Prateek Kaul
All rights Reserved.

The copyright to the computer program(s) herein is the property of Prateek Kaul
The program(s) may be used and/or copied only with the written permission of 
Prateek Kaul or in accordance with the terms and conditions stipulated
in the agreement/contract under which the program(s) have been supplied.
********************************************************************************/

#ifndef _BALLOONTIP_H
#define _BALLOONTIP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int ID_TIMER = 500;

/*---------------------------------------------------------------------------
 class CBalloonTip

 Created: Nov 1, 2001
 Author:  Prateek Kaul
 e-mail:  kaulpr@yahoo.com

 Abstract : For creating a balloon tip

 Revisions :none.
----------------------------------------------------------------------------*/

class CBalloonTip : public CWnd
{
    DECLARE_MESSAGE_MAP()

public:
	
// Implementation

    // Creates a new CBalloonTip Object
    static CBalloonTip* Show(
               CPoint pt,         // Point where the balloon tip will be 
			   //LPRECT lpClientRect,
			   LPRECT lpRect, 
               int nWidth,
			   int nHeight,
               CString strMessage,// Messsage to be shown
               LOGFONT lf, // A LOGFONT structure from which the message font will created  
               UINT nSecs, // Seconds for which the balloon will be shown
               BOOL bBalloonUp    // Is balloon show up or upside down?
            );

    // Makes the window invisible, destroys, and releases resources, if required to destroy before
    // the Timer ticks.
    static void Hide(CBalloonTip* pBalloonTip); 
//	static void SetParentWindow(CWnd * pParentWnd);
    static int nBalloonInstances;
	static CWnd	* pParentWindow;
	

protected:
	virtual void PostNcDestroy();

// Implementation

  	CBalloonTip(CString strMessage, LOGFONT lf, BOOL bBalloonUp);
    ~CBalloonTip();
    BOOL Create(CRect rect);       // Create the Windows(R) window, HANDLE etc ...
    void MakeVisisble(UINT nSecs); // Show the balloon for nSecs Seconds
	

// Overrides and messages
	//{{AFX_MSG(CBalloonTip)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
	afx_msg void OnPaint();
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	//}}AFX_MSG
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Attributes
    BOOL    m_bBalloonUp;   // Is balloon up or upside down
	CRect   m_rectText;     // Rectabgle where the text will be in the balloon
    CRect   m_rectWindow;   // Rectangle in which the balloon will be displayed in screen coordinates
    CString m_strMessage;   // Message to be displayed in the balloon
	CFont   m_fontBalloonText; // Font of the message
	CFont	m_fontBalloonTextBold;
	CRgn    m_rgnTip;          // The region of the tip
	CRgn    m_rgnRoundRect;   // The region of the round rectangle  
    CWnd    m_wndInvisibleParent; // Invisible parent of this window, this avoids the balloon's
                                  // appreance in the Taskbar
	CString m_strTitle;

	
};

#endif // _BALLOONTIP_H
