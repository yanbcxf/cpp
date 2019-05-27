#pragma once

class CCalcThisYearBonusSummary : public CCalculateTask
{
public:
	CCalcThisYearBonusSummary(HWND hwnd, int logType);
	~CCalcThisYearBonusSummary(void);

	string ClassName(){ return "CCalcThisYearBonusSummary"; }

	void Execute();
	static void Execute1(void* firstArg);

public:

	vector<int>		m_vec_code			;			//	"公司代码",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_bonus_year;				//	分红年度
	vector<double>	m_vec_bonus_cash;				//	现金分红
	vector<double>	m_vec_bonus_share;				//	送股
	vector<double>	m_vec_reserve_to_common_share;	//	转增股
	vector<string>	m_vec_right_record_date;		//	股权登记日期
	vector<string>	m_vec_exit_right_date;			//	除权除息日
	vector<double>	m_vec_recent_close;				//	最近收盘价
	vector<double>	m_vec_earning_percent;			//	收益比率
	vector<string>	m_vec_event_state;				//	方案状态

};
