/*******************************************************************************
File:        BalloonTip.cpp

Description: This file contains the module for creating a a balloon tip which can
             be shown anywhere on a parent window
             

Compiler with version number : Visual C++ 6.0


Copyright (c) 2001, Prateek Kaul
All rights Reserved.

The copyright to the computer program(s) herein is the property of Prateek Kaul
The program(s) may be used and/or copied only with the written permission of 
Prateek Kaul or in accordance with the terms and conditions stipulated
in the agreement/contract under which the program(s) have been supplied.
********************************************************************************/

#include "stdafx.h"
#include "BalloonTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/*----------------------------------------------------------------------------
   Message map
----------------------------------------------------------------------------*/

BEGIN_MESSAGE_MAP(CBalloonTip, CWnd)
	//{{AFX_MSG_MAP(CBalloonTip)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*-----------------------------------------------------------------------------
 Function : CBalloonTip::CBalloonTip()


 Abstract : Constructor

 Parameters : 
    1. strMessage -> Message to be shown in the balloon
    2. lf         -> LOGFONT structure from which the the message font
                     will be created.
    3. bBalloonUp -> Is the balloon up or upside down?
    
 Return Value : none

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

CBalloonTip::CBalloonTip(CString strMessage, LOGFONT lf, BOOL bBalloonUp)
{
    m_strMessage       = strMessage;
    m_bBalloonUp       = bBalloonUp;
	m_strTitle = "½Úµã";
    
//    VERIFY(m_fontBalloonText.CreateFontIndirect(&lf));
	m_fontBalloonText.CreateFont(-8, 0, 0, 0, 
	FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("MS Sans Serif"));

	m_fontBalloonTextBold.CreateFont(-8, 0, 0, 0, 
	FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("MS Sans Serif"));
}



/*-----------------------------------------------------------------------------
 Function : CBalloonTip::~CBalloonTip()


 Abstract : Destructor

 Parameters : none
    
 Return Value : none

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

CBalloonTip::~CBalloonTip()
{
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::Show()


 Abstract : Creates the CBalloonTip on the heap, forces heap creation

 Parameters : 
    1. pt -> Point of the balloon tip
    2. size -> Size of the window(width and height)
    3. strMessage -> Message to be shown in the balloon
    4. nSecs -> Seconds for which the balloon will be shown
    5. lf -> LOGFONT structure from which the the message font
             will be created.
    6. bBalloonUp -> Is balloon up or upside down?

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

CBalloonTip* CBalloonTip::Show(CPoint pt,LPRECT lpRect, 
							   int nCharWidth, int nCharHeight, CString strMessage, LOGFONT lf, 
							   UINT nSecs, BOOL bBalloonUp)
{
/*
CBalloonTip* CBalloonTip::Show(CPoint pt,LPRECT lpClientRect,LPRECT lpRect, 
							   int nCharWidth, int nCharHeight, CString strMessage, LOGFONT lf, 
							   UINT nSecs, BOOL bBalloonUp)
{
*/

    if( CBalloonTip::nBalloonInstances != 0) // No window creation if already one instance of the class
    {
        return NULL;
    }

    CBalloonTip* pBalloonTip = new CBalloonTip(strMessage, lf, bBalloonUp);
    
    CBalloonTip::nBalloonInstances = 1; // Only one instance of window possible.
        
    int nRectLeft;
	int nRectRight;
	int nRectTop;  
	int nRectBottom;

	// calculate the number of lines
	vector<string> vecStr;

	splitString(strMessage.GetBuffer(), "\n", vecStr);
	int nTextLength = 1;
	for (int i = 0; i < vecStr.size(); i++) {
		if (vecStr[i].length() > nTextLength)
			nTextLength = vecStr[i].length();
	}
	nTextLength = nTextLength * nCharWidth + 70;
	int nRectWidth = lpRect->right - lpRect->left;

	int nDiv =  nTextLength / (nRectWidth);
	int nHeight = (nCharHeight) * (nDiv + vecStr.size() + 11);


    // Rectangle co-ordinates w.r.t balloon tip point
	// Balloon tip at CPoint pt shold remain there without change in window size
    if (bBalloonUp)
    {
		if(nDiv == 0)
		{
	        nRectLeft   = pt.x - (nTextLength * 0.6);
		    nRectRight  = pt.x + (nTextLength * 0.4);
		}
		else
		{
			nRectLeft =  pt.x - (nRectWidth / 2); //lpRect->left; 
			nRectRight = pt.x + (nRectWidth / 2);//lpRect->right;
		}
	    nRectTop    = pt.y - nHeight; //size.cy;
	    nRectBottom = pt.y;
    }
    else
    {
		if(nDiv == 0)
		{
			nRectLeft   = pt.x - (nTextLength * 0.4);
			nRectRight  = pt.x + (nTextLength * 0.6);
		}
		else
		{
			nRectLeft =  pt.x - (nRectWidth / 2); //lpRect->left; 
			nRectRight = pt.x + (nRectWidth / 2);//lpRect->right;
		}
	    nRectTop    = pt.y;
	    nRectBottom = pt.y + nHeight; //(size.cy);
    }
        
	/*if(nRectLeft < 0)
		nRectLeft = 0;
	if(nRectTop < 0)
		nRectTop = 0;*/
	pBalloonTip->Create(CRect(nRectLeft, nRectTop, nRectRight, nRectBottom));    
    pBalloonTip->MakeVisisble(nSecs);

    return pBalloonTip;
}





/*-----------------------------------------------------------------------------
 Function : CBalloonTip::MakeVisible()


 Abstract : Shows the balloon and sets the timer for it's eventual destruction.

 Parameters : 
    1. nSecs -> Seconds for the balloon to be shown
    
 Return Value : none

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::MakeVisisble(UINT nSecs)
{
    ASSERT(nSecs > 0);

    SetTimer(ID_TIMER, (nSecs * 1000), NULL);
        
    CRect rect;
    GetWindowRect(&rect);

    // Caption bar height to offeset when the balloon is shown
    int nCaptionBarSize = ::GetSystemMetrics(SM_CYCAPTION);
    int nVerticalBorderSize = ::GetSystemMetrics(SM_CYSIZEFRAME);

    if(m_bBalloonUp)
    {
        // Account for the missing title bar and border thickness in the 3rd parameter
        // as the balloon shifts up by the height of title bar and window tickness
        SetWindowPos(
            &wndTopMost,
            m_rectWindow.left, 
            (m_rectWindow.top + nCaptionBarSize + (2 * nVerticalBorderSize)),
            m_rectWindow.right,
            m_rectWindow.bottom, 
            SWP_SHOWWINDOW | SWP_NOACTIVATE
        );
    }
    else
    {
        // Account for the missing title bar and border thickness in the 3rd parameter
        // as the balloon shifts up by the height of window tickness.
        SetWindowPos(
            &wndTopMost, 
            m_rectWindow.left,
            m_rectWindow.top - nVerticalBorderSize,
            m_rectWindow.right, 
            m_rectWindow.bottom,
            SWP_SHOWWINDOW | SWP_NOACTIVATE
        );
    }
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::OnTimer()


 Abstract : Gets called by MFC for an WM_TIMER message

 Parameters : 
        1. nIDEvent -> Timer ID    

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::OnTimer(UINT nIDEvent) 
{
  //  if( CBalloonTip::nBalloonInstances != 0) // No window creation if already one instance of the class
  //  {
  //      return ;
  //  }


    if(KillTimer(ID_TIMER))
	{
		try 
		{
			DestroyWindow();
		}
		catch(...)
		{

		}
	}
}	




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::OnLButtonDown()


 Abstract : Gets called by MFC for an WM_LBUTTONDOWN message

 Parameters : 
        See MFC documentation

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::OnLButtonDown(UINT nFlags, CPoint point) 
{

    if( CBalloonTip::nBalloonInstances != 0) // No window creation if already one instance of the class
    {
        return ;
    }

    if(KillTimer(ID_TIMER))
	{
		try 
		{
			DestroyWindow();
		}
		catch(...)
		{

		}
	}
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::OnRButtonDown()

 Abstract : Gets called by MFC for an WM_RBUTTONDOWN message

 Parameters : 
        See MFC documentation

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::OnRButtonDown(UINT nFlags, CPoint point) 
{
    if(KillTimer(ID_TIMER))
	{
		try 
		{
			DestroyWindow();
		}
		catch(...)
		{

		}
	}
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::OnCreate()


 Abstract : Gets called by MFC for an WM_CREATE message

 Parameters : 
        See MFC documentation

 Return Value : int

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

int CBalloonTip::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }
	
    // Remove caption and thick frame for proper shading
    ModifyStyle(WS_CAPTION  |  WS_THICKFRAME , 0); 

    CRect t_Rect;
	GetClientRect(&t_Rect);  // Get the client rectangle to set the balloon region

    if (m_bBalloonUp)
    {
        // Calculation of Text window, where to write text in the balloon
        m_rectText.left   = t_Rect.Width() * 0.10; 
        m_rectText.right  = t_Rect.Width() * 0.90;
        m_rectText.top    = t_Rect.Height() * 0.10; 
        m_rectText.bottom = t_Rect.Height()* 0.70; 
            	
        // Create an elliptical region out of the client rectangle
	    m_rgnRoundRect.CreateRoundRectRgn(
            t_Rect.left, 
            t_Rect.top, 
            t_Rect.right,
            t_Rect.bottom - (t_Rect.Height()  * 0.25), 
            abs(t_Rect.left - t_Rect.right) * 0.18,
            t_Rect.Height() * 0.25
        );
   
        // Start the process of creating the balloon tip
	    CPoint ptTri[4];
	    ptTri[0].x = (t_Rect.left) + (t_Rect.Width() * 0.93);
	    ptTri[0].y = (t_Rect.bottom) - (t_Rect.Height() * 0.40);
    
	    ptTri[1].x = (t_Rect.left) + (t_Rect.Width() * 0.65);
	    ptTri[1].y = t_Rect.bottom;

	    ptTri[2].x = (t_Rect.left) + (t_Rect.Width() * 0.70);
	    ptTri[2].y = (t_Rect.bottom) - (t_Rect.Height() * 0.40);
   
	    ptTri[3].x = (t_Rect.left) + (t_Rect.Width() * 0.93);
	    ptTri[3].y = (t_Rect.bottom) - (t_Rect.Height() * 0.40);
   
	    m_rgnTip.CreatePolygonRgn(ptTri, 3, ALTERNATE);
    }
    else
    {
        // Calculation of Text window, where to write text in the balloon
        m_rectText.left   = t_Rect.Width() * 0.10;
        m_rectText.right  = t_Rect.Width() * 0.90;
        m_rectText.top    = t_Rect.Height() * 0.30;
        m_rectText.bottom = t_Rect.Height() * 0.90;
        
        // Create an elliptical region out of the client rectangle
	    m_rgnRoundRect.CreateRoundRectRgn(
            t_Rect.left,
            t_Rect.bottom - (t_Rect.Height() * 0.75), 
            t_Rect.right, 
            t_Rect.bottom, 
            abs(t_Rect.left - t_Rect.right) * 0.18,
            t_Rect.Height() * 0.25
        );

        // Start the process of creating the balloon tip
	    CPoint ptTri[4];
	    ptTri[0].x = (t_Rect.left) + (t_Rect.Width() * 0.07);
	    ptTri[0].y = (t_Rect.Height() * 0.40);
            
	    ptTri[1].x = (t_Rect.left) + (t_Rect.Width() * 0.35);
	    ptTri[1].y = t_Rect.bottom - t_Rect.Height();

	    ptTri[2].x = (t_Rect.left) + (t_Rect.Width() * 0.30);
	    ptTri[2].y = t_Rect.Height() * 0.40;
           
	    ptTri[3].x = (t_Rect.left) + (t_Rect.Width() * 0.07);
	    ptTri[3].y = t_Rect.Height() * 0.40;
           
	    m_rgnTip.CreatePolygonRgn(ptTri, 3, ALTERNATE);
    }
    
    // Create the combined region with ellipse and tip
	CRgn rgnComb;	
    
    rgnComb.CreateRectRgn(t_Rect.left, t_Rect.top, t_Rect.right, t_Rect.bottom);
		
	int iRetComb = rgnComb.CombineRgn(&m_rgnTip, &m_rgnRoundRect, RGN_OR);
	
    if (iRetComb == ERROR)
	{
        ::AfxMessageBox( "ERROR in Combining Region" );
		return -1;
	}
   
    SetWindowRgn(rgnComb.operator HRGN(), TRUE);
	
	return 0;
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::OnPaint()


 Abstract : Gets called by MFC for a WM_PAINT message

 Parameters : 
        See MFC documentation

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    CRect t_Rect;
    GetClientRect(&t_Rect);

    // Get the total balloon, inclding all cut out regions
	CRgn rgnComb;	
    rgnComb.CreateRectRgn(t_Rect.left, t_Rect.top, t_Rect.right, t_Rect.bottom);
   
    // Create a balloon tip from this total region
    int iRetComb = rgnComb.CombineRgn(&m_rgnTip, &m_rgnRoundRect, RGN_OR );
    if (iRetComb == ERROR)
	{
        ::AfxMessageBox( "ERROR in Combining Region" );
        return;
	}

	CBrush brOutlineBrush;
    brOutlineBrush.CreateSolidBrush(RGB(0, 0, 0)); // blue
   


    CBrush brFillBrush;
	COLORREF crBackground = ::GetSysColor(COLOR_INFOBK);
    brFillBrush.CreateSolidBrush(/*RGB(250, 248, 220)*/crBackground); // light yellow

   
    dc.FillRgn(&rgnComb, &brFillBrush);  // Fill the balloon
    dc.FrameRgn(&rgnComb, &brOutlineBrush, 1, 1); // Outline the balloon

    int nBkMode          = dc.SetBkMode(TRANSPARENT);
    COLORREF clrPrevious =  dc.SetTextColor(RGB(0, 0, 0));

	CFont * pFont = dc.SelectObject(&m_fontBalloonTextBold);
    CSize czTitleWidth = dc.GetTextExtent(m_strTitle);
	

	CDC dc2;
	CDC dcTmp1;
    CDC dcTmp2;
    CBitmap bmpIcon;
    CBitmap bmpIconSmall;
    dc2.Attach(::GetDC(NULL));
    dcTmp1.CreateCompatibleDC(&dc2);
    dcTmp2.CreateCompatibleDC(&dc2);
    bmpIcon.CreateCompatibleBitmap(&dc2, 32,32);
    bmpIconSmall.CreateCompatibleBitmap(&dc2, 16,16);
    ::ReleaseDC(NULL, dc2.Detach());
         
         // i now have two device contexts and two bitmaps.
         // i will select a bitmap in each device context,
         // draw the icon into the larger one,
         // scale it into the smaller one,
         // and set the small one as the balloon icon.
         // This is a rather long process to get a small icon,
         // but ensures maximum compatibility between different
         // versions of Windows, while producing the best possible
         // results on each version.

	HICON hIcon = (HICON)::LoadImage(NULL, IDI_INFORMATION, IMAGE_ICON, 4,4, LR_SHARED);
//	COLORREF crBackground = ::GetSysColor(COLOR_INFOBK);
    CBitmap* pbmpOld1 = dcTmp1.SelectObject(&bmpIcon);
    CBitmap* pbmpOld2 = dcTmp2.SelectObject(&bmpIconSmall);
    dcTmp1.FillSolidRect(0,0,32,32, crBackground);
 //   ::DrawIconEx(dcTmp1, 0,0,hIcon,32,32,0,NULL,DI_NORMAL);
 //   dcTmp2.SetStretchBltMode(HALFTONE);
 //   dcTmp2.StretchBlt(0,0,16,16,&dcTmp1, 0,0,32,32,SRCCOPY);
 //   dcTmp1.SelectObject(pbmpOld1);
 //   dcTmp2.SelectObject(pbmpOld2);

	
	int nX = m_rectText.Width() * 0.05;
	int nY = m_rectText.top;


    if (czTitleWidth.cx < m_rectText.Width())
    {

		::DrawIconEx(dc, nX,nY,hIcon,16,16,0,NULL,DI_NORMAL);
		dc.DrawText(m_strTitle, m_rectText, DT_CENTER | DT_TOP | DT_SINGLELINE);
    }
    else
    {
	//	int nX = m_rectText.Width() * 0.05;
	//	int nY = m_rectText.top;
		

		::DrawIconEx(dc, nX,nY,hIcon,16,16,0,NULL,DI_NORMAL);
        dc.DrawText(m_strTitle, m_rectText, DT_CENTER | DT_WORDBREAK);
    }

    pFont = dc.SelectObject(&m_fontBalloonText);
   
    CSize czTextWidth = dc.GetTextExtent(m_strMessage);
   
    if (czTextWidth.cx < m_rectText.Width())
    {
       dc.DrawText(m_strMessage, m_rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
     
    else
    {
		m_rectText.top = m_rectText.top + 16;
        dc.DrawText(m_strMessage, m_rectText, DT_CENTER | DT_WORDBREAK);
		
    }
    
    // Restore the DC to its oroginal state
    dc.SelectObject(pFont);
    dc.SetBkColor(nBkMode);
    dc.SetTextColor(clrPrevious);
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::PreCreateWindow(CREATESTRUCT& cs)


 Abstract : Called by MFC when a before WM_CREATE message is received by the
            window.

            We override this function so that the window so created is not
            visible in the Taskbar. We make this window the child of an
            invisible parent m_wndInvisibleParent.

 Parameters : Look in MFC documentation

 Return Value : BOOL

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

BOOL CBalloonTip::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
    {
		return FALSE;
    }

 	// Create invisible parent window
 	if (!::IsWindow(m_wndInvisibleParent.m_hWnd))
 	{
        // Try creating the invisible parent window
        PCSTR pstrOwnerClass = ::AfxRegisterWndClass(0);
       
        BOOL bError = m_wndInvisibleParent.CreateEx(
                          0,
                          pstrOwnerClass, 
                          _T(""), 
                          WS_POPUP,
				          0,
                          0,
                          0,
                          0,
				          NULL,
                          0
                      );

        if (bError == FALSE)
        {
			return FALSE;
        }
 	}

    // Set the invisible window as the parent of "this" window
	cs.hwndParent = m_wndInvisibleParent.m_hWnd;

	return TRUE;
}




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::Create()


 Abstract : Called by the static CBalloonTip to create a Windows(R) window 

 Parameters : rect-> a rectangle in screen co ordinates where the balloon
              will be displayed.

 Return Value : BOOL

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

BOOL CBalloonTip::Create(CRect rect)
{
    m_rectWindow = rect;

    PCSTR pstrOwnerClass = ::AfxRegisterWndClass(0);
    // BOOL bResult = CWnd::Create(pstrOwnerClass, NULL, WS_OVERLAPPED, m_rectWindow);
	BOOL bResult = CWnd::Create(pstrOwnerClass, NULL, WS_OVERLAPPED, m_rectWindow, NULL, 1);
    
    return bResult;
}





/*-----------------------------------------------------------------------------
 Function : CBalloonTip::Hide()

 Created: Dec 08, 2001
 Author:  Prateek Kaul
 e-mail:  kaulpr@yahoo.com

 Abstract : Provided to destroy a balloon before it self destructs. If it is 
            already hidden, it will have no effect.
            
 Parameters : CBalloonTip*

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::Hide(CBalloonTip* pBalloonTip)
{
    if (CBalloonTip::nBalloonInstances != 0)
    {

		if((pBalloonTip != 0) ) 
		{
	        	
			if(pBalloonTip->KillTimer(ID_TIMER))
			{
				try 
				{
					pBalloonTip->DestroyWindow();
					pBalloonTip = 0;
					CBalloonTip::nBalloonInstances = 0;
				}
				catch(...)
				{

				}
			}
		}
    }
//	else
//		::AfxMessageBox("Count is 0");
}




/*-----------------------------------------------------------------------------
  int CBalloonTip::nBalloonInstances;

  Abstract: Used for preventing more than one instance of the class CBalloonTip
-----------------------------------------------------------------------------*/

int CBalloonTip::nBalloonInstances = 0;




/*-----------------------------------------------------------------------------
 Function : CBalloonTip::PostNcDestroy()

 Abstract : Last function the window receives when it is destroyed.
            Used for resetting the counter CBalloonTip::nBalloonInstances to 0,
            so that a new instance of this class can be created.
            
 Parameters : void

 Return Value : void

 Exceptions : none

 Revisions : none
----------------------------------------------------------------------------*/

void CBalloonTip::PostNcDestroy()
{
    CBalloonTip::nBalloonInstances = 0;
    CWnd::PostNcDestroy();
}


void CBalloonTip::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if((nState == WA_INACTIVE) || (nState == WA_CLICKACTIVE ))
	{
		if (CBalloonTip::nBalloonInstances != 0)
		{
			if(KillTimer(ID_TIMER))
			{
				try 
				{
					DestroyWindow();
				}
				catch(...)
				{

				}
			}
		}
	}
}


void CBalloonTip::OnActivateApp(BOOL bActive, DWORD hTask)
{
	 if(bActive == FALSE)
	 {
	  if (CBalloonTip::nBalloonInstances != 0)
	  {
	   if(KillTimer(ID_TIMER))
	   {
		DestroyWindow();
		CBalloonTip::nBalloonInstances = 0;
	   }
	  }
 
	 }
}

 

