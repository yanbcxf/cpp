#pragma once

class CCalcMacdViewData :public CCalculateTask
{
public:
	CCalcMacdViewData(HWND hwnd, int logType);
	~CCalcMacdViewData(void);

	string ClassName(){ return "CCalcMacdViewData"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<string>	m_vec_trade_date			;	//	"¹«Ë¾´úÂë",
	vector<double>	m_vec_total_value;
	vector<double>	m_vec_ema12;
	vector<double>	m_vec_ema26;
	vector<double>	m_vec_dif;
	vector<double>	m_vec_dea;

	int		m_nCode;
};
