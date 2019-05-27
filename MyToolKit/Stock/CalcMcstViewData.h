#pragma once

class CCalcMcstViewData :public CCalculateTask
{
public:
	CCalcMcstViewData(HWND hwnd, int logType);
	~CCalcMcstViewData(void);

	string ClassName(){ return "CCalcMcstViewData"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<string>	m_vec_trade_date			;	//	"¹«Ë¾´úÂë",
	//vector<double>	m_vec_price;
	vector<double>	m_vec_cost_price;
	vector<double>	m_vec_mcst_price;
	
	int		m_nCode;
};
