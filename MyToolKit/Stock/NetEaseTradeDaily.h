#pragma once
#include "stockdatamodel.h"


class CNetEaseTradeDailyData
{
public:
	double  open;
	double	high;
	double	low;
	double	close;
	double	volume;
	double	change;
	double	change_rate;
	double	turnover;
	double	turnover_rate;

	double	total_value;
	double	circulating_value;
	double  lclose;
};

class CNetEaseTradeDaily :
	public CStockDataModel
{
public:
	CNetEaseTradeDaily(void);
	~CNetEaseTradeDaily(void);

public:
	vector<string>	m_vec_report_date;
	vector<double>	m_vec_open;
	vector<double>	m_vec_high;
	vector<double>	m_vec_low;
	vector<double>	m_vec_close;
	vector<double>	m_vec_volume;		//	成交量（手）
	
	vector<double>	m_vec_change;			//	涨跌额
	vector<double>	m_vec_change_rate;		//	涨跌幅
	vector<double>	m_vec_turnover;			//	成交额
	vector<double>	m_vec_turnover_rate;	//	换手率

	vector<double>	m_vec_lclose;			//	前收盘价
	vector<double>	m_vec_total_value;		//	总市值
	vector<double>	m_vec_circulating_value;//	流通市值

	map<string, CNetEaseTradeDailyData>	m_vec_reorganize;
	
	string ModelType(){ return  "CNetEaseTradeDailyData" /*"每日交易报价表"*/; }

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

	//	将读入的数据重新组织，并纺织到 m_vec_reorganize
	void Reorganize();

private:
	string LastTradeDateFromDatabase();
};
