#include "StdAfx.h"
#include "CalcCffexFutureHs300.h"

CCalcCffexFutureHs300::CCalcCffexFutureHs300(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCffexFutureHs300::~CCalcCffexFutureHs300(void)
{
}


void CCalcCffexFutureHs300::Execute1(void* firstArg)
{
	CCalcCffexFutureHs300 * sink = (CCalcCffexFutureHs300 *)firstArg;
	sink->Execute();
}



void CCalcCffexFutureHs300::Execute()
{
	/* 汇总沪深300 指数，及相关的股指期货 */

	stringstream ss;
	m_vec_trade_date.clear();
	m_vec_agreement_code.clear();

	m_vec_settlement.clear();
	m_vec_close.clear();
	m_vec_hs300.clear();

	try{

		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");

		ss << "select date_format(a.trade_date, '%Y-%m-%d') as trade_date, a.agreement_code, a.settlement, a.close, "
			<< " (select  b.close from sohuindexdaily b where  b.code=300 and b.TradeDate=a.trade_date) as hs300 "
			<< " from (select trade_date, agreement_code, settlement , close "
			<< " from cffexfuturedaily where agreement_code like 'IF%' order by trade_date ) a ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("settlement");
				r.get<double>("close");
				r.get<double>("hs300");
				r.get<string>("agreement_code");
				r.get<string>("trade_date");
			}
			catch(...)
			{
				continue;
			}

			
			m_vec_trade_date.insert(m_vec_trade_date.end(), r.get<string>("trade_date"));
			m_vec_agreement_code.insert(m_vec_agreement_code.end(), r.get<string>("agreement_code"));
			m_vec_settlement.insert(m_vec_settlement.end(), r.get<double>("settlement"));
			m_vec_close.insert(m_vec_close.end(), r.get<double>("close"));
			m_vec_hs300.insert(m_vec_hs300.end(),r.get<double>("hs300"));

			cnt++;
		}

		ss.str("");
		ss << "CCalcCffexFutureHs300 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
