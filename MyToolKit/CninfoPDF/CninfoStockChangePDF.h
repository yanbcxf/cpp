#pragma once


class CCninfoStockChangePDF 
{
public:
	CCninfoStockChangePDF(void);
	~CCninfoStockChangePDF(void);

public:
	
	string ModelType(){ return  "CCninfoStockChangePDF"  /* 巨潮网公告 股权变动 PDF 文件分析 */; }


	string SaveAsCsv();
	string SqlScript();

	void ImportToDatabase();


public:

	HWND		m_hWnd;
	int			m_nLogType;

	string					announcementId;
	string					announcementTitle;
	string					adjunctUrl;

	string					secCode;
	string					secName;
	string					announcementTime;
	map<int, CPdfCatalog>	mapCatalog;
	
	
};
