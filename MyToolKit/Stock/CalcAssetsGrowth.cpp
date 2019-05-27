#include "StdAfx.h"
#include "CalcAssetsGrowth.h"

CCalcAssetsGrowth::CCalcAssetsGrowth(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{

}

CCalcAssetsGrowth::~CCalcAssetsGrowth(void)
{
}



void CCalcAssetsGrowth::Execute1(void* firstArg)
{
	CCalcAssetsGrowth * sink = (CCalcAssetsGrowth *)firstArg;
	sink->Execute();
}



void CCalcAssetsGrowth::Execute()
{
	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_report_date.clear();
	m_vec_total_assets.clear();
	m_vec_net_fixed_assets.clear();
	m_vec_construction_in_progress.clear();

	try
	{
		ss.str("");
		ss << "select b.code,  ( select a.abbreviation from stockinfo a where a.code = b.code ) as abbreviation, "
			<< "date_format(b.report_date, '%Y-%m-%d') as report_date ," 
			<< "b.colume_54 as total_assets, b.colume_39 as net_fixed_assets, b.colume_40 as construction_in_progress " 
			<< "from balance b "  
			<< " where code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ") and report_date > '2011-12-1' ";
		ss	<< " order by code, report_date ";

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

			m_vec_total_assets.insert(m_vec_total_assets.end(),  r.get<double>("total_assets"));
			m_vec_net_fixed_assets.insert(m_vec_net_fixed_assets.end(),  r.get<double>("net_fixed_assets"));
			m_vec_construction_in_progress.insert(m_vec_construction_in_progress.end(),  r.get<double>("construction_in_progress"));

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


