#pragma once

class CSohuMainIncomeComposition  : public CStockDataModel
{
public:
	CSohuMainIncomeComposition(void);
	~CSohuMainIncomeComposition(void);

public:
	map<int,string>	m_vec_report_date;
	map<int,string>	m_vec_composition_type;					//	构成
	map<int,string>	m_vec_composition_name;
	map<int,double>	m_vec_proportion_of_total_income;		//	占总收入比例
	map<int,double>	m_vec_income;
	map<int,double>	m_vec_income_change_year_on_year;		//	同比变化
	map<int,double>	m_vec_cost;								//	成本
	map<int,double>	m_vec_gross_profit_rate;				//	毛利率
	map<int,double>	m_vec_gross_profit_rate_change_year_on_year;	//	同比变化
	

	string ModelType(){ return  "CSohuMainIncomeComposition" /*"搜狐 主要收入构成"*/; }

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
