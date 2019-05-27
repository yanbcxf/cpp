#pragma once
#include "stockdatamodel.h"

class CHexunResearchPaper : public CStockDataModel
{
public:
	CHexunResearchPaper(void);
	~CHexunResearchPaper(void);

	string ModelType(){ return  "CHexunResearchPaper" /*"和讯研究报告"*/; }

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
	vector<string>	m_vec_report_date;
	vector<string>	m_vec_title;
	vector<string>	m_vec_source;
	vector<string>	m_vec_grade;
	vector<string>	m_vec_abstract;
	vector<string>	m_vec_url;

private:
	string LastTradeDateFromDatabase();
};
