#pragma once
#include "stockdatamodel.h"




class CCninfoDelistingList :
	public CStockDataModel
{
public:
	CCninfoDelistingList(void);
	~CCninfoDelistingList(void);

public:
	
	string ModelType(){ return  "CCninfoDelistingList"  /* ¾Þ³±Íø ÍËÊÐ¹«¸æ */; }

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


private:
	string LastTradeDateFromDatabase();

public:
	vector<string>	secCode;
	vector<string>	secName;
	
	vector<string>	delistingTime;

};
