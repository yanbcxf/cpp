// ComboBoxExtList.cpp : implementation file
//

#include "stdafx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBoxExtList

CComboBoxExtList::CComboBoxExtList()
	:m_bShowTooltip(TRUE)
	,m_bTooltipOnInfo(FALSE)
	,m_bShowListTooltipOverItem(FALSE)
	,m_bToolActive(FALSE)
	,m_nPrevItem(-1)
{
}

CComboBoxExtList::~CComboBoxExtList()
{
}


BEGIN_MESSAGE_MAP(CComboBoxExtList, CListBox)
	//{{AFX_MSG_MAP(CComboBoxExtList)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(LB_FINDSTRING, CComboBoxExtList::OnLbFindString)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxExtList message handlers

void CComboBoxExtList::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	// create tooltip
	m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST, 
							  TOOLTIPS_CLASS, 
							  NULL, 
							  TTS_NOPREFIX | TTS_ALWAYSTIP, 
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  CW_USEDEFAULT, 
							  NULL, 
							  NULL, 
							  NULL, 
							  NULL);
	ASSERT(m_hWndToolTip);
	// initialize toolinfo struct
	memset(&m_ToolInfo, 0, sizeof(m_ToolInfo));
#ifdef _UNICODE
	m_ToolInfo.cbSize = sizeof(TTTOOLINFOW_V2_SIZE);
#else
	m_ToolInfo.cbSize = sizeof(TTTOOLINFOA_V2_SIZE);
#endif
	m_ToolInfo.uFlags = TTF_TRACK | TTF_TRANSPARENT;
	m_ToolInfo.hwnd = m_hWnd;
	::SendMessage(m_hWndToolTip, TTM_SETMAXTIPWIDTH, (WPARAM)0, (LPARAM)SHRT_MAX);
	::SendMessage(m_hWndToolTip, TTM_ADDTOOL, (WPARAM)0, (LPARAM)(LPTOOLINFO)&m_ToolInfo);

	CListBox::PreSubclassWindow();
}

LRESULT CComboBoxExtList::OnLbFindString(WPARAM wParam, LPARAM lParam)
{
	return SendMessage(LB_FINDSTRINGEXACT, wParam, lParam);
}

void CComboBoxExtList::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CListBox::OnMouseMove(nFlags, point);

	if(NULL == m_pComboBox || ! m_bShowTooltip)
		return;

	CRect rectClient;
	GetClientRect(&rectClient);

	if(rectClient.PtInRect(point))
	{
		CPoint pointScreen;
		::GetCursorPos(&pointScreen);
		BOOL bOutside = FALSE;
		int nItem = ItemFromPoint(point, bOutside);  // calculate listbox item number (if any)

		if(! bOutside && nItem >= 0 && m_nPrevItem != nItem)
		{
			m_nPrevItem = nItem;
			m_bToolActive = FALSE;

			CString sText;
			if(! m_bTooltipOnInfo)
				GetText(nItem, sText);
			else
				m_pComboBox->GetLBInfo(nItem, sText);
			m_ToolInfo.lpszText = (LPTSTR)(LPCTSTR)sText;

			CRect rect;
			GetItemRect(nItem, &rect);
			ClientToScreen(&rect);

			if(! m_bTooltipOnInfo)
			{
				HDC hDC = ::GetDC(m_hWnd);
				ASSERT(hDC);
				CFont* pFont = GetFont();
				HFONT hOldFont = (HFONT)::SelectObject(hDC, (HFONT)*pFont);
				SIZE size;
				::GetTextExtentPoint32(hDC, sText, sText.GetLength(), &size);
				::SelectObject(hDC, hOldFont);
				::ReleaseDC(m_hWnd, hDC);

				if(! m_bToolActive && size.cx > rect.Width() - 3)
				{
					::SendMessage(m_hWndToolTip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&m_ToolInfo);
					::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, (WPARAM)0, (LPARAM)MAKELONG(rect.left, rect.top));
					::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
					m_bToolActive = TRUE;
				}
				else
				{
					::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
					m_bToolActive = FALSE;
					SetTimer(1, 80, NULL);
				}
			}
			else	// tooltip text is retrieved from additional info
			{
				if(! m_bToolActive && m_pComboBox->GetLBShowItemTooltipState(nItem) && ! sText.IsEmpty())
				{
					::SendMessage(m_hWndToolTip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&m_ToolInfo);
					::SendMessage(m_hWndToolTip, TTM_TRACKPOSITION, (WPARAM)0, (LPARAM)MAKELONG(m_bShowListTooltipOverItem ? rect.left : rect.right + 1, rect.top));
					::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
					m_bToolActive = TRUE;
				}
				else
				{
					::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
					m_bToolActive = FALSE;
					SetTimer(1, 80, NULL);
				}
			}
		}
	}
	else
	{
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
		m_bToolActive = FALSE;
		m_nPrevItem = -1;
		SetTimer(1, 80, NULL);
	}
}

void CComboBoxExtList::OnTimer(/*UINT*/UINT_PTR nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	CListBox::OnTimer(nIDEvent);

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);

	if(! rectClient.PtInRect(point) || 0 == (GetStyle() & WS_VISIBLE))
	{
		KillTimer(nIDEvent);
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)(LPTOOLINFO)&m_ToolInfo);
		m_bToolActive = FALSE;
		m_nPrevItem = -1;
	}
}
