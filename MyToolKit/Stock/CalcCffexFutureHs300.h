#pragma once

class CCalcCffexFutureHs300  : public CCalculateTask
{
public:
	CCalcCffexFutureHs300(HWND hwnd, int logType);
	~CCalcCffexFutureHs300(void);

	string ClassName(){ return "CCalcCffexFutureHs300"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<string>	m_vec_trade_date;
	vector<string>	m_vec_agreement_code;

	vector<double>	m_vec_settlement;
	vector<double>	m_vec_close;
	vector<double>  m_vec_hs300;
};
