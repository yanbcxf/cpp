#pragma once
#include "calculatetask.h"

class CCalcMainIncomeGrowth:
	public CCalculateTask
{
public:
	CCalcMainIncomeGrowth(HWND hwnd, int logType);
	~CCalcMainIncomeGrowth(void);

	string ClassName(){ return "CCalcMainIncomeGrowth"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	
	vector<int>		m_vec_code			;			//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<string>	m_vec_composition_name;
	vector<double>	m_vec_income;
	vector<double>	m_vec_proportion_of_total_income;
};
