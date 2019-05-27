#pragma once


class CCninfoAnnouncePDF 
{
public:
	CCninfoAnnouncePDF(void);
	virtual ~CCninfoAnnouncePDF(void);

public:
	
	string ModelType(){ return  "CCninfoAnnouncePDF"  /* 巨潮网公告 PDF 文件分析 */; }


	virtual string SaveAsCsv() = 0;
	virtual string SqlScript() = 0;

	virtual void ImportToDatabase(string csv) = 0;
	virtual list<CCninfoAnnouncePDF *> CreatePDF() = 0;
	void SaveAnalysisToDatabase(vector<CLineOrCell>  pdf);

public:

	HWND		m_hWnd;
	int			m_nLogType;

	string					announcementId;
	string					announcementTitle;
	string					adjunctUrl;

	string					secCode;
	string					secName;
	string					announcementTime;
	
	
	
};
