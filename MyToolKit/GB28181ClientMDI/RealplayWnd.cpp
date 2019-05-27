// CRealplayWnd.cpp : implementation file
//

#include "stdafx.h"

#include "RealplayWnd.h"
// #include "ClientCtl.h"
#include "RealplayPannel.h"
// #include "automessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRealplayWnd dialog


#define		VIDEO_REPAINT		WM_USER + 1988

#define    TTS_BALLOON  0x40
#define    TTS_CLOSE   0x80
#define    TTF_PARSELINKS   0x1000

#define    RATIO_TYPE_FULL	0
#define    RATIO_TYPE_4_3	1
#define    RATIO_TYPE_16_9	2
const float EPSINON = 0.0000001;


CRealplayWnd::CRealplayWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CRealplayWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRealplayWnd)
	//}}AFX_DATA_INIT
	m_hRecordEvent = NULL;
	m_hRecordThread = NULL;
	m_pParentWnd = NULL;
	m_hStaticBrush = CreateSolidBrush(m_WndColor);


	m_bTip = FALSE;
	hRecIcon = NULL;
	hStopRecIcon = NULL;
	hSoundIcon = NULL;
	hStopSoundIcon = NULL;
	hVedioEffIcon = NULL;

	hCaptureIcon = NULL;
	hStopPlayIcon = NULL;

	hCurRecIcon = NULL;
	hCurStopRecIcon = NULL;
	hCurCaptureIcon = NULL;
	hCurVedioIcon = NULL;	//当前窗口
	hCurSoundIcon = NULL;
	hCurStopSoundIcon = NULL;
	hCurStopPlayIcon = NULL;

	m_nWndNo = 0;
	m_iPlayhandle = -1;
	memset(m_sTreeNodeId,0,100);
	memset(m_sOldTreeNodeId,0,100);
	m_lUserId = -1;			//nvr登陆值

	memset(m_svtsIP,0,20);

	m_ivtsport = -1;		//vtsport
	memset(m_sdvrIP,0,20);
	memset(m_sdvruser,0,20);
	memset(m_sdvrpass,0,20);

	m_idvrport = -1;		//dvrport
	m_idvrrtspport = -1;
	m_idvrtype = -1;		//dvrtype
	m_ilinkmode = -1;	//realplay linkmode
	m_iChannel = -1;		//通道号
	m_nProtocol = -1;

	m_playport = -1;
	m_bPlaying = FALSE;		//预览
	m_bSound = FALSE;		//播放声音
	m_lplayport = -1;

	m_bSelWnd = FALSE;

	m_hRecordEvent = NULL;
	m_hRecordThread = NULL;
	m_svtsOldIP = "";		//vtsip
	m_ivtsOldPort = -1;			//port

	m_sdvrOldIP = "";		//dvrip
	m_iOldChannel = -1;		//通道
	m_iOldPort = -1;			//port
	m_sOldChanName = "";		//通道名称
	m_sChanName = "";

	m_bSetPara = FALSE;			//设置视频参数
	m_ibright = -1;
	m_icontrast = -1;
	m_isaturation= -1;
	m_ihue = -1;
	m_iVolume = -1;
	m_xValue = -1;
	m_yValue = -1;
	m_bRecord = FALSE;

	size = 16;
	font = 12;	//操作控件大小，通道字体大小
	recleft = 0;
	top = 0;	//录像控件位置
	soundleft = 0;	//声音控件位置

	m_nOldWndNum = 0;
	hwnd = NULL;	//播放窗口句柄

	m_bZoomView = FALSE;
	m_width = 0;
	m_height = 0;//像素大小
	m_dragsum = 0;       //放大次数统计

	m_bMouseMove = FALSE;	//鼠标移动
	m_GetPicFlag = FALSE;   //取图像大小标识

	m_nTypeRatio = 0;	// 窗口显示比例

	m_WndColor = RGB(10,10,10);

	m_play_port = -1;
	m_net_port = -1;

	memset(&m_sipd_catalog_item, 0, sizeof(sipd_catalog_item));
}


void CRealplayWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRealplayWnd)
	DDX_Control(pDX, IDC_STATIC_WND, m_black);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRealplayWnd, CDialog)
	//{{AFX_MSG_MAP(CRealplayWnd)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RECORD, OnRecord)
	ON_BN_CLICKED(IDC_EFFECT, OnEffect)
	ON_BN_CLICKED(IDC_CAPTURE, OnCapture)
	ON_BN_CLICKED(IDC_SOUND, OnSound)
	ON_BN_CLICKED(IDC_STOPPLAY, OnStopplay)
	ON_BN_CLICKED(IDC_BRIGHTADD, OnBrightadd)
	ON_BN_CLICKED(IDC_BRIGHTDEC, OnBrightdec)
	ON_BN_CLICKED(IDC_CONTRASTADD, OnContrastadd)
	ON_BN_CLICKED(IDC_CONTRASTDEC, OnContrastdec)
	ON_BN_CLICKED(IDC_SATURATIONADD, OnSaturationadd)
	ON_BN_CLICKED(IDC_SATURATIONDEC, OnSaturationdec)
	ON_BN_CLICKED(IDC_HUEADD, OnHueadd)
	ON_BN_CLICKED(IDC_HUEDEC, OnHuedec)
	ON_BN_CLICKED(IDC_VOLUMEADD, OnVolumeadd)
	ON_BN_CLICKED(IDC_VOLUMEDEC, OnVolumedec)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_FULL_SCREEN, OnFullscreen)
	ON_COMMAND(IDM_REGION_ZOOM, OnRegionZoom)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(VIDEO_REPAINT, OnRepaintWnd)
	ON_COMMAND(ID_SHOW_RATIO_4_3, &CRealplayWnd::OnShowRatio43)
	ON_COMMAND(ID_SHOW_RATIO_16_9, &CRealplayWnd::OnShowRatio169)
	ON_COMMAND(ID_SHOW_RATIO_FULL, &CRealplayWnd::OnShowRatioFull)
	ON_MESSAGE(UPDATE_TOOL_TIP, OnUpDateToolTip)
	ON_WM_MOUSELEAVE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRealplayWnd message handlers

void CRealplayWnd::SetCtrlPos()
{
	CRect rectClient;
	GetClientRect(&rectClient);

	DeferPreviewWindowPos(rectClient);
	if (!m_bSelWnd)
	{
		m_WndRect = rectClient;

		ShowToolBar(m_bSelWnd);

		return;
	}
	
	int t = rectClient.top;
	int l = rectClient.right-size;
	int r = rectClient.right;
	int top = t;

	GetDlgItem(IDC_STOPPLAY)->MoveWindow(l,t,size,size,TRUE);

	l=rectClient.right-2*(size+3);
	r=rectClient.right-1*(size+3);
	GetDlgItem(IDC_SOUND)->MoveWindow(l,t,size,size,TRUE);
	soundleft = l;

	l=rectClient.right-3*(size+3);
	r=rectClient.right-2*(size+3);
	GetDlgItem(IDC_EFFECT)->MoveWindow(l,t,size,size,TRUE);

	l=rectClient.right-4*(size+3);
	r=rectClient.right-3*(size+3);
	GetDlgItem(IDC_CAPTURE)->MoveWindow(l,t,size,size,TRUE);

	l=rectClient.right-5*(size+3);
	r=rectClient.right-4*(size+3);
	GetDlgItem(IDC_RECORD)->MoveWindow(l,t,size,size,TRUE);
	recleft=l;

	l=rectClient.right-9*(size+3);
	r=rectClient.right-4*(size+3);

	GetDlgItem(IDC_TOOLTIP)->MoveWindow(l,t,5*size,size,TRUE);
	l=rectClient.left;
	r=rectClient.right-9*(size+3);
	GetDlgItem(IDC_STATIC_NAME)->MoveWindow(l,t,r-l,size,TRUE);

	if (m_bSetPara)
	{
		t=rectClient.top+size;
		l=rectClient.right-size;
		r=rectClient.right;
		GetDlgItem(IDC_VOLUMEDEC)->MoveWindow(l,t,size,size,TRUE);
		
		l=rectClient.right-2*size;
		r=rectClient.right-size;
		GetDlgItem(IDC_VOLUMEADD)->MoveWindow(l,t,size,size,TRUE);

		l=rectClient.right-3*size;
		r=rectClient.right-2*size;
		GetDlgItem(IDC_HUEDEC)->MoveWindow(l,t,size,size,TRUE);
		
		l=rectClient.right-4*size;
		r=rectClient.right-3*size;
		GetDlgItem(IDC_HUEADD)->MoveWindow(l,t,size,size,TRUE);
		
		l=rectClient.right-5*size;
		r=rectClient.right-4*size;
		GetDlgItem(IDC_SATURATIONDEC)->MoveWindow(l,t,size,size,TRUE);
		
		l=rectClient.right-6*size;
		r=rectClient.right-5*size;
		GetDlgItem(IDC_SATURATIONADD)->MoveWindow(l,t,size,size,TRUE);		
		
		l=rectClient.right-7*size;
		r=rectClient.right-6*size;
		GetDlgItem(IDC_CONTRASTDEC)->MoveWindow(l,t,size,size,TRUE);
	
		l=rectClient.right-8*size;
		r=rectClient.right-7*size;
		GetDlgItem(IDC_CONTRASTADD)->MoveWindow(l,t,size,size,TRUE);
		
		l=rectClient.right-9*size;
		r=rectClient.right-8*size;
		GetDlgItem(IDC_BRIGHTDEC)->MoveWindow(l,t,size,size,TRUE);
		
		l=rectClient.right-10*size;
		r=rectClient.right-9*size;
		GetDlgItem(IDC_BRIGHTADD)->MoveWindow(l,t,size,size,TRUE);		
	}	

	ShowToolBar(m_bSelWnd);
}

void  CRealplayWnd::DeferPreviewWindowPos(CRect rectClient)
{
	if (!m_bSelWnd)
	{
		m_WndRect = rectClient;	
	}
	else
	{
		int b = rectClient.top+size;
		if(m_bSetPara)
		{
			b = rectClient.top+2*size;
		}

		m_WndRect.SetRect(rectClient.left,b,rectClient.right,rectClient.bottom);
	}

	CRect rect = m_WndRect;	
	//	计算画面显示窗口的尺寸
	SetWndRect(rect);
	m_black.MoveWindow(rect.left+2, rect.top+2, rect.Width()-4, rect.Height()-4,TRUE);
}

BOOL CRealplayWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_bSetPara = false;
	m_bPlaying = false;
	m_bSound = false;
	m_bRecord = false;
	m_iPlayhandle = -1;
	m_lUserId =-1;
	m_bSelWnd = FALSE;

	m_bZoomView=FALSE;
	m_GetPicFlag=FALSE;  //默认为不取图像大小
	m_dragsum=0;
	m_width=0;
	m_height=0;
	memset(&g_rcLast,0x00,sizeof(g_rcLast));

	m_sChanName = _T("");
	m_sOldChanName = _T("");
	m_svtsOldIP="";
	m_sdvrOldIP="";
	m_iOldChannel=-1;

	m_PopMenu.LoadMenu(IDR_MENU_FULL);

	m_nTypeRatio = 0;

	m_Static_Name.SubclassWindow(GetDlgItem(IDC_STATIC_NAME)->m_hWnd);
	m_Static_Name.SetTextColor(RGB(218,205,26));
	m_Static_Name.SetFontName("宋体");
	m_Static_Name.SetFontSize(12);
	m_Static_Name.SetFontBold(1);
	m_Static_Name.SetWindowText(m_sChanName);
	m_Static_Name.SetBkColor(m_WndColor);//RGB(184,190,199));

// 	m_tipName.SubclassWindow(GetDlgItem(IDC_STATIC_NAME)->m_hWnd);
	hRecIcon = AfxGetApp()->LoadIcon(IDI_ICON_RECORD); 
	hStopRecIcon = AfxGetApp()->LoadIcon(IDI_ICON_STOPRECORD); 
	hSoundIcon = AfxGetApp()->LoadIcon(IDI_ICON_SOUND);
	hStopSoundIcon = AfxGetApp()->LoadIcon(IDI_ICON_STOPSOUND);

	m_Static_Name.SetBkColor(m_WndColor);
	m_Static_Name.SetFontSize(font);

	m_TipInfo.SubclassWindow(GetDlgItem(IDC_TOOLTIP)->m_hWnd);
	m_TipInfo.SetTextColor(RGB(218,205,26));
	m_TipInfo.SetFontName("宋体");
	m_TipInfo.SetFontSize(9);
	m_TipInfo.SetFontBold(1);
	m_TipInfo.SetWindowText("");
	m_TipInfo.SetBkColor(m_WndColor);
	m_TipInfo.SetFontSize(font);

/************************************************************************/
/* new                                                                  */
/************************************************************************/
// 	hVedioEffIcon =  AfxGetApp()->LoadIcon(IDI_ICON_PARA);
// 	hCaptureIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CAPTURE);
// 	hStopPlayIcon =  AfxGetApp()->LoadIcon(IDI_ICON_STOPPLAY);
// 
// 	hCurRecIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_RECORD);
// 	hCurStopRecIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_STOPRECORD);
// 	hCurCaptureIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_CAPTURE);
// 	hCurVedioIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_PARA);
// 	hCurSoundIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_SOUND);
// 	hCurStopSoundIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_STOPSOUND);
// 
// 	hCurStopPlayIcon =  AfxGetApp()->LoadIcon(IDI_ICON_CUR_STOPPLAY);

	m_tipRecord.SubclassWindow(GetDlgItem(IDC_RECORD)->m_hWnd);
	m_tipRecord.SetToolTipText("开始录像");
 	
	m_tipCap.SubclassWindow(GetDlgItem(IDC_CAPTURE)->m_hWnd);
 	m_tipCap.SetToolTipText("抓图");

	m_tipVideo.SubclassWindow(GetDlgItem(IDC_EFFECT)->m_hWnd);
	m_tipVideo.SetToolTipText("视频参数");
 
	m_tipSound.SubclassWindow(GetDlgItem(IDC_SOUND)->m_hWnd);
 	m_tipSound.SetToolTipText("打开声音");
 
	m_tipStop.SubclassWindow(GetDlgItem(IDC_STOPPLAY)->m_hWnd);
 	m_tipStop.SetToolTipText("停止预览");

	EnableParaCtrl(m_bSetPara);

	m_bTip = FALSE;

	ShowToolBar(m_bSelWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CRealplayWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


void CRealplayWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
	if(pContainer != NULL)
	{
		BOOL bMulti = pContainer->GetMultiScreen();
		pContainer->SetMultiScreen(!bMulti);
	
		EnableParaCtrl(m_bSetPara);
	//	pContainer->SetActiveWnd(this);

		if (bMulti)
		{
			m_nOldWndNum = pContainer->m_nWindowNum;
		//	m_nWindowNum = 1;
		}
		else
		{
			pContainer->m_nWindowNum = m_nOldWndNum;
		}
		pContainer->SetWndPos(pContainer->m_nCurWnd);	

		//	发送“进入单屏幕”和 “退出单屏幕”
		/*if(!pContainer->GetMultiScreen())
		{
			::PostMessage(((CClientCtrl *)pContainer->GetParent())->m_hWnd,WND_EVENT_INFO,(WPARAM)this,EVT_ENTER_SINGLESCREEN);
		}
		else
		{
			::PostMessage(((CClientCtrl *)pContainer->GetParent())->m_hWnd,WND_EVENT_INFO,(WPARAM)this,EVT_EXIT_SINGLESCREEN);
		}*/
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CRealplayWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
	if(pContainer != NULL)
	{
		EnableParaCtrl(m_bSetPara);
		pContainer->SetActiveWnd(this,TRUE);		

		if(m_bZoomView)
		{
			m_bMouseMove=TRUE;
			ChangeRect.top = point.y;
			ChangeRect.left = point.x;
			ChangeRect.right = ChangeRect.left+1;
			ChangeRect.bottom = ChangeRect.top+1;
			m_ptLBDown = point;
		}
	}	

	CDialog::OnLButtonDown(nFlags, point);
}

// 画边框.
void CRealplayWnd::DrawWndBorder(CDC *pDC)
{
	CPen penWhite;
	//在窗口四周画一圈 绿线
	if(m_nWndNo == GetCurWnd())
		penWhite.CreatePen(PS_SOLID, 2, RGB(54,0xa8,0xfe));  //当前窗口	
	else
		penWhite.CreatePen(PS_SOLID, 1, RGB(0,0,0));  //其他窗口	

	CPen* pOldPen = pDC->SelectObject(&penWhite);
	pDC->MoveTo(m_WndRect.left, m_WndRect.top);
	pDC->LineTo(m_WndRect.right-1, m_WndRect.top+1);
	pDC->LineTo(m_WndRect.right-1, m_WndRect.bottom-1);
	pDC->LineTo(m_WndRect.left+1, m_WndRect.bottom-1);
	pDC->LineTo(m_WndRect.left+1, m_WndRect.top+1);
	pDC->SelectObject(pOldPen);
	penWhite.DeleteObject();
}

void CRealplayWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawWndBorder(&dc);
	
	if (!m_bSelWnd)
	{
		return;
	}
	
	int x = recleft;
	int y = top;
	if(m_nWndNo == GetCurWnd())
	{
		if (m_bRecord)
		{
			DrawIconEx(dc.m_hDC,x, y, hStopRecIcon, size, size, 0, NULL,DI_IMAGE  );
		}
		else
		{
 			DrawIconEx(dc.m_hDC,x, y, hRecIcon, size, size, 0, NULL, DI_IMAGE);
		}

		if (m_bSound)
		{
 			DrawIconEx(dc.m_hDC,soundleft, y, hStopSoundIcon, size, size, 0, NULL, DI_IMAGE);
		}
		else
		{
			DrawIconEx(dc.m_hDC,soundleft, y, hSoundIcon, size, size, 0, NULL, DI_IMAGE);
		}
	}
}



BOOL CRealplayWnd::StartPlay(sipd_catalog_item *  pItem)
{
	sipd_catalog_item item;
	memcpy(&item, pItem, sizeof(sipd_catalog_item));

	stringstream ss;
	ss << "RealplayWnd[" << m_nWndNo << "], StartPlay device_id = " << string(item.device_id);
	ss << ", HWND = " << m_black.GetSafeHwnd();
	LOG4CPLUS_DEBUG(g_Logger, ss.str());

	ss.str("");
	ss << "device_id = " << string(item.device_id);
	m_black.SetWindowText(ss.str().c_str());
	
	if(m_play_port< 0)
	{
		stream_media_format media_format;
		media_format.media_type = /*SIPD_MEDIA_TYPE_VIDEO*/ PLAYER_MEDIA_TYPE_MIX;
		media_format.video_type = PLAYER_MEDIA_F_VIDEO_TYPE_H264 /*PLAYER_MEDIA_F_VIDEO_TYPE_MPEG2*/;
		media_format.audio_type = PLAYER_MEDIA_F_AUDIO_TYPE_G711;

		m_play_port = player_stream_open(&media_format, m_black.GetSafeHwnd());
	}

	//	
	if(m_play_port>=0 && m_net_port<0)
	{
		sipd_media_session catalog;
		memset(&catalog, 0, sizeof(sipd_media_session));

		stringstream ss;
		ss << "sip:" << LOCAL_DOMAIN_ID << "@" << LOCAL_IP << ":" << LOCAL_PORT; 
		strncpy(catalog.from_uri, ss.str().c_str(), MAX_SIPD_URI);

		ss.str("");
		ss << "sip:" << string(item.device_id) << "@" << REMOTE_IP << ":" << REMOTE_PORT;

		strncpy(catalog.to_uri, ss.str().c_str(), MAX_SIPD_URI);
		strncpy(catalog.channel_id, item.device_id, MAX_SIPD_DEVICE_ID_LEN);    // 35000100491327000249:3
		strncpy(catalog.o_id, item.device_id, MAX_SIPD_DEVICE_ID_LEN);  

		catalog.stream_type = SIPD_STREAM_TYPE_PLAY /* SIPD_STREAM_TYPE_PLAY    SIPD_STREAM_TYPE_PLAYBACK */;

		//	对应点播 ，以下要填 0 
		strncpy(catalog.t_start_time, "0", MAX_SIPD_NORMAL_LEN);		// 1492358400
		strncpy(catalog.t_end_time,   "0", MAX_SIPD_NORMAL_LEN);			// 1492430400   1492358415

		m_net_port = Dll_Call_play_playback_download(&catalog);
	}

	if(m_net_port>=0 && m_play_port >=0)
	{
		JMutexAutoLock lock(g_mtxNet);
		g_mapNet2Player[m_net_port] = m_play_port;
		g_mapNet2Hwnd[m_net_port] = GetSafeHwnd();

		ss.str("");
		ss << "RealplayWnd[" << m_nWndNo << "], StartPlay m_net_port = " << m_net_port;
		ss << ", m_play_port = " << m_play_port;
		LOG4CPLUS_DEBUG(g_Logger, ss.str());

		return TRUE;
	}
	
	return FALSE;
}

void CRealplayWnd::StopPlay()
{

	if(m_net_port>=0)
	{
		Dll_Call_Bye(m_net_port);
	}

	if(m_play_port>=0)
	{
		player_stream_close(m_play_port);
	}

	{
		JMutexAutoLock lock(g_mtxNet);
		if(g_mapNet2Player.count(m_net_port)>0)
		{
			g_mapNet2Player.erase(m_net_port);
		}

		if(g_mapNet2Hwnd.count(m_net_port)>0)
		{
			g_mapNet2Hwnd.erase(m_net_port);
		}
	}

	m_play_port = -1;
	m_net_port = -1;

	m_black.SetWindowText("The End");

}

void CRealplayWnd::EnableParaCtrl(BOOL bEnable)
{
	GetDlgItem(IDC_BRIGHTADD)->ShowWindow(bEnable);
	GetDlgItem(IDC_BRIGHTDEC)->ShowWindow(bEnable);
	GetDlgItem(IDC_CONTRASTADD)->ShowWindow(bEnable);
	GetDlgItem(IDC_CONTRASTDEC)->ShowWindow(bEnable);
	GetDlgItem(IDC_SATURATIONADD)->ShowWindow(bEnable);
	GetDlgItem(IDC_SATURATIONDEC)->ShowWindow(bEnable);
	GetDlgItem(IDC_HUEADD)->ShowWindow(bEnable);
	GetDlgItem(IDC_HUEDEC)->ShowWindow(bEnable);
	GetDlgItem(IDC_VOLUMEADD)->ShowWindow(bEnable);
	GetDlgItem(IDC_VOLUMEDEC)->ShowWindow(bEnable);
	
	if(bEnable)
	{
		CreateVTip();
	}
}

void CRealplayWnd::EndRecord()
{

	if (m_iPlayhandle<0)
		return;
	
		
}


void CRealplayWnd::OnRecord() 
{
	// TODO: Add your control notification handler code here

	
// 	return;
	if (m_iPlayhandle<0)
		return;
	//char dir[256];
	char filename[256];

	m_CurRecordtime = CTime::GetCurrentTime();
	
	/*if (!m_bRecord)
	{
		sprintf(m_dir,"%s:\\mpeg4record\\%s\\%s_%02d_%05d",g_szRecPath,m_CurRecordtime.Format("%Y-%m-%d"),m_sdvrIP,m_iChannel,m_idvrport);
	}*/

	

	m_bRecord =!m_bRecord;	

	if (m_bRecord)
	{
		m_tipRecord.SetToolTipText("停止录像");
	}
	else
	{
		m_tipRecord.SetToolTipText("开始录像");
 	}

	Invalidate(true);
	if (m_bRecord)
	{
		return;
	}
	CString msg;
	msg.Format("录像存放路径 %s",m_dir);
	
	/*HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
	if(hUser32)
	{
		MessageBoxTimeout(NULL,msg,"录像路径",MB_OK, 0, 1*2000);
	}
	FreeLibrary(hUser32);*/
}

void CRealplayWnd::OnEffect() 
{
	try
	{
		m_bSetPara = !m_bSetPara;
		EnableParaCtrl(m_bSetPara);
		SetCtrlPos();
		Invalidate();		
	}
	catch (...)
	{
		DebugTrace("OnEffect  异常");
	}
}

void CRealplayWnd::OnCapture() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	char path[MAX_PATH];
	//sprintf(path,"%s:\\Picture",g_szPicPath);
	BOOL bGet=FALSE;

	
	CString msg;
	if (bGet)
	{
		msg.Format("抓图存放路径 %s",path);
	}
	else
	{
		msg.Format("抓图失败 路径=%s",path);
	}

	/*HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
	if(hUser32)
	{
		MessageBoxTimeout(NULL,msg,"抓图路径",MB_OK, 0, 1*1500);
	}
	FreeLibrary(hUser32);*/
}

void CRealplayWnd::OnSound() 
{

	if(m_net_port< 0 || m_play_port <0)
	{
		sipd_catalog_item item;
		memset(&item.device_id, 0, MAX_SIPD_DEVICE_ID_LEN + 1);
		if(m_nWndNo==0)
			memcpy(&item.device_id, "35000100491187000191", MAX_SIPD_DEVICE_ID_LEN);

		else if(m_nWndNo==1)
			memcpy(&item.device_id, "35000100491187000195", MAX_SIPD_DEVICE_ID_LEN);

		else if(m_nWndNo==2)
			memcpy(&item.device_id, "35000100491187000196", MAX_SIPD_DEVICE_ID_LEN);

		else
			memcpy(&item.device_id, "35000100491187000197", MAX_SIPD_DEVICE_ID_LEN);

		StartPlay(&item);

		return;
	}

	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;

	if (m_bSound)//需要改变状态
	{
		
		m_tipSound.SetToolTipText("打开声音");
	}
	else
	{
		
 		m_tipSound.SetToolTipText("关闭声音");

	}	
	m_bSound = !m_bSound;
	Invalidate(true);
}

void CRealplayWnd::OnStopplay() 
{
	// TODO: Add your control notification handler code here
//	DebugTrace("OnStopplay");
	StopPlay();
	//sprintf(this->m_sTreeNodeId,"%s" ,""); 
	CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
	/*if(pContainer != NULL)
	{
		::PostMessage(((CClientCtrl *)pContainer->GetParent())->m_hWnd,WND_EVENT_INFO,(WPARAM)this,EVT_REALPLAY_CLOSE);
	}*/
	
}

void CRealplayWnd::OnBrightadd() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	
	
}

void CRealplayWnd::OnBrightdec() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	

	if (m_ibright<=0)
	{
		m_ibright = 0;
	}
	
}

void CRealplayWnd::OnContrastadd() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	

	m_icontrast+=PARA_STEP;
	
}

void CRealplayWnd::OnContrastdec() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	
	m_icontrast-=PARA_STEP;

	if (m_icontrast<=0)
	{
		m_icontrast = 0;
	}
	try
	{
		
	}
	catch (...)
	{
		DebugTrace("OnContrastdec  异常");
	}
	
}

void CRealplayWnd::OnSaturationadd() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	
	m_isaturation+=PARA_STEP;

	try
	{
		
	}
	catch (...)
	{
		DebugTrace("OnSaturationadd  异常");
	}
	
}

void CRealplayWnd::OnSaturationdec() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	
	m_isaturation-=PARA_STEP;

	if (m_isaturation<=0)
	{
		m_isaturation = 0;
	}
	try
	{
		
	}
	catch (...)
	{
		DebugTrace("OnSaturationdec  异常");
	}
	
}

void CRealplayWnd::OnHueadd() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	
	m_ihue+=PARA_STEP;

	try
	{
		
	}
	catch (...)
	{
		DebugTrace("OnHueadd  异常");
	}
	
}

void CRealplayWnd::OnHuedec() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	

	m_ihue-=PARA_STEP;

	if (m_ihue<=0)
	{
		m_ihue = 0;
	}
	try
	{
		
	}
	catch (...)
	{
		DebugTrace("OnHuedec  异常");
	}
	
}

void CRealplayWnd::OnVolumeadd() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	if(!m_bSound)
	{
		OnSound();
	}

	try
	{		
		
	}
	catch (...)
	{
		DebugTrace("OnVolumeadd  异常");
	}
		
}

void CRealplayWnd::OnVolumedec() 
{
	// TODO: Add your control notification handler code here
	if (m_iPlayhandle<0)
		return;
	try
	{
		if (1)
		{
			m_iVolume-=VOLUME_STEP;
			if (m_iVolume<=0)
			{
				m_iVolume = 0;
			}	
			
		}
		else
		{
			

			m_iVolume-=VOLUME_STEP;
			if (m_iVolume<=0)
			{
				m_iVolume = 0;
			}	
			
		}
	}
	catch (...)
	{
		DebugTrace(" OnVolumedec 异常");
	}
	
	if(m_bSound && m_iVolume == 0)
	{
		OnSound();
	}
}

void CRealplayWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
	if(pContainer)
	{
		EnableParaCtrl(m_bSetPara);
		pContainer->SetActiveWnd(this,TRUE);		
	}

	

	ClientToScreen(&point);

	CMenu* pPopupMenu = (CMenu *)m_PopMenu.GetSubMenu(0);

	if (pPopupMenu != NULL)
	{
		if(!pContainer->m_bFullScreen)
		{
			pPopupMenu->ModifyMenu(0, MF_BYPOSITION, IDM_FULL_SCREEN, "全  屏");
		}
		else
		{
			pPopupMenu->ModifyMenu(0, MF_BYPOSITION, IDM_FULL_SCREEN, "退出全屏");
		}
		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CRealplayWnd::OnFullscreen() 
{
	CRealplayPannel * pContainer = (CRealplayPannel *)GetParentScreenPannel();
	//	发送"全屏"事件通知
	if(!pContainer->GetFullScreen())
	{
		//::PostMessage(((CClientCtrl *)pContainer->GetParent())->m_hWnd,WND_EVENT_INFO,(WPARAM)this,EVT_ENTER_FULLSCREEN);
	}

	WndFullScreen();

	//	发送"退出全屏"事件通知
	if(!pContainer->GetFullScreen())
	{
		//::PostMessage(((CClientCtrl *)pContainer->GetParent())->m_hWnd,WND_EVENT_INFO,(WPARAM)this,EVT_EXIT_FULLSCREEN);
	}
}

void CRealplayWnd::OnRegionZoom() 
{
	// TODO: Add your command handler code here
	if ( m_iPlayhandle!=-1)
	{
		CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
		m_bZoomView = !m_bZoomView;
		if(m_bZoomView )
		{
			m_dragsum=0;
			m_GetPicFlag=FALSE;
			memset(&g_rcLast,0x00,sizeof(g_rcLast));

			
		}
	}

}

LRESULT CRealplayWnd::OnRepaintWnd(WPARAM wParam, LPARAM lParam)
{
	Invalidate();
	return 0;
}

LRESULT CRealplayWnd::OnUpDateToolTip(WPARAM wParam, LPARAM lParam)
{
	//
	m_TipInfo.SetWindowText((LPCTSTR)wParam);
	return 0;
}

BOOL CRealplayWnd::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}


BOOL CRealplayWnd::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(rect);
	CBrush brush (m_WndColor);
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->FillRect(rect, CBrush::FromHandle((HBRUSH)GetStockObject(m_WndColor)));
	pDC->SelectObject(pOldBrush);

	return TRUE;
}

int CRealplayWnd::GetCurWnd()
{
	CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
	if(pContainer != NULL)
	{
		return pContainer->m_nCurWnd;
	}

	return 0;
}

void CRealplayWnd::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	StopPlay();
	SetEvent(m_hRecordEvent);

	::DeleteObject(m_hStaticBrush);
	m_hStaticBrush = NULL;

	m_PopMenu.DestroyMenu();

	CDialog::OnClose();
}

void CRealplayWnd::CloseWnd()
{
	OnClose();
}

void CRealplayWnd::WndFullScreen()
{
	CRealplayPannel *pContainer = (CRealplayPannel *)GetParentScreenPannel();
	if(pContainer != NULL)
	{
		pContainer->SetFullScreen(!pContainer->GetFullScreen());
		pContainer->SetWndPos(m_nWndNo);
	}
}

void CRealplayWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseMove && m_bZoomView)
	{
		if (point.x-m_ptLBDown.x<=10 && point.y-m_ptLBDown.y<=10)
		{
			m_bMouseMove=FALSE;
			return;
		}	

		CRect rect;
		CRect rc;
		long wid,hei;
		
		m_bMouseMove = FALSE;
		
		GetClientRect(&rc);
		if(!m_GetPicFlag)
		{
			
			m_GetPicFlag=true;
			wid=m_width;
			hei=m_height;
		}
		else 
		{
			wid = g_rcLast.Width();
			hei = g_rcLast.Height();
		}
		
		float x =((float)wid)/((float)rc.Width());
		float y =((float)hei)/((float)rc.Height());
		
		if (m_ptLBDown.x>point.x)
		{
			rect.left = point.x;
			rect.right = m_ptLBDown.x;
		}
		else
		{
			rect.left = m_ptLBDown.x;
			rect.right = point.x;
		}
		if (m_ptLBDown.y>point.y)
		{
			rect.top = point.y;
			rect.bottom = m_ptLBDown.y;
		}
		else
		{
			rect.top = m_ptLBDown.y;
			rect.bottom = point.y;
		}
		
		g_rcLast.right = g_rcLast.left+(int)(rect.right*x);
		g_rcLast.left = g_rcLast.left+(int)(rect.left*x);
		
		g_rcLast.bottom = g_rcLast.top+(int)(rect.bottom*y);
		g_rcLast.top = g_rcLast.top+(int)(rect.top*y);

       
		m_bMouseMove=FALSE;
	}		
	CDialog::OnLButtonUp(nFlags, point);
}

void CRealplayWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bMouseMove && m_bZoomView)
	{
		CClientDC dc(this); 
		RECT rect;
		rect.left=ChangeRect.left;
		rect.top=ChangeRect.top;
		rect.right=point.x;
		rect.bottom=point.y;

		CPen   pen(PS_SOLID |PS_GEOMETRIC|PS_ENDCAP_ROUND, 2, RGB(0,0,255)); 
		dc.SelectObject(&pen); 
		LOGBRUSH   lb; 
		lb.lbStyle   =   BS_HOLLOW; 
		CBrush   brush; 
		brush.CreateBrushIndirect(&lb); 
		dc.SelectObject(&brush); 
		dc.Rectangle(&rect);
		ChangeRect.right=point.x;
		ChangeRect.bottom=point.y;
	}

	CString strTemp; 
	m_TipInfo.GetWindowText(strTemp);
	if (!strTemp.IsEmpty())
	{
		m_TipInfo.SetWindowText("");
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CRealplayWnd::CreateVTip()
{
	if (!m_bTip)
	{	
		m_tipAddb.SubclassWindow(GetDlgItem(IDC_BRIGHTADD)->m_hWnd);
 		m_tipAddb.SetToolTipText("增加亮度");
 		m_tipDecb.SubclassWindow(GetDlgItem(IDC_BRIGHTDEC)->m_hWnd);
 		m_tipDecb.SetToolTipText("减少亮度");
 		
 		m_tipAddc.SubclassWindow(GetDlgItem(IDC_CONTRASTADD)->m_hWnd);
 		m_tipAddc.SetToolTipText("增加对比度");
 		m_tipDecc.SubclassWindow(GetDlgItem(IDC_CONTRASTDEC)->m_hWnd);
 		m_tipDecc.SetToolTipText("减少对比度");
 		
 		m_tipAdds.SubclassWindow(GetDlgItem(IDC_SATURATIONADD)->m_hWnd);
 		m_tipAdds.SetToolTipText("增加饱和度");
 		m_tipDecs.SubclassWindow(GetDlgItem(IDC_SATURATIONDEC)->m_hWnd);
 		m_tipDecs.SetToolTipText("减少饱和度");
 		m_tipAddh.SubclassWindow(GetDlgItem(IDC_HUEADD)->m_hWnd);
 		m_tipAddh.SetToolTipText("增加色度");
 		m_tipDech.SubclassWindow(GetDlgItem(IDC_HUEDEC)->m_hWnd);
 		m_tipDech.SetToolTipText("减少色度");
 		
 		m_tipAddv.SubclassWindow(GetDlgItem(IDC_VOLUMEADD)->m_hWnd);
 		m_tipAddv.SetToolTipText("增加音量");
 		m_tipDecv.SubclassWindow(GetDlgItem(IDC_VOLUMEDEC)->m_hWnd);
 		m_tipDecv.SetToolTipText("减小音量");

 		m_bTip = TRUE;
	}
}


void CRealplayWnd::ShowToolBar(BOOL bShow)
{
	GetDlgItem(IDC_STATIC_NAME)->ShowWindow(bShow);
	GetDlgItem(IDC_RECORD)->ShowWindow(bShow);
	GetDlgItem(IDC_CAPTURE)->ShowWindow(bShow);
	GetDlgItem(IDC_EFFECT)->ShowWindow(bShow);
	GetDlgItem(IDC_SOUND)->ShowWindow(bShow);
	GetDlgItem(IDC_STOPPLAY)->ShowWindow(bShow);

	GetDlgItem(IDC_BRIGHTADD)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_BRIGHTDEC)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_CONTRASTADD)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_CONTRASTDEC)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_SATURATIONADD)->ShowWindow(bShow&&m_bSetPara);

	GetDlgItem(IDC_SATURATIONDEC)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_HUEADD)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_HUEDEC)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_VOLUMEADD)->ShowWindow(bShow&&m_bSetPara);
	GetDlgItem(IDC_VOLUMEDEC)->ShowWindow(bShow&&m_bSetPara);
}

void CRealplayWnd::HidePreWnd(int nWndNo)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	m_black.MoveWindow(rectClient.left+1,rectClient.top+1,rectClient.right-1,rectClient.bottom-1,true);//
	m_WndRect.SetRect(rectClient.left,rectClient.top,rectClient.right-1,rectClient.bottom);
	Invalidate();
}

// 按4：3比例修改窗口
void CRealplayWnd::SetWndRect(CRect &rect)
{
	int nWidth = 0;
	int nHeight = 0;
	int nDis= 0;
	int nWidthOld = rect.Width();
	int nHeightOld = rect.Height();
	double dbRatio = nWidthOld/(double)nHeightOld;

	if (dbRatio - 1.77 > EPSINON)
	{
		nWidth = nHeightOld * 1.77;
		nDis = (nWidthOld - nWidth)/2;

		rect.left = rect.left + nDis;
		rect.right = rect.right - nDis;		
	}
	else if (dbRatio - 1.33 > EPSINON)
	{
		nWidth = nHeightOld * 1.33;
		nDis = (nWidthOld - nWidth)/2;

		rect.left = rect.left + nDis;
		rect.right = rect.right - nDis;		
	}
	else
	{
		nHeight = nWidthOld * 0.75;
		nDis = (nHeightOld - nHeight)/2;

		rect.top = rect.top + nDis;
		rect.bottom = rect.bottom - nDis;
	}
}

HBRUSH CRealplayWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(pWnd->GetDlgCtrlID() == IDC_STATIC_WND)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(m_WndColor);
		pDC->SetTextColor(RGB(255,255,255));
	
		CFont font;
		VERIFY(font.CreateFont(  
			14,                        // nHeight   
			0,                         // nWidth   
			0,                         // nEscapement   
			0,                         // nOrientation   
			FW_NORMAL,                 // nWeight   
			FALSE,                     // bItalic   
			FALSE,                     // bUnderline   
			0,                         // cStrikeOut   
			ANSI_CHARSET,              // nCharSet   
			OUT_DEFAULT_PRECIS,        // nOutPrecision   
			CLIP_DEFAULT_PRECIS,       // nClipPrecision   
			DEFAULT_QUALITY,           // nQuality   
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily   
			"宋体"));                 // lpszFacename   
		pDC->SelectObject(&font);

		/*pStatic->setfSetFontName("宋体");
		pStatic->SetFontSize(12);
		pStatic->SetFontBold(1);*/
		
		return m_hStaticBrush;
	}
	else if(nCtlColor == CTLCOLOR_DLG)
	{
		pDC->SetBkColor(m_WndColor);
		return m_hStaticBrush;
	}
	
	return CWnd::OnCtlColor(pDC,pWnd,nCtlColor);
}

void CRealplayWnd::OnShowRatio43()
{
	
	m_nTypeRatio = RATIO_TYPE_4_3;
	CheckMenu();
}

void CRealplayWnd::OnShowRatio169()
{
	m_nTypeRatio = RATIO_TYPE_16_9;
	CheckMenu();
}

void CRealplayWnd::OnShowRatioFull()
{
	m_nTypeRatio = RATIO_TYPE_FULL;
	CheckMenu();
}

void CRealplayWnd::CheckMenu()
{
	CMenu* pPopupMenu = (CMenu *)m_PopMenu.GetSubMenu(0);

	pPopupMenu->CheckMenuItem(ID_SHOW_RATIO_4_3,MF_UNCHECKED);
	pPopupMenu->CheckMenuItem(ID_SHOW_RATIO_16_9,MF_UNCHECKED);
	pPopupMenu->CheckMenuItem(ID_SHOW_RATIO_FULL,MF_UNCHECKED);

	if (m_nTypeRatio == RATIO_TYPE_4_3)
	{
		pPopupMenu->CheckMenuItem(ID_SHOW_RATIO_4_3,MF_CHECKED);
	}
	else if (m_nTypeRatio == RATIO_TYPE_16_9)
	{
		pPopupMenu->CheckMenuItem(ID_SHOW_RATIO_16_9,MF_CHECKED);
	}
	else if (m_nTypeRatio == RATIO_TYPE_FULL)
	{
		pPopupMenu->CheckMenuItem(ID_SHOW_RATIO_FULL,MF_CHECKED);
	}
}
void CRealplayWnd::OnMouseLeave()
{
	
	m_TipInfo.SetWindowText("");
	CDialog::OnMouseLeave();
}



void CRealplayWnd::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	StopPlay();
}
