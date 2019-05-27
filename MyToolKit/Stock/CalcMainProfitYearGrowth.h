#pragma once
#include "calculatetask.h"



class CMainProfitYearGrowthData
{
public:
	CMainProfitYearGrowthData()
	{
		for(int i=0; i<64; i++)
		{
			m_main_profit_from_2012[i] = -1;
			m_growth_percent_from_2012[i] = -1;
		}
	}

	~CMainProfitYearGrowthData(){}

	int		m_code;
	string	m_abbreviation;
	double  m_main_profit_from_2012[64];		//	保存 2012 、2013、2014、 ...... 等年份的主营利润
	double	m_growth_percent_from_2012[64];		//	保存 2012 、2013、2014、 ...... 等年份的主营收入的增长率
};



class CCalcMainProfitYearGrowth:
	public CCalculateTask
{
public:
	CCalcMainProfitYearGrowth(HWND hwnd, int logType);
	~CCalcMainProfitYearGrowth(void);


	string ClassName(){ return "CCalcMainProfitYearGrowth"; }

	void Execute();
	static void Execute1(void* firstArg);

private:

	vector<int>		m_vec_code			;			//	"公司代码",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<double>	m_vec_main_profit;
	vector<double>	m_vec_growth_percent;

public:
	vector< CMainProfitYearGrowthData>		m_vec_year_growth_data;
	int				m_max_year_index;		
};
