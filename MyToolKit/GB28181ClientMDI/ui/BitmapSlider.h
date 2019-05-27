#if !defined _BITMAPSLIDER_H_
#define _BITMAPSLIDER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "memdc.h"

#define WM_BITMAPSLIDER_MOVING		WM_USER + 9425
#define WM_BITMAPSLIDER_MOVED		WM_USER + 9426

class CBitmapSlider : public CStatic
{
public:
	CBitmapSlider();

public:
	//{{AFX_VIRTUAL(CBitmapSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSliderID(UINT nSliderID){ m_nSliderID = nSliderID;};
	UINT GetSliderID()				{ return m_nSliderID; };
	void GetRange( int &nMin, int &nMax ) { nMin = m_nMin; nMax = m_nMax; };
	int GetRangeMax() { return m_nMax; };
	int GetRangeMin() { return m_nMin; };

	int GetPos() { return m_nPos; };

	void SetRange( int nMin, int nMax, BOOL bRedraw=FALSE );
	void SetRangeMin(int nMin, BOOL bRedraw = FALSE);
	void SetRangeMax( int nMax, BOOL bRedraw = FALSE );

	void SetPos( int nPos );
	int SetPageSize( int nSize );

	BOOL SetBitmapChannel(
		UINT nChannelID, UINT nActiveID=NULL, BOOL bTransparent=FALSE,
		COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );

	BOOL SetBitmapThumb(
		UINT nThumbID, UINT nActiveID=NULL, BOOL bTransparent=FALSE,
		COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );

	void SetMargin( int nLeft, int nTop, int nRight, int nBottom );
	void SetMarginTop( int nMargin ) { m_nMarginTop = nMargin; };
	void SetMarginLeft( int nMargin ) { m_nMarginLeft = nMargin; };
	void SetMarginRight( int nMargin ) { m_nMarginRight = nMargin; };
	void SetMarginBottom( int nMargin ) { m_nMarginBottom = nMargin; };

	void SetVertical( BOOL bVertical=TRUE ) { m_bVertical = bVertical; };

	void Enable( BOOL bEnable = TRUE );

	void DrawFocusRect( BOOL bDraw = TRUE, BOOL bRedraw = FALSE );

	virtual ~CBitmapSlider();

	void InitToolTip();
	void SetTooltipText(CString spText);

	void SetShowToolTipPos(BOOL bShowToolTipPos = TRUE)		{ m_bShowToolTipPos = bShowToolTipPos; }
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
protected:
	void RestoreBackground(CDC *pDC, int nXDst, int nYDst, int nWidth, int nHeight, CBitmap *pBmSrc);

	void CopyBackground(CDC *pDC, int nXSrc, int nYSrc, int nWidth, int nHeight, CBitmap *pBmDst );

	void DrawBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,CDC* pTmpDC, int xSource, int ySource,CBitmap *bmMask = NULL, BOOL bTransparent = FALSE );

	void DrawTransparentBitmap(CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,CDC* pTmpDC, int xSource, int ySource, CBitmap *bmMask );

	void PrepareMask(CBitmap* pBmpSource, CBitmap* pBmpMask,COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );

	int Pixel2Pos( int nPixel );
	int Pos2Pixel( int nPos );

	int m_nSliderID;
	int m_nMax, m_nMin, m_nPos, m_nPage;

	CRect m_rect;
	int m_nWidth, m_nHeight;
	int m_nThumbWidth, m_nThumbHeight;
	int m_nMarginLeft, m_nMarginRight, m_nMarginTop, m_nMarginBottom;
	int m_nThumbBgX, m_nThumbBgY;
	int m_nMouseOffset;

	BOOL m_bVertical;
	BOOL m_bChannelActive, m_bThumbActive;
	BOOL m_bTransparentChannel, m_bTransparentThumb, m_bThumb, m_bChannel;
	BOOL m_bLButtonDown, m_bFocus, m_bFocusRect, m_bDrawFocusRect;
	BOOL m_bEnable;
	BOOL m_bMouseOnButton;

	CBitmap m_bmChannel, m_bmChannelMask, m_bmChannelActive, m_bmChannelActiveMask;
	CBitmap m_bmThumb, m_bmThumbMask, m_bmThumbActive, m_bmThumbActiveMask, m_bmThumbBg;

	CToolTipCtrl m_ToolTip;
	CString		 m_strToolTip;

	BOOL		m_bShowToolTipPos;	// 是否在提示框中显示当前进度位置.

	//{{AFX_MSG(CBitmapSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	// This is CStatic method
	void SetBitmap( HBITMAP hBitmap ) {};
};

#endif