#pragma once

class CCalcSohuIndexDaily : public CCalculateTask
{
public:
	CCalcSohuIndexDaily(HWND hwnd, int logType);
	~CCalcSohuIndexDaily(void);

	string ClassName(){ return "CCalcSohuIndexDaily"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_trade_date;
	vector<double>	m_vec_close;
	vector<double>	m_vec_turnover;
	vector<int>	m_vec_volume;
};
