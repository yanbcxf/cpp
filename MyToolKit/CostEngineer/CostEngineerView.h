
// CostEngineerView.h: CCostEngineerView 类的接口
//

#pragma once

class CCostEngineerCntrItem;

class CCostEngineerView : public CBaseGridCtlView
{
protected: // 仅从序列化创建
	CCostEngineerView() noexcept;
	DECLARE_DYNCREATE(CCostEngineerView)

// 特性
public:
	CCostEngineerDoc* GetDocument() const;
	// m_pSelection 将所选内容保存在当前 CCostEngineerCntrItem 中。
	// 对于许多应用程序，这种成员变量不足以
	//  表示某个选择，例如在不属于
	//  CCostEngineerCntrItem 的对象中选定的一个或多个对象。提供这种选择
	//  机制的目的只是帮助入门

	// TODO: 用适合应用程序的选择机制替换此选择机制
	CCostEngineerCntrItem* m_pSelection;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// 容器支持

// 实现
public:
	virtual ~CCostEngineerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // CostEngineerView.cpp 中的调试版本
inline CCostEngineerDoc* CCostEngineerView::GetDocument() const
   { return reinterpret_cast<CCostEngineerDoc*>(m_pDocument); }
#endif

