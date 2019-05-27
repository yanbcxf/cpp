#include "StdAfx.h"
#include "CalcCninfoGetLandQuery.h"

CCalcCninfoGetLandQuery::CCalcCninfoGetLandQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCninfoGetLandQuery::~CCalcCninfoGetLandQuery(void)
{
}



void CCalcCninfoGetLandQuery::Execute1(void* firstArg)
{
	CCalcCninfoGetLandQuery * sink = (CCalcCninfoGetLandQuery *)firstArg;
	sink->Execute();
}


void CCalcCninfoGetLandQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_land_purpose.clear();
	m_vec_land_area.clear();
	m_vec_url.clear();

	m_vec_announcementId.clear();
	m_vec_land_num.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM cninfo_get_land b " 
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
		ss << "SELECT b.secCode, secName as abbreviation ,b.announcementTime, b.land_name,  b.adjunctUrl, " 
			<< " b.announcementId, b.land_num, b.land_purpose, b.land_area, b.rate_of_capacity, b.deal_money, "
			<< " b.area_of_structure FROM cninfo_get_land b " 
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
				m_vec_land_purpose.insert(m_vec_land_purpose.end(), r.get<string>("land_purpose") );
			}
			catch(...)
			{
				m_vec_land_purpose.insert(m_vec_land_purpose.end(), "" );
			}

			try{
				m_vec_land_area.insert(m_vec_land_area.end(), r.get<string>("land_area") );
			}
			catch(...)
			{
				m_vec_land_area.insert(m_vec_land_area.end(), "" );
			}

			try{
				m_vec_area_of_structure.insert(m_vec_area_of_structure.end(), r.get<string>("area_of_structure") );
			}
			catch(...)
			{
				m_vec_area_of_structure.insert(m_vec_area_of_structure.end(), "" );
			}

			try{
				m_vec_rate_of_capacity.insert(m_vec_rate_of_capacity.end(), r.get<string>("rate_of_capacity") );
			}
			catch(...)
			{
				m_vec_rate_of_capacity.insert(m_vec_rate_of_capacity.end(), "" );
			}

			try{
				m_vec_deal_money.insert(m_vec_deal_money.end(), r.get<string>("deal_money") );
			}
			catch(...)
			{
				m_vec_deal_money.insert(m_vec_deal_money.end(), "" );
			}

			try{
				m_vec_land_name.insert(m_vec_land_name.end(), r.get<string>("land_name") );
			}
			catch(...)
			{
				m_vec_land_name.insert(m_vec_land_name.end(), "" );
			}

			try{
				m_vec_url.insert(m_vec_url.end(), r.get<string>("adjunctUrl") );
			}
			catch(...)
			{
				m_vec_url.insert(m_vec_url.end(), "" );
			}

			m_vec_announcementId.insert(m_vec_announcementId.end(),  r.get<string>("announcementId"));
			m_vec_land_num.insert(m_vec_land_num.end(),  Int2String(r.get<int>("land_num")));

			cnt++;
		}

		ss.str("");
		ss << "巨潮网 竞得土地公告查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

