#pragma once

#define	OBV_SMOOTHING_DAY 12


class CHushenStockIndexData
{
public:
	string	tradedate;
	string	real_date;			//	流通股变动日
	double	circulating;		//	流通股数
	double	circulating_value;
	double	repair_value;
	double	change_rate;
	double	close_index;		/* 交易日的复权价 */

	vector<string>	vecParent;	//	所属板块 ID

	//	本日发生的事件 
	string	remark;
	double  close;

	CHushenStockIndexData()
	{
		tradedate = "";
		circulating_value = 0;
		repair_value = 0;
		change_rate = 0;
		close_index = 0;
		close = 0;
		circulating = 0;

		remark = "";
	}
};


class CHushenStockIndex : public CAbstractIndex
{
public:
	CHushenStockIndex(bool bAnalysisUsed = false);
	~CHushenStockIndex(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "Hushen 沪深股票指数指标"; }

	void ExportFromDatabase(string strCode, double startTime = 0, double endTime = 0);

	int TradedateNumber(string a_time_to_market, string strTradedate);

	//	key 为股票代码
	map<string, CHushenStockIndexData>		m_mapIndex;

	//	key 为板块ID
	map<string, CStockPlateData>			m_mapPlate;

	//	保存 ExportFromDatabase 的结果, 按照交易日排序
	vector<CHushenStockIndexData>		m_vec_index;

	//	存放上市交易日
	vector<string>			m_vecTradedate;

	//	存放上市公司的名称
	map<string, string>		m_mapAbbreviation;

};
