
// StockView.h : CStockView 类的接口
//


#pragma once


class CStockView : public CView
{
protected: // 仅从序列化创建
public:
	CStockView();
	DECLARE_DYNCREATE(CStockView)

// 属性
public:
	CStockDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	void SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName);

// 实现
public:
	virtual ~CStockView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	CDMGraphCtrl	m_DMGraph;

	DATE	m_StartDate;
	DATE	m_EndDate;
	string	m_strViewType;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelectStock();
	afx_msg void OnSetStockRange();
private:
	void TradingDailyCurve(int nStart, int nEnd, string strViewType);
	void PeCurve(int nStart, int nEnd);
};

#ifndef _DEBUG  // StockView.cpp 中的调试版本
inline CStockDoc* CStockView::GetDocument() const
   { return reinterpret_cast<CStockDoc*>(m_pDocument); }
#endif

