#pragma once

class CCalcLotteryNumberList : public CCalculateTask
{
public:
	CCalcLotteryNumberList(HWND hwnd, int logType);
	~CCalcLotteryNumberList(void);

	string ClassName(){ return "CCalcLotteryNumberList"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	LotteryInfo	m_lottery_info;
	int		m_start_number;
	int		m_end_number;
	bool	m_bContainSpecial;
	vector<int> m_region_start, m_region_end;
	double  m_alpha, m_beta, m_upsilon, m_theta, m_eta;

	vector<LotteryNumber>	m_vec_lottery_number;
};
