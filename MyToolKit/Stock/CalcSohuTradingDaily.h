#pragma once

class CCalcSohuTradingDaily : public CCalculateTask
{
public:
	CCalcSohuTradingDaily(HWND hwnd, int logType);
	~CCalcSohuTradingDaily(void);

	string ClassName(){ return "CCalcSohuTradingDaily"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_trade_date;
	vector<double>	m_vec_turnover;
	vector<double>	m_vec_turnover_rate;
	vector<double>	m_vec_change_amount;
	vector<double>	m_vec_change_rate;
	vector<double>	m_vec_volume;
};
