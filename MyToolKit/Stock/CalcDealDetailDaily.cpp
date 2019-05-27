#include "StdAfx.h"
#include "CalcDealDetailDaily.h"

CCalcDealDetailDaily::CCalcDealDetailDaily(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcDealDetailDaily::~CCalcDealDetailDaily(void)
{
}




void CCalcDealDetailDaily::Execute1(void* firstArg)
{
	CCalcDealDetailDaily * sink = (CCalcDealDetailDaily *)firstArg;
	sink->Execute();
}



void CCalcDealDetailDaily::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_nRequestDealDate<0)
		return ;

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	m_vec_code.clear();
	m_vec_deal_type.clear();
	m_vec_deal_time.clear();

	m_vec_deal_price.clear();
	m_vec_deal_volume.clear();
	m_vec_deal_turnover.clear();

	try{

		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code ,TIME_FORMAT(deal_time, '%H:%i:%S') as  deal_time, "
			<< " deal_price, deal_volume, deal_type, deal_turnover "
			<< " from sinadealdetail_" << Int2String(m_nRequestDealDate) <<" a where  code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m] ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " )  ORDER BY code, deal_time  ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("deal_price");
				r.get<double>("deal_volume");
				r.get<string>("deal_type");
				r.get<double>("deal_turnover");
				r.get<string>("deal_time");
				r.get<int>("code");
			}
			catch(...)
			{
				continue;
			}

			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			// m_vec_abbreviation.insert(m_vec_abbreviation.end(), Utf8_GBK( r.get<string>("name")));

			
			m_vec_deal_price.insert(m_vec_deal_price.end(), r.get<double>("deal_price"));
			m_vec_deal_volume.insert(m_vec_deal_volume.end(), r.get<double>("deal_volume"));
			m_vec_deal_type.insert(m_vec_deal_type.end(), r.get<string>("deal_type"));
			m_vec_deal_turnover.insert(m_vec_deal_turnover.end(), r.get<double>("deal_turnover"));
			m_vec_deal_time.insert(m_vec_deal_time.end(),r.get<string>("deal_time"));

			cnt++;
		}

		ss.str("");
		ss << "CCalcDealDetailDaily 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
