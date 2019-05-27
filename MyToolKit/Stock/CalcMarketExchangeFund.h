#pragma once

class CCalcMarketExchangeFund : public CCalculateTask
{
public:
	CCalcMarketExchangeFund(HWND hwnd, int logType);
	~CCalcMarketExchangeFund(void);

	string ClassName(){ return "CCalcMarketExchangeFund"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	
	vector<string>	m_vec_begin_date;
	vector<string>	m_vec_end_date;
	vector<double>	m_vec_average_fund;

	vector<double>	m_vec_final_fund;
	vector<double>	m_vec_input_fund;
	vector<double>	m_vec_output_fund;
	vector<double>	m_vec_net_fund;
	vector<double>	m_vec_sum_volume;
	vector<double>	m_vec_sum_turnover;
	vector<double>	m_vec_begin_index;
	vector<double>	m_vec_end_index;
};
