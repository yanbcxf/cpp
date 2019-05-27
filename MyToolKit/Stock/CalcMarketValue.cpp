#include "StdAfx.h"
#include "CalcMarketValue.h"

CCalcMarketValue::CCalcMarketValue(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMarketValue::~CCalcMarketValue(void)
{
}



void CCalcMarketValue::Execute1(void* firstArg)
{
	CCalcMarketValue * sink = (CCalcMarketValue *)firstArg;
	sink->Execute();
}



void CCalcMarketValue::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_trade_date.clear();

	m_vec_market_index.clear();
	m_vec_stock_index.clear();
	m_vec_circulating_value.clear();
	m_vec_total_value.clear();
	m_vec_corrected_stock_index.clear();
	
	try{
		/* 平均成本 MCST  统计  */
		session sql(g_MysqlPool);
		sql << "call CalculateAllStockRecentMarketValue()";

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(trade_date, '%Y-%m-%d') as trade_date, market_index, "
			<< " stock_index, circulating_value, total_value, corrected_stock_index "
			<< " from market_value_daily a where code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " )  ORDER BY code, trade_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("market_index");
				r.get<double>("stock_index");
				r.get<double>("circulating_value");
				r.get<double>("total_value");
				r.get<double>("corrected_stock_index");

			}
			catch(...)
			{
				continue;
			}

			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			//m_vec_abbreviation.insert(m_vec_abbreviation.end(), Utf8_GBK( r.get<string>("abbreviation")));

			try
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), r.get<string>("trade_date"));
			}
			catch(...)
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), "");
			}

			m_vec_market_index.insert(m_vec_market_index.end(), r.get<double>("market_index"));
			m_vec_stock_index.insert(m_vec_stock_index.end(), r.get<double>("stock_index"));
			m_vec_circulating_value.insert(m_vec_circulating_value.end(), r.get<double>("circulating_value"));

			m_vec_total_value.insert(m_vec_total_value.end(), r.get<double>("total_value"));
			m_vec_corrected_stock_index.insert(m_vec_corrected_stock_index.end(), r.get<double>("corrected_stock_index"));

			cnt++;
		}

		ss.str("");
		ss << "market_value_daily 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
