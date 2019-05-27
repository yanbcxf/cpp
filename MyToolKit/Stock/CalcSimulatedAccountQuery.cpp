#include "StdAfx.h"
#include "CalcSimulatedAccountQuery.h"

CCalcSimulatedAccountQuery::CCalcSimulatedAccountQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcSimulatedAccountQuery::~CCalcSimulatedAccountQuery(void)
{
}



void CCalcSimulatedAccountQuery::Execute1(void* firstArg)
{
	CCalcSimulatedAccountQuery * sink = (CCalcSimulatedAccountQuery *)firstArg;
	sink->Execute();
}


void CCalcSimulatedAccountQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_report_date.clear();
	m_vec_capital.clear();
	m_vec_shares.clear();

	try{

		session sql(g_MysqlPool);

		/*ss << "create TEMPORARY table tmp_table " 
			<< " select code, tradedate, capital, shares, cost, loss_earning, action, remark, "
			<< " desc_macd, desc_kdj, desc_k "
			<< " from simulated_account "
		sql << ss.str();*/

		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM simulated_account b " 
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
				ss << " and action like \'%" << m_request_like << "%\' "; 
			}
			ss << " ORDER BY tradedate desc";

			string test = ss.str();
			//test = Gbk2Utf8(test);

			int cnt;
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
			<< " date_format(b.tradedate, '%Y-%m-%d') as tradedate,  "
			<< " capital, shares, cost, loss_earning, action, remark, "
			<< " desc_macd, desc_kdj, desc_k "
			<< " FROM simulated_account b " 
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
			ss << " and action like \'%" << m_request_like << "%\' "; 
		}
		ss << " ORDER BY tradedate desc ";
		ss << " limit " << ((m_page_no - 1) * RECORD_NUM_PER_PAGE) << ", " <<  RECORD_NUM_PER_PAGE;

		string test = ss.str();
		//test = Gbk2Utf8(test);

		
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		vector<int>	vecCalculateEps;
		int cnt = 0;
		while (st.fetch())
		{
			int code = atoi(r.get<string>("code").c_str());
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation", "")));

			m_vec_report_date.insert(m_vec_report_date.end(), r.get<string>("tradedate", ""));
			
			m_vec_capital.insert(m_vec_capital.end(), r.get<double>("capital", 0));
			m_vec_shares.insert(m_vec_shares.end(), r.get<int>("shares", 0));
			m_vec_cost.insert(m_vec_cost.end(), r.get<double>("cost", 0));
			m_vec_loss_earning.insert(m_vec_loss_earning.end(), r.get<double>("loss_earning", 0));

			m_vec_action.insert(m_vec_action.end(), r.get<string>("action", "") );
			m_vec_remark.insert(m_vec_remark.end(), r.get<string>("remark", "") );
			m_vec_desc_macd.insert(m_vec_desc_macd.end(), r.get<string>("desc_macd", "") );
			m_vec_desc_kdj.insert(m_vec_desc_kdj.end(), r.get<string>("desc_kdj", "") );
			m_vec_desc_k.insert(m_vec_desc_k.end(), r.get<string>("desc_k", "") );

			cnt++;
		}

		/*sql << "drop table tmp_table";*/

		ss.str("");
		ss << "技术分析 - 模拟交易，查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		string test = ss.str();
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

