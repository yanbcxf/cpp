#pragma once


//////////////////////////////////////////////////////////////////////////
//	该交易数据来源 Yahoo
class CTradingDaily : public CStockDataModel
{
public:
	CTradingDaily(void);
	~CTradingDaily(void);

public:
	map<int, string>	m_vec_report_date;
	map<int, double>	m_vec_open;
	map<int, double>	m_vec_high;
	map<int, double>	m_vec_low;
	map<int, double>	m_vec_close;
	map<int, double>	m_vec_volume;
	map<int, double>	m_vec_adj_close;

	string ModelType(){ return  "CTradingDaily" /*"每日交易报价表"*/; }

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

};
