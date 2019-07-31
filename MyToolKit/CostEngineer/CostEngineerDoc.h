
// CostEngineerDoc.h: CCostEngineerDoc 类的接口
//
#pragma once



class CCostEngineerDoc : public COleDocument
{
protected: // 仅从序列化创建
	CCostEngineerDoc() noexcept;
	DECLARE_DYNCREATE(CCostEngineerDoc)
	virtual ~CCostEngineerDoc() {
		for (CIntegratedEvaluation * p : integratedEvaluations) delete p;
		for (CProjectSettlement * p : projectSettlements) delete p;
	};

// 特性
public:
	vector<CBeamObj>	beams;
	vector<CColumnObj>	columns;
	vector<CSimilarEngineerBudget> similarEngineerBudgets;
	vector<CBudgetIndex> budgetIndexs;
	vector<CBuildingBudget>	buildingBudgets;
	vector<CConsumptionQuota> consumptionQuotas;
	vector<CItemOfUnitPrice> compositeUnitPrices;
	vector<CItemOfTotalPrice>	itemOfTotalPrices;
	vector<CDecisionTree> decisionTrees;
	vector<CActivityOnArrow> activityOnArrows;
	vector<CCashFlow> cashFlows;
	vector<CIntegratedEvaluation *> integratedEvaluations;
	vector<CProjectSettlement*> projectSettlements;
	vector<CContractPrice*> contractPrices;
	vector<CProjectSettlementEx5*> projectSettlementEx5s;
	vector<CFinanceAnalysis*> financeAnalysis;

// 操作
public:


// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
