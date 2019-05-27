#include "stdafx.h"
#include "PlaybackPannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  WINDOWS_OFFSET 2	// 窗口偏移量,两个相邻窗口间间距 要X2.

CPlaybackPannel::CPlaybackPannel()
{
	m_nLastSplit = -1;
	m_nCurPannelIndex = 0;
	m_pActivePage	= NULL;
	m_bFullScreen	= FALSE;	// 全屏标记
	m_bMultiScreen	= TRUE;		// 多屏标记
	m_bAutoAdjustPos= FALSE;	// 自动调节标记
	m_penSelect.CreatePen(PS_SOLID, WINDOWS_OFFSET, RGB(54,0xa8,0xfe));  //当前窗口	
	m_penNorm.CreatePen(PS_SOLID, 1, RGB(192,192,192));		//其他窗口
}

CPlaybackPannel::~CPlaybackPannel()
{
	m_penNorm.DeleteObject();
	m_penSelect.DeleteObject();
}

BEGIN_MESSAGE_MAP(CPlaybackPannel, CWnd)
	//{{AFX_MSG_MAP(CPlaybackPannel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaybackPannel member functions
void CPlaybackPannel::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd *pWnd;
	pWnd = GetParent();
	pWnd->SendMessage(WM_MOUSEMOVE);

	CWnd::OnMouseMove(nFlags,point);
}

void CPlaybackPannel::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType,cx,cy);

	SetShowPlayWin(m_nLastSplit);
}

/////////////////////////////////////////////////////////////////////////////
// CPlaybackPannel member functions
void CPlaybackPannel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DebugTrace("OnPaint --- Playback.ocx -CPlaybackPannel ");
}

BOOL CPlaybackPannel::OnEraseBkgnd(CDC* pDC) 
{
	CRect rt;
	GetClientRect(&rt);

	CBrush br;
	br.CreateSolidBrush(VIDEO_BACK_COLOR);
	pDC->FillRect(&rt,&br);

	int nSrc = pDC->SaveDC();

	for(int i = 0; i < MAX_WINDOW; i ++)
	{
		if(m_wndVideo[i].GetSafeHwnd() != NULL && m_wndVideo[i].IsWindowVisible())
		{
			if(i == m_nCurPannelIndex)
			{
				pDC->SelectObject(&m_penSelect);
			}
			else
			{
				pDC->SelectObject(&m_penNorm);
			}

			CRect rect;
			m_wndVideo[i].GetWindowRect(rect);
			ScreenToClient(rect);
			// 绘制边框.
			int nOffset = WINDOWS_OFFSET/2;
			pDC->MoveTo(rect.left-nOffset, rect.top-nOffset);
			pDC->LineTo(rect.right+nOffset, rect.top-nOffset);
			pDC->LineTo(rect.right+nOffset, rect.bottom+nOffset);
			pDC->LineTo(rect.left-nOffset, rect.bottom+nOffset);
			pDC->LineTo(rect.left-nOffset, rect.top-nOffset);
		}
	}

	pDC->RestoreDC(nSrc);

	return TRUE;
}

BOOL CPlaybackPannel::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext )
{
	dwStyle|=WS_EX_TOOLWINDOW|WS_CLIPCHILDREN;
	return CWnd::Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,nID,pContext );
}

int CPlaybackPannel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	for(int i = 0; i < MAX_WINDOW; i++)
	{
		m_wndVideo[i].Create(
			NULL, 
			NULL, 
			WS_VISIBLE | WS_CHILD,
			CRect(0, 0, 0, 0),
			this,
			1979+i,
			NULL);
		//
		m_wndVideo[i].SetWinID(i);
	}

	return 0;
}

int CPlaybackPannel::SetShowPlayWin(int nMain)
{
	if(m_nLastSplit != nMain)
	{
		m_nLastSplit = nMain;
	}

	int nNum = 4;
	switch(nMain) 
	{
	case SPLIT1:
		nNum = 1;
		break;
	case SPLIT4:
		nNum = 4;
		break;
	default:
		nNum = 4;
		break;
	}

	int i = 0;
	// 隐藏不显示的窗口.
	for(i = nNum; i < MAX_WINDOW; i ++)
	{
		if(m_wndVideo[i].GetSafeHwnd() == NULL)
		{
			return 0;
		}

		if(m_wndVideo[i].IsWindowVisible())
		{
			m_wndVideo[i].ShowWindow(SW_HIDE);
		}
	}

	CRect rtClient,rt;
	GetClientRect(&rtClient);
	// 外层有3D效果,偏移1像素.
	rtClient.DeflateRect(WINDOWS_OFFSET,WINDOWS_OFFSET);

	//调整Container位置
	if(m_bAutoAdjustPos)		
	{
		AdjustRect(&rtClient);
	}

	// 多画面.
	if(m_bMultiScreen)
	{
		HDWP hdwp = BeginDeferWindowPos(nNum);
		for (i = 0;i < nNum; i ++)
		{
			rt = rtClient;
			CalcPageRect(rt,i,nNum);
			hdwp = ::DeferWindowPos( hdwp, m_wndVideo[i].GetSafeHwnd(), GetSafeHwnd(),
				rt.left,rt.top,
				rt.Width(),rt.Height(),
				SWP_NOZORDER);	
		}

		::EndDeferWindowPos(hdwp);
		SetActivePage(&m_wndVideo[m_nCurPannelIndex]);

		for (i = 0; i < nNum; i ++)
		{	
			if(!m_wndVideo[i].IsWindowVisible())
			{
				m_wndVideo[i].ShowWindow(SW_SHOW);
			}
		}
	}
	else
	{
		rtClient.DeflateRect(WINDOWS_OFFSET,WINDOWS_OFFSET);
		for (i = 0;i < MAX_WINDOW; i ++)
		{
			if(m_pActivePage == &m_wndVideo[i])
			{
				m_nCurPannelIndex = i;				
				m_wndVideo[i].MoveWindow(rtClient,TRUE);
			}
			else
			{
				m_wndVideo[i].ShowWindow(SW_HIDE);				
			}
		}

		SetActivePage(&m_wndVideo[m_nCurPannelIndex]);
	}

	Invalidate();

	return 1;
}

void CPlaybackPannel::OnDestroy() 
{
	for(int i=0; i < MAX_WINDOW; i++)
	{
		m_wndVideo[i].DestroyWindow();
	}

	CWnd::OnDestroy();
}

CWnd * CPlaybackPannel::GetPage(int nIndex)
{
	if (nIndex >= 0 && nIndex < MAX_WINDOW)
	{		
		return &m_wndVideo[nIndex];
	}

	return NULL;
}

void CPlaybackPannel::SetPlayPort(int port,int type,int windowID) 
{
	m_wndVideo[m_nCurPannelIndex].m_playport = port;
	m_wndVideo[m_nCurPannelIndex].m_playtype = type;
	m_wndVideo[m_nCurPannelIndex].m_playwindowID = m_nCurPannelIndex;
}

/************************************************************************/
/* 设置当前窗口                                                         */
/************************************************************************/
int CPlaybackPannel::SetCurrentWnd(int windowID) 
{
	m_nCurPannelIndex = windowID;
	SetActivePage(&m_wndVideo[windowID], TRUE);

	return 0;
}

// 刷新指定窗口.
void CPlaybackPannel::InvalidateWnd(int nWndIndex)
{
	if(nWndIndex < 0 || nWndIndex >= MAX_WINDOW)
	{
		return;
	}

	m_wndVideo[nWndIndex].Invalidate();
}

void  CPlaybackPannel::SetWndToolTip(int nWndIndex,CString strToolTip)
{
	if(nWndIndex < 0 || nWndIndex >= MAX_WINDOW)
	{
		return;
	}

	m_wndVideo[nWndIndex].SetTooltipText(strToolTip);
}

CString  CPlaybackPannel::GetWndToolTip(int nWndIndex)
{
	if(nWndIndex < 0 || nWndIndex >= MAX_WINDOW)
	{
		return "";
	}

	return m_wndVideo[nWndIndex].GetTooltipText();
}

void CPlaybackPannel::SetActivePage(CWnd *pWnd, BOOL bRepaint)
{
	// check parameter
	if(	pWnd == NULL || !IsWindow(pWnd->m_hWnd) )	return;
	m_pActivePage = pWnd;

	if(bRepaint)
	{
		Invalidate();
	}
}

///////////////////////////////////////////////////
// full screen
void CPlaybackPannel::SetFullScreen(BOOL bFlag)
{
	if(bFlag == m_bFullScreen) return;

	if( bFlag )
	{
		//全屏
		//得到显示器分辨率
		int cx=GetSystemMetrics(SM_CXSCREEN);
		int cy=GetSystemMetrics(SM_CYSCREEN);

		//保存位置信息
		GetWindowPlacement(&_temppl);
		//修改风格
		//ModifyStyle(WS_SIZEBOX, 0, 0);//针对控件
		//ModifyStyle(WS_CHILD,WS_POPUP);//针对应用程序
		//修改父窗口
		_tempparent=SetParent(NULL);
		_tempparent->ShowWindow(SW_HIDE);
		//移动窗口
		MoveWindow(0,0,cx,cy);
	}
	else
	{
		//还原
		LockWindowUpdate();
		//还原父窗口
		_tempparent->ShowWindow(SW_SHOW);
		SetParent(_tempparent);
		//还原风格
		ModifyStyle(WS_POPUP,WS_CHILD);
		//还原位置
		SetWindowPlacement(&_temppl);
		UnlockWindowUpdate();
	}
	m_bFullScreen=bFlag;
}

BOOL CPlaybackPannel::GetFullScreen()
{
	return m_bFullScreen;
}

///////////////////////////////////////////////////
// multiscreen
void CPlaybackPannel::SetMultiScreen(BOOL bFlag,BOOL bReSplitWnd)
{
	if(m_bMultiScreen == bFlag) 
	{
		return;
	}

	m_bMultiScreen = bFlag;
	if(bReSplitWnd)
	{
		SetShowPlayWin(m_nLastSplit);
	}
}

BOOL CPlaybackPannel::GetMultiScreen()
{
	return m_bMultiScreen;
}

//////////////////////////////////////////////////
// autoadjustpos
void CPlaybackPannel::SetAutoAdjustPos(BOOL bFlag)
{
	if(m_bAutoAdjustPos==bFlag)
	{
		return;
	}

	m_bAutoAdjustPos=bFlag;
	SetShowPlayWin(m_nLastSplit);
}

BOOL CPlaybackPannel::GetAutoAdjustPos()
{
	return m_bAutoAdjustPos;
}

///////////////////////////////////////////////////
// get a rect by the index of a child
void CPlaybackPannel::CalcPageRect(LPRECT lpRect,int nIndex,int nPageCount)
{
	if((nPageCount<=0)||(nIndex>=nPageCount))
	{
		lpRect->left=lpRect->right=lpRect->top=lpRect->bottom=0;
		return;
	}
	//get row count
	int nRow=0;
	while((nRow)*(nRow)<nPageCount) nRow++;

	//get singledlg width and height
	int nWidth=(lpRect->right-lpRect->left)/nRow;
	int nHeight=(lpRect->bottom-lpRect->top)/nRow;

	//get top-left point
	CPoint pt;
	pt.x=lpRect->left+nWidth*(nIndex%nRow);
	pt.y=lpRect->top+nHeight*(nIndex/nRow);

	//set rect return back
	lpRect->left=pt.x + WINDOWS_OFFSET;
	lpRect->top=pt.y + WINDOWS_OFFSET;
	lpRect->right=lpRect->left+nWidth - WINDOWS_OFFSET *2;
	lpRect->bottom=lpRect->top+nHeight - WINDOWS_OFFSET *2;
}

///////////////////////////////////////////////////
// adjust a rect by defined proportion 
void CPlaybackPannel::AdjustRect(LPRECT lpRect)
{
	int nWidth=lpRect->right-lpRect->left;
	int nHeight=lpRect->bottom-lpRect->top;
	CPoint pt((lpRect->left+lpRect->right)/2,(lpRect->top+lpRect->bottom)/2);

	int nTemp=nWidth*8/11;
	if(nTemp>nHeight)
	{
		nWidth=nHeight*11/8;
	}
	else if(nTemp<nHeight)
	{
		nHeight=nTemp;
	}
	lpRect->left=pt.x-nWidth/2;
	lpRect->right=pt.x+nWidth/2;
	lpRect->top=pt.y-nHeight/2;
	lpRect->bottom=pt.y+nHeight/2;
}



int CPlaybackPannel::HitTest(CPoint pt)
{
	for(int i=0; i< MAX_WINDOW; i++)
	{
		CRect rect;
		m_wndVideo[i].GetClientRect(&rect);
		m_wndVideo[i].ClientToScreen(&rect);
		if(rect.PtInRect(pt))
			return i;
	}
	return -1;
}