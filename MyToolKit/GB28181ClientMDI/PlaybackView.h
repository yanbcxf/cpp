#pragma once

#include "PlayTimeCtrl.h"
#include "SkinWnd.h"
#include "PlaybackPannel.h"

#define IDC_SCREENPANNEL	    1015	// 播放窗口.
#define IDC_CTRLWNDTIME         1016	// 时间轴控件ID.
#define IDC_CTRLBTNS			1017	// 按钮控制组ID.
#define IDC_PLAY_VOICE          1021	// 打开声音.
#define IDC_PLAY_VOICE_CLOSE    1022	// 关闭声音.
#define IDC_PLAY_PLAY           1023	// 播放.
#define IDC_PLAY_PAUSE          1024	// 暂停.
#define IDC_PLAY_STOP           1025	// 停止.
#define IDC_PLAY_FRAME          1026	// 单帧.
#define IDC_PLAY_PIC            1027	// 截图.
#define IDC_PLAY_REC            1028	// 录像.
#define IDC_PLAY_REC_STOP       1029	// 停止录像.
#define IDC_PLAY_DOWN           1030	// 下载.
#define IDC_ONEPAN              1032	// 单画面.
#define IDC_FOURPAN             1033	// 四画面.
#define IDC_FULLPAN             1034	// 全屏.
#define IDC_SLIDER_SPEED        1036	// 速度滑块.
#define IDC_SLIDER_SPEED_TEXT   1037	// 速度文本背景.
#define IDC_HIDE_PLAYCTRL       1038	// 隐藏时间条.
#define IDC_SHOW_PLAYCTRL       1039	// 显示时间条.
#define IDC_FIND				1040	// 查找.
#define IDC_DEVICE_TYPE			1041	// 设备类型.
#define IDC_DATATIME			1042	// 时间.

#define IDC_PLAY_SEPBMP1        1050	// 分隔图片.
#define IDC_PLAY_SEPBMP2        1051	// 分隔图片.
#define IDC_PLAY_SEPBMP3        1052	// 分隔图片.

#define BUFFER_SIZE  1024*1024
#define FILESTREAM   1



// CPlaybackView 窗体视图

class CPlaybackView : public CFormView
{
	DECLARE_DYNCREATE(CPlaybackView)

	friend class CPlaybackWnd;
protected:
public:
	CPlaybackView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPlaybackView();

public:
	enum { IDD = IDD_PLAYBACKVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	int		m_nCurWndIndex;		// 当前窗口号
	int		m_nSearchTimeMod;

protected:
	void CreateSkinBtns();		// 创建按钮组.

	void AutoRecalcLayout();	// 自动调整按钮组大小.	


	BOOL ShowPlayCtrlTime(BOOL bShow);// 显示/隐藏时间条.

	// 设置无操作按钮状态.
	void SetNormBtns();
	// 设置当前窗口的按钮状态.
	void SetCurWndBtns(int nWndIndex);	

	// 设置界面按钮状态.
	// TRUE 播放 / FALSE 暂停.
	void SetBtnsPlay(BOOL bPlay);
	// 设置界面按钮状态.
	// FALSE 录像 / TRUE 停止录像.
	void SetBtnsRecord(BOOL bRecord,BOOL bEnable = TRUE);
	// 设置界面按钮状态.
	// TRUE 关闭声音 / FALSE 打开声音.
	void SetBtnsSound(BOOL bSound);
	// 设置界面滑块位置.
	void SetBtnSpeed(int nPos);

	void OnOnePannel();
	void OnFourPannel();
	void OnFullScreenPannel();

	void SetCurrentWnd(int nWndIndex);	// 设置当前窗口号.
	int  GetCurrentWnd();				// 获取当前窗口号.

	BOOL RecalcDlg();			// 重新布局当前对话框.


private:
	CPlaybackPannel	m_screenPannel;		//播放屏幕底板－子窗口

	CPlayTimeCtrl	m_CtrlPlayTime;		// 时间轴.
	CSkinWnd		m_SkinBtns;			// 按钮组.

	int				m_nSkinBtnsH;		// 按钮组高度.
	int				m_nCtrlPalyTimeH;	// 时间轴高度.

	BOOL			m_bShowCtrlPlay;	// 是否显示时间轴.

	COleDropTarget	m_CTreeDropTarget;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEventSelWnd(WPARAM wParam,LPARAM lParam);//修改下载用参数	
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
};


