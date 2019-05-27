#pragma once
#include "stockdatamodel.h"

class CCffexFutureDaily  : public CStockDataModel
{
public:
	CCffexFutureDaily(void);
	~CCffexFutureDaily(void);

	string ModelType(){ return  "CCffexFutureDaily" /* 中国金融期货交易所的 每日行情 */; }

	int  CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){ return 0;};
	void ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData){};
	bool ParseCsvFileName(string szFileName){ return true; };
	void ParseXmlElement(void){};
	CStockDataModel * NewCopy() { return NULL; }

	void ImportToDatabase(void);
	void ExportFromDatabase(double startTime = 0, double endTime = 0);

	string GetDownloadUrl(){ return ""; };
	string SaveAsCsv();
	string SqlScript();

	string GetCsvFileName(){ return ""; };
	void NotifyDatabaseImportedToWindow();

	vector<string>	NeedDownload();


public:
	vector<string>	m_vec_agreement_code;		// 合约代码
	vector<double>	m_vec_open;					// 开盘价
	vector<double>	m_vec_high;					// 最高价
	vector<double>	m_vec_low;					// 最低价
	vector<int>		m_vec_volume;				// 成交量
	vector<double>	m_vec_turnover;				// 成交金额
	vector<int>		m_vec_holding;				// 持仓量
	vector<double>	m_vec_close;				// 收盘价
	vector<double>	m_vec_settlement;			// 结算价
	vector<double>	m_vec_change1;				// 涨跌1
	vector<double>	m_vec_change2;				// 涨跌2

	string	m_request_trade_date;				//	请求下载的交易日期
};
