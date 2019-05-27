#pragma once
#include "stockdatamodel.h"

//	网易 个股资金流向

class CCapitalFlows163 : public CStockDataModel
{
public:
	CCapitalFlows163(void);
	~CCapitalFlows163(void);

public:
	map<int, string>	m_vec_report_date;
	map<int, double>	m_vec_close;
	map<int, double>	m_vec_change_rate;		//	涨跌幅
	map<int, double>	m_vec_turnover_rate;	//	换手率
	map<int, double>	m_vec_flow_in;
	map<int, double>	m_vec_flow_out;
	map<int, double>	m_vec_net_flow;
	map<int, double>	m_vec_main_flow_in;
	map<int, double>	m_vec_main_flow_out;
	map<int, double>	m_vec_main_net_flow;

	string ModelType(){ return  "CCapitalFlows163" /*"网易 个股资金流向"*/; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	bool ParseCsvFileName(string szFileName);
	void ParseXmlElement(void){}
	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	CStockDataModel * NewCopy(){ return NULL; };

	string GetDownloadUrl(){return "" ; };
	string SaveAsCsv();
	string SaveAsCsv_old();
	string SqlScript();

	string GetCsvFileName();
	void NotifyDatabaseImportedToWindow(){} ;

private:
	string LastTradeDateFromDatabase();
};
