#pragma once
#include "calculatetask.h"

class CCalcCninfoAnnouncementQuery :
	public CCalculateTask
{
public:
	CCalcCninfoAnnouncementQuery(HWND hwnd, int logType);
	~CCalcCninfoAnnouncementQuery(void);

	string ClassName(){ return "CCalcCninfoAnnouncementQuery"; }

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
	vector<string>	m_vec_type;
	vector<string>	m_vec_url;
	vector<string>	m_vec_announcementId;
	vector<int>		m_vec_analyzed;

};
