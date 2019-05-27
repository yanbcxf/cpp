#include "stdafx.h" 
#include "SkinWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CSkinWnd, CWnd)
	//{{AFX_MSG_MAP(CSkinWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_BITMAPSLIDER_MOVED, OnBitmapSliderMoved)
	ON_MESSAGE(WM_BITMAPSLIDER_MOVING, OnBitmapSliderMoving)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSkinWnd::CSkinWnd()
{	
	//{{AFX_DATA_INIT(CSkinWnd)
	m_bReDraw		= TRUE;
	m_bstrechRight	= FALSE;
	m_pOldBitmap	= NULL;
	m_mapSepBmp.clear();
	//}}AFX_DATA_INIT
}

CSkinWnd::~CSkinWnd()
{
	FreeAll();
}

BOOL CSkinWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS );

	return  CWnd::CreateEx(NULL, 
		className, NULL, dwStyle, 
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU) nID);
}

void CSkinWnd::FreeAll()
{
	if(m_MemDC.GetSafeHdc() != NULL)
	{
		m_MemDC.SelectObject(m_pOldBitmap);
		m_MemDC.DeleteDC();
	}

	if(m_Bitmap.GetSafeHandle())
	{
		m_Bitmap.DeleteObject();
	}

	m_bmpLeft.DeleteObject();
	m_bmpBK.DeleteObject();
	m_bmpRight.DeleteObject();

	int i = 0;
	for(i = 0; i < m_Buttons.GetSize(); i++)
	{
		delete m_Buttons.GetAt(i);
	}

	m_Buttons.RemoveAll();

	for(i = 0; i < m_Sliders.GetSize(); i ++ )
	{
		delete m_Sliders.GetAt(i);
	}
	m_Sliders.RemoveAll();

	for(i = 0; i < m_cmbs.GetSize(); i ++ )
	{
		delete m_cmbs.GetAt(i);
	}
	m_cmbs.RemoveAll();

	for(i = 0; i < m_CtrlDateTime.GetSize(); i ++ )
	{
		delete m_CtrlDateTime.GetAt(i);
	}
	m_CtrlDateTime.RemoveAll();	

	m_mapSepBmp.clear();
}

void CSkinWnd::DrawBKBitmap(CDC* pMemDC, CRect &rect,int nRightPosX)
{
	if(pMemDC == NULL)
	{
		return;
	}

	CDC memdc;
	if(!memdc.CreateCompatibleDC(pMemDC))
	{
		return;
	}
	
	CBitmap *pOldbmp = NULL;
	// left.
	BITMAP bmp;
	m_bmpLeft.GetBitmap(&bmp);
	pOldbmp = memdc.SelectObject(&m_bmpLeft);	
	pMemDC->BitBlt(rect.left, rect.top,bmp.bmWidth ,bmp.bmHeight, &memdc, 0, 0, SRCCOPY);
	
	POINT  DrawPonit;
	DrawPonit.y = rect.top;
	DrawPonit.x = rect.left + bmp.bmWidth;
	
	BITMAP bmpright;
	m_bmpRight.GetBitmap(&bmpright);
	if(m_bstrechRight && nRightPosX != -1)
	{
		// mid
		m_bmpBK.GetBitmap(&bmp);
		memdc.SelectObject(&m_bmpBK);
		pMemDC->StretchBlt(DrawPonit.x,DrawPonit.y,nRightPosX-DrawPonit.x,bmp.bmHeight,&memdc,0,0,bmp.bmWidth, bmp.bmHeight,SRCCOPY);
	}
	else
	{
		// mid
		m_bmpBK.GetBitmap(&bmp);
		memdc.SelectObject(&m_bmpBK);
		pMemDC->StretchBlt(DrawPonit.x,DrawPonit.y,rect.right-bmpright.bmWidth-DrawPonit.x,bmp.bmHeight,&memdc,0,0,bmp.bmWidth, bmp.bmHeight,SRCCOPY);
	}

	// right
	if(m_bstrechRight && nRightPosX != -1)
	{
		memdc.SelectObject(&m_bmpRight);	
		pMemDC->StretchBlt(nRightPosX, rect.top,rect.right- nRightPosX,bmpright.bmHeight, &memdc, 0, 0,bmpright.bmWidth, bmpright.bmHeight, SRCCOPY);
	}
	else
	{
		memdc.SelectObject(&m_bmpRight);	
		pMemDC->BitBlt(rect.right-bmpright.bmWidth, rect.top,bmpright.bmWidth ,bmpright.bmHeight, &memdc, 0, 0, SRCCOPY);
	}

	// 画分隔位图.
	SepBmpIt it;
	for(it = m_mapSepBmp.begin(); it != m_mapSepBmp.end(); it ++)
	{
		CBitmap bmpSep;
		if(!bmpSep.LoadBitmap(it->second.uBmpID))
		{
			continue;
		}

		bmpSep.GetBitmap(&bmp);
		memdc.SelectObject(&bmpSep);	
		pMemDC->BitBlt(it->second.rect.left, it->second.rect.top,bmp.bmWidth ,bmp.bmHeight, &memdc, 0, 0, SRCCOPY);

		bmpSep.DeleteObject();
	}	

	memdc.SelectObject(pOldbmp);
	memdc.DeleteDC();
}

void CSkinWnd::RecalcLayout(BOOL bReDraw)
{
	m_bReDraw = bReDraw;
	Invalidate();
}

void CSkinWnd::OnPaint() 
{
	CPaintDC dc(this);
	
	CRect rect;
	GetClientRect(&rect);	

	if (m_MemDC.GetSafeHdc() == NULL)
	{
		m_MemDC.CreateCompatibleDC(&dc);		
		m_bReDraw = TRUE;
	}

	if(m_bReDraw)
	{
		if(m_Bitmap.GetSafeHandle() != NULL)
		{
			m_Bitmap.DeleteObject();
		}

		// 右边位图需要拉伸.
		int nRightPosX = -1;
		if(m_bstrechRight)
		{
			// 计算按钮的最右边位置.
			for(int i = 0; i < m_Buttons.GetSize(); i++)
			{
				CRect rect;
				CSkinButton* pButton = m_Buttons.GetAt(i);
				if(pButton == NULL || !pButton->IsWindowVisible())
				{
					continue;
				}

				pButton->GetWindowRect(&rect);
				ScreenToClient(rect);
				nRightPosX = max(nRightPosX,rect.right + 1);
			}
		}

		m_Bitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
		m_pOldBitmap = m_MemDC.SelectObject(&m_Bitmap);	
		DrawBKBitmap(&m_MemDC,rect,nRightPosX);
		m_bReDraw = FALSE;
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &m_MemDC, 0, 0, SRCCOPY);

	int i = 0;
	for(i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		pButton->Invalidate();
	}

	for(i = 0; i < m_Sliders.GetSize(); i ++)
	{
		CBitmapSlider *pSlider = m_Sliders.GetAt(i);
		pSlider->Invalidate(FALSE);
	}	

	for(i = 0; i < m_cmbs.GetSize(); i ++)
	{
		CComboBox *pCmb = m_cmbs.GetAt(i);
		pCmb->Invalidate(FALSE);
	}

	for(i = 0; i < m_CtrlDateTime.GetSize(); i ++)
	{
		CDateTimeCtrl *pCtrlDataTime = m_CtrlDateTime.GetAt(i);
		pCtrlDataTime->Invalidate(FALSE);
	}	
}

BOOL CSkinWnd::LoadDlgBitmap(UINT uLeftID,UINT uBKID,UINT uRightID,BOOL bstrechRight)
{
	m_bstrechRight = bstrechRight;

	m_bmpLeft.DeleteObject();
	if(!m_bmpLeft.LoadBitmap(uLeftID))
	{
		return FALSE;
	}

	m_bmpBK.DeleteObject();
	if(!m_bmpBK.LoadBitmap(uBKID))
	{
		m_bmpLeft.DeleteObject();
		return FALSE;
	}

	m_bmpRight.DeleteObject();
	if(!m_bmpRight.LoadBitmap(uRightID))
	{
		m_bmpLeft.DeleteObject();
		m_bmpBK.DeleteObject();
		return FALSE;
	}

	return TRUE;
}

int CSkinWnd::AddButton(UINT uID,CRect rect,CString strToolTip,CString strText,DWORD dwStyle)
{
	CSkinButton* pButton = new CSkinButton;

	pButton->Create(strText, WS_CHILD|WS_VISIBLE|WS_TABSTOP|SS_NOTIFY, rect, this, uID);
	pButton->m_nBtnID = uID;
	pButton->SetTooltipText(strToolTip);
	pButton->SetButtonTextStyle(dwStyle);
	pButton->SetFont(GetFont());

	// 修改按钮为自绘属性.
	pButton->SetButtonStyle(pButton->GetButtonStyle() | BS_OWNERDRAW);

	return m_Buttons.Add(pButton);
}

// 
void CSkinWnd::CopyFrom(UINT uBtnID,CRect rect, CBitmap& Normal, CBitmap& Over, CBitmap& Down, CBitmap& Disabled)
{
	CSkinButton *pButton = GetButton(uBtnID);
	if(pButton != NULL)
	{
		pButton->CopyFrom(rect, Normal, Over, Down, Disabled);
	}
}

void CSkinWnd::CopyFrom(UINT uBtnID,UINT uNormal, UINT uOver, UINT uDown, UINT uDisabled)
{
	CBitmap bmpNorm,bmpOver,bmpDown,bmpDisabled;
	if(!bmpNorm.LoadBitmap(uNormal))
	{
		return ;
	}

	if(!bmpOver.LoadBitmap(uOver))
	{
		bmpNorm.DeleteObject();
		return ;
	}

	if(!bmpDown.LoadBitmap(uDown))
	{
		bmpNorm.DeleteObject();
		bmpOver.DeleteObject();
		return ;
	}

	if(!bmpDisabled.LoadBitmap(uDisabled))
	{
		bmpNorm.DeleteObject();
		bmpOver.DeleteObject();
		bmpDown.DeleteObject();
		return ;
	}

	BITMAP bmpInfo;
	bmpNorm.GetBitmap(&bmpInfo);
	CRect rect;
	rect.top = 0;
	rect.left = 0;
	rect.bottom = bmpInfo.bmHeight;
	rect.right = bmpInfo.bmWidth;

	CopyFrom(uBtnID,rect,bmpNorm,bmpOver,bmpDown,bmpDisabled);

	bmpDown.DeleteObject();
	bmpNorm.DeleteObject();
	bmpOver.DeleteObject();
	bmpDisabled.DeleteObject();
}

BOOL CSkinWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(lParam == BN_CLICKED)
	{
		GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
	}
	
	return CWnd::OnCommand(wParam,lParam);
}

void CSkinWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);
}

CSkinButton * CSkinWnd::GetButton(int nBtnID)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		if(pButton->m_nBtnID == nBtnID)
		{
			return pButton;
		}
	}

	return NULL;
}

void CSkinWnd::SetButtonEnable(int nBtnID, BOOL m_Enable)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		if(pButton->m_nBtnID == nBtnID)
		{
			pButton->EnableWindow(m_Enable);
			break;
		}
	}
}

BOOL CSkinWnd::GetButtonCheck(int nBtnID)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		if(pButton->m_nBtnID == nBtnID)
		{
			return pButton->m_Check;
		}
	}
	return FALSE;
}

// 设置某个按钮为选中状态,其余为非选中状态.
void CSkinWnd::SetButtonSelect(int nBtnID)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		if(pButton->m_nBtnID == nBtnID)
		{
			pButton->SetCheck(TRUE);
		}
		else
		{
			pButton->SetCheck(FALSE);
		}
	}
}

void CSkinWnd::SetButtonCheck(int nBtnID, BOOL m_NewValue)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		if(pButton->m_nBtnID == nBtnID)
		{
			pButton->SetCheck(m_NewValue);
			break;
		}
	}
}

void CSkinWnd::SetButtonToolTip(int nBtnID, CString m_NewToolTip)
{
	for(int i = 0; i < m_Buttons.GetSize(); i++)
	{
		CSkinButton* pButton = m_Buttons.GetAt(i);
		if(pButton->m_nBtnID == nBtnID)
		{
			pButton->SetTooltipText(m_NewToolTip);
			break;
		}
	}
}

CBitmapSlider *CSkinWnd::GetSlider(int nSliderID)
{
	for(int i = 0; i < m_Sliders.GetSize(); i++)
	{
		CBitmapSlider* pSlider = m_Sliders.GetAt(i);
		if(pSlider->GetSliderID() == nSliderID)
		{
			return pSlider;
			break;
		}
	}

	return NULL;	
}

int CSkinWnd::AddSlider(UINT uID,CRect rect,CString strToolTip)
{
	CBitmapSlider* pSlider = new CBitmapSlider;

	pSlider->Create(_T(""),   
		WS_CHILD|WS_VISIBLE|WS_TABSTOP|SS_NOTIFY,   
		rect,   
		this,   
		uID);

	pSlider->SetSliderID(uID);
	pSlider->SetTooltipText(strToolTip);

	return m_Sliders.Add(pSlider);
}

// 添加combobox
int CSkinWnd::AddComboBox(UINT uID,CRect rect,DWORD dwStyle)
{
	CComboBox* pCmb = new CComboBox;

	//CBS_DROPDOWN | CBS_SORT | WS_VSCROLL | WS_TABSTOP
	DWORD dwCmbStyle = dwStyle|CBS_DROPDOWN | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP;
	pCmb->Create(dwCmbStyle,rect,this,uID);

	return m_cmbs.Add(pCmb);
}

CComboBox *CSkinWnd::GetComboBox(int nCmbID)
{
	for(int i = 0; i < m_cmbs.GetSize(); i++)
	{
		CComboBox* pCmb = m_cmbs.GetAt(i);
		if(pCmb->GetDlgCtrlID() == nCmbID)
		{
			return pCmb;
		}
	}

	return NULL;	
}

// 往文本中添加字符串,并设置相应的值.
BOOL CSkinWnd::AddComboboxString(UINT uID,CString strText,DWORD dwValue)
{
	CComboBox *pCmb = GetComboBox(uID);
	if(pCmb == NULL)
	{
		return FALSE;
	}

	int nIndex = pCmb->AddString(strText);
	pCmb->SetItemData(nIndex,dwValue);

	return TRUE;
}

DWORD CSkinWnd::GetCurItemData(UINT uID)
{
	CComboBox *pCmb = GetComboBox(uID);
	if(pCmb == NULL)
	{
		return -1;
	}

	int nCurSel = pCmb->GetCurSel();
	if(nCurSel < 0 || nCurSel >= pCmb->GetCount())
	{
		return -1;
	}

	return pCmb->GetItemData(nCurSel);
}

BOOL CSkinWnd::SetComboboxCurSel(UINT uID,DWORD dwValue)
{
	CComboBox *pCmb = GetComboBox(uID);
	if(pCmb == NULL)
	{
		return FALSE;
	}

	for(int i = 0; i < pCmb->GetCount(); i ++)
	{
		if(dwValue == pCmb->GetItemData(i))
		{
			pCmb->SetCurSel(i);
			return TRUE;
		}
	}

	// 没有找到.默认设置第0个.
	pCmb->SetCurSel(0);

	return FALSE;
}

int CSkinWnd::AddDateTimeCtrl(UINT uID,CRect rect,DWORD dwStyle)
{
	DWORD dwCtrlStyle = dwStyle| DTS_APPCANPARSE | WS_VISIBLE;

	CDateTimeCtrl* pCtrlDateTime = new CDateTimeCtrl;
	pCtrlDateTime->Create (dwCtrlStyle ,rect, this,uID);

	return m_CtrlDateTime.Add(pCtrlDateTime);
}

CDateTimeCtrl *CSkinWnd::GetDateTimeCtrl(int nID)
{
	for(int i = 0; i < m_CtrlDateTime.GetSize(); i++)
	{
		CDateTimeCtrl* pCtrlDateTime = m_CtrlDateTime.GetAt(i);
		if(pCtrlDateTime->GetDlgCtrlID() == nID)
		{
			return pCtrlDateTime;
		}
	}

	return NULL;	
}

CTime CSkinWnd::GetDateCtrl(UINT uID)
{
	CDateTimeCtrl *pCtrlDateTime = GetDateTimeCtrl(uID);
	if(pCtrlDateTime == NULL)
	{
		CTime   tmCur;    
		tmCur = CTime::GetCurrentTime();
		return tmCur;
	}
	COleDateTime oleTime;
	pCtrlDateTime->GetTime(oleTime);
	return CTime(oleTime.GetYear(),oleTime.GetMonth(),oleTime.GetDay(),oleTime.GetHour(),oleTime.GetMinute(),oleTime.GetSecond());
}

// 添加分隔位图.
void CSkinWnd::AddSepBmp(UINT uID,UINT ubmpID,CRect rect)
{
	SepBmpIt it = m_mapSepBmp.find(uID);
	if(it != m_mapSepBmp.end())
	{
		it->second.uBmpID = ubmpID;
		it->second.rect.CopyRect(rect);
		return;
	}

	StruSepBmp struData;
	struData.rect.CopyRect(rect);
	struData.uBmpID = ubmpID;

	m_mapSepBmp.insert(SepBmpValueType(uID,struData));
}

// 编辑分隔位图的区域.
void CSkinWnd::EditSepBmpRect(UINT uID,CRect rect)
{
	SepBmpIt it = m_mapSepBmp.find(uID);
	if(it != m_mapSepBmp.end())
	{
		it->second.rect.CopyRect(rect);
		return;
	}
}

// 滑动结束.
LRESULT CSkinWnd::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam)
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_BITMAPSLIDER_MOVED,wParam, lParam );

	return 0;
}

// 滑动过程.
LRESULT CSkinWnd::OnBitmapSliderMoving(WPARAM wParam, LPARAM lParam)
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_BITMAPSLIDER_MOVING,wParam, lParam );
	return 0;
}