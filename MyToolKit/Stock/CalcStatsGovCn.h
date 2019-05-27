#pragma once

class CCalcStatsGovCn : public CCalculateTask
{
public:
	CCalcStatsGovCn(HWND hwnd, int logType);
	~CCalcStatsGovCn(void);

	string ClassName(){ return "CCalcStatsGovCn"; }

	void Execute();
	static void Execute1(void* firstArg);


public :
	string				m_str_db_code;
	int					m_nOutput;			//	0 表格输出， 1 图表展示
	vector<string>		m_vec_zb_code;
	vector<string>		m_vec_reg_code;

	

};
