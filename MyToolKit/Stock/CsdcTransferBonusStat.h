#pragma once
#include "stockdatamodel.h"

class CCsdcTransferBonusStat: public CStockDataModel
{
public:
	CCsdcTransferBonusStat(void);
	~CCsdcTransferBonusStat(void);

	string ModelType(){ return  "CCsdcTransferBonusStat" /* 中国结算的每周过户及代发红利统计表 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }
	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow(){};
	string GetDownloadUrl(){ return ""; };
	void ExportFromDatabase(double startTime = 0, double endTime = 0){};

	void ImportToDatabase(void);
	string SaveAsCsv();
	string SqlScript();

public:
};
