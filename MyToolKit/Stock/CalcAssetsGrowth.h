#pragma once

class CCalcAssetsGrowth  : public CCalculateTask
{
public:
	CCalcAssetsGrowth(HWND hwnd, int logType);
	~CCalcAssetsGrowth(void);

	string ClassName(){ return "CCalcAssetsGrowth"; }

	void Execute();
	static void Execute1(void* firstArg);

public:

	vector<int>		m_vec_code			;	//	"公司代码",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<double>	m_vec_total_assets;
	vector<double>	m_vec_net_fixed_assets;				
	vector<double>	m_vec_construction_in_progress;		//	在建工程

};
