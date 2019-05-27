#pragma once
#include "stockdatamodel.h"

class CSouhuStockPlates : public CStockDataModel
{
public:
	CSouhuStockPlates(void);
	~CSouhuStockPlates(void);


	string ModelType(){ return  "CSouhuStockPlate" /*"ËÑºü°å¿é·ÖÀà±í"*/; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	bool ParseCsvFileName(string szFileName);
	void ParseXmlElement(void);
	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	CStockDataModel * NewCopy();

	string GetDownloadUrl();
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName();
	void NotifyDatabaseImportedToWindow();

	CStockPlateTree * CSouhuStockPlates::GenerateTree();
	vector<CStockPlateData> FetchDataFromTree(CStockPlateTree * pPlates, int nPlateCode);
	
public:
	map<int, int>		m_vec_code;
	map<int, string>	m_vec_name;
	map<int, int>		m_vec_company_number;
	map<int, double>	m_vec_total_volume;
	map<int, double>	m_vec_total_turnover;
	map<int, int>		m_vec_parent_code;
	map<int, int>		m_vec_is_stock;

protected:
	string	m_strEnglishName;


};
