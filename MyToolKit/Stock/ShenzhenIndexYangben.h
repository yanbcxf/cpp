#pragma once
#include "stockdatamodel.h"

class CShenzhenIndexYangben : public CStockDataModel
{
public:
	CShenzhenIndexYangben(void);
	~CShenzhenIndexYangben(void);

public:
	string ModelType(){ return  "CShenzhenIndexYangben" /* 深证股指 样本股 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void){};
	void ExportFromDatabase(double startTime = 0, double endTime = 0){};

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript(){ return ""; };

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow(){};

	bool ParseXls(string  xlsfilename);

public:
	vector<string>		m_vec_code;
	vector<string>		m_vec_name;
	vector<long long>		m_vec_total_shares;
	vector<long long>		m_vec_circulating;
	vector<string>		m_vec_industry;
};
