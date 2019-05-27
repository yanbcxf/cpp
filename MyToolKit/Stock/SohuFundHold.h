#pragma once
#include "stockdatamodel.h"

//	股票的基金持有状况

class CSohuFundHold :
	public CStockDataModel
{
public:
	CSohuFundHold(void);
	~CSohuFundHold(void);

public:
	map<int,string>	m_vec_report_date;		//	报告期
	map<int,int>	m_vec_fund_code;		//	基金代码
	map<int,string>	m_vec_fund_name;		//	基金简称
	map<int,double>	m_vec_value_hold;		//	持仓市值(元)
	map<int,double>	m_vec_number_hold;		//	持仓数量(股)
	map<int,double>	m_vec_percent;			//	占基金净值比例(%)

	string ModelType(){ return  "CSohuFundHold" /*"搜狐 基金持有股票"*/; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	bool ParseCsvFileName(string szFileName);
	void ParseXmlElement(void){}
	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	CStockDataModel * NewCopy(){ return NULL; };

	string GetDownloadUrl(){return "" ; };
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName();
	void NotifyDatabaseImportedToWindow(){} ;
};
