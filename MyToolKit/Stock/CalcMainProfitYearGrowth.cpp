#include "StdAfx.h"
#include "CalcMainProfitYearGrowth.h"

CCalcMainProfitYearGrowth::CCalcMainProfitYearGrowth(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMainProfitYearGrowth::~CCalcMainProfitYearGrowth(void)
{
}




void CCalcMainProfitYearGrowth::Execute1(void* firstArg)
{
	CCalcMainProfitYearGrowth * sink = (CCalcMainProfitYearGrowth *)firstArg;
	sink->Execute();
}



void CCalcMainProfitYearGrowth::Execute()
{

	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_report_date.clear();
	m_vec_main_profit.clear();
	m_vec_growth_percent.clear();

	try
	{
		ss << "select a.code, ( select c.abbreviation from stockinfo c where a.code = c.code ) as abbreviation, "
			<< "a.report_date, a.income ,b.income as income1, "
			<< "(a.income - b.income)/ b.income as growth_percent from " 
			<< "(select code, date_format(report_date, '%Y-%m-%d') as report_date , " 
			<< "colume_33 as income from incomestatement  "
			<< "where report_date > '2011-12-1' and MONTh(report_date) = 12 "
			<< " and code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss	<< " ) order by code, report_date ) a, ";

		ss << "(select code, date_format(report_date, '%Y-%m-%d') as report_date , "
			<< "colume_33 as income "
			<< "from incomestatement " 
			<< "where report_date > '2011-12-1' and MONTh(report_date) = 12 "
			<< " and code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss	<< " ) order by code, report_date) b "
			<< " where a.code = b.code and YEAR(a.report_date) = YEAR(b.report_date) + 1 ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				m_vec_report_date.insert(m_vec_report_date.end(), r.get<string>("report_date"));
			}
			catch(...)
			{
				m_vec_report_date.insert(m_vec_report_date.end(), "");
			}

			try
			{
				m_vec_main_profit.insert(m_vec_main_profit.end(),  r.get<double>("income"));

			}
			catch (...)
			{
				m_vec_main_profit.insert(m_vec_main_profit.end(),  -1);
			}

			try
			{
				m_vec_growth_percent.insert(m_vec_growth_percent.end(), r.get<double>("growth_percent"));
			}
			catch(...)
			{
				m_vec_growth_percent.insert(m_vec_growth_percent.end(), -1);
			}

			cnt++;
		}

		/*»ã×ÜÊý¾Ý*/
		m_max_year_index = 0;
		map<int, CMainProfitYearGrowthData>	 map_year_growth_data;
		for(int i=0; i< m_vec_code.size(); i++)
		{
			if(map_year_growth_data.find(m_vec_code[i])==map_year_growth_data.end())
			{
				CMainProfitYearGrowthData data;
				data.m_code = m_vec_code[i];
				data.m_abbreviation = m_vec_abbreviation[i];
				map_year_growth_data[data.m_code] = data;
			}
			map<int , CMainProfitYearGrowthData>::iterator it = map_year_growth_data.find(m_vec_code[i]);
			COleDateTime odt;
			odt.ParseDateTime(m_vec_report_date[i].c_str());
			int nIndex = odt.GetYear() - 2012;
			if(nIndex>m_max_year_index)
				m_max_year_index = nIndex;
			it->second.m_main_profit_from_2012[nIndex] = m_vec_main_profit[i];
			it->second.m_growth_percent_from_2012[nIndex] = m_vec_growth_percent[i];
		}

		map<int , CMainProfitYearGrowthData>::iterator it = map_year_growth_data.begin();
		for(; it!=map_year_growth_data.end(); it++)
		{
			m_vec_year_growth_data.insert(m_vec_year_growth_data.end(), it->second);
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}


}