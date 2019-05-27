#pragma once
#include "calculatetask.h"

class CCalcCandlesticksJapanQuery :
	public CCalculateTask
{
public:
	CCalcCandlesticksJapanQuery(HWND hwnd, int logType);
	~CCalcCandlesticksJapanQuery(void);

	string ClassName(){ return "CCalcCandlesticksJapanQuery"; }

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
	vector<string>	m_vec_picture;

	vector<double>	m_vec_tomorrow_change;
	

};
