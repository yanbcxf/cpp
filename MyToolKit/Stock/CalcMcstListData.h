#pragma once

class CCalcMcstListData :public CCalculateTask
{
public:
	CCalcMcstListData(HWND hwnd, int logType);
	~CCalcMcstListData(void);

	string ClassName(){ return "CCalcMcstListData"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_trade_date;
	vector<double>	m_vec_turnover;
	vector<double>	m_vec_turnover_rate;
	vector<double>	m_vec_price;
	vector<double>	m_vec_change_rate;
	vector<double>	m_vec_mcst_price;
	vector<double>	m_vec_mcst_price_change_rate;
	vector<double>	m_vec_mcst_percent;

};
