#pragma once

 /*计算被大盘指数涨跌幅 修正后的个股总市值*/
class CCalcMarketValue : public CCalculateTask
{
public:
	CCalcMarketValue(HWND hwnd, int logType);
	~CCalcMarketValue(void);

	string ClassName(){ return "CCalcMarketValue"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"公司代码",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_trade_date;
	vector<double>	m_vec_market_index;
	vector<double>	m_vec_stock_index;
	vector<double>	m_vec_circulating_value;
	vector<double>	m_vec_total_value;
	vector<double>	m_vec_corrected_stock_index;

};
