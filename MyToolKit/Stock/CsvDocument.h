#pragma once

// CCsvDocument 文档

class CCsvDocument : public CDocument
{
	DECLARE_DYNCREATE(CCsvDocument)

public:
	CCsvDocument();
	virtual ~CCsvDocument();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCsvOpen();

public:
	vector<string>  m_arrayHeader;
	vector<vector<string>>  m_arrayData;

	string	m_strCsvType;		//	“资产负债表”，“现金流量表”,

	vector<CStockDataModel *> m_vecStockDataModel; // 保存支持导入的 CSV 文件集合

	int	 m_nCurrStockData;
	
	afx_msg void OnCsvImport();
	bool CheckProfitsReport(void);
};
