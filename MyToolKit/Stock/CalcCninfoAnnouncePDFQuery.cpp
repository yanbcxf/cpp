#include "StdAfx.h"
#include "CalcCninfoAnnouncePDFQuery.h"

CCalcCninfoAnnouncePDFQuery::CCalcCninfoAnnouncePDFQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCninfoAnnouncePDFQuery::~CCalcCninfoAnnouncePDFQuery(void)
{
}



void CCalcCninfoAnnouncePDFQuery::Execute1(void* firstArg)
{
	CCalcCninfoAnnouncePDFQuery * sink = (CCalcCninfoAnnouncePDFQuery *)firstArg;
	sink->Execute();
}


void CCalcCninfoAnnouncePDFQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_title.clear();
	m_vec_catalogNo.clear();
	m_vec_catalogName.clear();
	m_vec_catalogContent.clear();
	m_vec_url.clear();


	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM cninfoannouncepdf b " 
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
				ss << " and catalogName like \'%" << m_request_like << "%\' "; 
			}
			ss << " ORDER BY announcementTime desc";

			string test = ss.str();
			//test = Gbk2Utf8(test);

			int cnt;
			session sql(g_MysqlPool);
			sql << test, into(cnt);
			m_max_page = cnt /MYSQL_RECORD_NUM_PER_PAGE;
			if(cnt%MYSQL_RECORD_NUM_PER_PAGE >0)
				m_max_page++;

		}
	
		
		if(m_page_no>m_max_page) m_page_no = m_max_page;
		if(m_page_no<1) m_page_no =1;


		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "SELECT b.secCode, b.secName as abbreviation ," 
			<< "b.announcementTime, b.announcementTitle, b.catalogNo,  b.catalogName, "
			<< "b.catalogContent, b.adjunctUrl "
			<< "FROM cninfoannouncepdf b " 
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
			ss << " and catalogName like \'%" << m_request_like << "%\' "; 
		}
		ss << " ORDER BY announcementTime desc ";
		ss << " limit " << ((m_page_no - 1) * MYSQL_RECORD_NUM_PER_PAGE) << ", "<< MYSQL_RECORD_NUM_PER_PAGE;

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

			m_vec_title.insert(m_vec_title.end(), /*Utf8_GBK*/( r.get<string>("announcementTitle")));
			m_vec_catalogName.insert(m_vec_catalogName.end(), /*Utf8_GBK*/( r.get<string>("catalogName")));
			m_vec_catalogContent.insert(m_vec_catalogContent.end(), /*Utf8_GBK*/( r.get<string>("catalogContent")));
			m_vec_url.insert(m_vec_url.end(), r.get<string>("adjunctUrl"));

			cnt++;
		}

		ss.str("");
		ss << "巨潮网 PDF分析 查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

