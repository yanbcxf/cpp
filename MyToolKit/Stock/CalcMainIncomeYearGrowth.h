#pragma once
#include "calculatetask.h"


class CMainIncomeYearGrowthData
{
public:
	CMainIncomeYearGrowthData()
	{
		for(int i=0; i<64; i++)
		{
			m_income_from_2012[i] = -1;
			m_growth_percent_from_2012[i] = -1;
		}
	}

	~CMainIncomeYearGrowthData(){}
	
	int		m_code;
	string	m_abbreviation;
	double  m_income_from_2012[64];				//	保存 2012 、2013、2014、 ...... 等年份的主营收入
	double	m_growth_percent_from_2012[64];		//	保存 2012 、2013、2014、 ...... 等年份的主营收入的增长率
};


class CCalcMainIncomeYearGrowth :
	public CCalculateTask
{
public:
	CCalcMainIncomeYearGrowth(HWND hwnd, int logType);
	~CCalcMainIncomeYearGrowth(void);

	string ClassName(){ return "CCalcMainIncomeYearGrowth"; }

	void Execute();
	static void Execute1(void* firstArg);

private:

	vector<int>		m_vec_code			;			//	"公司代码",
	vector<string>	m_vec_abbreviation;
	vector<string>	m_vec_report_date;
	vector<double>	m_vec_income;
	vector<double>	m_vec_growth_percent;

public:
	vector< CMainIncomeYearGrowthData>		m_vec_year_growth_data;
	int				m_max_year_index;		
};
