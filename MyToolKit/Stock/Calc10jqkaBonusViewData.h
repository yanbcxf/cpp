#pragma once

class CCalc10jqkaBonusViewData : public CCalculateTask
{
public:
	CCalc10jqkaBonusViewData(HWND hwnd, int logType);
	~CCalc10jqkaBonusViewData(void);

	string ClassName(){ return "CCalc10jqkaBonusViewData"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<string>	m_vec_right_record_date			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_exit_right_date;
	//vector<double>	m_vec_price;
	vector<double>	m_vec_bonus_cash;
	vector<double>	m_vec_bonus_share;
	vector<double>	m_vec_reserve_to_common_share;

	int		m_nCode;
};
