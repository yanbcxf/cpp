#if !defined(AFX_PLAYBACK_PANNEL_H__F79F0D7D_1068_4E35_8380_22AE3E322707__INCLUDED_)
#define AFX_PLAYBACK_PANNEL_H__F79F0D7D_1068_4E35_8380_22AE3E322707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlaybackWnd.h"

class CPlaybackPannel:public CWnd
{
// Construction
public:
	CPlaybackPannel();

// Implementation
public:
	virtual ~CPlaybackPannel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPlaybackPannel)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// 创建对象(Container)
	BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

public:
	//现在nSub没有用,请填零
	int SetShowPlayWin(int nMain);
	CWnd * GetPage(int nIndex);
    int   SetCurrentWnd(int nWndIndex);

	void   SetWndToolTip(int nWndIndex,CString strToolTip);
	CString  GetWndToolTip(int nWndIndex);

	// 刷新指定窗口.
	void InvalidateWnd(int nWndIndex);

public:
	// 设置激活窗口
	void SetActivePage(CWnd *pWnd, BOOL bRepaint = TRUE);	

	///////////////////////////////////////////////////
	//	全屏显示
	void SetFullScreen(BOOL bFlag);
	BOOL GetFullScreen();

	///////////////////////////////////////////////////
	//	多屏显示
	void SetMultiScreen(BOOL bFlag,BOOL bReSplitWnd = TRUE);
	BOOL GetMultiScreen();

	//////////////////////////////////////////////////
	//	自动调节尺寸
	void SetAutoAdjustPos(BOOL bFlag);
	BOOL GetAutoAdjustPos();

	void SetPlayPort(int port,int playtype,int windowID) ; //播放端口号

	///////////////////////////////////////////////////
	//	根据子窗口的序号 和 显示范围 ,得到窗口的位置
	virtual void CalcPageRect(LPRECT lpRect,int nIndex,int nPageCount);

	///////////////////////////////////////////////////
	//	根据预定义的比例得到显示区域(11/8)
	void AdjustRect(LPRECT lpRect);

	//////////////////////////////////////////////////////////////////////////
	//
	int HitTest(CPoint pt);

public:
	CPlaybackWnd m_wndVideo[MAX_WINDOW];

private:	
	int		m_nLastSplit;
	int		m_nCurPannelIndex;//当前窗口号

	// 激活窗口指针
	CWnd *m_pActivePage;
	// 全屏标记
	BOOL m_bFullScreen;
	// 多屏标记
	BOOL m_bMultiScreen;	
	// 自动调整标记
	BOOL m_bAutoAdjustPos;

	CPen m_penSelect;	// 选中窗口画笔.
	CPen m_penNorm;		// 未选中窗口画笔.

private:
	//	全屏时保存原窗口信息,用来恢复窗口
	WINDOWPLACEMENT _temppl;		//window's placement
	CWnd *			_tempparent;	//window's parent
};

#endif 