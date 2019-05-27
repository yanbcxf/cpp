#include "StdAfx.h"
#include "CalcAddIssuanceSummary.h"

CCalcAddIssuanceSummary::CCalcAddIssuanceSummary(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcAddIssuanceSummary::~CCalcAddIssuanceSummary(void)
{
}




void CCalcAddIssuanceSummary::Execute1(void* firstArg)
{
	CCalcAddIssuanceSummary * sink = (CCalcAddIssuanceSummary *)firstArg;
	sink->Execute();
}



void CCalcAddIssuanceSummary::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_market_date.clear();
	//m_vec_price.clear();
	m_vec_placement_price.clear();
	m_vec_recent_close.clear();
	m_vec_change_rate.clear();
	m_vec_add_issuance_price.clear();
	m_vec_abbreviation.clear();
	m_vec_market_close.clear();

	try{
		
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select c.code, c.abbreviation, c.placement_price, "
			<< " DATE_FORMAT(c.market_date, '%Y-%m-%d') as market_date, c.add_issuance_price ,c.market_close, c.recent_close , "
			<< " (c.recent_close - c.add_issuance_price) * 100 / c.add_issuance_price as change_rate "
			<< " from (select code, ( select b.abbreviation from stockinfo b where b.code = a.code ) as abbreviation, "
			<< " placement_price, market_date, CurrentAddIssuancePrice(code,market_date, placement_price) as add_issuance_price, "
			<< " (select e.close from  netease_trade_daily e where " 
			<< " e.code = a.code and e.TradeDate = (select max(d.TradeDate) "  
			<< " from netease_trade_daily d where a.code = d.code  and d.TradeDate<=a.market_date )) as market_close, "
			<< " (select e.close from  netease_trade_daily e where " 
			<< " e.code = a.code and e.TradeDate = (select max(d.TradeDate) "  
			<< " from netease_trade_daily d where a.code = d.code )) as recent_close "
			<< " from cfi_add_issuance a where a.code in (";  
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << ")  ) c order by change_rate desc ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("market_close");
				r.get<string>("market_date");
			}
			catch(...)
			{
				continue;
			}

			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				m_vec_market_date.insert(m_vec_market_date.end(), r.get<string>("market_date"));
			}
			catch(...)
			{
				m_vec_market_date.insert(m_vec_market_date.end(), "");
			}

			//m_vec_price.insert(m_vec_price.end(), r.get<double>("price"));
			m_vec_placement_price.insert(m_vec_placement_price.end(), r.get<double>("placement_price"));
			m_vec_add_issuance_price.insert(m_vec_add_issuance_price.end(), r.get<double>("add_issuance_price"));
			m_vec_recent_close.insert(m_vec_recent_close.end(), r.get<double>("recent_close"));
			m_vec_change_rate.insert(m_vec_change_rate.end(), r.get<double>("change_rate"));
			m_vec_market_close.insert(m_vec_market_close.end(), r.get<double>("market_close"));

			cnt++;
		}

		ss.str("");
		ss << "股票增发盈亏汇总 计算完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

