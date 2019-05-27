#include "StdAfx.h"
#include "CalcCninfoShareHoldingQuery.h"

CCalcCninfoShareHoldingQuery::CCalcCninfoShareHoldingQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCninfoShareHoldingQuery::~CCalcCninfoShareHoldingQuery(void)
{
}



void CCalcCninfoShareHoldingQuery::Execute1(void* firstArg)
{
	CCalcCninfoShareHoldingQuery * sink = (CCalcCninfoShareHoldingQuery *)firstArg;
	sink->Execute();
}


void CCalcCninfoShareHoldingQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_share_percent.clear();
	m_vec_change_percent.clear();
	m_vec_share_holder.clear();
	m_vec_url.clear();

	m_vec_announcementId.clear();
	m_vec_holder_character.clear();
	m_vec_year_quarter.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM cninfo_share_holding b " 
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
			ss << " ORDER BY announcementTime desc, announcementId , holder_num";

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
		ss << "SELECT b.secCode, secName as abbreviation ,b.announcementTime,  b.adjunctUrl, " 
			<< " b.announcementId, b.holder_num, b.share_holder, b.number_of_holding, b.number_of_restrict,  "
			<< " b.holder_character, b.share_percent,"
			<< " (select b.share_percent - a.share_percent from cninfo_share_holding a where  a.secCode = b.secCode "
			<< " and a.announcementTime < b.announcementTime and a.share_holder = b.share_holder ORDER BY a.announcementTime desc limit 0, 1 ) as change_percent, "
			<< " b.year_quarter FROM cninfo_share_holding b " 
			<< "where 1=1 " ;
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
		ss << " ORDER BY announcementTime desc ";
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
			int code = atoi(r.get<string>("secCode").c_str());
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				std::tm  tm = r.get<std::tm>("announcementTime");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date.insert(m_vec_report_date.end(), ss.str());
			}
			catch(...)
			{
				m_vec_report_date.insert(m_vec_report_date.end(), "");
			}

					

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
				m_vec_share_percent.insert(m_vec_share_percent.end(), r.get<double>("share_percent") );
			}
			catch(...)
			{
				m_vec_share_percent.insert(m_vec_share_percent.end(), -1 );
			}

			try{
				m_vec_change_percent.insert(m_vec_change_percent.end(), Double2String(r.get<double>("change_percent")) );
			}
			catch(...)
			{
				if("2014年4季度" == m_vec_year_quarter[m_vec_year_quarter.size() -1] || m_vec_year_quarter[m_vec_year_quarter.size() -1].empty()  )
					m_vec_change_percent.insert(m_vec_change_percent.end(), "" );
				else
					m_vec_change_percent.insert(m_vec_change_percent.end(),  Double2String(m_vec_share_percent[m_vec_share_percent.size() -1]) );
			}

			try{
				m_vec_number_of_restrict.insert(m_vec_number_of_restrict.end(), r.get<int>("number_of_restrict") );
			}
			catch(...)
			{
				m_vec_number_of_restrict.insert(m_vec_number_of_restrict.end(), -1 );
			}

			try{
				m_vec_number_of_holding.insert(m_vec_number_of_holding.end(), r.get<int>("number_of_holding") );
			}
			catch(...)
			{
				m_vec_number_of_holding.insert(m_vec_number_of_holding.end(), -1 );
			}

			
			try{
				m_vec_url.insert(m_vec_url.end(), r.get<string>("adjunctUrl") );
			}
			catch(...)
			{
				m_vec_url.insert(m_vec_url.end(), "" );
			}

			m_vec_announcementId.insert(m_vec_announcementId.end(),  r.get<string>("announcementId"));
			m_vec_holder_num.insert(m_vec_holder_num.end(),  Int2String(r.get<int>("holder_num")));

			cnt++;
		}

		ss.str("");
		ss << "巨潮网 前10 名股东持有情况 查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

