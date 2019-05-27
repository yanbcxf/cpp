#include "stdafx.h"    
#include "HyperLink.h"    
   
#ifdef _DEBUG    
#define new DEBUG_NEW    
#undef THIS_FILE    
static char THIS_FILE[] = __FILE__;   
#endif    
   
#define TOOLTIP_ID 1    
   
/////////////////////////////////////////////////////////////////////////////    
// CHyperLink    
   
CHyperLink::CHyperLink()   
{   
    m_hLinkCursor       = NULL;                 // No cursor as yet    
	m_crLinkColour      = RGB(  0,   0, 238);   // Blue    
	m_crVisitedColour   = RGB( 85,  26, 139);   // Purple    
    m_crHoverColour     = ::GetSysColor(COLOR_HIGHLIGHT);   
    m_bOverControl      = FALSE;                // Cursor not yet over control    
    m_bVisited          = FALSE;                // Hasn't been visited yet.    
    m_bUnderline        = TRUE;                 // Underline the link?    
	m_strURL			= _T("");
	m_strWindowText		= _T("");
	m_bClick			= TRUE;
}   
   
CHyperLink::~CHyperLink()   
{ 
}   
   
BEGIN_MESSAGE_MAP(CHyperLink, CStatic)   
    //{{AFX_MSG_MAP(CHyperLink)    
    ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)   
    ON_WM_CTLCOLOR_REFLECT()   
    ON_WM_SETCURSOR()   
    ON_WM_MOUSEMOVE()   
    //}}AFX_MSG_MAP    
END_MESSAGE_MAP()   
   
/////////////////////////////////////////////////////////////////////////////    
// CHyperLink message handlers    
   
BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)    
{
	if(m_ToolTip.GetSafeHwnd() != NULL)
	{
		m_ToolTip.RelayEvent(pMsg); 
	}

    return CStatic::PreTranslateMessage(pMsg);   
}   
   
void CHyperLink::OnClicked()   
{   
	if(!m_bClick)
	{
		return;
	}

    int result = (int)GotoURL(m_strURL, SW_SHOWMAXIMIZED);   
    m_bVisited = (result > HINSTANCE_ERROR);   
    if (m_bVisited) 
	{   
		SetVisited();
    }  

	::SendMessage(GetParent()->GetSafeHwnd(),BN_CLICK_STATIC,0,0);
}   
   
HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor)    
{   
    ASSERT(nCtlColor == CTLCOLOR_STATIC);   
   
	// transparent text.    
    pDC->SetBkMode(TRANSPARENT);  
    if (m_bOverControl)   
        pDC->SetTextColor(m_crHoverColour);   
    else if (m_bVisited)   
        pDC->SetTextColor(m_crVisitedColour);   
    else   
        pDC->SetTextColor(m_crLinkColour);    
 
    return (HBRUSH)GetStockObject(NULL_BRUSH);   
}   
   
void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)    
{   
    CStatic::OnMouseMove(nFlags, point);   
   
    if (m_bOverControl)        // Cursor is currently over control    
    {   
        CRect rect;   
        GetClientRect(rect);   
   
        if (!rect.PtInRect(point))   
        {   
            m_bOverControl = FALSE;   
            ReleaseCapture();   
            RedrawWindow();   
            return;   
        }   
    }   
    else                      // Cursor has just moved over control    
    {   
        m_bOverControl = TRUE;   
        RedrawWindow();   
        SetCapture();   
    }   
}   
   
BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)    
{   
    if (m_hLinkCursor)   
    {   
        ::SetCursor(m_hLinkCursor);   
        return TRUE;   
    }

    return FALSE;   
}   
   
void CHyperLink::PreSubclassWindow()    
{   
    // We want to get mouse clicks via STN_CLICKED    
    DWORD dwStyle = GetStyle();   
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);   
       
	GetWindowText(m_strWindowText);
 
    SetDefaultCursor();      // Try and load up a "hand" cursor    
   
    // Create the tooltip    
    CRect rect;    
	GetClientRect(rect);   
	if(m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool(this, m_strWindowText, rect, TOOLTIP_ID); 
	}
   
    CStatic::PreSubclassWindow();   
}   
   
/////////////////////////////////////////////////////////////////////////////    
// CHyperLink operations    
   
void CHyperLink::SetURL(CString strWindowText,CString strURL,BOOL bClick)   
{
	m_bClick		= bClick;
	m_strWindowText = strWindowText;
    m_strURL		= strURL;
	m_bVisited		= FALSE;
	
	SetWindowText(m_strWindowText);
   
    if (::IsWindow(GetSafeHwnd()) && m_ToolTip.GetSafeHwnd() != NULL) 
	{   
        m_ToolTip.UpdateTipText(m_strWindowText, this, TOOLTIP_ID);   
    }   
}   
   
CString CHyperLink::GetURL() const   
{    
    return m_strURL;      
}   
   
void CHyperLink::SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,   
                            COLORREF crHoverColour /* = -1 */)    
{    
    m_crLinkColour    = crLinkColour;    
    m_crVisitedColour = crVisitedColour;   
   
    if (crHoverColour == -1)   
        m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);   
    else   
        m_crHoverColour = crHoverColour;   
   
    if (::IsWindow(m_hWnd))   
	{
		Invalidate();
	}
}   
   
COLORREF CHyperLink::GetLinkColour() const   
{    
    return m_crLinkColour;    
}   
   
COLORREF CHyperLink::GetVisitedColour() const   
{   
    return m_crVisitedColour;    
}   
   
COLORREF CHyperLink::GetHoverColour() const   
{   
    return m_crHoverColour;   
}   
   
void CHyperLink::SetVisited(BOOL bVisited /* = TRUE */)    
{    
    m_bVisited = bVisited;    
   
    if (::IsWindow(GetSafeHwnd()))   
	{
		Invalidate();
	}
}   
   
BOOL CHyperLink::GetVisited() const   
{    
    return m_bVisited;    
}   
   
void CHyperLink::SetLinkCursor(HCURSOR hCursor)   
{    
    m_hLinkCursor = hCursor;   
    if (m_hLinkCursor == NULL)   
	{
		SetDefaultCursor();   
	}
}   
   
HCURSOR CHyperLink::GetLinkCursor() const   
{   
    return m_hLinkCursor;   
}   
   
void CHyperLink::SetUnderline(BOOL bUnderline /* = TRUE */)   
{   
    m_bUnderline = bUnderline;   
   
    if (::IsWindow(GetSafeHwnd()))   
    {    
        Invalidate();    
    }   
}   
   
BOOL CHyperLink::GetUnderline() const   
{    
    return m_bUnderline;    
} 

    
/////////////////////////////////////////////////////////////////////////////    
// CHyperLink implementation    
   
// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.    
// It loads a "hand" cursor from the winhlp32.exe module    
void CHyperLink::SetDefaultCursor()   
{   
    if (m_hLinkCursor == NULL)                // No cursor handle - load our own    
    {   
        // Get the windows directory    
        CString strWndDir;   
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);   
        strWndDir.ReleaseBuffer();   
   
        strWndDir += _T("\\winhlp32.exe");   
        // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer    
        HMODULE hModule = LoadLibrary(strWndDir);   
        if (hModule) {   
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));   
            if (hHandCursor)   
                m_hLinkCursor = CopyCursor(hHandCursor);   
        }   
        FreeLibrary(hModule);   
    }   
} 
   
HINSTANCE CHyperLink::GotoURL(LPCTSTR url, int showcmd)   
{   
    HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);   
  
    return result;   
}   
