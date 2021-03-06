
#include "stdafx.h"
#include "mainfrm.h"
#include "LotteryMenuView.h"
#include "Resource.h"
#include "Lottery.h"

#include "LotteryDoc.h"
#include "LotteryNumberGridView.h"
#include "LotteryNumberGridBView.h"
#include "TrendDistributionGridView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CLotteryMenuView::CLotteryMenuView()
{
}

CLotteryMenuView::~CLotteryMenuView()
{
}

BEGIN_MESSAGE_MAP(CLotteryMenuView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LOTTERY_NUMBER_LIST, &CLotteryMenuView::OnLotteryNumberList)
	ON_COMMAND(ID_TREND_DISTRIBUTION_VIEW, &CLotteryMenuView::OnTrendDistributionView)
	ON_COMMAND(ID_LOTTERY_NUMBER_LIST_B, &CLotteryMenuView::OnLotteryNumberListB)
	ON_COMMAND(ID_LOTTERY_NEW_FILE, &CLotteryMenuView::OnLotteryNewFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CLotteryMenuView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像: 
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	FillFileView();
	AdjustLayout();

	return 0;
}

void CLotteryMenuView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CLotteryMenuView::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("Lotto 彩票"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	for (int i = 0; i< vecInfo.size(); i++)
	{
		HTREEITEM ht = m_wndFileView.InsertItem(vecInfo[i].name.c_str(), 1, 1, hRoot);
		m_wndFileView.SetItemData(ht, i);

	}
	m_wndFileView.Expand(hRoot, TVE_EXPAND);

}

void CLotteryMenuView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项: 
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_LOTTERY, point.x, point.y, this, TRUE);
}

void CLotteryMenuView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CLotteryMenuView::OnProperties()
{
	AfxMessageBox(_T("属性...."));

}

void CLotteryMenuView::OnFileOpen()
{
	// TODO: 在此处添加命令处理程序代码
}

void CLotteryMenuView::OnFileOpenWith()
{
	// TODO: 在此处添加命令处理程序代码
}

void CLotteryMenuView::OnDummyCompile()
{
	// TODO: 在此处添加命令处理程序代码
}

void CLotteryMenuView::OnEditCut()
{
	// TODO: 在此处添加命令处理程序代码
}

void CLotteryMenuView::OnEditCopy()
{
	// TODO: 在此处添加命令处理程序代码
}

void CLotteryMenuView::OnEditClear()
{
	// TODO: 在此处添加命令处理程序代码
}

void CLotteryMenuView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CLotteryMenuView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CLotteryMenuView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}




void CLotteryMenuView::OnLotteryNumberList()
{
	// 以下代码已经不用了， 因为 相应的弹出菜单已经删除了 

	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndFileView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
		int idx = (int)m_wndFileView.GetItemData(hTreeItem);
		

		CLotteryNumberGridView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;
		pDocTemplate = ((CLotteryApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);

		// 确保同一彩票的文档仅创建一份
		CLotteryDoc * pNewDoc = (CLotteryDoc *)((CLotteryApp *)AfxGetApp())->SearchDoc(idx);
		if (pNewDoc == NULL)
		{
			pNewDoc = new CLotteryDoc(); //strCarMunber);
			vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
			pNewDoc->m_lottery_info = vecInfo[idx];
			//pNewDoc->LoadDocumentFromDB();
			pDocTemplate->AddDocument(pNewDoc);
		}

		CString title;
		title.Format("%s",vecInfo[idx].name.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd = pDocTemplate->CreateNewFrame(pNewDoc, NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd, pNewDoc, TRUE);
		//pDocTemplate->AddDocument(pNewDoc);
	}
}


void CLotteryMenuView::OnTrendDistributionView()
{
	// 以下代码已经不用了， 因为 相应的弹出菜单已经删除了 

	// TODO: 在此添加命令处理程序代码
	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndFileView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
		int idx = (int)m_wndFileView.GetItemData(hTreeItem);

		CTrendDistributionGridView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;
		pDocTemplate = ((CLotteryApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);

		// 确保同一彩票的文档仅创建一份
		CLotteryDoc * pNewDoc = (CLotteryDoc *)((CLotteryApp *)AfxGetApp())->SearchDoc(idx);
		if (pNewDoc == NULL)
		{
			pNewDoc = new CLotteryDoc(); //strCarMunber);
			vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
			pNewDoc->m_lottery_info = vecInfo[idx];
			//pNewDoc->LoadDocumentFromDB();
			pDocTemplate->AddDocument(pNewDoc);
		}
		
		CString title;
		title.Format("%s", vecInfo[idx].name.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd = pDocTemplate->CreateNewFrame(pNewDoc, NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd, pNewDoc, TRUE);
		//pDocTemplate->AddDocument(pNewDoc);
	}
}


void CLotteryMenuView::OnLotteryNumberListB()
{
	// 以下代码已经不用了， 因为 相应的弹出菜单已经删除了 

	
}


void CLotteryMenuView::OnLotteryNewFile()
{
	// TODO: 在此添加命令处理程序代码

	CDocTemplate * pDocTemplate = NULL;
	CView * pView = NULL;

	HTREEITEM hTreeItem = m_wndFileView.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
		int idx = (int)m_wndFileView.GetItemData(hTreeItem);


		CLotteryNumberGridView  /*CIndiCninfoAnnouncementRoadView*/	MonitorListView;

		pView = &MonitorListView;
		pDocTemplate = ((CLotteryApp *)AfxGetApp())->GetCurrDocTemplate(pView);

		pView = NULL;
		ASSERT_VALID(pDocTemplate);

		CLotteryDoc * pNewDoc = new CLotteryDoc(); //strCarMunber);
		pNewDoc->m_lottery_info = vecInfo[idx];
		pNewDoc->SetPathName(vecInfo[idx].name.c_str(), FALSE);
		//pNewDoc->LoadDocumentFromDB();
		pDocTemplate->AddDocument(pNewDoc);

		CString title;
		title.Format("%s", vecInfo[idx].name.c_str());
		pNewDoc->SetTitle(title);
		CFrameWnd * wnd = pDocTemplate->CreateNewFrame(pNewDoc, NULL);//create view
		wnd->ShowWindow(SW_SHOWMAXIMIZED);
		pDocTemplate->InitialUpdateFrame(wnd, pNewDoc, TRUE);
	}
}
