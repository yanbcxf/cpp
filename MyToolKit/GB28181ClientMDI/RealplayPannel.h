#if !defined(AFX_REALPLAY_PANNEL_H__F79F0D7D_1068_4E35_8380_22AE3E322707__INCLUDED_)
#define AFX_REALPLAY_PANNEL_H__F79F0D7D_1068_4E35_8380_22AE3E322707__INCLUDED_

#include "RealplayWnd.h"

class CRealplayPannel : public CWnd
{
// Construction
public:
	CRealplayPannel();

// Implementation
public:
	virtual ~CRealplayPannel();
	// Generated message map functions
protected:
	//{{AFX_MSG(CRealplayPannel)
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetWndPos(int npos);
	BOOL SetShowPlayWin(int nMain);

	// 设置激活窗口
	void SetActiveWnd(CRealplayWnd *pWnd,BOOL bRePaint = FALSE);

public:
	// 创建对象(Container)
	BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	//	全屏显示
	void SetFullScreen(BOOL bFlag);
	BOOL GetFullScreen();

	//	多屏显示
	void SetMultiScreen(BOOL bFlag);
	BOOL GetMultiScreen();

	//	根据子窗口的序号 和 显示范围 ,得到窗口的位置
	void CalcPageRect(LPRECT lpRect,int nIndex,int nPageCount);

	int HitTest(CPoint pt);

public:
	CRealplayWnd m_wndVideo[MAX_WNDNUM];
	int nLastSplit;
	int m_nCurWnd;
	int m_nWindowNum;

	

	// 全屏标记
	BOOL m_bFullScreen;
	///////////////////////////////////////////////////
	// 激活窗口指针
	CRealplayWnd *m_pActivePage;
	///////////////////////////////////////////////////
	// 多屏标记
	BOOL m_bMultiScreen;
	//	全屏时保存原窗口信息,用来恢复窗口
	WINDOWPLACEMENT _temppl;		//window's placement
	CWnd *			_tempparent;	//window's parent
};

#endif