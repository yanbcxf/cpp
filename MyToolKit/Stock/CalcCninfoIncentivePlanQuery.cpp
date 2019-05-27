#include "StdAfx.h"
#include "CalcCninfoIncentivePlanQuery.h"

CCalcCninfoIncentivePlanQuery::CCalcCninfoIncentivePlanQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCninfoIncentivePlanQuery::~CCalcCninfoIncentivePlanQuery(void)
{
}



void CCalcCninfoIncentivePlanQuery::Execute1(void* firstArg)
{
	CCalcCninfoIncentivePlanQuery * sink = (CCalcCninfoIncentivePlanQuery *)firstArg;
	sink->Execute();
}


void CCalcCninfoIncentivePlanQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_content.clear();
	m_vec_incentive_year.clear();
	m_vec_url.clear();

	m_vec_announcementId.clear();
	m_vec_net_profit.clear();
	m_vec_sales_from_operations.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM cninfo_incentive_plan b " 
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
				ss << " and content like \'%" << m_request_like << "%\' "; 
			}
			ss << " ORDER BY announcementTime desc";

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
		ss << "SELECT b.secCode, secName as abbreviation ,b.announcementTime,b.announcementTitle, b.content,  b.adjunctUrl, " 
			<< " b.announcementId, b.base_from_year, b.base_to_year, b.incentive_year, b.increase_percent, b.net_profit, "
			<< " b.sales_from_operations  FROM cninfo_incentive_plan b " 
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
			ss << " and content like \'%" << m_request_like << "%\' "; 
		}
		ss << " ORDER BY announcementTime desc ";
		ss << " limit " << ((m_page_no - 1) * RECORD_NUM_PER_PAGE) << ", " << RECORD_NUM_PER_PAGE;

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
				m_vec_base_from_year.insert(m_vec_base_from_year.end(),Int2String(r.get<int>("base_from_year")) );
			}
			catch(...)
			{
				m_vec_base_from_year.insert(m_vec_base_from_year.end(), "" );
			}

			try{
				m_vec_base_to_year.insert(m_vec_base_to_year.end(),Int2String(r.get<int>("base_to_year")) );
			}
			catch(...)
			{
				m_vec_base_to_year.insert(m_vec_base_to_year.end(), "" );
			}

			try{
				m_vec_incentive_year.insert(m_vec_incentive_year.end(), Int2String(r.get<int>("incentive_year")) );
			}
			catch(...)
			{
				m_vec_incentive_year.insert(m_vec_incentive_year.end(), "" );
			}

			try{
				m_vec_increase_percent.insert(m_vec_increase_percent.end(), Double2String(r.get<double>("increase_percent")) );
			}
			catch(...)
			{
				m_vec_increase_percent.insert(m_vec_increase_percent.end(), "" );
			}

			try{
				m_vec_net_profit.insert(m_vec_net_profit.end(), Double2String(r.get<double>("net_profit")) );
			}
			catch(...)
			{
				m_vec_net_profit.insert(m_vec_net_profit.end(), "" );
			}

			try{
				m_vec_sales_from_operations.insert(m_vec_sales_from_operations.end(), Double2String(r.get<double>("sales_from_operations")) );
			}
			catch(...)
			{
				m_vec_sales_from_operations.insert(m_vec_sales_from_operations.end(), "" );
			}

			try{
				m_vec_content.insert(m_vec_content.end(), r.get<string>("content") );
			}
			catch(...)
			{
				m_vec_content.insert(m_vec_content.end(), "(略：解析不成功)" );
			}

			try{
				m_vec_url.insert(m_vec_url.end(), r.get<string>("adjunctUrl") );
			}
			catch(...)
			{
				m_vec_url.insert(m_vec_url.end(), "" );
			}

			m_vec_announcementId.insert(m_vec_announcementId.end(), /*Utf8_GBK*/( r.get<string>("announcementId")));

			cnt++;
		}

		ss.str("");
		ss << "巨潮网 股权激励查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

