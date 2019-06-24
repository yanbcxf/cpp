
// CostEngineerView.cpp: CCostEngineerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CostEngineer.h"
#endif

#include "CostEngineerDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "CostEngineerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCostEngineerView

IMPLEMENT_DYNCREATE(CCostEngineerView, CBaseMessageFormView)

BEGIN_MESSAGE_MAP(CCostEngineerView, CBaseMessageFormView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_OLE_INSERT_NEW, &CCostEngineerView::OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, &CCostEngineerView::OnCancelEditCntr)
	ON_COMMAND(ID_FILE_PRINT, &CCostEngineerView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCostEngineerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CCostEngineerView 构造/析构

CCostEngineerView::CCostEngineerView() noexcept
{
	m_pSelection = nullptr;
	// TODO: 在此处添加构造代码

}

CCostEngineerView::~CCostEngineerView()
{
}

BOOL CCostEngineerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}



void CCostEngineerView::OnInitialUpdate()
{
	CBaseMessageFormView::OnInitialUpdate();


	// TODO: 写入最终选择模式代码之后移除此代码
	m_pSelection = nullptr;    // 初始化所选内容

}


// CCostEngineerView 打印


void CCostEngineerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCostEngineerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCostEngineerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCostEngineerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CCostEngineerView::OnDestroy()
{
	// 析构时停用此项；这在
	// 使用拆分器视图时非常重要 
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != nullptr && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == nullptr);
   }
   CView::OnDestroy();
}

// CCostEngineerView 绘图

void CCostEngineerView::OnDraw(CDC* pDC)
{
	if (!pDC)
		return;

	CCostEngineerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	// TODO: 同时绘制文档中的所有 OLE 项

	// 在任意位置绘制选定项。  一旦
	//  实现了真正的绘制代码后，应移除此代码。  此位置
	//  与 CCostEngineerCntrItem 返回的矩形完全对应，
	//  从而产生就地编辑的效果。

	// TODO: 最终绘制代码完成后移除此代码。

}

// OLE 客户端支持和命令

BOOL CCostEngineerView::IsSelected(const CObject* pDocItem) const
{
	//下面的实现是足够的，如果所选内容包括
	// CCostEngineerCntrItem 对象，则以下实现就足够了。若要处理其他选择机制，
	//  则应替换此处的实现

	// TODO: 实现对所选 OLE 客户端项进行测试的函数

	return pDocItem == m_pSelection;
}

void CCostEngineerView::OnInsertObject()
{
	// 调用标准的“插入对象”对话框以获得有关
	//  对于新的 CCostEngineerCntrItem 对象
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CCostEngineerCntrItem* pItem = nullptr;
	TRY
	{
		// 创建与此文档相连接的新项
		CCostEngineerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CCostEngineerCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// 通过对话框数据初始化该项
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // 任何异常都将导致该结果
		ASSERT_VALID(pItem);

        if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);
		// 作为任意用户界面设计，这会将所选内容
		//  设置为插入的最后一项

		// TODO: 重新实现所选内容，使其适合于您的应用程序
		m_pSelection = pItem;   // 将所选内容设置为插入的最后一项
		pDoc->UpdateAllViews(nullptr);
	}
	CATCH(CException, e)
	{
		if (pItem != nullptr)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

// 以下命令处理程序提供了标准键盘
//  用户界面以取消就地编辑会话。  此处，
//  容器(而不是服务器)将导致停用
void CCostEngineerView::OnCancelEditCntr()
{
	// 关闭此视图中的任何就地活动项。
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != nullptr)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == nullptr);
}

// 在就地编辑一个对象时，容器需要对 OnSetFocus 和 OnSize 
//  进行特殊处理
void CCostEngineerView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != nullptr &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// 如果该项处于同一视图中，则需要将焦点设置到该项
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != nullptr)
		{
			pWnd->SetFocus();   // 不要调用基类
			return;
		}
	}

	CView::OnSetFocus(pOldWnd);
}


void CCostEngineerView::OnFilePrint()
{
	//默认情况下，要求活动文档使用 IOleCommandTarget 打印自身。
	//如果不需要此行为，
	//请移除对 COleDocObjectItem::DoDefaultPrinting 调用。
	//如果由于某种原因调用失败，则将尝试使用 IPrint 接口打印
	//该文档对象。
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != nullptr);
	if (S_OK == COleDocObjectItem::DoDefaultPrinting(this, &printInfo))
		return;

	CView::OnFilePrint();

}


void CCostEngineerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCostEngineerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCostEngineerView 诊断

#ifdef _DEBUG
void CCostEngineerView::AssertValid() const
{
	CView::AssertValid();
}

void CCostEngineerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCostEngineerDoc* CCostEngineerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCostEngineerDoc)));
	return (CCostEngineerDoc*)m_pDocument;
}
#endif //_DEBUG


// CCostEngineerView 消息处理程序


void CCostEngineerView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
}


void CCostEngineerView::PostGridClick(int gridId, int nRow, int nCol) {
	CCostEngineerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	bool bRedraw = false;

	if (gridId == 0 && m_nChildrenCode != nRow - 1) {
		m_nChildrenCode = nRow - 1;
		bRedraw = true;
	}
		
	CString source ;
	if (gridId == 0) {
		source = m_Grid.GetItemText(nRow, nCol);
	}
	if (gridId == 1) {
		source = m_Grid1.GetItemText(nRow, nCol);
	}

	if (source == "修改（update）") {
		if (gridId == 0) {
			/* 主表格 */
			if (CColumnObj::Update(m_strMenuCode, nRow, pDoc->columns)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CBeamObj::Update(m_strMenuCode, nRow, pDoc->beams)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CSimilarEngineerBudget::Update(m_strMenuCode, nRow, pDoc->similarEngineerBudgets)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CBudgetIndex::Update(m_strMenuCode, nRow, pDoc->budgetIndexs)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CBuildingBudget::Update(m_strMenuCode, nRow, pDoc->buildingBudgets)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CItemOfUnitPrice::Update(m_strMenuCode, nRow, pDoc->compositeUnitPrices)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CConsumptionQuota::Update(m_strMenuCode, nRow, pDoc->consumptionQuotas)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CItemOfTotalPrice::Update(m_strMenuCode, nRow, pDoc->itemOfTotalPrices)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
			if (CDecisionTree::Update(m_strMenuCode, nRow, pDoc->decisionTrees)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
		}
		else {
			/* 子表格 */
			if (CBeamSpan::Update(m_strMenuCode, nRow, pDoc->beams[m_nChildrenCode].m_spans)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}

			if (CSimilarEngineerBudgetObj::Update(m_strMenuCode, nRow, pDoc->similarEngineerBudgets[m_nChildrenCode].m_percents)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}

			if (CBudgetIndexObj::Update(m_strMenuCode, nRow, pDoc->budgetIndexs[m_nChildrenCode].m_materials)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}

			if (CBuildingBudgetObj::Update(m_strMenuCode, nRow, pDoc->buildingBudgets[m_nChildrenCode].m_materials)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}

			if (CItemOfUnitPriceObj::Update(m_strMenuCode, nRow, pDoc->compositeUnitPrices[m_nChildrenCode].m_materials, pDoc->consumptionQuotas)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}

			if (CConsumptionQuotaObj::Update(m_strMenuCode, nRow, pDoc->consumptionQuotas[m_nChildrenCode].m_materials)) {
				pDoc->SetModifiedFlag();
				bRedraw = true;
			}
		}
		
	}
	if (source == "删除（delete）") {
		int nRes = AfxMessageBox(_T("确定删除吗？"), MB_OKCANCEL | MB_ICONQUESTION);
		if (nRes == IDOK) {
			if (gridId == 0) {
				/* 主表格 */
				if (CColumnObj::Delete(m_strMenuCode, nRow, pDoc->columns)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CBeamObj::Delete(m_strMenuCode, nRow, pDoc->beams)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CSimilarEngineerBudget::Delete(m_strMenuCode, nRow, pDoc->similarEngineerBudgets)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CBudgetIndex::Delete(m_strMenuCode, nRow, pDoc->budgetIndexs)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CBuildingBudget::Delete(m_strMenuCode, nRow, pDoc->buildingBudgets)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CItemOfUnitPrice::Delete(m_strMenuCode, nRow, pDoc->compositeUnitPrices)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CConsumptionQuota::Delete(m_strMenuCode, nRow, pDoc->consumptionQuotas)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CItemOfTotalPrice::Delete(m_strMenuCode, nRow, pDoc->itemOfTotalPrices)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
				if (CDecisionTree::Delete(m_strMenuCode, nRow, pDoc->decisionTrees)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}
			else {
				/* 子表格 */
				if (CBeamSpan::Delete(m_strMenuCode, nRow, pDoc->beams[m_nChildrenCode].m_spans)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}

				if (CSimilarEngineerBudgetObj::Delete(m_strMenuCode, nRow, pDoc->similarEngineerBudgets[m_nChildrenCode].m_percents)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}

				if (CBudgetIndexObj::Delete(m_strMenuCode, nRow, pDoc->budgetIndexs[m_nChildrenCode].m_materials)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}

				if (CBuildingBudgetObj::Delete(m_strMenuCode, nRow, pDoc->buildingBudgets[m_nChildrenCode].m_materials)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}

				if (CItemOfUnitPriceObj::Delete(m_strMenuCode, nRow, pDoc->compositeUnitPrices[m_nChildrenCode].m_materials)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}

				if (CConsumptionQuotaObj::Delete(m_strMenuCode, nRow, pDoc->consumptionQuotas[m_nChildrenCode].m_materials)) {
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}
		}
	}
	if (source == "增加（create）") {
		if (gridId == 0) {
			/* 主表格 */
			{
				CBeamSpan c;
				if (c.CreateOrUpdate(m_strMenuCode)) {
					pDoc->beams[m_nChildrenCode].m_spans.push_back(c);
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}
			
			{
				CSimilarEngineerBudgetObj c;
				if (c.CreateOrUpdate(m_strMenuCode)) {
					pDoc->similarEngineerBudgets[m_nChildrenCode].m_percents.push_back(c);
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}

			{
				CBudgetIndexObj c;
				if (c.CreateOrUpdate(m_strMenuCode)) {
					pDoc->budgetIndexs[m_nChildrenCode].m_materials.push_back(c);
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}

			{
				CBuildingBudgetObj c;
				if (c.CreateOrUpdate(m_strMenuCode)) {
					pDoc->buildingBudgets[m_nChildrenCode].m_materials.push_back(c);
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}
			
			{
				CItemOfUnitPriceObj c;
				if (c.CreateOrUpdate(m_strMenuCode, pDoc->consumptionQuotas)) {
					pDoc->compositeUnitPrices[m_nChildrenCode].m_materials.push_back(c);
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}

			{
				CConsumptionQuotaObj c;
				if (c.CreateOrUpdate(m_strMenuCode)) {
					pDoc->consumptionQuotas[m_nChildrenCode].m_materials.push_back(c);
					pDoc->SetModifiedFlag();
					bRedraw = true;
				}
			}
		}
	}

	if (bRedraw) 
		RedrawView();;
}



void CCostEngineerView::RedrawView() {
	CCostEngineerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (CColumnObj::Draw(m_strMenuCode, &m_Grid, pDoc->columns)) {
		m_Grid1.SetRowCount(0);
		
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 9;
		m_down_percent = 1;
		ReLayout();
		return;
	}

	if (CBeamObj::Draw(m_strMenuCode, &m_Grid, pDoc->beams)) {
		if (m_nChildrenCode < pDoc->beams.size()) {
			CBeamSpan::Draw(&m_Grid1, pDoc->beams[m_nChildrenCode].m_spans);
		}
		else {
			m_Grid1.SetRowCount(0);
		}
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 4;
		m_down_percent = 6;
		ReLayout();
		return;
	}

	if (CSimilarEngineerBudget::Draw(m_strMenuCode, &m_Grid, pDoc->similarEngineerBudgets)) {
		if (m_nChildrenCode < pDoc->similarEngineerBudgets.size()) {
			CSimilarEngineerBudgetObj::Draw(&m_Grid1, pDoc->similarEngineerBudgets[m_nChildrenCode].m_percents);
		}
		else {
			m_Grid1.SetRowCount(0);
		}
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 4;
		m_down_percent = 6;
		ReLayout();
		return;
	}

	if (CBudgetIndex::Draw(m_strMenuCode, &m_Grid, pDoc->budgetIndexs)) {
		if (m_nChildrenCode < pDoc->budgetIndexs.size()) {
			CBudgetIndexObj::Draw(&m_Grid1, pDoc->budgetIndexs[m_nChildrenCode].m_materials);
		}
		else {
			m_Grid1.SetRowCount(0);
		}
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 4;
		m_down_percent = 6;
		ReLayout();
		return;
	}

	if (CBuildingBudget::Draw(m_strMenuCode, &m_Grid, pDoc->buildingBudgets)) {
		if (m_nChildrenCode < pDoc->buildingBudgets.size()) {
			CBuildingBudgetObj::Draw(&m_Grid1, pDoc->buildingBudgets[m_nChildrenCode].m_materials);
		}
		else {
			m_Grid1.SetRowCount(0);
		}
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 4;
		m_down_percent = 6;
		ReLayout();
		return;
	}

	if (CItemOfUnitPrice::Draw(m_strMenuCode, &m_Grid, pDoc->compositeUnitPrices)) {
		if (m_nChildrenCode < pDoc->compositeUnitPrices.size()) {
			CItemOfUnitPriceObj::Draw(&m_Grid1, pDoc->compositeUnitPrices[m_nChildrenCode].m_materials, pDoc->consumptionQuotas);
		}
		else {
			m_Grid1.SetRowCount(0);
		}
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 4;
		m_down_percent = 6;
		ReLayout();
		return;
	}

	if (CConsumptionQuota::Draw(m_strMenuCode, &m_Grid, pDoc->consumptionQuotas)) {
		if (m_nChildrenCode < pDoc->consumptionQuotas.size()) {
			CConsumptionQuotaObj::Draw(&m_Grid1, pDoc->consumptionQuotas[m_nChildrenCode].m_materials);
		}
		else {
			m_Grid1.SetRowCount(0);
		}
		m_display_mode = DisplayModes::Grid_Grid;
		m_upper_percent = 4;
		m_down_percent = 6;
		ReLayout();
		return;
	}

	if (CItemOfTotalPrice::Draw(m_strMenuCode, &m_Grid, pDoc->itemOfTotalPrices)) {
		m_Grid1.SetRowCount(0);

		m_display_mode = DisplayModes::Grid;
		m_upper_percent = 19;
		m_down_percent = 1;
		ReLayout();
		return;
	}

	if (CDecisionTree::Draw(m_strMenuCode, &m_Grid, pDoc->decisionTrees)) {
		m_Grid1.SetRowCount(0);

		m_display_mode = DisplayModes::Grid;
		m_upper_percent = 19;
		m_down_percent = 1;
		ReLayout();
		return;
	}

	try {

		m_display_mode = DisplayModes::None;

		/* 主表格清空 */
		m_Grid.SetRowCount(1);
		m_Grid.SetColumnCount(5 + 3);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(1);
		m_Grid.SetHeaderSort(TRUE);
		m_Grid.DeleteRow(0);

		/* 子表格清空 */
		m_Grid1.SetRowCount(1);
		m_Grid1.SetColumnCount(5 + 3);
		m_Grid1.SetFixedRowCount(1);
		m_Grid1.SetFixedColumnCount(1);
		m_Grid1.SetHeaderSort(TRUE);
		m_Grid1.DeleteRow(0);

		ReLayout();
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return;
	}
}