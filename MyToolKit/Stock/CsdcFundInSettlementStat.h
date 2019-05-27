#pragma once
#include "stockdatamodel.h"

class CCsdcFundInSettlementStat: public CStockDataModel
{
public:
	CCsdcFundInSettlementStat(void);
	~CCsdcFundInSettlementStat(void);

	string ModelType(){ return  "CCsdcFundInSettlementStat" /* 中国结算的每周资金结算统计表 */; }

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
	vector<string>		m_vec_begin_date;
	vector<string>		m_vec_end_date;
};
