#pragma once
#include "calculatetask.h"

class CCalcCninfoShareHoldingStatistics :
	public CCalculateTask
{
public:
	CCalcCninfoShareHoldingStatistics(HWND hwnd, int logType);
	~CCalcCninfoShareHoldingStatistics(void);

	string ClassName(){ return "CCalcCninfoShareHoldingStatistics"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	string			m_after_report_date;
	string			m_request_like;

	int				m_max_page;
	int				m_page_no;

	vector<string>	m_vec_share_holder;
	vector<long>	m_vec_company_number;
	vector<string>	m_vec_holder_character;
	vector<string>	m_vec_year_quarter;

};
