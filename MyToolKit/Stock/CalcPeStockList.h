#pragma once

class CCalcPeStockList : public CCalculateTask 
{
public:
	CCalcPeStockList(HWND hwnd, int logType);
	~CCalcPeStockList(void);

	string ClassName(){ return "CCalcPeStockList"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<double>	m_vec_min_pe;
	vector<string>	m_vec_min_date;
	vector<double>	m_vec_recent_pe;
	vector<string>	m_vec_recent_date;
	vector<double>	m_vec_defferent;

};
