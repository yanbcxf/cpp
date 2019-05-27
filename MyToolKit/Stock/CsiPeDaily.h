#pragma once
#include "stockdatamodel.h"

class CCsiPeDaily : public CStockDataModel
{
public:
	CCsiPeDaily(void);
	~CCsiPeDaily(void);

	string ModelType(){ return  "CCsiPeDaily" /*""*/; }

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

	string LastTradeDateFromDatabase();
	bool Unzip(string zipfilename, string & xlsfilename);
	bool ParseXls(string  xlsfilename);

	CStockPlateTree * GenerateTree();

public:
	
	vector<int>		m_vec_code;			//  行业代码 或者 证券代码
	vector<string>	m_vec_name;			//	行业名称 或者 证券名称
	vector<int>		m_vec_is_stock;
	
	vector<int>		m_vec_company_number;
	vector<int>		m_vec_parent_code;

	vector<double>	m_vec_static_pe;
	vector<double>	m_vec_dynamic_pe;
	vector<double>	m_vec_pb;
	vector<double>	m_vec_dividend_yield_ratio;
};
