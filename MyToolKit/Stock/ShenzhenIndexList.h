#pragma once
#include "stockdatamodel.h"

class CShenzhenIndexList : public CStockDataModel
{
public:
	CShenzhenIndexList(void);
	~CShenzhenIndexList(void);

public:
	string ModelType(){ return  "CShenzhenIndexList" /* 深证股指列表  */; }

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
	void GenerateTree();

public:
	vector<string>		m_vec_code;
	vector<string>		m_vec_name;
	vector<string>		m_vec_base_date;
	vector<double>		m_vec_base_point;
	vector<string>		m_vec_initial_date;

	CStockPlateTree *	m_pTree;

};
