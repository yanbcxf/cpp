#pragma once

class CCalcCsdcInvestorStat : public CCalculateTask
{
public:
	CCalcCsdcInvestorStat(HWND hwnd, int logType);
	~CCalcCsdcInvestorStat(void);

	string ClassName(){ return "CCalcCsdcInvestorStat"; }

	void Execute();
	static void Execute1(void* firstArg);

public:

	vector<string>	begin_date;
	vector<string>	end_date;

	vector<double>	final_a_natural_investor;
	vector<double>	final_a_legal_investor;
	vector<double>	final_a_position_investor;
	vector<double>	period_a_trading_investor;
	
};
