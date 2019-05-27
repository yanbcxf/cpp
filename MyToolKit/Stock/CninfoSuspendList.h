#pragma once
#include "stockdatamodel.h"




class CCninfoSuspendList :
	public CStockDataModel
{
public:
	CCninfoSuspendList(void);
	~CCninfoSuspendList(void);

public:
	
	string ModelType(){ return  "CCninfoSuspendList"  /* 巨潮网 暂停上市公告 */; }

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
	
	vector<string>	suspendTime;

};
