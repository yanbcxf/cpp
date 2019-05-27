#include "StdAfx.h"
#include "CalcMainIncomeGrowth.h"

CCalcMainIncomeGrowth::CCalcMainIncomeGrowth(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMainIncomeGrowth::~CCalcMainIncomeGrowth(void)
{
}




void CCalcMainIncomeGrowth::Execute1(void* firstArg)
{
	CCalcMainIncomeGrowth * sink = (CCalcMainIncomeGrowth *)firstArg;
	sink->Execute();
}



void CCalcMainIncomeGrowth::Execute()
{

	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_report_date.clear();
	m_vec_composition_name.clear();
	m_vec_income.clear();
	m_vec_proportion_of_total_income.clear();
	
	try
	{
		ss << "select code , ( select a.abbreviation from stockinfo a where a.code = b.code ) as abbreviation, "
			<< "report_date, composition_name , income,  proportion_of_total_income "
			<< "from " 
			<< "((select code, date_format(report_date, '%Y-%m-%d') as report_date ," 
			<< " '总计' as composition_name, colume_2 as income, 100.00 as proportion_of_total_income  "
			<< " from incomestatement "
			<< " where report_date > '2011-12-1' "
			<< " and code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss	<< " ) order by code, report_date ) "
			<< " UNION "
			<< " (select code , date_format(report_date, '%Y-%m-%d') as report_date , "
			<< " composition_name, income * 10000 as income, proportion_of_total_income "
			<< " from sohumainincomecomposition  "
			<< " where report_date > '2011-12-1' "
			<< " and composition_type='主营产品构成（万）' "
			<< " and code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss	<< " ) order by code, report_date, composition_name "
			<< " )) b "
			<< " order by b.code, b.report_date, b.composition_name ";

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

			string compositionName = /*Utf8_GBK*/( r.get<string>("composition_name"));
			compositionName = ReplaceString(compositionName, "其中：", "");
			compositionName = ReplaceString(compositionName, "其中:", "");
			if(compositionName.find("其他")!=string::npos || compositionName.find("其它")!=string::npos)
			{
				compositionName = "其他";
			}
			m_vec_composition_name.insert(m_vec_composition_name.end(), compositionName);

			try
			{
				m_vec_report_date.insert(m_vec_report_date.end(), r.get<string>("report_date"));
			}
			catch(...)
			{
				m_vec_report_date.insert(m_vec_report_date.end(), "");
			}

			m_vec_income.insert(m_vec_income.end(),  r.get<double>("income"));
			m_vec_proportion_of_total_income.insert(m_vec_proportion_of_total_income.end(), r.get<double>("proportion_of_total_income"));
			
			cnt++;
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	

}