
// StockDoc.h : CStockDoc 类的接口
//


#pragma once


class CStockDoc : public CDocument
{
protected: // 仅从序列化创建
public:
	CStockDoc();
	DECLARE_DYNCREATE(CStockDoc)

// 属性
public:
public:
	//	TradingDaily
	map<int, double>	m_vecTradeDate;
	map<int, double>	m_vecOpen;
	int		m_nRowNum;

	//	Shareholding
	map<int, double>	m_vecReportDateOfShareholding;
	map<int, double>	m_vecTotalShare;
	int		m_nRowNumOfShareholding;

	//	FinanceReport
	map<int, double>	m_vecReportDateOfFinanceReport;
	map<int, double>	m_vecNetProfit;
	int		m_nRowNumOfFinanceReport;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CStockDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()



};


