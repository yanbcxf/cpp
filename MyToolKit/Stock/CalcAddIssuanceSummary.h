#pragma once

class CCalcAddIssuanceSummary :public CCalculateTask
{
public:
	CCalcAddIssuanceSummary(HWND hwnd, int logType);
	~CCalcAddIssuanceSummary(void);

	string ClassName(){ return "CCalcAddIssuanceSummary"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"公司代码",
	vector<string>	m_vec_abbreviation;
	vector<double>	m_vec_placement_price;
	vector<string>	m_vec_market_date;
	vector<double>	m_vec_add_issuance_price;
	vector<double>	m_vec_recent_close;
	vector<double>	m_vec_change_rate;
	vector<double>	m_vec_market_close;		//	增发上市日的收盘价
};
