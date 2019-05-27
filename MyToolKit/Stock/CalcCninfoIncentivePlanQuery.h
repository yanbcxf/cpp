#pragma once
#include "calculatetask.h"

class CCalcCninfoIncentivePlanQuery :
	public CCalculateTask
{
public:
	CCalcCninfoIncentivePlanQuery(HWND hwnd, int logType);
	~CCalcCninfoIncentivePlanQuery(void);

	string ClassName(){ return "CCalcCninfoIncentivePlanQuery"; }

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
	vector<string>	m_vec_content;
	vector<string>	m_vec_base_from_year;
	vector<string>	m_vec_base_to_year;
	vector<string>	m_vec_incentive_year;
	vector<string>	m_vec_net_profit;
	vector<string>	m_vec_sales_from_operations;
	vector<string>	m_vec_announcementId;
	vector<string>	m_vec_increase_percent;
	vector<string>	m_vec_url;

};
