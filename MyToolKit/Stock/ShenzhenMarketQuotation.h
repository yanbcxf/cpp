#pragma once

#include "stockdatamodel.h"

class CShenzhenMarketQuotation  :
	public CStockDataModel
{
public:
	CShenzhenMarketQuotation(void);
	~CShenzhenMarketQuotation(void);

public:
	map<int, string>	m_vec_report_date;				//	当 ExportFromDatabaseByCode 时使用

	string				m_str_report_date;
	map<int, int>		m_vec_code;
	map<int, string>	m_vec_name;						//	证券简称
	map<int, double>	m_vec_yesterday_close;			//	昨收
	map<int, double>	m_vec_today_close;				//	今收
	map<int, double>	m_vec_up_down;					//	升跌(%)
	map<int, double>	m_vec_turnover_amount;			//	成交金额(元)
	map<int, double>	m_vec_price_earnings_ratio;		//	市盈率

	string ModelType(){ return "CShenzhenMarketQuotation" /*"深圳所有股票每日行情表"*/; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);
	bool ParseCsvFileName(string szFileName);
	void ParseXmlElement(void);
	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	void ExportFromDatabaseByCode();

	CStockDataModel * NewCopy();

	string GetDownloadUrl();
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName();
};
