#include "StdAfx.h"
#include "CalcMacdQuery.h"

CCalcMacdQuery::CCalcMacdQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMacdQuery::~CCalcMacdQuery(void)
{
}



void CCalcMacdQuery::Execute1(void* firstArg)
{
	CCalcMacdQuery * sink = (CCalcMacdQuery *)firstArg;
	sink->Execute();
}


void CCalcMacdQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_extra_desc.clear();
	m_vec_percent.clear();
	m_vec_basic_desc.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM tech_analysis_event b " 
				<< "where b.index_name like '%MACD%' " ;
			if(m_vec_request_code.size()>0)
			{
				ss << " and code in ( ";
				for(int i=0; i< m_vec_request_code.size(); i++)
				{
					ss << "\'" <<  m_vec_request_code[i] << "\'";
					if(i<m_vec_request_code.size()-1)
						ss << ",";
				}
				ss << ")";
			}

			if(m_after_report_date.empty()==false)
			{
				ss << " and tradedate >= \'" << m_after_report_date << "\' " ;
			}

			if(m_request_like.empty()==false)
			{
				ss << " and extra_desc like \'%" << m_request_like << "%\' "; 
			}
			ss << " ORDER BY tradedate desc";

			string test = ss.str();
			//test = Gbk2Utf8(test);

			int cnt;
			session sql(g_MysqlPool);
			sql << test, into(cnt);
			m_max_page = cnt /RECORD_NUM_PER_PAGE;
			if(cnt%RECORD_NUM_PER_PAGE >0)
				m_max_page++;

		}
	
		
		if(m_page_no>m_max_page) m_page_no = m_max_page;
		if(m_page_no<1) m_page_no =1;


		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "SELECT b.code, ( select a.abbreviation from stockinfo a where b.code = a.code ) as abbreviation , " 
			<< " date_format(b.tradedate, '%Y-%m-%d') as tradedate, b.extra_desc, b.basic_desc, "
			<< " b.percent FROM tech_analysis_event b " 
			<< "where b.index_name like '%MACD%' " ;
		if(m_vec_request_code.size()>0)
		{
			ss << " and code in ( ";
			for(int i=0; i< m_vec_request_code.size(); i++)
			{
				ss << "\'" << m_vec_request_code[i] << "\'";
				if(i<m_vec_request_code.size()-1)
					ss << ",";
			}
			ss << ")";
		}
		
		if(m_after_report_date.empty()==false)
		{
			ss << " and tradedate >= \'" << m_after_report_date << "\' " ;
		}

		if(m_request_like.empty()==false)
		{
			ss << " and extra_desc like \'%" << m_request_like << "%\' "; 
		}
		ss << " ORDER BY tradedate desc ";
		ss << " limit " << ((m_page_no - 1) * RECORD_NUM_PER_PAGE) << ", " <<  RECORD_NUM_PER_PAGE;

		string test = ss.str();
		//test = Gbk2Utf8(test);

		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		vector<int>	vecCalculateEps;
		int cnt = 0;
		while (st.fetch())
		{
			int code = atoi(r.get<string>("code").c_str());
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				m_vec_report_date.insert(m_vec_report_date.end(), r.get<string>("tradedate"));
			}
			catch(...)
			{
				m_vec_report_date.insert(m_vec_report_date.end(), "");
			}

					

			try{
				m_vec_extra_desc.insert(m_vec_extra_desc.end(), r.get<string>("extra_desc") );
			}
			catch(...)
			{
				m_vec_extra_desc.insert(m_vec_extra_desc.end(), "" );
			}

			m_vec_percent.insert(m_vec_percent.end(), r.get<double>("percent", 0) );

			m_vec_basic_desc.insert(m_vec_basic_desc.end(), r.get<string>("basic_desc", "") );

			cnt++;
		}

		ss.str("");
		ss << "技术分析 - MACD，查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

