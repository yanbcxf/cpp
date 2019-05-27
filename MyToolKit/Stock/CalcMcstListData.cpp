#include "StdAfx.h"
#include "CalcMcstListData.h"

CCalcMcstListData::CCalcMcstListData(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMcstListData::~CCalcMcstListData(void)
{
}




void CCalcMcstListData::Execute1(void* firstArg)
{
	CCalcMcstListData * sink = (CCalcMcstListData *)firstArg;
	sink->Execute();
}



void CCalcMcstListData::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_trade_date.clear();
	m_vec_turnover.clear();
	m_vec_turnover_rate.clear();
	m_vec_price.clear();
	m_vec_change_rate.clear();
	m_vec_mcst_price.clear();
	m_vec_mcst_price_change_rate.clear();
	m_vec_mcst_percent.clear();

	try{
		/* 平均成本 MCST  统计  */
		session sql(g_MysqlPool);
		sql << "call CalculateAllStockRecentMCST()";

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select e.code, e.abbreviation, date_format(e.trade_date, '%Y-%m-%d') as trade_date, e.Turnover, e.Turnover_Rate, "
			<< " e.price, e.Change_Rate,  e.mcst_price , e.mcst_price_change_rate, "
			<< " (e.price - e.mcst_price)*100/ e.mcst_price as mcst_percent "
			<< " from (select c.code, c.abbreviation, c.trade_date, "
			<< " (select d.Turnover from netease_trade_daily d where d.code = c.code and d.tradedate = c.trade_date limit 0,1 ) as turnover,"
			<< " (select d.Turnover_Rate from netease_trade_daily d where d.code = c.code and d.tradedate = c.trade_date limit 0,1 ) as turnover_rate, "
			<< " (select d.Change_Rate from netease_trade_daily d where d.code = c.code and d.tradedate = c.trade_date limit 0,1 ) as change_rate,"
			<< " (select d.close from netease_trade_daily d where d.code = c.code and d.tradedate = c.trade_date limit 0,1 ) as price, "
			<< " (select d.mcst_price_change_rate from mcst d where d.code = c.code and d.trade_date = c.trade_date limit 0,1 ) as mcst_price_change_rate, "
			<< " (select d.mcst_price from mcst d where d.code = c.code and d.trade_date = c.trade_date limit 0,1 ) as mcst_price " 
			<< " from (select code , abbreviation , (select max(b.trade_date) from mcst b where b.code=a.code ) as trade_date "
			<< " from stockinfo a where code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ) c ) e order by mcst_percent desc ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("Turnover");
				r.get<double>("Turnover_Rate");
				r.get<double>("price");
				r.get<double>("Change_Rate");
				r.get<double>("mcst_price");
				r.get<double>("mcst_price_change_rate");

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
				m_vec_trade_date.insert(m_vec_trade_date.end(), r.get<string>("trade_date"));
			}
			catch(...)
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), "");
			}

			m_vec_turnover.insert(m_vec_turnover.end(), r.get<double>("Turnover"));
			m_vec_turnover_rate.insert(m_vec_turnover_rate.end(), r.get<double>("Turnover_Rate"));

			m_vec_price.insert(m_vec_price.end(), r.get<double>("price"));
			m_vec_change_rate.insert(m_vec_change_rate.end(), r.get<double>("Change_Rate"));

			m_vec_mcst_price.insert(m_vec_mcst_price.end(), r.get<double>("mcst_price"));
			m_vec_mcst_price_change_rate.insert(m_vec_mcst_price_change_rate.end(), r.get<double>("mcst_price_change_rate"));

			m_vec_mcst_percent.insert(m_vec_mcst_percent.end(), r.get<double>("mcst_percent"));

			cnt++;
		}

		ss.str("");
		ss << "MCST 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
