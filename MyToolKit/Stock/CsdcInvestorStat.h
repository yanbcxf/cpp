#pragma once
#include "stockdatamodel.h"

class CCsdcInvestorStat: public CStockDataModel
{
public:
	CCsdcInvestorStat(void);
	~CCsdcInvestorStat(void);

	string ModelType(){ return  "CCsdcInvestorStat" /* 中国结算的投资者情况统计表 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }
	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow(){};
	string GetDownloadUrl(){ return ""; };
	void ExportFromDatabase(double startTime = 0, double endTime = 0){};

	void ImportToDatabase(void);
	string SaveAsCsv();
	string SqlScript();


public:
	vector<string> m_vec_begin_date;
	vector<string> m_vec_end_date;
	vector<double> m_vec_final_a_natural_investor;
	vector<double> m_vec_final_a_legal_investor;
	vector<double> m_vec_final_b_natural_investor;
	vector<double> m_vec_final_b_legal_investor;
	vector<double> m_vec_final_a_position_investor;
	vector<double> m_vec_final_b_position_investor;
	vector<double> m_vec_period_a_trading_investor;
	vector<double> m_vec_period_b_trading_investor;
};
