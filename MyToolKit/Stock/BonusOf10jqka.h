#pragma once

#include "stockdatamodel.h"

class CBonusOf10jqka : public CStockDataModel
{
public:
	CBonusOf10jqka(void);
	~CBonusOf10jqka(void);


	string ModelType(){ return  "CBonusOf10jqka" /* 同花顺分红送转 */; }

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
	vector<string>	m_vec_bonus_year;
	vector<double>	m_vec_bonus_share;
	vector<double>	m_vec_reserve_to_common_share;
	vector<double>	m_vec_bonus_cash;
	vector<string>	m_vec_plan_announcement_date;
	vector<string>	m_vec_right_record_date;
	vector<string>	m_vec_exit_right_date;
	vector<string>	m_vec_event_state;
};
