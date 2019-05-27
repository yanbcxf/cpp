#pragma once

class CCalcMacdListData : public CCalculateTask
{
public:
	CCalcMacdListData(HWND hwnd, int logType);
	~CCalcMacdListData(void);

	string ClassName(){ return "CCalcMacdListData"; }

	void Execute();
	static void Execute1(void* firstArg);

public:
	vector<int>		m_vec_code			;	//	"¹«Ë¾´úÂë",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_date1;
	vector<string>	m_vec_date2;
	vector<double>	m_vec_dif1;
	vector<double>	m_vec_dif2;
	vector<double>	m_vec_dea1;	
	vector<double>	m_vec_dea2;

};
