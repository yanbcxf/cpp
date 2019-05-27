#pragma once
#include "calculatetask.h"

class CCalcMacdQuery :
	public CCalculateTask
{
public:
	CCalcMacdQuery(HWND hwnd, int logType);
	~CCalcMacdQuery(void);

	string ClassName(){ return "CCalcMacdQuery"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	string			m_after_report_date;
	string			m_request_like;

	int				m_max_page;
	int				m_page_no;

	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<string>	m_vec_basic_desc;
	vector<string>	m_vec_extra_desc;

	vector<double>	m_vec_percent;
	

};
