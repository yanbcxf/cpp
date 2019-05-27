#pragma once

class CCapitalStructure : public CStockDataModel
{
public:
	CCapitalStructure(void);
	~CCapitalStructure(void);

public:

	map<int, string>	m_vec_real_date;
	map<int, double>	m_vec_ashares;				//	流通的 A 股
	map<int, double>	m_vec_bshares;
	map<int, int>		m_vec_change_reason;
	map<int, string>	m_vec_change_reason_desc;
	map<int, int>		m_vec_seq;
	map<int, double>	m_vec_total_shares;
	map<int, double>	m_vec_a_total_shares;		//	所有的 A 股

	string ModelType(){ return "CCapitalStructure" /*"上海股本结构"*/; }

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

	double CurrentTotalShare(string curDate);
	double CurrentACalculateShare(string curDate);
};
