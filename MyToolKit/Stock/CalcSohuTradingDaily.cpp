#include "StdAfx.h"
#include "CalcSohuTradingDaily.h"

CCalcSohuTradingDaily::CCalcSohuTradingDaily(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcSohuTradingDaily::~CCalcSohuTradingDaily(void)
{
}




void CCalcSohuTradingDaily::Execute1(void* firstArg)
{
	CCalcSohuTradingDaily * sink = (CCalcSohuTradingDaily *)firstArg;
	sink->Execute();
}



void CCalcSohuTradingDaily::Execute()
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
	m_vec_change_rate.clear();
	m_vec_change_amount.clear();
	m_vec_volume.clear();

	try{
		session sql(g_MysqlPool);
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(TradeDate, '%Y-%m-%d') as trade_date, Turnover, "
			<< " Turnover_Rate, Change_Amount, Change_Rate, Volume "
			<< " from souhutradingdaily a where code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) and TradeDate >'2010-1-1' ORDER BY code, trade_date ";

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
				r.get<double>("Change_Amount");
				r.get<double>("Change_Rate");
				r.get<int>("Volume");

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

			m_vec_turnover.insert(m_vec_turnover.end(), r.get<double>("Turnover"));
			m_vec_turnover_rate.insert(m_vec_turnover_rate.end(), r.get<double>("Turnover_Rate"));
			m_vec_change_rate.insert(m_vec_change_rate.end(), r.get<double>("Change_Rate"));

			m_vec_change_amount.insert(m_vec_change_amount.end(), r.get<double>("Change_Amount"));
			m_vec_volume.insert(m_vec_volume.end(), r.get<int>("Volume"));

			cnt++;
		}

		ss.str("");
		ss << "CCalcSohuTradingDaily 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
