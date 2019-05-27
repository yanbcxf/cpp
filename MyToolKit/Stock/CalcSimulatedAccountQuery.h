#pragma once
#include "calculatetask.h"

class CCalcSimulatedAccountQuery :
	public CCalculateTask
{
public:
	CCalcSimulatedAccountQuery(HWND hwnd, int logType);
	~CCalcSimulatedAccountQuery(void);

	string ClassName(){ return "CCalcSimulatedAccountQuery"; }

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
	vector<double>	m_vec_capital;
	vector<long>	m_vec_shares;
	vector<double>	m_vec_cost;
	vector<double>	m_vec_loss_earning;

	vector<string>	m_vec_action;
	vector<string>	m_vec_remark;
	vector<string>	m_vec_desc_macd;
	vector<string>	m_vec_desc_kdj;
	vector<string>	m_vec_desc_k;

};
