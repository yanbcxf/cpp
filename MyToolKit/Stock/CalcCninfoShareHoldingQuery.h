#pragma once
#include "calculatetask.h"

class CCalcCninfoShareHoldingQuery :
	public CCalculateTask
{
public:
	CCalcCninfoShareHoldingQuery(HWND hwnd, int logType);
	~CCalcCninfoShareHoldingQuery(void);

	string ClassName(){ return "CCalcCninfoShareHoldingQuery"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	string			m_after_report_date;
	string			m_request_like;

	int				m_max_page;
	int				m_page_no;

	vector<string>	m_vec_announcementId;
	vector<string>	m_vec_holder_num;

	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<string>	m_vec_url;

	vector<string>	m_vec_share_holder;

	vector<long>	m_vec_number_of_holding;
	vector<double>	m_vec_share_percent;
	vector<string>	m_vec_change_percent;
	vector<long>	m_vec_number_of_restrict;
	vector<string>	m_vec_holder_character;
	vector<string>	m_vec_year_quarter;

};
