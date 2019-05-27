
// ChildFrm.h : CChildFrame 类的接口
//

#pragma once

class CChildSplitterFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildSplitterFrame)
public:
	CChildSplitterFrame();

// 属性
public:

// 操作
public:

// 重写
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildSplitterFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CMySplitterWndEx m_wndSplitter;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};
