
// FfplaySdl2TestView.h : interface of the CFfplaySdl2TestView class
//

#pragma once


class CFfplaySdl2TestView : public CView
{
protected: // create from serialization only
	CFfplaySdl2TestView();
	DECLARE_DYNCREATE(CFfplaySdl2TestView)

// Attributes
public:
	CFfplaySdl2TestDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CFfplaySdl2TestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStreamOpen();
};

#ifndef _DEBUG  // debug version in FfplaySdl2TestView.cpp
inline CFfplaySdl2TestDoc* CFfplaySdl2TestView::GetDocument() const
   { return reinterpret_cast<CFfplaySdl2TestDoc*>(m_pDocument); }
#endif

