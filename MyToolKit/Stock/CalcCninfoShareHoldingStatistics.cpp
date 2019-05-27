#include "StdAfx.h"
#include "CalcCninfoShareHoldingStatistics.h"

CCalcCninfoShareHoldingStatistics::CCalcCninfoShareHoldingStatistics(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCninfoShareHoldingStatistics::~CCalcCninfoShareHoldingStatistics(void)
{
}



void CCalcCninfoShareHoldingStatistics::Execute1(void* firstArg)
{
	CCalcCninfoShareHoldingStatistics * sink = (CCalcCninfoShareHoldingStatistics *)firstArg;
	sink->Execute();
}


void CCalcCninfoShareHoldingStatistics::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_share_holder.clear();
	m_vec_company_number.clear();
	m_vec_holder_character.clear();
	m_vec_year_quarter.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt from " 
				<< "( select 1 from cninfo_share_holding  " 
				<< "where 1=1 " ;
			if(m_vec_request_code.size()>0)
			{
				ss << " and secCode in ( ";
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
				ss << " and announcementTime >= \'" << m_after_report_date << "\' " ;
			}

			if(m_request_like.empty()==false)
			{
				ss << m_request_like;
			}
			ss << " GROUP BY year_quarter, share_holder , holder_character ) as a";

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
		ss << "select year_quarter, share_holder, holder_character , count(*) as company_number " 
			<< " from cninfo_share_holding   "
			<< " where 1=1 " ;
		if(m_vec_request_code.size()>0)
		{
			ss << " and secCode in ( ";
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
			ss << " and announcementTime >= \'" << m_after_report_date << "\' " ;
		}

		if(m_request_like.empty()==false)
		{
			ss <<  m_request_like ;
		}
		ss << " GROUP BY year_quarter, share_holder , holder_character  ";
		ss << " order by year_quarter desc , company_number desc";
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
			

			try{
				m_vec_holder_character.insert(m_vec_holder_character.end(), r.get<string>("holder_character") );
			}
			catch(...)
			{
				m_vec_holder_character.insert(m_vec_holder_character.end(), "" );
			}

			try{
				m_vec_year_quarter.insert(m_vec_year_quarter.end(), r.get<string>("year_quarter") );
			}
			catch(...)
			{
				m_vec_year_quarter.insert(m_vec_year_quarter.end(), "" );
			}

			try{
				m_vec_share_holder.insert(m_vec_share_holder.end(), r.get<string>("share_holder") );
			}
			catch(...)
			{
				m_vec_share_holder.insert(m_vec_share_holder.end(), "" );
			}

			
			try{
				m_vec_company_number.insert(m_vec_company_number.end(), r.get<long long>("company_number") );
			}
			catch(...)
			{
				m_vec_company_number.insert(m_vec_company_number.end(), -1 );
			}

			
			cnt++;
		}

		ss.str("");
		ss << "巨潮网 前10 名股东持有情况 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

