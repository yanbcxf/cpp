#pragma once
#include "stockdatamodel.h"


class CSinaDealDetail  : public CStockDataModel
{
public:
	CSinaDealDetail(void);
	~CSinaDealDetail(void);

	string ModelType(){ return  "CSinaDealDetail" /* 新浪网的每日所有股票的成交明细 */; }

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

	vector<int>	NeedDownload();


public:
	vector<string>	m_vec_deal_time;			// 成交时间
	vector<string>	m_vec_deal_type;			// 性质(买盘/卖盘)
	vector<double>	m_vec_deal_price;			// 成交价
	vector<long>	m_vec_deal_volume;			// 成交量(手)
	vector<double>	m_vec_deal_turnover;		// 成交额(元)

	string	m_request_trade_date;				//	请求下载的交易日期
};
