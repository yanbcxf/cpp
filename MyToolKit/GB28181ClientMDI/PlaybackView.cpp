// PlaybackView.cpp : 实现文件
//

#include "stdafx.h"
#include "GB28181ClientMDI.h"
#include "PlaybackView.h"


// CPlaybackView

IMPLEMENT_DYNCREATE(CPlaybackView, CFormView)

CPlaybackView::CPlaybackView()
	: CFormView(CPlaybackView::IDD)
{
	m_nCtrlPalyTimeH	= 140;
	m_nSkinBtnsH		= 50;
	m_nCurWndIndex		= 0;		// 当前窗口号.


	m_bShowCtrlPlay = TRUE;
}

CPlaybackView::~CPlaybackView()
{
	m_SkinBtns.FreeAll();
}

void CPlaybackView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPlaybackView, CFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_MESSAGE(WM_EVENT_SEL_WND, OnEventSelWnd)			// 鼠标选择窗口
	ON_MESSAGE(WM_BITMAPSLIDER_MOVED,OnBitmapSliderMoved)
END_MESSAGE_MAP()


// CPlaybackView 诊断

#ifdef _DEBUG
void CPlaybackView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlaybackView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// 创建按钮组.
void CPlaybackView::CreateSkinBtns()
{
	CBitmap bmp;
	if(!bmp.LoadBitmap(IDB_BTNS_BACKGROUND))
	{
		return ;
	}

	BITMAP hbmp;
	bmp.GetBitmap(&hbmp);
	bmp.DeleteObject();

	m_nSkinBtnsH = hbmp.bmHeight;	// 按钮组高度.

	m_SkinBtns.Create(WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this,IDC_CTRLBTNS);
	m_SkinBtns.ShowWindow(SW_SHOW);
	m_SkinBtns.LoadDlgBitmap(IDB_BTNS_BACKGROUND,IDB_BTNS_BACKGROUND,IDB_BTNS_BACKGROUND);

	CString strTip;
	CRect rect(0,0,40,40);
	strTip = "打开声音";
	m_SkinBtns.AddButton(IDC_PLAY_VOICE,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_VOICE,IDB_VOICE_N,IDB_VOICE_M,IDB_VOICE_N,IDB_VOICE_N);

	strTip = "关闭声音";
	m_SkinBtns.AddButton(IDC_PLAY_VOICE_CLOSE,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_VOICE_CLOSE,IDB_VOICECLOSE_N,IDB_VOICECLOSE_M,IDB_VOICECLOSE_N,IDB_VOICECLOSE_N);
	m_SkinBtns.GetButton(IDC_PLAY_VOICE_CLOSE)->ShowWindow(SW_HIDE);

	m_SkinBtns.AddSepBmp(IDC_PLAY_SEPBMP1,IDB_BTNS_SEP,rect);

	strTip = "播放";
	m_SkinBtns.AddButton(IDC_PLAY_PLAY,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_PLAY,IDB_PLAY_N,IDB_PLAY_M,IDB_PLAY_D,IDB_PLAY_N);

	// 暂停.
	strTip = "暂停";
	m_SkinBtns.AddButton(IDC_PLAY_PAUSE,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_PAUSE,IDB_PAUSE_N,IDB_PAUSE_M,IDB_PAUSE_D,IDB_PAUSE_N);
	m_SkinBtns.GetButton(IDC_PLAY_PAUSE)->ShowWindow(SW_HIDE);

	strTip = "停止";
	m_SkinBtns.AddButton(IDC_PLAY_STOP,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_STOP,IDB_STOP_N,IDB_STOP_M,IDB_STOP_D,IDB_STOP_N);

	strTip = "单帧";
	m_SkinBtns.AddButton(IDC_PLAY_FRAME,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_FRAME,IDB_ONEBYONE_N,IDB_ONEBYONE_M,IDB_ONEBYONE_D,IDB_ONEBYONE_N);

	// 速度滑块.
	m_SkinBtns.AddSlider(IDC_SLIDER_SPEED,rect,_T("播放速度"));
	CBitmapSlider *pSlider = m_SkinBtns.GetSlider(IDC_SLIDER_SPEED);
	if(pSlider != NULL)
	{
		pSlider->SetBitmapChannel(IDB_SPEED_BK_N, IDB_SPEED_BK_N);
		pSlider->SetBitmapThumb(IDB_SPEED_THUMB_N, IDB_SPEED_THUMB_N, FALSE);
		pSlider->DrawFocusRect(FALSE);

		pSlider->SetRange(0,8);	
		pSlider->SetMargin(8,0,8,0);
		pSlider->SetPageSize(1);
		pSlider->SetPos(4);
	}

	// 滑块下方背景.
	m_SkinBtns.AddSepBmp(IDC_SLIDER_SPEED_TEXT,IDB_SPEED_TEXT,rect);

	m_SkinBtns.AddSepBmp(IDC_PLAY_SEPBMP2,IDB_BTNS_SEP,rect);

	// 截图.
	strTip = "截图";
	m_SkinBtns.AddButton(IDC_PLAY_PIC,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_PIC,IDB_CAPTURE_N,IDB_CAPTURE_M,IDB_CAPTURE_D,IDB_CAPTURE_N);

	// 录像.
	strTip = "开始录像";
	m_SkinBtns.AddButton(IDC_PLAY_REC,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_REC,IDB_RECORD_N,IDB_RECORD_M,IDB_RECORD_D,IDB_RECORD_N);

	// 停止录像.
	strTip = "停止录像";
	m_SkinBtns.AddButton(IDC_PLAY_REC_STOP,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_REC_STOP,IDB_STOP_RECORD_N,IDB_STOP_RECORD_M,IDB_STOP_RECORD_D,IDB_STOP_RECORD_N);
	m_SkinBtns.GetButton(IDC_PLAY_REC_STOP)->ShowWindow(SW_HIDE);

	// 下载.
	strTip = "下载";
	m_SkinBtns.AddButton(IDC_PLAY_DOWN,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_PLAY_DOWN,IDB_DOWN_N,IDB_DOWN_M,IDB_DOWN_D,IDB_DOWN_N);

	m_SkinBtns.AddSepBmp(IDC_PLAY_SEPBMP3,IDB_BTNS_SEP,rect);

	// 单画面.
	strTip = "单画面";
	m_SkinBtns.AddButton(IDC_ONEPAN,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_ONEPAN,IDB_ONE_N,IDB_ONE_M,IDB_ONE_D,IDB_ONE_N);

	// 四画面.
	strTip = "四画面";
	m_SkinBtns.AddButton(IDC_FOURPAN,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_FOURPAN,IDB_FOUR_N,IDB_FOUR_M,IDB_FOUR_D,IDB_FOUR_N);

	// 全屏.
	strTip = "全屏";
	m_SkinBtns.AddButton(IDC_FULLPAN,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_FULLPAN,IDB_FULLSCREEN_N,IDB_FULLSCREEN_M,IDB_FULLSCREEN_D,IDB_FULLSCREEN_N);

	// 隐藏时间轴.
	strTip = "隐藏时间轴";
	m_SkinBtns.AddButton(IDC_HIDE_PLAYCTRL,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_HIDE_PLAYCTRL,IDB_HIDE_N,IDB_HIDE_M,IDB_HIDE_D,IDB_HIDE_N);

	// 显示时间轴.
	strTip = "显示时间轴";
	m_SkinBtns.AddButton(IDC_SHOW_PLAYCTRL,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_SHOW_PLAYCTRL,IDB_SHOW_N,IDB_SHOW_M,IDB_SHOW_D,IDB_SHOW_N);
	m_SkinBtns.GetButton(IDC_SHOW_PLAYCTRL)->ShowWindow(SW_HIDE);
	// 


	strTip = "检索录像文件";
	m_SkinBtns.AddButton(IDC_FIND,rect,strTip);
	m_SkinBtns.CopyFrom(IDC_FIND,IDB_FIND_N,IDB_FIND_D,IDB_FIND_N,IDB_FIND_N);
	m_SkinBtns.GetButton(IDC_FIND)->ShowWindow(SW_HIDE);


	strTip = "录像检索时间";
	// 时间.
	m_SkinBtns.AddDateTimeCtrl(IDC_DATATIME,rect);
	m_SkinBtns.GetDateTimeCtrl(IDC_DATATIME)->ShowWindow(SW_HIDE);	

	// 设备类型.
	m_SkinBtns.AddComboBox(IDC_DEVICE_TYPE,rect);
	m_SkinBtns.AddComboboxString(IDC_DEVICE_TYPE,_T("存储录像"),NVRTYPE);
	m_SkinBtns.AddComboboxString(IDC_DEVICE_TYPE,_T("DVR录像"),DVRTYPE);
	m_SkinBtns.AddComboboxString(IDC_DEVICE_TYPE,_T("本地录像"),LOCALTYPE);
	//	m_SkinBtns.AddComboboxString(IDC_DEVICE_TYPE,_T("邦诺录像"),BRSTYPE);
	m_SkinBtns.SetComboboxCurSel(IDC_DEVICE_TYPE,NVRTYPE);
	m_SkinBtns.GetComboBox(IDC_DEVICE_TYPE)->ShowWindow(SW_HIDE);	

}

// CPlaybackView 消息处理程序

void CPlaybackView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	m_screenPannel.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_SCREENPANNEL);
	m_screenPannel.ShowWindow(SW_SHOW);

	// 创建时间轴.
	m_CtrlPlayTime.Create(WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this,IDC_CTRLWNDTIME);
	m_CtrlPlayTime.InvalidateCtrl();
	if(m_bShowCtrlPlay)
	{
		m_CtrlPlayTime.ShowWindow(SW_SHOW);
	}
	else
	{
		m_CtrlPlayTime.ShowWindow(SW_HIDE);
	}

	CreateSkinBtns();

	// 创建下载对话框.
	for(int i = 0; i < MAX_WINDOW; i ++)
	{
		CString strText;
		strText.Format("下载窗口%d",i+1);
		/*m_showfile[i].Create(IDD_SHOWFILEINFO,this);
		m_showfile[i].SetDownLoadWindowText(strText);
		m_showfile[i].ShowWindow(SW_HIDE);*/
	}

	OnFourPannel();

	RecalcDlg();
}


// 单窗口.
void CPlaybackView::OnOnePannel() 
{
	m_screenPannel.SetMultiScreen(TRUE,FALSE);
	m_screenPannel.SetShowPlayWin(SPLIT1);
	SetCurrentWnd(0);
}

// 四窗口
void CPlaybackView::OnFourPannel() 
{
	m_screenPannel.SetMultiScreen(TRUE,FALSE);
	m_screenPannel.SetShowPlayWin(SPLIT4);
	SetCurrentWnd(0);
}

// 全屏.
void CPlaybackView::OnFullScreenPannel() 
{
	m_screenPannel.SetMultiScreen(TRUE,FALSE);
	m_screenPannel.SetFullScreen(TRUE);
}


void CPlaybackView::SetCurrentWnd(int nWndIndex)
{
	m_nCurWndIndex = nWndIndex;
	m_screenPannel.SetCurrentWnd(nWndIndex);
	SetCurWndBtns(nWndIndex);
	m_CtrlPlayTime.SetCurWndText(m_screenPannel.GetWndToolTip(nWndIndex));
}



int  CPlaybackView::GetCurrentWnd()
{
	return m_nCurWndIndex;
}


// 自动调整按钮组大小.
void CPlaybackView::AutoRecalcLayout()
{
	if(m_SkinBtns.GetSafeHwnd() == NULL)
	{
		return ;
	}

	CBitmap bmp;
	if(!bmp.LoadBitmap(IDB_PLAY_N))
	{
		return ;
	}

	BITMAP hbmp;
	bmp.GetBitmap(&hbmp);
	bmp.DeleteObject();

	// 单个位图宽高.
	int nBmpWidth	= hbmp.bmWidth;
	int nBmpHeight	= hbmp.bmHeight;
	int nOffset	= 4;
	int nSepW	= 2; // 分隔条宽度.

	CRect rectParent;
	m_SkinBtns.GetClientRect(&rectParent);

	CRect rect;
	rect.top = rectParent.top + rectParent.Height()/2 - nBmpHeight/2;
	rect.bottom = rect.top + nBmpHeight;

	// 开打/关闭声音.
	rect.left = nOffset;
	rect.right = rect.left + 22;
	m_SkinBtns.GetButton(IDC_PLAY_VOICE)->MoveWindow(rect);
	m_SkinBtns.GetButton(IDC_PLAY_VOICE_CLOSE)->MoveWindow(rect);

	// 分隔条.
	CRect rectSep;
	rectSep.left = rect.right + nOffset;
	rectSep.right = rectSep.left + nSepW;
	rectSep.top = rectParent.top;
	rectSep.bottom = rectParent.bottom;
	rect.right = rectSep.right;
	m_SkinBtns.EditSepBmpRect(IDC_PLAY_SEPBMP1,rectSep);

	// 停止.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_PLAY_STOP)->MoveWindow(rect);

	// 播放/ 暂停.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_PLAY_PLAY)->MoveWindow(rect);	
	m_SkinBtns.GetButton(IDC_PLAY_PAUSE)->MoveWindow(rect);

	// 单帧.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_PLAY_FRAME)->MoveWindow(rect);

	// 速度滑块.
	CRect rectSlider;
	CBitmap bmpSlider;
	if(bmpSlider.LoadBitmap(IDB_SPEED_BK_N))
	{
		bmpSlider.GetBitmap(&hbmp);
		bmpSlider.DeleteObject();
	}

	rectSlider.top = rectParent.top + 8;
	rectSlider.bottom = rectSlider.top + hbmp.bmHeight;
	rectSlider.left = rect.right + nOffset * 2;
	rectSlider.right = rectSlider.left + hbmp.bmWidth;
	rect.right = rectSlider.right;

	CBitmapSlider *pSlider = m_SkinBtns.GetSlider(IDC_SLIDER_SPEED);
	if(pSlider != NULL)
	{
		pSlider->MoveWindow(rectSlider);
	}

	// 滑块文本背景.
	if(bmpSlider.LoadBitmap(IDB_SPEED_TEXT))
	{
		bmpSlider.GetBitmap(&hbmp);
		bmpSlider.DeleteObject();
	}

	rectSlider.top = rectSlider.bottom + nOffset;
	rectSlider.bottom = rectSlider.top + hbmp.bmHeight;
	m_SkinBtns.EditSepBmpRect(IDC_SLIDER_SPEED_TEXT,rectSlider);

	rectSep.left = rect.right + nOffset;
	rectSep.right = rectSep.left + nSepW;
	rect.right = rectSep.right;
	m_SkinBtns.EditSepBmpRect(IDC_PLAY_SEPBMP2,rectSep);

	// 截图.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_PLAY_PIC)->MoveWindow(rect);

	// 录像/停止录像.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_PLAY_REC)->MoveWindow(rect);
	m_SkinBtns.GetButton(IDC_PLAY_REC_STOP)->MoveWindow(rect);

	// 下载/停止下载.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_PLAY_DOWN)->MoveWindow(rect);

	// 分隔条.
	rectSep.left = rect.right + nOffset * 2;
	rectSep.right = rectSep.left + nSepW;
	rect.right = rectSep.right;
	m_SkinBtns.EditSepBmpRect(IDC_PLAY_SEPBMP3,rectSep);

	// 单画面.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_ONEPAN)->MoveWindow(rect);

	// 四画面.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_FOURPAN)->MoveWindow(rect);

	// 全屏.
	rect.left = rect.right + nOffset;
	rect.right = rect.left + nBmpWidth;
	m_SkinBtns.GetButton(IDC_FULLPAN)->MoveWindow(rect);

	if(m_nSearchTimeMod>0)
	{
		// 分隔条.
		rectSep.left = rect.right + nOffset * 2;
		rectSep.right = rectSep.left + nSepW;
		rect.right = rectSep.right;
		m_SkinBtns.EditSepBmpRect(IDC_PLAY_SEPBMP3,rectSep);		

		int nDateTime = 24;
		int nTop = 0;
		int nBottom = 0;
		CRect rectTime;
		nTop = rectParent.top + rectParent.Height()/2 - nDateTime/2;
		nBottom = nTop + nDateTime;

		// 获取当前选择的设备类型.

		if(m_nSearchTimeMod==1)
		{
			// 时间,高度置成30.		
			rect.left = rect.right + nOffset;
			rect.right = rect.left + 100;
			m_SkinBtns.GetDateTimeCtrl(IDC_DATATIME)->MoveWindow(CRect(rect.left,nTop,rect.right,nBottom));
			m_SkinBtns.GetDateTimeCtrl(IDC_DATATIME)->ShowWindow(SW_SHOW);
		}

		if(m_nSearchTimeMod>1)
		{		
			// 获取当前选择的设备类型.
			rect.left = rect.right + nOffset;
			rect.right = rect.left + 80;						
			m_SkinBtns.GetComboBox(IDC_DEVICE_TYPE)->MoveWindow(CRect(rect.left,nTop,rect.right,300));
			m_SkinBtns.GetComboBox(IDC_DEVICE_TYPE)->ShowWindow(SW_SHOW);

			// 时间,高度置成30.		
			rect.left = rect.right + nOffset;
			rect.right = rect.left + 100;
			m_SkinBtns.GetDateTimeCtrl(IDC_DATATIME)->MoveWindow(CRect(rect.left,nTop,rect.right,nBottom));
			m_SkinBtns.GetDateTimeCtrl(IDC_DATATIME)->ShowWindow(SW_SHOW);

			// 查找.
			rect.right = rectParent.right- 20;			
			rect.left = rect.right - 55;
			rect.top+=2;
			m_SkinBtns.GetButton(IDC_FIND)->MoveWindow(rect);
			m_SkinBtns.GetButton(IDC_FIND)->ShowWindow(SW_SHOW);
		}
	}
	// 显示/隐藏时间条.
	rect.right = rectParent.right;
	rect.left = rect.right - 18;
	m_SkinBtns.GetButton(IDC_HIDE_PLAYCTRL)->MoveWindow(rect);
	m_SkinBtns.GetButton(IDC_SHOW_PLAYCTRL)->MoveWindow(rect);

	m_SkinBtns.RecalcLayout();
}

// 设置无操作按钮状态.
void CPlaybackView::SetNormBtns()
{
	SetBtnsPlay(TRUE);
	SetBtnsRecord(FALSE);
	SetBtnsSound(FALSE);
	SetBtnSpeed(4);
}


// 设置当前窗口的按钮状态.
void CPlaybackView::SetCurWndBtns(int nWndIndex)
{
	
	if(m_screenPannel.m_wndVideo[nWndIndex].m_listFiles.size()==0)
	{
		SetNormBtns();
		return ;
	}

	BOOL bEnableRecord = TRUE;
	int  nSpeed		= m_screenPannel.m_wndVideo[nWndIndex].nSpeed;
	BOOL bVoice		= m_screenPannel.m_wndVideo[nWndIndex].bVoice;
	BOOL bRecord	= m_screenPannel.m_wndVideo[nWndIndex].bRecordfile;
	BOOL bShowPlay	= FALSE;

	// 没有播放.显示播放按钮.
	if(m_screenPannel.m_wndVideo[nWndIndex].m_play_port < 0 || m_screenPannel.m_wndVideo[nWndIndex].m_net_port < 0 )
	{
		bShowPlay	= TRUE;
		bVoice		= FALSE;
		bRecord		= FALSE;
		nSpeed		= 4;
	}
	else
	{	
		// 单帧. 显示播放按钮.
		if(m_screenPannel.m_wndVideo[nWndIndex].bOnebyone)
		{
			bShowPlay = TRUE;
		}	
		else if(m_screenPannel.m_wndVideo[nWndIndex].m_bPause)
		{	
			// 暂停状态显示播放按钮.
			bShowPlay = TRUE;
		}
	}

	SetBtnsPlay(bShowPlay);
	SetBtnsSound(bVoice);
	SetBtnsRecord(bRecord,bEnableRecord);
	SetBtnSpeed(nSpeed);
}


// 设置界面滑块位置.
void CPlaybackView::SetBtnSpeed(int nPos)
{
	CBitmapSlider *pSlider = m_SkinBtns.GetSlider(IDC_SLIDER_SPEED);
	if(pSlider != NULL)
		pSlider->SetPos(nPos); 
}

// 设置界面按钮状态.
// TRUE 播放 / FALSE 暂停.
void CPlaybackView::SetBtnsPlay(BOOL bPlay)
{
	CSkinButton *pBtnPlay = m_SkinBtns.GetButton(IDC_PLAY_PLAY);
	CSkinButton *pBtnPause = m_SkinBtns.GetButton(IDC_PLAY_PAUSE);
	if(pBtnPause == NULL || pBtnPlay == NULL)
	{
		return;
	}

	if(bPlay)
	{		
		pBtnPlay->ShowWindow(SW_SHOW);
		pBtnPause->ShowWindow(SW_HIDE);
	}
	else
	{
		pBtnPlay->ShowWindow(SW_HIDE);
		pBtnPause->ShowWindow(SW_SHOW);
	}
}

// 设置界面按钮状态.
// FALSE 录像 / TRUE 停止录像.
void CPlaybackView::SetBtnsRecord(BOOL bRecord,BOOL bEnable)
{
	CSkinButton *pBtn1 = m_SkinBtns.GetButton(IDC_PLAY_REC);
	CSkinButton *pBtn2 = m_SkinBtns.GetButton(IDC_PLAY_REC_STOP);
	if(pBtn1 == NULL || pBtn2 == NULL)
	{
		return;
	}

	if(bRecord)
	{		
		pBtn1->ShowWindow(SW_HIDE);
		pBtn2->ShowWindow(SW_SHOW);
	}
	else
	{
		pBtn1->ShowWindow(SW_SHOW);
		pBtn2->ShowWindow(SW_HIDE);
	}

	pBtn1->EnableWindow(bEnable);
	pBtn2->EnableWindow(bEnable);
}

// 设置界面按钮状态.
// TRUE 关闭声音 / FALSE 打开声音.
void CPlaybackView::SetBtnsSound(BOOL bSound)
{
	CSkinButton *pBtn1 = m_SkinBtns.GetButton(IDC_PLAY_VOICE);
	CSkinButton *pBtn2 = m_SkinBtns.GetButton(IDC_PLAY_VOICE_CLOSE);
	if(pBtn1 == NULL || pBtn2 == NULL)
	{
		return;
	}

	if(bSound)
	{		
		pBtn1->ShowWindow(SW_HIDE);
		pBtn2->ShowWindow(SW_SHOW);
	}
	else
	{
		pBtn1->ShowWindow(SW_SHOW);
		pBtn2->ShowWindow(SW_HIDE);
	}
}

// 显示/隐藏时间条.
BOOL CPlaybackView::ShowPlayCtrlTime(BOOL bShow)
{
	CSkinButton *pBtn1 = m_SkinBtns.GetButton(IDC_HIDE_PLAYCTRL);
	CSkinButton *pBtn2 = m_SkinBtns.GetButton(IDC_SHOW_PLAYCTRL);
	if(pBtn1 == NULL || pBtn2 == NULL)
	{
		return FALSE;
	}

	m_bShowCtrlPlay = bShow;
	if(m_bShowCtrlPlay)
	{
		m_CtrlPlayTime.ShowWindow(SW_SHOW);
		pBtn1->ShowWindow(SW_SHOW);
		pBtn2->ShowWindow(SW_HIDE);
	}
	else
	{
		m_CtrlPlayTime.ShowWindow(SW_HIDE);
		pBtn1->ShowWindow(SW_HIDE);
		pBtn2->ShowWindow(SW_SHOW);
	}

	RecalcDlg();

	return TRUE;
}

BOOL CPlaybackView::RecalcDlg()
{
	if( m_screenPannel.GetSafeHwnd() == NULL  ||
		m_CtrlPlayTime.GetSafeHwnd() == NULL ||
		m_SkinBtns.GetSafeHwnd() == NULL)
	{
		return FALSE;
	}

	int nCtrlPalyTimeH = 0;
	if(m_bShowCtrlPlay)
	{
		nCtrlPalyTimeH = m_nCtrlPalyTimeH;
	}

	CRect rcMain,rectWnd;
	GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd);

	rcMain.top		= rectWnd.top;
	rcMain.bottom	= rectWnd.bottom- nCtrlPalyTimeH - m_nSkinBtnsH;
	rcMain.left		= rectWnd.left;
	rcMain.right	= rectWnd.right;
	m_screenPannel.MoveWindow(&rcMain);

	CRect rect;	
	rect.left = rcMain.left;
	rect.right = rcMain.right;

	// 时间条.
	if(m_bShowCtrlPlay)
	{
		rect.bottom = rectWnd.bottom;
		rect.top = rect.bottom - m_nCtrlPalyTimeH;
		m_CtrlPlayTime.MoveWindow(rect);
		m_CtrlPlayTime.RecalcLayout();
	}
	else
	{
		rect.top = rectWnd.bottom;
	}

	// 按钮组.
	rect.bottom = rect.top;
	rect.top = rect.bottom - m_nSkinBtnsH;
	m_SkinBtns.MoveWindow(rect);
	AutoRecalcLayout();

	//标题
	CWnd *pWnd = GetDlgItem(IDC_TITLE);
	if(pWnd != NULL)
	{
		rect.top	= 8;
		rect.bottom = 28;
		pWnd->MoveWindow(rect);
	}

	return TRUE;
}

void CPlaybackView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	ShowScrollBar(SB_BOTH,FALSE);//垂直水平方向的滚动条都不显示。

	RecalcDlg();
}

BOOL CPlaybackView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(lParam != BN_CLICKED)
	{
		return CFormView::OnCommand(wParam,lParam);
	}

	UINT uID = (UINT)wParam;
	switch (uID)
	{
	case IDC_HIDE_PLAYCTRL:
		{
			ShowPlayCtrlTime(FALSE);
			break;
		}
	case IDC_SHOW_PLAYCTRL:
		{
			ShowPlayCtrlTime(TRUE);
			break;
		}
	case IDC_PLAY_VOICE:
	case IDC_PLAY_VOICE_CLOSE:
		{			
			//OnPlayVoice();// 打开或关闭声音.
			break;
		}
	case IDC_PLAY_PLAY:
		{			
			//OnPlay();// 播放.
			int nWndIndex = GetCurrentWnd();
			m_screenPannel.m_wndVideo[nWndIndex].OnPlay();
			break;
		}
	case IDC_PLAY_PAUSE:
		{
			// 暂停操作.
			int nWndIndex = GetCurrentWnd();
			m_screenPannel.m_wndVideo[nWndIndex].OnPause();
			
			break;
		}
	case IDC_PLAY_STOP:
		{			
			int nWndIndex = GetCurrentWnd();
			m_screenPannel.m_wndVideo[nWndIndex].OnStop();	// 停止播放.
			break;
		}
	case IDC_PLAY_FRAME:
		{			
			//OnPlayFrame();// 单帧播放.
			break;
		}
	case IDC_PLAY_PIC:
		{
			//OnPlayPicture();	// 截图.
			break;
		}
	case IDC_PLAY_REC:
	case IDC_PLAY_REC_STOP:
		{
			//OnPlayRecord();	// 打开/关闭录像.
			break;
		}
	case IDC_PLAY_DOWN:
		{
			//OnPlayDown();	// 下载.
			break;
		}
	case IDC_ONEPAN:
		{
			OnOnePannel();	// 单画面.
			break;
		}
	case IDC_FOURPAN:
		{
			OnFourPannel();// 四画面.
			break;
		}
	case IDC_FULLPAN:
		{
			OnFullScreenPannel();// 全屏.
			break;
		}
	case IDC_FIND:				// 查找.
		{
			//OnFind();
			break;
		}
	}

	int nWndIndex = GetCurrentWnd();
	SetCurWndBtns(nWndIndex);

	return CFormView::OnCommand(wParam, lParam);
}

void CPlaybackView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	// 停止下载.
	for(int i = 0; i < MAX_WINDOW; i ++)
	{
		//m_showfile[i].DestroyWindow();
	}

	
}

HBRUSH CPlaybackView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_STATIC ) 
	{
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextColor(RGB(150,150,150));

		return (HBRUSH)GetStockObject (HOLLOW_BRUSH); 
	}

	//设定控件的背景色、前景色以及字体等等，别的信息也可以设置
	return hbr;
}


LRESULT CPlaybackView::OnEventSelWnd(WPARAM wParam,LPARAM lParam)
{
	SetCurrentWnd((int)wParam);

	m_screenPannel.Invalidate();

	return 0;		 
}

// 滑动结束.
LRESULT CPlaybackView::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam)
{
	UINT uID = (UINT)wParam;
	int nPos = (int)lParam;

	if(uID != IDC_SLIDER_SPEED)
	{
		return 0;
	}

	int nWndIndex = GetCurrentWnd();
	// 速度滑块.
	if(m_screenPannel.m_wndVideo[nWndIndex].m_net_port< 0 || m_screenPannel.m_wndVideo[nWndIndex].m_play_port< 0)
	{
		//g_pPlayBackCtrl->ShowHyperLinkDlg("播放操作","窗口视频未启动，不支持变速播放",FALSE);
		return 0;
	}

	// 单帧不支持快进\慢放操作.
	if (m_screenPannel.m_wndVideo[nWndIndex].bOnebyone)
	{
		return 0;
	}

	DebugTrace("nPos=%d",nPos);
	// 没有变化,不处理.
	if(m_screenPannel.m_wndVideo[nWndIndex].nSpeed == nPos)
	{
		//g_pPlayBackCtrl->ShowHyperLinkDlg("播放操作","播放速度未改变",FALSE);
		return 0;
	}
	else
		m_screenPannel.m_wndVideo[nWndIndex].nSpeed=nPos;

	sipd_playback_mans_rtsp_ind rtspDesc={0};
	if(nPos==4)
	{
		//	正常播放
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 1.0\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 1.0);
	}
	// 慢放
	else if(nPos==3)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 0.5\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 0.5);
	}
	else if(nPos==2)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 0.25\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 0.25);
	}
	else if(nPos==1)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 0.125\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 0.125);
	}
	else if(nPos==0)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 0.0625\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 0.0625);
	}
	//	快放
	else if(nPos==5)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 2.0\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 2.0);
	}
	else if(nPos==6)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 4.0\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 4.0);
	}
	else if(nPos==7)
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale: 8.0\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 8.0);
	}
	else 
	{
		sprintf(rtspDesc.rtsp_des,"PLAY RTSP/1.0\r\nCSeq: 1\r\nScale:16.0\r\n");

		player_stream_speed(m_screenPannel.m_wndVideo[nWndIndex].m_play_port, 16.0);
	}
	
	rtspDesc.len = strlen(rtspDesc.rtsp_des);
	int iCmdId = Dll_Call_media_control(m_screenPannel.m_wndVideo[nWndIndex].m_net_port, SIPD_TO_CMS_CMD_TYPE_PLAYBACK_CONTROL_RTSP,&rtspDesc);


	// 设置按钮组位图状态.
	SetCurWndBtns(nWndIndex);

	return 0;
}
DROPEFFECT CPlaybackView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	return DROPEFFECT_COPY;

	return CFormView::OnDragEnter(pDataObject, dwKeyState, point);
}

void CPlaybackView::OnDragLeave()
{
	// TODO: 在此添加专用代码和/或调用基类

	CFormView::OnDragLeave();
}

DROPEFFECT CPlaybackView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	return DROPEFFECT_COPY;

	return CFormView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CPlaybackView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类
	HGLOBAL hgMem = NULL; 
	if (pDataObject->IsDataAvailable(CF_TEXT))
	{
		hgMem = pDataObject->GetGlobalData(CF_TEXT);
		sipd_catalog_item * pItem = (sipd_catalog_item *)GlobalLock(hgMem);
		if (pItem != NULL)
		{
			int nHit = m_screenPannel.HitTest(point);
			if(nHit>=0)
			{
				SetCurrentWnd(nHit);
				m_CtrlPlayTime.ClearWnd(nHit);

				m_screenPannel.m_wndVideo[nHit].OnStop();

				// 设置无操作按钮状态.
				SetNormBtns();
				m_screenPannel.m_wndVideo[nHit].SearchFile(pItem);
			}
			else
				AfxMessageBox("未找到窗口");
		}

		if(hgMem)
		{
			// 空间的解锁和释放
			GlobalUnlock(hgMem);
			GlobalFree((HGLOBAL)hgMem);
		}
	}

	return CFormView::OnDrop(pDataObject, dropEffect, point);
}

int CPlaybackView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_CTreeDropTarget.Register(this);
	return 0;
}

