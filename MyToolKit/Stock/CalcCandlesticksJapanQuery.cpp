#include "StdAfx.h"
#include "CalcCandlesticksJapanQuery.h"

CCalcCandlesticksJapanQuery::CCalcCandlesticksJapanQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCandlesticksJapanQuery::~CCalcCandlesticksJapanQuery(void)
{
}



void CCalcCandlesticksJapanQuery::Execute1(void* firstArg)
{
	CCalcCandlesticksJapanQuery * sink = (CCalcCandlesticksJapanQuery *)firstArg;
	sink->Execute();
}


void CCalcCandlesticksJapanQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_picture.clear();
	m_vec_tomorrow_change.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM candlesticks_japan b " 
				<< "where 1=1 " ;
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
				ss << " and picture like \'%" << m_request_like << "%\' "; 
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
			<< " date_format(b.tradedate, '%Y-%m-%d') as tradedate, b.picture, "
			<< " b.tomorrow_change FROM candlesticks_japan b " 
			<< "where 1=1 " ;
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
			ss << " and picture like \'%" << m_request_like << "%\' "; 
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
				m_vec_picture.insert(m_vec_picture.end(), r.get<string>("picture") );
			}
			catch(...)
			{
				m_vec_picture.insert(m_vec_picture.end(), "" );
			}

			try{
				m_vec_tomorrow_change.insert(m_vec_tomorrow_change.end(), r.get<double>("tomorrow_change") );
			}
			catch(...)
			{
				m_vec_tomorrow_change.insert(m_vec_tomorrow_change.end(), 0.0 );
			}

			cnt++;
		}

		ss.str("");
		ss << "技术分析 - 蜡烛图，查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

