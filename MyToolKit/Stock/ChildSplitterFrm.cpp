
// ChildFrm.cpp : CChildFrame 类的实现
//

#include "stdafx.h"
#include "Stock.h"

#include "ChildSplitterFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildSplitterFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildSplitterFrame, CMDIChildWndEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildSplitterFrame::CChildSplitterFrame()
{
	// TODO: 在此添加成员初始化代码
}

CChildSplitterFrame::~CChildSplitterFrame()
{
}

BOOL CChildSplitterFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,			// TODO: 调整行数和列数
		CSize(10, 10),	// TODO: 调整最小窗格大小
		pContext);

}


BOOL CChildSplitterFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式

	 cs.style &= ~WS_MAXIMIZEBOX; 

	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildSplitterFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildSplitterFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序

void CChildSplitterFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CIndividualShareDoc *  pDoc =  (CIndividualShareDoc *)GetActiveView()->GetDocument();

	CString strTitle = pDoc->GetTitle();
	if(string(strTitle.GetBuffer()).find("自动下载")!=string::npos)
	{
		if(pDoc->m_vecCommandCode.size()>0)
			AfxMessageBox("本页面下载完成后，自动关闭");
		else
			CMDIChildWndEx::OnClose();
	}
	else
		CMDIChildWndEx::OnClose();
}
