// TabSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "CninfoPDF.h"
#include "TabSheet.h"


// CTabSheet

IMPLEMENT_DYNAMIC(CTabSheet, CTabCtrl)

CTabSheet::CTabSheet()
{
	m_nCurrentPage = 0;
	m_nNumOfPages = 0;
}

CTabSheet::~CTabSheet()
{
	for(int i = 0; i<m_nNumOfPages; i++)
	{
		if(m_pPages[i])
			delete m_pPages[i];
	}
}


BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CTabSheet 消息处理程序



BOOL CTabSheet::AddPage(LPCTSTR title, CMyDialog * pDialog, UINT ID)
{
	if(MAXPAGES == m_nNumOfPages)
		return FALSE;

	m_nNumOfPages++;
	m_pPages[m_nNumOfPages - 1] = pDialog;
	m_IDD[m_nNumOfPages - 1] = ID;
	m_Title[m_nNumOfPages - 1] = title;
	return TRUE;
}

void CTabSheet::Show(void)
{
	for(int i=0; i< m_nNumOfPages; i++)
	{
		m_pPages[i]->Create(m_IDD[i], this);
		InsertItem(i, m_Title[i]);
	}

	m_pPages[0]->ShowWindow(SW_SHOW);
	m_nCurrentPage = 0;

	for(int i=1; i< m_nNumOfPages; i++)
		m_pPages[i]->ShowWindow(SW_HIDE);

	SetRect();
}

void CTabSheet::SetRect(void)
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;

	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	//	计算出各子对话框的相对于 Tab Control 的位置和大小
	nX = itemRect.left;
	nY = itemRect.bottom + 1;
	nXc = tabRect.right - itemRect.left -2;
	nYc = tabRect.bottom - nY - 2;

	//	利用计算出的数据对各子对话框进行调整
	m_pPages[0]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	for(int nCount = 1; nCount < m_nNumOfPages; nCount++)
		m_pPages[nCount]->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_HIDEWINDOW);

}

void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CTabCtrl::OnLButtonDown(nFlags, point);

	if(m_nCurrentPage!=GetCurFocus())
	{
		//	将原先的子对话框隐藏
		m_pPages[m_nCurrentPage]->ShowWindow(SW_HIDE);
	}

	m_nCurrentPage = GetCurFocus();

	//	显示当前标签所对应的子对话框
	m_pPages[m_nCurrentPage]->ShowWindow(SW_SHOW);
}


void CTabSheet::ProcessMessage(string strMsg)
{
	for(int nCount = 0; nCount < m_nNumOfPages; nCount++)
		m_pPages[nCount]->ProcessMessage(strMsg);
}