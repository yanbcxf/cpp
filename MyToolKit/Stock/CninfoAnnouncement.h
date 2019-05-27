#pragma once
#include "stockdatamodel.h"




class CCninfoAnnouncement :
	public CStockDataModel
{
public:
	CCninfoAnnouncement(void);
	~CCninfoAnnouncement(void);

public:
	
	string ModelType(){ return  "CCninfoAnnouncement"  /* 巨潮网公告信息 */; }

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
	vector<string>	orgId;
	vector<string>	announcementId;
	vector<string>	announcementTitle;
	vector<string>	announcementTime;
	vector<string>	adjunctUrl;
	vector<string>	adjunctSize;
	vector<string>	adjunctType;
	vector<string>	storageTime;
	vector<string>	columnId;
	vector<string>	announcementType;
	vector<string>	pageColumn;
	vector<string>	announcementTypeName;


public:
	static map<string, string>	map_plate;
	static map<string, string>	map_category;
	static map<string, string>	map_trade;

	string	m_orgId;

private: 
	vector<CCninfoAnnouncePDF>  vecPDF;
	
};
