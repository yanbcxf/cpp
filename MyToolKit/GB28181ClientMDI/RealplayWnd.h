#if !defined(AFX_CRealplayWnd_H__F1A1A4D4_F68C_42B5_8CFE_ED2BBA4331F3__INCLUDED_)
#define AFX_CRealplayWnd_H__F1A1A4D4_F68C_42B5_8CFE_ED2BBA4331F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CRealplayWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRealplayWnd dialog
#include "resource.h"       // main symbols
class CRealplayWnd : public CDialog
{
public:
	CRealplayWnd(CWnd* pParent = NULL);   // standard constructor
	// Dialog Data
	//{{AFX_DATA(CRealplayWnd)
	enum { IDD = IDD_DIALOG_OUTPUT };
	//}}AFX_DATA

// Construction
public:
	void SetParentScreenPannel(CWnd *pWnd)	{ m_pParentWnd = pWnd; }
	CWnd *GetParentScreenPannel()			{ return m_pParentWnd; }
public:
	void HidePreWnd(int nWndNo);
	void ShowToolBar(BOOL bShow);
	void SwitchVolume(int dvrtype,int &nVolume);
	void CreateVTip();
	void WndFullScreen();
	void CloseWnd();
	int GetCurWnd();
	void EnableParaCtrl(BOOL bEnable);
	void StopPlay();
	BOOL StartPlay(sipd_catalog_item * pItem);

	void SetCtrlPos();

	// 画边框.
	void DrawWndBorder(CDC *pDC);

	void DeferPreviewWindowPos(CRect rectClient);

	void EndRecord();
	

public:
	CStatic	m_black;
	CToolPic    m_tipRecord;
	CToolPic    m_tipCap;
	CToolPic    m_tipVideo;
	CToolPic    m_tipSound;
	CToolPic    m_tipStop;

	CToolPic	m_tipDecv;
	CToolPic	m_tipAddv;
	CToolPic	m_tipDecs;
	CToolPic	m_tipAdds;
	CToolPic	m_tipAddh;
	CToolPic	m_tipAddc;
	CToolPic	m_tipAddb;
	CToolPic	m_tipDecc;
	CToolPic	m_tipDecb;
	CToolPic	m_tipDech;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRealplayWnd)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRealplayWnd)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRecord();
	afx_msg void OnEffect();
	afx_msg void OnCapture();
	afx_msg void OnSound();
	afx_msg void OnStopplay();
	afx_msg void OnBrightadd();
	afx_msg void OnBrightdec();
	afx_msg void OnContrastadd();
	afx_msg void OnContrastdec();
	afx_msg void OnSaturationadd();
	afx_msg void OnSaturationdec();
	afx_msg void OnHueadd();
	afx_msg void OnHuedec();
	afx_msg void OnVolumeadd();
	afx_msg void OnVolumedec();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFullscreen();
	afx_msg void OnRegionZoom();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnUpDateToolTip(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowRatio43();
	afx_msg void OnShowRatio169();
	afx_msg void OnShowRatioFull();
	void CheckMenu();
	//}}AFX_MSG
	afx_msg LRESULT OnRepaintWnd(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void InitToolTip();
	void SetWndRect(CRect &rect);// 按4：3比例修改窗口

public:
	BOOL	m_bTip;
	HICON	hRecIcon;
	HICON	hStopRecIcon;
	HICON	hSoundIcon;
	HICON	hStopSoundIcon;
	HICON	hVedioEffIcon;

	HICON   hCaptureIcon;
	HICON   hStopPlayIcon;

	HICON	hCurRecIcon;
	HICON	hCurStopRecIcon;
	HICON   hCurCaptureIcon;
	HICON   hCurVedioIcon;	//当前窗口
	HICON	hCurSoundIcon;
	HICON	hCurStopSoundIcon;
	HICON   hCurStopPlayIcon;

	int		m_nWndNo;
	int		m_iPlayhandle;
	char	m_sTreeNodeId[100];
	char	m_sOldTreeNodeId[100];
	long    m_lUserId;			//nvr登陆值

	char 	m_svtsIP[20];		//vtsip
	int		m_ivtsport;		//vtsport
	char 	m_sdvrIP[20];		//dvrip
	char 	m_sdvruser[20];		//dvruser
	char 	m_sdvrpass[20];		//dvrpass
	int		m_idvrport;		//dvrport
	int     m_idvrrtspport;
	int		m_idvrtype;		//dvrtype
	int		m_ilinkmode;	//realplay linkmode
	int	 	m_iChannel;		//通道号
	int     m_nProtocol;

	long    m_playport;
	BOOL	m_bPlaying;		//预览
	BOOL	m_bSound;		//播放声音
	long    m_lplayport;

	BOOL    m_bSelWnd;

	HANDLE  m_hRecordEvent;
	HANDLE  m_hRecordThread;
	CTime	m_CurRecordtime;

	CString 	m_svtsOldIP;		//vtsip
	int	 		m_ivtsOldPort;			//port

	CString 	m_sdvrOldIP;		//dvrip
	int	 		m_iOldChannel;		//通道
	int	 		m_iOldPort;			//port
	CString 	m_sOldChanName;		//通道名称
	CString		m_sChanName;

	BOOL	m_bSetPara;			//设置视频参数
	int		m_ibright,m_icontrast,m_isaturation,m_ihue,m_iVolume, m_xValue, m_yValue;
	BOOL	m_bRecord;
	char    m_dir[256];

	int size,font;	//操作控件大小，通道字体大小
	int recleft,top;	//录像控件位置
	int soundleft;	//声音控件位置

private:
	CRect m_WndRect;	//播放的窗口
	CLabel	m_Static_Name;
	CLabel  m_TipInfo;
	int	  m_nOldWndNum;
	HWND hwnd;	//播放窗口句柄
	HDC		m_hdc;

	BOOL	m_bZoomView;
	LONG    m_width,m_height;//像素大小
	BYTE    m_dragsum;       //放大次数统计
	CRect   g_rcLast,ChangeRect;
	BOOL	m_bMouseMove;	//鼠标移动
	BOOL    m_GetPicFlag;   //取图像大小标识

	CPoint   m_ptLBDown;
	CWnd	 *m_pParentWnd;	// 父窗口句柄。
	HBRUSH   m_hStaticBrush;

	CMenu	m_PopMenu;
	CMenu*  m_pPopupMenu;
	int		m_nTypeRatio;	// 窗口显示比例

	COLORREF m_WndColor;

	int		m_play_port;		//	播放器句柄
	int		m_net_port;			//	网络流句柄

	sipd_catalog_item	m_sipd_catalog_item;

public:
	afx_msg void OnMouseLeave();
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRealplayWnd_H__F1A1A4D4_F68C_42B5_8CFE_ED2BBA4331F3__INCLUDED_)
