
// CostEngineerDoc.cpp: CCostEngineerDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CostEngineer.h"
#endif

#include "CostEngineerDoc.h"
#include "CntrItem.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCostEngineerDoc

IMPLEMENT_DYNCREATE(CCostEngineerDoc, COleDocument)

BEGIN_MESSAGE_MAP(CCostEngineerDoc, COleDocument)
	// 启用默认的 OLE 容器实现
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, &COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, &COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, &COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &COleDocument::OnUpdateEditLinksMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_POPUP, &CCostEngineerDoc::OnUpdateObjectVerbPopup)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &COleDocument::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()


// CCostEngineerDoc 构造/析构

CCostEngineerDoc::CCostEngineerDoc() noexcept
{
	// 使用 OLE 复合文件
	EnableCompoundFile();

	// TODO: 在此添加一次性构造代码

}


BOOL CCostEngineerDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CCostEngineerDoc 序列化

void CCostEngineerDoc::Serialize(CArchive& ar)
{
	CString str;
	int num;
	double db;
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码

		// 梁
		ar << CString(CBeamObj::m_ObjectCode.c_str());
		ar << CBeamObj::m_ObjectVersion;
		ar << beams.size();
		for (int i = 0; i < beams.size(); i++) {
			beams[i].Serialize(ar, CBeamObj::m_ObjectVersion);
		}

		// 柱
		ar << CString(CColumnObj::m_ObjectCode.c_str());
		ar << CColumnObj::m_ObjectVersion;
		ar << columns.size();
		for (int i = 0; i < columns.size(); i++) {
			columns[i].Serialize(ar, CColumnObj::m_ObjectVersion);
		}

		// 类似工程预算法
		ar << CString(CSimilarEngineerBudget::m_ObjectCode.c_str());
		ar << CSimilarEngineerBudget::m_ObjectVersion;
		ar << similarEngineerBudgets.size();
		for (int i = 0; i < similarEngineerBudgets.size(); i++) {
			similarEngineerBudgets[i].Serialize(ar, CSimilarEngineerBudget::m_ObjectVersion);
		}

		// 概算指标法
		ar << CString(CBudgetIndex::m_ObjectCode.c_str());
		ar << CBudgetIndex::m_ObjectVersion;
		ar << budgetIndexs.size();
		for (int i = 0; i < budgetIndexs.size(); i++) {
			budgetIndexs[i].Serialize(ar, CBudgetIndex::m_ObjectVersion);
		}

		// 施工图预算法
		ar << CString(CBuildingBudget::m_ObjectCode.c_str());
		ar << CBuildingBudget::m_ObjectVersion;
		ar << budgetIndexs.size();
		for (int i = 0; i < buildingBudgets.size(); i++) {
			buildingBudgets[i].Serialize(ar, CBuildingBudget::m_ObjectVersion);
		}

		// 消耗量定额
		ar << CString(CConsumptionQuota::m_ObjectCode.c_str());
		ar << CConsumptionQuota::m_ObjectVersion;
		ar << consumptionQuotas.size();
		for (int i = 0; i < consumptionQuotas.size(); i++) {
			consumptionQuotas[i].Serialize(ar, CConsumptionQuota::m_ObjectVersion);
		}

		// 分部分项和单价项目
		ar << CString(CItemOfUnitPrice::m_ObjectCode.c_str());
		ar <<CItemOfUnitPrice::m_ObjectVersion;
		ar << compositeUnitPrices.size();
		for (int i = 0; i < compositeUnitPrices.size(); i++) {
			compositeUnitPrices[i].Serialize(ar,CItemOfUnitPrice::m_ObjectVersion);
		}

		// 总计措施项目
		ar << CString(CItemOfTotalPrice::m_ObjectCode.c_str());
		ar << CItemOfTotalPrice::m_ObjectVersion;
		ar << itemOfTotalPrices.size();
		for (int i = 0; i < itemOfTotalPrices.size(); i++) {
			itemOfTotalPrices[i].Serialize(ar, CItemOfTotalPrice::m_ObjectVersion);
		}

		// 决策树
		ar << CString(CDecisionTree::m_ObjectCode.c_str());
		ar << CDecisionTree::m_ObjectVersion;
		ar << decisionTrees.size();
		for (int i = 0; i < decisionTrees.size(); i++) {
			decisionTrees[i].Serialize(ar, CDecisionTree::m_ObjectVersion);
		}

		// 双代号网络图
		ar << CString(CActivityOnArrow::m_ObjectCode.c_str());
		ar << CActivityOnArrow::m_ObjectVersion;
		ar << activityOnArrows.size();
		for (int i = 0; i < activityOnArrows.size(); i++) {
			activityOnArrows[i].Serialize(ar, CActivityOnArrow::m_ObjectVersion);
		}

		// 现金流量表
		ar << CString(CCashFlow::m_ObjectCode.c_str());
		ar << CCashFlow::m_ObjectVersion;
		ar << cashFlows.size();
		for (int i = 0; i < cashFlows.size(); i++) {
			cashFlows[i].Serialize(ar, CCashFlow::m_ObjectVersion);
		}

		// 投标 - 综合评估法
		ar << CString(CIntegratedEvaluation::m_ObjectCode.c_str());
		ar << CIntegratedEvaluation::m_ObjectVersion;
		ar << integratedEvaluations.size();
		for (int i = 0; i < integratedEvaluations.size(); i++) {
			CIntegratedEvaluation::Serialize(ar, CIntegratedEvaluation::m_ObjectVersion, integratedEvaluations[i]);
		}

		// 工程结算
		ar << CString(CProjectSettlement::m_ObjectCode.c_str());
		ar << CProjectSettlement::m_ObjectVersion;
		ar << projectSettlements.size();
		for (int i = 0; i < projectSettlements.size(); i++) {
			CProjectSettlement::Serialize(ar, CProjectSettlement::m_ObjectVersion, projectSettlements[i]);
		}

		// 文件结束标志
		ar << CString("eof");
	}
	else
	{
		// TODO: 在此添加加载代码

		while (true) {
			ar >> str;
			if (str == "eof") break;
			ar >> db;
			ar >> num;
			for (int i = 0; i < num; i++) {
				// 梁
				if (str == CBeamObj::m_ObjectCode.c_str()) {
					CBeamObj b;
					b.Serialize(ar, db);
					beams.push_back(b);
				}
				// 柱
				if (str == CColumnObj::m_ObjectCode.c_str()) {
					CColumnObj b;
					b.Serialize(ar, db);
					columns.push_back(b);
				}
				// 类似工程预算法
				if (str == CSimilarEngineerBudget::m_ObjectCode.c_str()) {
					CSimilarEngineerBudget b;
					b.Serialize(ar, db);
					similarEngineerBudgets.push_back(b);
				}
				// 概算指标法
				if (str == CBudgetIndex::m_ObjectCode.c_str()) {
					CBudgetIndex b;
					b.Serialize(ar, db);
					budgetIndexs.push_back(b);
				}
				// 施工图预算法
				if (str == CBuildingBudget::m_ObjectCode.c_str()) {
					CBuildingBudget b;
					b.Serialize(ar, db);
					buildingBudgets.push_back(b);
				}
				// 消耗量定额
				if (str == CConsumptionQuota::m_ObjectCode.c_str()) {
					CConsumptionQuota b;
					b.Serialize(ar, db);
					consumptionQuotas.push_back(b);
				}
				// 分部分项和单价项目
				if (str ==CItemOfUnitPrice::m_ObjectCode.c_str()) {
					CItemOfUnitPrice b;
					b.Serialize(ar, db);
					compositeUnitPrices.push_back(b);
				}
				// 总计措施项目
				if (str == CItemOfTotalPrice::m_ObjectCode.c_str()) {
					CItemOfTotalPrice b;
					b.Serialize(ar, db);
					itemOfTotalPrices.push_back(b);
				}
				// 决策树
				if (str == CDecisionTree::m_ObjectCode.c_str()) {
					CDecisionTree b;
					b.Serialize(ar, db);
					decisionTrees.push_back(b);
				}
				// 双代号网络图
				if (str == CActivityOnArrow::m_ObjectCode.c_str()) {
					CActivityOnArrow b;
					b.Serialize(ar, db);
					activityOnArrows.push_back(b);
				}
				// 现金流量表
				if (str == CCashFlow::m_ObjectCode.c_str()) {
					CCashFlow b;
					b.Serialize(ar, db);
					cashFlows.push_back(b);
				}
				// 投标 - 综合评估法
				if (str == CIntegratedEvaluation::m_ObjectCode.c_str()) {
					CIntegratedEvaluation * p;
					CIntegratedEvaluation::Serialize(ar, db, p);
					integratedEvaluations.push_back(p);
				}
				// 工程结算
				if (str == CProjectSettlement::m_ObjectCode.c_str()) {
					CProjectSettlement *p;
					CProjectSettlement::Serialize(ar, db, p);
					projectSettlements.push_back(p);
				}
			}
		}
	}

	// 调用基类 COleDocument 会启用序列化
	//  容器文档的 COleClientItem 对象的序列化。
	COleDocument::Serialize(ar);
}


#ifdef SHARED_HANDLERS

// 缩略图的支持
void CCostEngineerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CCostEngineerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CCostEngineerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCostEngineerDoc 诊断

#ifdef _DEBUG
void CCostEngineerDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CCostEngineerDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG


// CCostEngineerDoc 命令
