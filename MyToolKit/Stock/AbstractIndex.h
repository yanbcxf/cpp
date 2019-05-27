#pragma once

class CTechnicalAnalysis;


class CTechnicalAnalysisData
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

	double	circulating_value;
	double	lclose;

	//	暂停上市、退市日期
	bool	is_exit_from_market;

	//	本日为除权日时， 转增、红股、分红等数据
	double	bonus_share;
	double	reserve_to_common_share;
	double	bonus_cash;

	//	股本结构
	double	circulating;
	string	change_reason_desc;
	string	real_date;

	//	上市日期
	string	a_time_to_market;

	CTechnicalAnalysisData()
	{
		close = -1;
		lclose = -1;

		bonus_cash = -1;
		reserve_to_common_share = -1;
		bonus_share = -1;

		circulating = 0;

		is_exit_from_market = false;
		change_reason_desc = "";

		a_time_to_market = "";
	}

};

class CAbstractIndex
{
public:
	CAbstractIndex(bool bAnalysisUsed);
	~CAbstractIndex(void);

	//	开始计算指标，读入已有指标，返回最晚的已有指标日期
	virtual void   BeginCalculate() = 0;
	
	//	开始某个交易日的计算
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate) = 0;

	//	
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate) = 0;

	//	结束某个交易日的计算
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate) = 0;

	//	结束指标计算，将计算完成的指标保存数据库
	virtual void   EndCalculate() = 0;

	virtual string IndexName() = 0;

	//	前复权价，将以前的价格按照 转增及分红 进行转换
	double  RecoverPrice(double price, CTechnicalAnalysisData tad, bool subCash = true);

	double  RecoverVolume(double volume, CTechnicalAnalysisData tad);

public:
	//	上次指标计算的 截止交易日
	string	m_strLatest;

};
