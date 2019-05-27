#pragma once
#include "stockdatamodel.h"




class CCninfoStockInfo :
	public CStockDataModel
{
public:
	CCninfoStockInfo(void);
	~CCninfoStockInfo(void);

public:
	
	string ModelType(){ return  "CCninfoStockInfo"  /* 巨潮网公告信息 */; }

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
	vector<string>	orgId;
	vector<string>	category;
	vector<string>	code;
	vector<string>	pinyin;
	vector<string>	zwjc;

};
