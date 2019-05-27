#pragma once

#include "stockdatamodel.h"

class CSohuPerformanceForecast :  public CStockDataModel
{
public:
	CSohuPerformanceForecast(void);
	~CSohuPerformanceForecast(void);


	string ModelType(){ return  "CSohuPerformanceForecast" /*"ËÑºü ¸ö¹ÉÒµ¼¨Ô¤¸æ"*/; }

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

	double NetProfitBelongingToParentCompany(string currDate, string & baseDate);

private:
	string LastTradeDateFromDatabase();

public:
	map<int, string>	m_vec_report_date;
	map<int, string>	m_vec_report_type;
	map<int, double>	m_vec_change_rate;		//	ÕÇµø·ù
};
