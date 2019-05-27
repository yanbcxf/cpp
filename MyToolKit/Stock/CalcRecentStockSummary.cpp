#include "StdAfx.h"
#include "CalcRecentStockSummary.h"

CCalcRecentStockSummary::CCalcRecentStockSummary(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcRecentStockSummary::~CCalcRecentStockSummary(void)
{
}




void CCalcRecentStockSummary::Execute1(void* firstArg)
{
	CCalcRecentStockSummary * sink = (CCalcRecentStockSummary *)firstArg;
	sink->Execute();
}



void CCalcRecentStockSummary::Execute()
{
	/*股票概况统计*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();

	m_vec_a_time_to_market.clear();			
	m_vec_recent_forecast_date.clear();		
	m_vec_recent_finance_date.clear();
	m_vec_recent_trade_date.clear();		

	m_vec_a_total_share.clear();			
	m_vec_estimate_profit.clear();
	m_vec_earning_per_year.clear();
	m_vec_pe.clear();
	m_vec_a_total_value.clear();
	m_vec_a_circulating_value.clear();
	m_vec_recent_forecast_rate.clear();
	m_vec_close.clear();
	m_vec_change_rate.clear();		
	m_vec_turnover_rate.clear();

	m_vec_static_pe.clear();
	m_vec_dynamic_pe.clear();
	m_vec_pb.clear();
	m_vec_dividend_yield_ratio.clear();

	try{
	
		ss.str("");
		ss << " select g.code, g.abbreviation, date_format(g.a_time_to_market,'%Y-%m-%d') as a_time_to_market, " 
			"date_format(g.recent_trade_date,'%Y-%m-%d') as recent_trade_date , g.recent_close, g.a_total_share, g.turnover_rate, "
			"g.a_total_value , g.a_circulating_value, date_format(g.recent_forecast_date,'%Y-%m-%d') as recent_forecast_date , "
			"(select h.change_rate  from sohuperformanceforecast h where g.code = h.code and g.recent_forecast_date = h.report_date ) as  forcast_change_rate, "
			"EstimateNetProfits(g.code, g.recent_trade_date) as earning_per_year, " 
			"date_format(RecentFinanceDate(g.code, now()) , '%Y-%m-%d')  as recent_finance_date, "
			"g.recent_close * 10000/( EstimateNetProfits(g.code, g.recent_trade_date)/ g.a_total_share) as pe, "
			"(select h.static_pe  from csipedaily h where g.code=h.code and  h.is_stock=1 and h.TradeDate=g.recent_trade_date) as static_pe, "
			"(select h.dynamic_pe  from csipedaily h where g.code=h.code and  h.is_stock=1 and h.TradeDate=g.recent_trade_date) as dynamic_pe, "
			"(select h.pb  from csipedaily h where g.code=h.code and  h.is_stock=1 and h.TradeDate=g.recent_trade_date) as pb, "
			"(select h.dividend_yield_ratio  from csipedaily h where g.code=h.code and  h.is_stock=1 and h.TradeDate=g.recent_trade_date) as dividend_yield_ratio "

			"from ( select e.code, e.abbreviation, e.a_time_to_market, e.recent_trade_date, e.recent_close, e.a_total_share, e.turnover_rate, "
			"e.total_value/(10000 * 10000) as a_total_value , e.circulating_value/(10000 * 10000) as a_circulating_value, "
			"(select  max(report_date) from sohuperformanceforecast f where f.code = e.code ) as recent_forecast_date " 
			"from (select c.code, c.abbreviation, c.a_time_to_market, c.recent_trade_date, "
			"(select max(d.close) from netease_trade_daily d where c.code = d.code and c.recent_trade_date = d.TradeDate ) as recent_close, "
			"(select max(d.turnover_rate) from netease_trade_daily d where c.code = d.code and c.recent_trade_date = d.TradeDate ) as turnover_rate, "
			"(select max(d.total_value) from netease_trade_daily d where c.code = d.code and c.recent_trade_date = d.TradeDate ) as total_value, "
			"(select max(d.circulating_value) from netease_trade_daily d where c.code = d.code and c.recent_trade_date = d.TradeDate ) as circulating_value, "
			"CurrentATotalShares(c.code, c.recent_trade_date) as a_total_share "  
			"from (select a.code, a.abbreviation, a.a_time_to_market, " 
			"(select max(b.TradeDate) from netease_trade_daily b where b.code = a.code) as recent_trade_date " 
			"from stockinfo a  where 1 = 1 " ;

		if(m_vec_request_code.size()>0)
		{
			ss << " and code in ( ";
			for(int i=0; i< m_vec_request_code.size(); i++)
			{
				ss << m_vec_request_code[i];
				if(i<m_vec_request_code.size()-1)
					ss << ",";
			}
			ss << ")";
		}
		ss << ") c ) e ) g;";
		

		string test = ss.str();
		//test = Gbk2Utf8(test);

		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			if(cnt==1270)
			{
				int ttttt = 1;
			}
			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				m_vec_a_time_to_market.insert(m_vec_a_time_to_market.end(), r.get<string>("a_time_to_market"));
			}
			catch(...)
			{
				m_vec_a_time_to_market.insert(m_vec_a_time_to_market.end(), "");
			}

			try
			{
				m_vec_recent_trade_date.insert(m_vec_recent_trade_date.end(), r.get<string>("recent_trade_date"));
			}
			catch(...)
			{
				m_vec_recent_trade_date.insert(m_vec_recent_trade_date.end(), "");
			}

			try
			{
				m_vec_close.insert(m_vec_close.end(),  r.get<double>("recent_close"));
			}
			catch (...)
			{
				m_vec_close.insert(m_vec_close.end(),  -12);
			}
			
			
			try
			{
				m_vec_a_total_share.insert(m_vec_a_total_share.end(),  r.get<double>("a_total_share"));
			}
			catch(...)
			{
				m_vec_a_total_share.insert(m_vec_a_total_share.end(),  -123456);
			}

			try
			{
				m_vec_a_total_value.insert(m_vec_a_total_value.end(),  r.get<double>("a_total_value"));
			}
			catch(...)
			{
				m_vec_a_total_value.insert(m_vec_a_total_value.end(), -123456);
			}

			m_vec_a_circulating_value.insert(m_vec_a_circulating_value.end(), r.get<double>("a_circulating_value", -123456));
			
			try
			{
				m_vec_recent_forecast_date.insert(m_vec_recent_forecast_date.end(), r.get<string>("recent_forecast_date"));
			}
			catch(...)
			{
				m_vec_recent_forecast_date.insert(m_vec_recent_forecast_date.end(), "");
			}

			try{
				m_vec_recent_forecast_rate.insert(m_vec_recent_forecast_rate.end(),  r.get<double>("forcast_change_rate"));
			}
			catch(...)
			{
				m_vec_recent_forecast_rate.insert(m_vec_recent_forecast_rate.end(),  -123456);
			}

			try{

				m_vec_earning_per_year.insert(m_vec_earning_per_year.end(),  r.get<double>("earning_per_year"));
			}catch(...)
			{
				m_vec_earning_per_year.insert(m_vec_earning_per_year.end(),  -123456);

			}

			try
			{
				m_vec_recent_finance_date.insert(m_vec_recent_finance_date.end(), r.get<string>("recent_finance_date"));
			}
			catch(...)
			{
				m_vec_recent_finance_date.insert(m_vec_recent_finance_date.end(), "");
			}

			try{
				m_vec_pe.insert(m_vec_pe.end(),  r.get<double>("pe"));
			}
			catch(...)
			{
				m_vec_pe.insert(m_vec_pe.end(),  -123456);
			}

			//////////////////////////////////////////////////////////////////////////
			try{

				m_vec_static_pe.insert(m_vec_static_pe.end(),  r.get<double>("static_pe"));
			}catch(...)
			{
				m_vec_static_pe.insert(m_vec_static_pe.end(),  -123456);

			}

			try{

				m_vec_dynamic_pe.insert(m_vec_dynamic_pe.end(),  r.get<double>("dynamic_pe"));
			}catch(...)
			{
				m_vec_dynamic_pe.insert(m_vec_dynamic_pe.end(),  -123456);

			}

			try{

				m_vec_pb.insert(m_vec_pb.end(),  r.get<double>("pb"));
			}catch(...)
			{
				m_vec_pb.insert(m_vec_pb.end(),  -123456);

			}

			try{

				m_vec_dividend_yield_ratio.insert(m_vec_dividend_yield_ratio.end(),  r.get<double>("dividend_yield_ratio"));
			}catch(...)
			{
				m_vec_dividend_yield_ratio.insert(m_vec_dividend_yield_ratio.end(),  -123456);

			}

			try{

				m_vec_turnover_rate.insert(m_vec_turnover_rate.end(),  r.get<double>("turnover_rate"));
			}catch(...)
			{
				m_vec_turnover_rate.insert(m_vec_turnover_rate.end(),  -123456);

			}
		
			cnt++;
		}

		ss.str("");
		ss << "股票概况统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
