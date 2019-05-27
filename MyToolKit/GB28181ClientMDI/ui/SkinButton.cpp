#include "stdafx.h" 
#include "SkinButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void CopyBitmap(CDC* dc, CBitmap& bmpResult, CBitmap& bmpSrc, RECT r)
{
	if(!bmpSrc.m_hObject || dc == NULL) 
		return;

	CDC memdc, hDC;
	if(!hDC.CreateCompatibleDC(dc))
	{
		return;
	}

	if(!memdc.CreateCompatibleDC(dc))
	{
		hDC.DeleteDC();
		return ;
	}

	int w = r.right - r.left;
	int h = r.bottom - r.top;	
	
	bmpResult.CreateCompatibleBitmap(dc, w, h);	
	
	CBitmap *pOldbmp2 = memdc.SelectObject(&bmpSrc);
	CBitmap *pOldBmp = hDC.SelectObject(&bmpResult);

	hDC.StretchBlt(0, 0, w, h, &memdc, r.left, r.top, w, h, SRCCOPY); 
	memdc.SelectObject(pOldbmp2);
	hDC.SelectObject(pOldBmp);
	
	memdc.DeleteDC();
	hDC.DeleteDC();
}

CSkinButton::CSkinButton()
{
	m_nBtnID = -1;
	m_CheckedButton = m_Check = m_MouseOnButton = FALSE;
	m_clrText = COLOR_TEXT_SELECT;
	m_strToolTip = _T("");

	m_TipFlag = false;

}

CSkinButton::~CSkinButton()
{
	m_Normal.DeleteObject();
	m_Over.DeleteObject();
	m_Down.DeleteObject();
	m_Disabled.DeleteObject();
	m_MouseOnButton = FALSE;
}


BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	//{{AFX_MSG_MAP(CSkinButton)
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSkinButton::SetBitmap(CBitmap& mNrml, CBitmap& mOvr, CBitmap& mDwn, CBitmap& mDsbl)
{
	m_Normal.DeleteObject();
	m_Over.DeleteObject();
	m_Down.DeleteObject();
	m_Disabled.DeleteObject();

	m_Normal.Attach(mNrml);
	m_Over.Attach(mOvr);
	m_Down.Attach(mDwn);
	m_Disabled.Attach(mDsbl);
}

void CSkinButton::CopyFrom(CRect r, CBitmap& bmpNorm, CBitmap& bmpOver, CBitmap& bmpDown, CBitmap& bmpDisabled)
{
	CDC* pDC = GetDC();
	if(pDC == NULL)	
	{	
		return;
	}

	m_Normal.DeleteObject();
	m_Over.DeleteObject();
	m_Down.DeleteObject();
	m_Disabled.DeleteObject();

	CopyBitmap(pDC, m_Normal, bmpNorm, r);
	CopyBitmap(pDC, m_Over,   bmpOver, r);
	CopyBitmap(pDC, m_Down,   bmpDown, r);
	CopyBitmap(pDC, m_Disabled, bmpDisabled, r);
	
	ReleaseDC(pDC);

	ModifyStyle(WS_BORDER , 0);
}

void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if(pDC == NULL)
	{
		return ;
	}
	
	BOOL bIsPressed  = (lpDrawItemStruct->itemState & ODS_SELECTED);
	BOOL bIsFocused  = (lpDrawItemStruct->itemState & ODS_FOCUS);
	BOOL bIsDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);
	
	COLORREF clrText = m_clrText;
	CBitmap *pOldbmp = NULL;
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	if(!m_CheckedButton)
	{
		if (m_Check /* && !m_MouseOnButton */)
		{
			pOldbmp = memdc.SelectObject(&m_Down);
		}
		else if (bIsPressed)
		{
			pOldbmp = memdc.SelectObject(&m_Down);
		}
		else if (m_MouseOnButton)
		{
			pOldbmp = memdc.SelectObject(&m_Over);
		}
		else if (bIsDisabled)
		{
			pOldbmp = memdc.SelectObject(&m_Disabled);
			clrText = COLOR_TEXT;
		}
		else
		{
			pOldbmp = memdc.SelectObject(&m_Normal);
			clrText = COLOR_TEXT;
		}
	}
	else
	{
		if (m_Check)
		{
			pOldbmp = memdc.SelectObject(&m_Down);
		}
		else
		{
			pOldbmp = memdc.SelectObject(&m_Normal);
			clrText = COLOR_TEXT;
		}
	}

	CRect rect;
	GetClientRect(&rect);
	
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);
	if(pOldbmp != NULL)
	{
		memdc.SelectObject(pOldbmp);
	}
	memdc.DeleteDC();

	// 绘制按钮文本.
	CString strText = _T("");
	GetWindowText(strText);
	if(!strText.IsEmpty())
	{
		int nOldMode = pDC->SetBkMode(TRANSPARENT);
		CFont font;
		COLORREF clOldText = pDC->GetTextColor();
		pDC->SetTextColor(clrText);
		CFont *pOldFont = pDC->SelectObject(GetFont());

		CPoint pt;
		CSize size = pDC->GetTextExtent(strText);
		pt.y = rect.top + (rect.bottom-rect.top)/2 - size.cy/2;	

		switch(m_dwTextStyle)
		{
		case TEXT_CENTER:
			pt.x = rect.left + rect.Width()/2 - size.cx/2;
			break;
		case TEXT_RIGHT:
			pt.x = rect.right - size.cx - 1;
			break;
		case TEXT_BOTTOM:
			{
				pt.x = rect.left + (rect.Width() - size.cx)/2;
				pt.y = rect.bottom - size.cy - 4;
				break;
			}
		default:
			pt.x = rect.left+1;
			break;
		}

		pDC->TextOut(pt.x,pt.y ,strText);
		pDC->SelectObject(pOldFont);

		pDC->SetBkMode(nOldMode);
		pDC->SetTextColor(clOldText);
	}
}

void CSkinButton::PreSubclassWindow() 
{
	UINT nBS = GetButtonStyle();
	SetButtonStyle(nBS | BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}

void CSkinButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	m_MouseOnButton = FALSE;
	Invalidate();	
}

void CSkinButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CButton::OnMouseMove(nFlags, point);

	if (nFlags & MK_LBUTTON && m_MouseOnButton == FALSE) 
	{
		return;
	}

	if(m_ToolTipBtn.GetSafeHwnd() != NULL)
	{
		MSG msg; 
		msg.hwnd = GetSafeHwnd(); 
		msg.message = WM_MOUSEMOVE; 
		msg.wParam = WPARAM(nFlags); 
		msg.time = 0; 
		msg.pt.x = point.x; 
		msg.pt.y = point.y; 
		msg.lParam = MAKELPARAM(LOWORD(point.x), LOWORD(point.y)); 

		m_ToolTipBtn.RelayEvent(&msg);
	}

	CWnd* pWnd = GetActiveWindow();
	CWnd* pParent = GetOwner();

	if ((GetCapture() != this) && (pWnd != NULL && pParent != NULL)) 
	{
		m_MouseOnButton = TRUE;
		SetCapture();
		Invalidate();
	}
	else
	{
		POINT p2 = point;
		ClientToScreen(&p2);
		CWnd* wndUnderMouse = WindowFromPoint(p2);
		if (wndUnderMouse && wndUnderMouse->m_hWnd != this->m_hWnd)
		{
			if (m_MouseOnButton)
			{
				m_MouseOnButton = FALSE;
				Invalidate();
			}
			if (!(nFlags & MK_LBUTTON)) ReleaseCapture();
		}
	}

	if(m_ToolTipBtn.GetSafeHwnd() == NULL)
	{
		return ;
	}

	if(m_MouseOnButton )
	{
		if( false == m_TipFlag){
			m_TipFlag = true;
			m_ToolTipBtn.UpdateTipText(m_strToolTip,this);
			m_ToolTipBtn.Update();
		}
		else{
			m_TipFlag = false;
		}
	}
	else
	{
		m_ToolTipBtn.Pop();
	}
}

void CSkinButton::OnCaptureChanged(CWnd *pWnd) 
{
	if (m_MouseOnButton == TRUE)
	{
		ReleaseCapture();
		Invalidate();
	}
	CButton::OnCaptureChanged(pWnd);
}

void CSkinButton::InitToolTip()
{
	if(m_ToolTipBtn.GetSafeHwnd() != NULL)
	{
		return;
	}

	m_ToolTipBtn.Create(this);
	m_ToolTipBtn.Activate(TRUE);
	m_ToolTipBtn.AddTool(this, LPSTR_TEXTCALLBACK);
	m_ToolTipBtn.SetDelayTime(100);
}

BOOL CSkinButton::PreTranslateMessage(MSG* pMsg) 
{		
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd* pParent = GetParent();
		if (pParent != NULL)
		{
			pParent->SendMessage(WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),BN_CLICKED), (LPARAM) m_hWnd);
			return TRUE;
		}
	}
	
	return CButton::PreTranslateMessage(pMsg);
}

void CSkinButton::SetTooltipText(CString strText)
{
	InitToolTip();
	m_strToolTip = strText;
}

BOOL CSkinButton::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE; //CButton::OnEraseBkgnd(pDC);
}

BOOL CSkinButton::GetCheck()
{
	return m_Check;
}

void CSkinButton::SetCheck(BOOL m_NewValue)
{
	if(m_NewValue != m_Check)
	{
		m_Check = m_NewValue;
		Invalidate();
	}
}

void CSkinButton::OnClicked()
{
	GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), BN_CLICKED);
}
