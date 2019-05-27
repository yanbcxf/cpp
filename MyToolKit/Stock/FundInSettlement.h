#pragma once
#include "stockdatamodel.h"

class CFundInSettlement : public CStockDataModel
{
public:
	CFundInSettlement(void);
	~CFundInSettlement(void);

	string ModelType(){ return  "CFundInSettlement" /* 中国证券投资者保护基金下的每周交易结算基金 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow();

	
public:

	static map<string, string>	m_month_url;

	vector<string>	m_vec_week;
	vector<string>	m_vec_fund_type;
	vector<double>	m_vec_final_fund;
	vector<double>	m_vec_average_fund;
	vector<double>	m_vec_input_fund;
	vector<double>	m_vec_output_fund;
	vector<double>	m_vec_net_fund;
	vector<string>	m_vec_remark;
};
