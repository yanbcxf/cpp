#pragma once
#include "calculatetask.h"

class CCalcHexunResearchPaperQuery :
	public CCalculateTask
{
public:
	CCalcHexunResearchPaperQuery(HWND hwnd, int logType);
	~CCalcHexunResearchPaperQuery(void);

	string ClassName(){ return "CCalcHexunResearchPaperQuery"; }

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
	vector<string>	m_vec_title;
	vector<string>	m_vec_source;
	vector<string>	m_vec_grade;
	vector<string>	m_vec_url;
	vector<string>	m_vec_abstract;

	vector<int>		m_vec_initial_year;
	vector<double>	m_vec_first_eps;
	vector<double>	m_vec_second_eps;
	vector<double>	m_vec_third_eps;
	vector<double>	m_vec_recent_close;

	vector<double>	m_vec_today_pe;

};
