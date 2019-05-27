#include "stdafx.h"
#include "RealplayPannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CRealplayPannel::CRealplayPannel()
{
	m_pActivePage	= NULL;
	m_bFullScreen	= FALSE;	// 全屏标记
	m_bMultiScreen	= TRUE;		// 多屏标记

}

CRealplayPannel::~CRealplayPannel()
{

}

BEGIN_MESSAGE_MAP(CRealplayPannel, CWnd)
	//{{AFX_MSG_MAP(CRealplayPannel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CRealplayPannel::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext )
{
	dwStyle|=WS_EX_TOOLWINDOW|WS_CLIPCHILDREN;
	return CWnd::Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,nID,pContext );
}

/////////////////////////////////////////////////////////////////////////////
// CRealplayPannel member functions
void CRealplayPannel::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CWnd *pWnd;
	pWnd = GetParent();
	pWnd->SendMessage(WM_MOUSEMOVE);
	CWnd::OnMouseMove(nFlags,point);
}

void CRealplayPannel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
}

BOOL CRealplayPannel::OnEraseBkgnd(CDC* pDC) 
{
	CRect rt;
	GetClientRect(&rt);

	CBrush br;
	br.CreateSolidBrush(RGB(10,10,10));
	pDC->FillRect(&rt,&br);
	return TRUE;
}

int CRealplayPannel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	int i;
	for(i = 0; i < MAX_WNDNUM; i++)
	{
		m_wndVideo[i].m_nWndNo = i;
		m_wndVideo[i].Create(IDD_DIALOG_OUTPUT,this);
		m_wndVideo[i].SetParentScreenPannel(this);
		m_wndVideo[i].ShowWindow(SW_HIDE);
	}

	SetActiveWnd(&m_wndVideo[0]);
	SetShowPlayWin(SPLIT4);

	return 0;
}

BOOL CRealplayPannel::SetShowPlayWin(int nMain)
{
	switch(nMain) 
	{
	case SPLIT1:
		m_nWindowNum = 1;
		break;
	case SPLIT4:
		m_nWindowNum = 4;
		break;
	case SPLIT9:
		m_nWindowNum = 9;
		break;
	case SPLIT16:
		m_nWindowNum = 16;
		break;
	case SPLIT25:
		m_nWindowNum = 25;
		break;
	case SPLIT36:
		m_nWindowNum = 36;
		break;
	case SPLIT49:
		m_nWindowNum = 49;
		break;
	case SPLIT64:
		m_nWindowNum = 64;
		break;
	default:
		m_nWindowNum = 4;
		break;
	}

	nLastSplit = nMain;

	SetMultiScreen(TRUE);
	SetWndPos(0);

	return TRUE;
}

void CRealplayPannel::OnDestroy() 
{
	for(int i = 0; i < MAX_WNDNUM; i ++)
	{
		m_wndVideo[i].DestroyWindow();
	}

	CWnd::OnDestroy();
}

void CRealplayPannel::SetActiveWnd(CRealplayWnd *pWnd,BOOL bRePaint)
{
	if(pWnd == NULL)
	{
		return;
	}

	// 设置当前窗口号.
	m_nCurWnd = pWnd->m_nWndNo;
	pWnd->m_bSelWnd = TRUE;
	if(m_pActivePage == pWnd)
	{
		return;
	}

	if(m_pActivePage != NULL)
	{
		m_pActivePage->m_bSelWnd = FALSE;
		m_pActivePage->SetCtrlPos();
		if(m_pActivePage->IsWindowVisible() && bRePaint)
		{
			m_pActivePage->Invalidate();
		}		
	}

	m_pActivePage = pWnd;
	m_pActivePage->m_bSelWnd = TRUE;
	m_pActivePage->SetCtrlPos();
	if(bRePaint)
	{
		m_pActivePage->Invalidate();
	}
}

void CRealplayPannel::SetWndPos(int npos)
{
	int i = 0;
	// 隐藏不显示的窗口.
	for(i = m_nWindowNum; i < MAX_WNDNUM; i ++)
	{
		if(m_wndVideo[i].IsWindowVisible())
		{
			m_wndVideo[i].ShowWindow(SW_HIDE);
		}
	}

	CRect rtClient,rt;
	GetClientRect(&rtClient);

	// 多画面.
	if(m_bMultiScreen)
	{
		HDWP hdwp = BeginDeferWindowPos(m_nWindowNum);
		for (i = 0;i < m_nWindowNum; i ++)
		{
			rt = rtClient;
			CalcPageRect(rt,i,m_nWindowNum);
			hdwp = ::DeferWindowPos( hdwp, m_wndVideo[i].GetSafeHwnd(), GetSafeHwnd(),
				rt.left,rt.top,
				rt.Width(),rt.Height(),
				SWP_NOZORDER);	
		}

		::EndDeferWindowPos(hdwp);

		if(m_nCurWnd >= m_nWindowNum)
		{			
			m_nCurWnd = 0;			
		}
		SetActiveWnd(&m_wndVideo[m_nCurWnd]);

		for (i =  0;i < m_nWindowNum; i ++)
		{	
			m_wndVideo[i].SetCtrlPos();
			if(!m_wndVideo[i].IsWindowVisible())
			{
				m_wndVideo[i].ShowWindow(SW_SHOW);
			}
		}

		m_wndVideo[m_nCurWnd].Invalidate();
	}
	else
	{
		for (i = 0;i < MAX_WNDNUM; i ++)
		{
			if(m_pActivePage == &m_wndVideo[i])
			{
				m_nCurWnd = i;
			}
			else
			{
				m_wndVideo[i].ShowWindow(SW_HIDE);				
			}
		}

		m_wndVideo[m_nCurWnd].MoveWindow(rtClient,TRUE);		
		SetActiveWnd(&m_wndVideo[m_nCurWnd],FALSE);
		m_wndVideo[m_nCurWnd].SetCtrlPos();
		m_wndVideo[m_nCurWnd].Invalidate();
	}	

	Invalidate();
}

void CRealplayPannel::SetFullScreen(BOOL bFlag)
{
	if(bFlag==m_bFullScreen) return;
	if( bFlag )
	{//全屏
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
		//		DebugTrace("全屏 窗口 %d",_tempparent);
		MoveWindow(0,0,cx,cy);
	}
	else
	{//还原
		LockWindowUpdate();
		//还原父窗口
		_tempparent->ShowWindow(SW_SHOW);
		SetParent(_tempparent);
		//		DebugTrace("还原 窗口 %d",_tempparent);
		//还原风格
		ModifyStyle(WS_POPUP,WS_CHILD);
		//还原位置
		SetWindowPlacement(&_temppl);
		UnlockWindowUpdate();
	}
	m_bFullScreen=bFlag;
	Invalidate();
}

BOOL CRealplayPannel::GetFullScreen()
{
	return m_bFullScreen;
}

///////////////////////////////////////////////////
// multiscreen
void CRealplayPannel::SetMultiScreen(BOOL bFlag)
{
	m_bMultiScreen = bFlag;
}

BOOL CRealplayPannel::GetMultiScreen()
{
	return m_bMultiScreen;
}

///////////////////////////////////////////////////
// get a rect by the index of a child
void CRealplayPannel::CalcPageRect(LPRECT lpRect,int nIndex,int nPageCount)
{
	if((nPageCount<=0)||(nIndex>=nPageCount))
	{
		lpRect->left=lpRect->right=lpRect->top=lpRect->bottom=0;
		return;
	}

	int nRow=0;
	//get row count
	while((nRow)*(nRow)<nPageCount) nRow++;

	//get singledlg width and height
	int nWidth=(lpRect->right-lpRect->left)/nRow;
	int nHeight=(lpRect->bottom-lpRect->top)/nRow;

	//get top-left point
	CPoint pt;
	pt.x=lpRect->left+nWidth*(nIndex%nRow);
	pt.y=lpRect->top+nHeight*(nIndex/nRow);

	//set rect return back
	lpRect->left=pt.x;
	lpRect->top=pt.y;
	lpRect->right=lpRect->left+nWidth;
	lpRect->bottom=lpRect->top+nHeight;
}

int CRealplayPannel::HitTest(CPoint pt)
{
	for(int i=0; i<m_nWindowNum; i++)
	{
		CRect rect;
		m_wndVideo[i].GetClientRect(&rect);
		m_wndVideo[i].ClientToScreen(&rect);
		if(rect.PtInRect(pt))
			return i;
	}
	return -1;
}