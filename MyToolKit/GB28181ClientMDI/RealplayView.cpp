// RealplayView.cpp : 实现文件
//

#include "stdafx.h"
#include "GB28181ClientMDIDoc.h"
#include "GB28181ClientMDI.h"
#include "RealplayView.h"


// CRealplayView

IMPLEMENT_DYNCREATE(CRealplayView, CFormView)

CRealplayView::CRealplayView()
	: CFormView(CRealplayView::IDD)
{
	
}

CRealplayView::~CRealplayView()
{
	
	
}

void CRealplayView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRealplayView, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CRealplayView 诊断

#ifdef _DEBUG
void CRealplayView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CRealplayView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CRealplayView 消息处理程序

void CRealplayView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码


	/*CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC);
	if(pStatic)
	{
		CRect rect;
		GetClientRect(rect);

		pStatic->MoveWindow(rect);
	}*/

}

int CRealplayView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_CTreeDropTarget.Register(this);
	return 0;
}

void CRealplayView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CRect t_winid;
	this->GetWindowRect(&t_winid);
	ScreenToClient(&t_winid);

	if(m_screenPannel.GetSafeHwnd() == NULL)
	{
		m_screenPannel.Create(
			NULL,
			NULL,
			WS_CHILD|WS_VISIBLE, 
			CRect(0,0,0,0), 
			this, 
			0);
		m_screenPannel.MoveWindow(t_winid);
		// 		DebugTrace("create emapvideo ");
	}
	m_screenPannel.ShowWindow(SW_SHOW);
	m_VideoRect = t_winid;
	m_screenPannel.SetWndPos(0);

	return;
}

BOOL CRealplayView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	HGLOBAL hgMem = NULL; 
	if (pDataObject->IsDataAvailable(CF_TEXT))
	{
		hgMem = pDataObject->GetGlobalData(CF_TEXT);
		sipd_catalog_item * pItem = (sipd_catalog_item *)GlobalLock(hgMem);
		if (pItem != NULL)
		{
			int nHit = m_screenPannel.HitTest(point);
			if(nHit>=0)
			{
				m_screenPannel.m_wndVideo[nHit].StartPlay(pItem);
			}
			else
				AfxMessageBox("未找到窗口");
		}

		if(hgMem)
		{
			// 空间的解锁和释放
			GlobalUnlock(hgMem);
			GlobalFree((HGLOBAL)hgMem);
		}
	}
	return CFormView::OnDrop(pDataObject, dropEffect, point);

}

DROPEFFECT CRealplayView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	return DROPEFFECT_COPY;

	return CFormView::OnDragEnter(pDataObject, dwKeyState, point);
}

void CRealplayView::OnDragLeave()
{
	// TODO: 在此添加专用代码和/或调用基类

	CFormView::OnDragLeave();
}

DROPEFFECT CRealplayView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: 在此添加专用代码和/或调用基类

	return DROPEFFECT_COPY;

	return CFormView::OnDragOver(pDataObject, dwKeyState, point);
}
