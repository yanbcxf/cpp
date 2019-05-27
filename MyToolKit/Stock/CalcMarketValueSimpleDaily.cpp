#include "StdAfx.h"
#include "CalcMarketValueSimpleDaily.h"

CCalcMarketValueSimpleDaily::CCalcMarketValueSimpleDaily(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMarketValueSimpleDaily::~CCalcMarketValueSimpleDaily(void)
{
}




void CCalcMarketValueSimpleDaily::Execute1(void* firstArg)
{
	CCalcMarketValueSimpleDaily * sink = (CCalcMarketValueSimpleDaily *)firstArg;
	sink->Execute();
}



void CCalcMarketValueSimpleDaily::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_trade_date.clear();

	m_vec_company_number.clear();
	m_vec_is_stock.clear();
	m_vec_pb.clear();
	m_vec_static_pe.clear();
	m_vec_dynamic_pe.clear();
	m_vec_dividend_yield_ratio.clear();

	try{
		
		session sql(g_MysqlPool);

		sql << "call CalculateAllStockRecentMarketValueSimple()";
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select e.code, (select abbreviation from stockinfo where code = e.code ) as abbreviation," 
			"(select change_rate from market_value_daily_simple where code = e.code and trade_date = e.trade_date ) as change_rate,"
			"(e.stock_index - e.stock_index_1_week) * 100 / e.stock_index_1_week as change_rate_1_week, "
			"(e.stock_index - e.stock_index_1_month) * 100 / e.stock_index_1_month as change_rate_1_month," 
			"(e.stock_index - e.stock_index_3_month) * 100 / e.stock_index_3_month as change_rate_3_month, "
			"(e.stock_index - e.stock_index_6_month) * 100 / e.stock_index_6_month as change_rate_6_month, "
			"(e.stock_index - e.stock_index_9_month) * 100 / e.stock_index_9_month as change_rate_9_month, "
			"(e.stock_index - e.stock_index_1_year) * 100 / e.stock_index_1_year as change_rate_1_year, "
			"(e.stock_index - e.stock_index_2_year) * 100 / e.stock_index_2_year as change_rate_2_year, "
			"(e.stock_index - e.stock_index_3_year) * 100 / e.stock_index_3_year as change_rate_3_year, "
			"(e.stock_index - e.stock_index_4_year) * 100 / e.stock_index_4_year as change_rate_4_year "
			"from ( "
			"select d.code, d.trade_date, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date ) as stock_index, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_1_week ) as stock_index_1_week, " 
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_1_month ) as stock_index_1_month," 
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_3_month ) as stock_index_3_month, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_6_month ) as stock_index_6_month, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_9_month ) as stock_index_9_month, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_1_year ) as stock_index_1_year, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_2_year ) as stock_index_2_year, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_3_year ) as stock_index_3_year, "
			"(select stock_index from market_value_daily_simple where code = d.code and trade_date = d.trade_date_4_year ) as stock_index_4_year "
			"from ( "
			"select c.code, c.trade_date,  c.trade_date_1_week,  c.trade_date_1_month, c.trade_date_3_month, c.trade_date_6_month, c.trade_date_9_month, " 
			"c.trade_date_1_year, c.trade_date_2_year, c.trade_date_3_year, c.trade_date_4_year  "
			"from ( select b.code, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= now() ) as trade_date, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 1 WEEK) )  as trade_date_1_week, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 1 MONTH) ) as trade_date_1_month, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 3 MONTH) ) as trade_date_3_month, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 6 MONTH) ) as trade_date_6_month, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 9 MONTH) ) as trade_date_9_month, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 1 YEAR) ) as trade_date_1_year, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 2 YEAR) ) as trade_date_2_year, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 3 YEAR) ) as trade_date_3_year, "
			"( select max(a.trade_date) from market_value_daily_simple a where b.code = a.code and a.trade_date <= DATE_SUB(now(),INTERVAL 4 YEAR) ) as trade_date_4_year "
			"from stockinfo  b where b.code in ("; 
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss <<  m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ) c " 
			") d "
			") e";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			
			//	标准的输出

			if(title_code.empty())
			{
				title_code = "*";
				title_name = "个股近期涨跌排序";
			}

					
			char tmp[128];
			sprintf_s(tmp, 128, "%06d", r.get<int>("code"));
			string strCode = string(tmp);
			string strAbbreviation = /*Utf8_GBK*/(r.get<string>("abbreviation"));

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "前1交易日涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate"));
			unit.insert(unit.end(), "%");
			

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "1周涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_1_week"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "1个月涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_1_month"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "3个月涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_3_month"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "6个月涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_6_month"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "9个月涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_9_month"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "1年涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_1_year"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "2年涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_2_year"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "3年涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_3_year"));
			unit.insert(unit.end(), "%");

			zb_code.insert(zb_code.end(), strCode);
			zb_name.insert(zb_name.end(), strAbbreviation);
			sj_code.insert(sj_code.end(), "4年涨跌");
			data.insert(data.end(), GetDoubleValue(r, "change_rate_4_year"));
			unit.insert(unit.end(), "%");
			cnt++;
		}

		ss.str("");
		ss << "CCalcMarketValueSimple 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
