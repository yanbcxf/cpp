#if !defined BITMAPBTN_H_
#define BITMAPBTN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define COLOR_TEXT  (RGB(176,176,176))			// 文本颜色.
#define COLOR_TEXT_SELECT  (RGB(255,255,255))	// 选中文本颜色.

enum {
	TEXT_LEFT = 0,
	TEXT_CENTER = 1,
	TEXT_RIGHT = 2,
	TEXT_BOTTOM = 3
};

class CSkinButton : public CButton
{
public:
	CSkinButton();
	virtual ~CSkinButton();

	void InitToolTip();
	BOOL GetCheck();
	void SetCheck(BOOL m_NewValue);
	void SetBitmap(CBitmap& mNrml, CBitmap& mOvr, CBitmap& mDwn, CBitmap& mDsbl);
	void SetTooltipText(CString strText);

	void SetButtonTextStyle(DWORD dwStyle = TEXT_CENTER) { m_dwTextStyle = dwStyle; }

	void CopyFrom(CRect r, CBitmap& bmpNorm, CBitmap& bmpOver, CBitmap& bmpDown, CBitmap& bmpDisabled);

	//{{AFX_VIRTUAL(CSkinButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//{{AFX_MSG(CSkinButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	UINT m_nBtnID;
	BOOL m_Check;	

protected:
	CToolTipCtrl m_ToolTipBtn;	
	CString		 m_strToolTip;
	BOOL m_CheckedButton;

	DWORD m_dwTextStyle;	// 文本样式.
	COLORREF m_clrText;		// 文本颜色.
	BOOL m_MouseOnButton;
	CBitmap m_Normal;
	CBitmap m_Over;
	CBitmap m_Down;
	CBitmap m_Disabled;
private:
	BOOL        m_TipFlag;

};

#endif