
#include "stdafx.h"
#include "mainfrm.h"
#include "MenuTreeView.h"
#include "CostEngineer.h"

#include "CostEngineerDoc.h"
#include "CostEngineerView.h"
#include "GridDlg.h"
#include "DyncItemGroupDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CMenuTreeView::CMenuTreeView()
{
}

CMenuTreeView::~CMenuTreeView()
{
}

BEGIN_MESSAGE_MAP(CMenuTreeView, CDockablePane)
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
	/*ON_COMMAND(ID_LOTTERY_NUMBER_LIST, &CLotteryMenuView::OnLotteryNumberList)
	ON_COMMAND(ID_TREND_DISTRIBUTION_VIEW, &CLotteryMenuView::OnTrendDistributionView)
	ON_COMMAND(ID_LOTTERY_NUMBER_LIST_B, &CLotteryMenuView::OnLotteryNumberListB)
	ON_COMMAND(ID_LOTTERY_NEW_FILE, &CLotteryMenuView::OnLotteryNewFile)*/
	ON_COMMAND(ID_CONCRETE_QUANTITY, &CMenuTreeView::OnConcreteQuantity)
	ON_NOTIFY(TVN_SELCHANGED, ID_VIEW_MENUTREE, OnSelChange)
	ON_COMMAND(ID_INPUT_COMPONENT, &CMenuTreeView::OnInputComponent)
	ON_COMMAND(ID_STEEL_QUANTITY, &CMenuTreeView::OnSteelQuantity)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

string getTreeItemCode(CTreeCtrl* pWndTree, HTREEITEM hTreeItem) {
	CString text = pWndTree->GetItemText(hTreeItem);

	vector<string> firstMatch;
	string strPattern = string("\\([A-Za-z\\d]+\\)");
	if (Pcre2Grep(strPattern.c_str(), text.GetBuffer(), firstMatch) <= 0)
	{
		AfxMessageBox("获取代码错误");
		return "";
	}
	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(1, pos1 - 1);
	return codeStr;
}

int CMenuTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, ID_VIEW_MENUTREE))
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

void CMenuTreeView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void createTree(CViewTree &  wndFileView,  HTREEITEM hRoot, CostEngineerInfo & info) {
	for (int i = 0; i < info.list.size(); i++)
	{
		CString str;
		str.Format("%s (%s)", info.list[i].name.c_str(), info.list[i].code.c_str());
		HTREEITEM ht = wndFileView.InsertItem(str, 1, 1, hRoot);
		/* 以便判断是否为 叶子节点 */
		wndFileView.SetItemData(ht, info.list[i].list.size());

		createTree(wndFileView, ht, info.list[i]);
	}
	wndFileView.Expand(hRoot, TVE_EXPAND);
}

void CMenuTreeView::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("Root"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	createTree(m_wndFileView, hRoot, ((CCostEngineerApp *)AfxGetApp())->m_cost_engineer_info);
	m_wndFileView.Expand(hRoot, TVE_EXPAND);
}

void CMenuTreeView::AdjustLayout()
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

void CMenuTreeView::OnProperties()
{
	AfxMessageBox(_T("属性...."));

}

void CMenuTreeView::OnFileOpen()
{
	// TODO: 在此处添加命令处理程序代码
}

void CMenuTreeView::OnFileOpenWith()
{
	// TODO: 在此处添加命令处理程序代码
}

void CMenuTreeView::OnDummyCompile()
{
	// TODO: 在此处添加命令处理程序代码
}

void CMenuTreeView::OnEditCut()
{
	// TODO: 在此处添加命令处理程序代码
}

void CMenuTreeView::OnEditCopy()
{
	// TODO: 在此处添加命令处理程序代码
}

void CMenuTreeView::OnEditClear()
{
	// TODO: 在此处添加命令处理程序代码
}

void CMenuTreeView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CMenuTreeView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CMenuTreeView::OnChangeVisualStyle()
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




void CMenuTreeView::OnSelChange(NMHDR *pNMHDR, LRESULT *pResult) {

	if (!AfxGetApp()->m_pMainWnd) {
		*pResult = 0;
		return;
	}

	CCostEngineerView * pView = (CCostEngineerView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	if (!pView) {
		*pResult = 0;
		return;
	}
	CCostEngineerDoc * pDoc = (CCostEngineerDoc *)pView->GetDocument();

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	pNMTreeView->itemNew;	 //选中HTREEITEM
	pNMTreeView->itemOld;	 //上次选中HTREEITEM
	
	// TODO:  在此添加控件通知处理程序代码
	string codeStr = getTreeItemCode((CTreeCtrl*)&m_wndFileView, pNMTreeView->itemNew.hItem);
	pView->m_strMenuCode = codeStr;
	pView->m_nChildrenCode = 0;
	pView->RedrawView();

	*pResult = 0;
}


void CMenuTreeView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndFileView;
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
			int nLeaf = pWndTree->GetItemData(hTreeItem);
			if (nLeaf == 0) {
				/* 叶子节点 */
				string codeStr = getTreeItemCode(pWndTree, hTreeItem);
				if (CColumnObj::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CColumnObj::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CBeamObj::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CBeamObj::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CSimilarEngineerBudget::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CSimilarEngineerBudget::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CBudgetIndex::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CBudgetIndex::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CBuildingBudget::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CBuildingBudget::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CConsumptionQuota::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CConsumptionQuota::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CItemOfUnitPrice::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CItemOfUnitPrice::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CItemOfTotalPrice::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CItemOfTotalPrice::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CDecisionTree::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CDecisionTree::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CActivityOnArrow::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CActivityOnArrow::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CCashFlow::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CCashFlow::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CIntegratedEvaluation::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CIntegratedEvaluation::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CProjectSettlement::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CProjectSettlement::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CContractPrice::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CContractPrice::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CProjectSettlementEx5::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CProjectSettlementEx5::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
				if (CFinanceAnalysis::PopupMenuId(codeStr)) {
					theApp.GetContextMenuManager()->ShowPopupMenu(CFinanceAnalysis::PopupMenuId(codeStr), point.x, point.y, this, TRUE);
				}
			}

		}
	}
	pWndTree->SetFocus();
}


void CMenuTreeView::OnInputComponent()
{
	CCostEngineerView * pView = (CCostEngineerView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	if (!pView) {
		AfxMessageBox(_T("当前没有文档"));
		return;
	}
		
	CCostEngineerDoc * pDoc = (CCostEngineerDoc *)pView->GetDocument();

	// TODO: 在此添加命令处理程序代码
	HTREEITEM hTreeItem = m_wndFileView.GetSelectedItem();
	string codeStr = getTreeItemCode((CTreeCtrl*)&m_wndFileView, hTreeItem);
	// AfxMessageBox(codeStr.c_str());

	{
		CColumnObj c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->columns.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}
	
	{
		CBeamObj c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->beams.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CSimilarEngineerBudget c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->similarEngineerBudgets.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CBudgetIndex c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->budgetIndexs.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CBuildingBudget c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->buildingBudgets.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CConsumptionQuota c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->consumptionQuotas.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CItemOfUnitPrice c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->compositeUnitPrices.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}
	
	{
		CItemOfTotalPrice c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->itemOfTotalPrices.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CDecisionTree c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->decisionTrees.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CActivityOnArrow c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->activityOnArrows.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CCashFlow c;
		if (c.CreateOrUpdate(codeStr)) {
			pDoc->cashFlows.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CIntegratedEvaluation * c = NULL;
		if (CIntegratedEvaluation::CreateOrUpdate(codeStr, c)) {
			pDoc->integratedEvaluations.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CProjectSettlement* c = NULL;
		if (CProjectSettlement::Create(codeStr, c)) {
			pDoc->projectSettlements.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CContractPrice* c = NULL;
		if (CContractPrice::Create(codeStr, c)) {
			pDoc->contractPrices.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CProjectSettlementEx5* c = NULL;
		if (CProjectSettlementEx5::Create(codeStr, c)) {
			pDoc->projectSettlementEx5s.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	{
		CFinanceAnalysis* c = NULL;
		if (CFinanceAnalysis::Create(codeStr, c)) {
			pDoc->financeAnalysis.push_back(c);
			pDoc->SetModifiedFlag();
		}
	}

	pView->RedrawView();
}


void CMenuTreeView::OnConcreteQuantity()
{
	// TODO: 在此添加命令处理程序代码
	CCostEngineerView * pView = (CCostEngineerView *)((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	if (!pView) {
		AfxMessageBox(_T("当前没有文档"));
		return;
	}

	CCostEngineerDoc * pDoc = (CCostEngineerDoc *)pView->GetDocument();

	// TODO: 在此添加命令处理程序代码
	HTREEITEM hTreeItem = m_wndFileView.GetSelectedItem();
	string codeStr = getTreeItemCode((CTreeCtrl*)&m_wndFileView, hTreeItem);

	CColumnObj::SteelQuantity(codeStr, pDoc->columns);
	CBeamObj::SteelQuantity(codeStr, pDoc->beams);
	CSimilarEngineerBudget::Calculate(codeStr, pDoc->similarEngineerBudgets);
	CBudgetIndex::Calculate(codeStr, pDoc->budgetIndexs);
	CBuildingBudget::Calculate(codeStr, pDoc->buildingBudgets);
	CConsumptionQuota::Calculate(codeStr, pDoc->consumptionQuotas);
	CItemOfUnitPrice::Calculate(codeStr, pDoc->compositeUnitPrices, pDoc->consumptionQuotas);
	CItemOfTotalPrice::Calculate(codeStr, pDoc->itemOfTotalPrices);
	CDecisionTree::Calculate(codeStr, pDoc->decisionTrees);
	CActivityOnArrow::Calculate(codeStr, pDoc->activityOnArrows);
	CCashFlow::Calculate(codeStr, pDoc->cashFlows);
	// CProjectSettlementEx5::Calculate(codeStr, pDoc->projectSettlementEx5s);

	pView->RedrawView();
}


void CMenuTreeView::OnSteelQuantity()
{
	// TODO: 在此添加命令处理程序代码
	CView * pView = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	if (!pView) {
		AfxMessageBox(_T("当前没有文档"));
		return;
	}

	CCostEngineerDoc * pDoc = (CCostEngineerDoc *)pView->GetDocument();

	// TODO: 在此添加命令处理程序代码
	HTREEITEM hTreeItem = m_wndFileView.GetSelectedItem();
	string codeStr = getTreeItemCode((CTreeCtrl*)&m_wndFileView, hTreeItem);

	CItemOfUnitPrice::Calculate1(codeStr, pDoc->compositeUnitPrices, pDoc->consumptionQuotas);
}
