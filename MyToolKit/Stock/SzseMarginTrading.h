#pragma once
#include "stockdatamodel.h"

class CSzseMarginTrading  : public CStockDataModel
{
public:
	CSzseMarginTrading(void);
	~CSzseMarginTrading(void);

	string ModelType(){ return  "CSzseMarginTrading" /*""*/; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0){};

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow();

	bool Unzip(string zipfilename, string & xlsfilename);
	bool ParseXls(string  xlsfilename);

public:

	vector<int>		m_vec_code;			//  行业代码 或者 证券代码
	vector<string>	m_vec_name;			//	行业名称 或者 证券名称
	
	vector<double>	m_vec_financing_buying;		//	融资买入额
	vector<double>	m_vec_financing_balance;	//	融资余额
	vector<double>	m_vec_stock_loan_selling;	//	融券卖出量
	vector<double>	m_vec_stock_loan_remaider;	//	融券余量
	vector<double>	m_vec_stock_loan_balance;	//	融券余额
	vector<double>	m_vec_financing_and_stock_loan_balance;	//	融资融券余额
};
