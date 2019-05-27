#pragma once

class CCalcLotteryNumberEnum : public CCalculateTask
{
public:
	CCalcLotteryNumberEnum(HWND hwnd, int logType);
	~CCalcLotteryNumberEnum(void);

	string ClassName(){ return "CCalcLotteryNumberEnum"; }

	void Execute();
	static void Execute1(void* firstArg);
	string SqlScript();
	void EnumSpecialNumber(LotteryNumber  lo);
	void ImportToDatabase(void);
	bool NeedGenerate(void);

public:
	
	string	m_lottery_sql;
	string	m_file_path;

	int		m_max_1, m_max_2, m_max_3, m_max_4, m_max_5;

	vector<LotteryNumber>	m_vec_lottery_number;

	// 生成过滤条件
	/*map<int, int>	m_tail_1, m_tail_2, m_tail_3, m_tail_4, m_tail_5, m_tail_6;
	map<int, int>	m_consecutive_1, m_consecutive_2, m_consecutive_3, m_consecutive_4, m_consecutive_5, m_consecutive_6;
	map<int, double>	m_fred_start, m_fred_end;
	vector<double>	m_fred_alpha, m_fred_beta, m_fred_upsilon, m_fred_theta, m_fred_eta;*/

	CLotteryDoc m_lottery_doc;

};
