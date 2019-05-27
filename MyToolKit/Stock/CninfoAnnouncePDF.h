#pragma once
#include "stockdatamodel.h"




class CCninfoAnnouncePDF :
	public CStockDataModel
{
public:
	CCninfoAnnouncePDF(void);
	~CCninfoAnnouncePDF(void);

public:
	
	string ModelType(){ return  "CCninfoAnnouncePDF"  /* 巨潮网公告 PDF 文件分析 */; }

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
	string					announcementId;
	string					announcementTitle;
	string					adjunctUrl;
	
	map<int, CPdfCatalog>	mapCatalog;
	
	
};
