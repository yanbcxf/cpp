#pragma once
#include "stockdatamodel.h"

class CShenzhenIndexDaily : public CStockDataModel
{
public:
	CShenzhenIndexDaily(void);
	~CShenzhenIndexDaily(void);

public:
	string ModelType(){ return  "CShenzhenIndexDaily" /* 深证股指  */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0){};

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript(){ return ""; };

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow(){};

	bool ParseXls(string  xlsfilename);

public:
	vector<string>		m_vec_tradedate;
	vector<string>		m_vec_code;
	vector<string>		m_vec_name;
	vector<double>		m_vec_lclose;
	vector<double>		m_vec_close;
	vector<double>		m_vec_change_rate;
	vector<double>		m_vec_turnover;
};
