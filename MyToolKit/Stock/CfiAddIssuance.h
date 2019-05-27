#pragma once
#include "stockdatamodel.h"

class CCfiAddIssuance : public CStockDataModel
{
public:
	CCfiAddIssuance(void);
	~CCfiAddIssuance(void);

	string ModelType(){ return  "CCfiAddIssuance" /*"中财网股份增发"*/; }

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

public:
	vector<string>	m_vec_placement_year;
	vector<double>	m_vec_placement_ratio;
	vector<double>	m_vec_placement_price;
	vector<string>	m_vec_plan_announcement_date;
	vector<string>	m_vec_placement_object;
	vector<string>	m_vec_right_record_date;
	vector<string>	m_vec_exit_right_date;
	vector<string>	m_vec_market_date;
};
