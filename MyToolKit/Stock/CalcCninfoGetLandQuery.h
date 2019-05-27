#pragma once
#include "calculatetask.h"

class CCalcCninfoGetLandQuery :
	public CCalculateTask
{
public:
	CCalcCninfoGetLandQuery(HWND hwnd, int logType);
	~CCalcCninfoGetLandQuery(void);

	string ClassName(){ return "CCalcCninfoGetLandQuery"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	string			m_after_report_date;
	string			m_request_like;

	int				m_max_page;
	int				m_page_no;

	vector<string>	m_vec_announcementId;
	vector<string>	m_vec_land_num;

	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<string>	m_vec_url;

	vector<string>	m_vec_land_name;

	vector<string>	m_vec_deal_money;
	vector<string>	m_vec_rate_of_capacity;
	vector<string>	m_vec_land_purpose;
	vector<string>	m_vec_land_area;
	vector<string>	m_vec_area_of_structure;
	

};
