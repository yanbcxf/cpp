// CsvView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CsvView.h"

#include "CsvDocument.h"


#include "soci.h"
#include "soci-mysql.h"
#include "test/common-tests.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <cmath>
#include <ctime>
#include <ciso646>
#include <cstdlib>
#include <mysqld_error.h>
#include <errmsg.h>

using namespace soci;
using namespace soci::tests;

// CCsvView

IMPLEMENT_DYNCREATE(CCsvView, CView)

CCsvView::CCsvView()
{
	m_pGridCtrl = NULL;
}

CCsvView::~CCsvView()
{
	if (m_pGridCtrl)
		delete m_pGridCtrl;
}

BEGIN_MESSAGE_MAP(CCsvView, CView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCsvView 绘图

void CCsvView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CCsvView 诊断

#ifdef _DEBUG
void CCsvView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CCsvView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCsvView 消息处理程序

void CCsvView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl) return;

		// Create the Gridctrl window
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->Create(rect, this, 100);

		// fill it up with stuff
		m_pGridCtrl->SetEditable(TRUE);
		m_pGridCtrl->EnableDragAndDrop(TRUE);

		try {
			m_pGridCtrl->SetRowCount(20);
			m_pGridCtrl->SetColumnCount(10);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}

		
	}
}

void CCsvView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if (m_pGridCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->MoveWindow(rect);
	}
}

void CCsvView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类

	// Document 通知 view ，某些数据已经改变了

	CCsvDocument* pDoc = (CCsvDocument*)GetDocument();

	if(m_pGridCtrl)
	{
		try {
			m_pGridCtrl->SetRowCount(pDoc->m_arrayData.size()+1);
			m_pGridCtrl->SetColumnCount(pDoc->m_arrayHeader.size()+1);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}


		// fill rows/cols with text
		for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
		{
			for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;
				Item.col = col;
				if (row < 1) {
					Item.nFormat = DT_LEFT|DT_WORDBREAK;
					if(col==0)
						Item.strText.Format(_T("Column %d"),col);
					else
					{
						string val = pDoc->m_arrayHeader[col-1];
						Item.strText.Format(_T("%s"), val.c_str());
					}
				} else if (col < 1) {
					Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					Item.strText.Format(_T("Row %d"),row);
				} else {
					Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					string val = pDoc->m_arrayData[row-1][col-1];
					Item.strText.Format(_T("%s"), val.c_str());
				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

			m_pGridCtrl->AutoSize();
	}
	
}
