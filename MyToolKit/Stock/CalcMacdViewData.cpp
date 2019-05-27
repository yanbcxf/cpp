#include "StdAfx.h"
#include "CalcMacdViewData.h"

CCalcMacdViewData::CCalcMacdViewData(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMacdViewData::~CCalcMacdViewData(void)
{
}

void CCalcMacdViewData::Execute1(void* firstArg)
{
	CCalcMacdViewData * sink = (CCalcMacdViewData *)firstArg;
	sink->Execute();
}


void CCalcMacdViewData::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_trade_date.clear();
	m_vec_total_value.clear();
	m_vec_ema12.clear();
	m_vec_ema26.clear();
	m_vec_dif.clear();
	m_vec_dea.clear();

	try{
		/* 计算最近的市盈率 PE */
		session sql(g_MysqlPool);
		// sql << "call CalculateAllStockRecentMACD()";

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select tradedate, ema12, ema26, dif, dea from daily_macd  where code =";
		ss << "\'" << m_nCode << "\' ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			
			try
			{
				std::tm  tm = r.get<std::tm>("tradedate");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_trade_date.insert(m_vec_trade_date.end(), ss.str());
			}
			catch(...)
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), "");
			}

			//m_vec_total_value.insert(m_vec_total_value.end(), r.get<double>("total_value"));
			m_vec_ema12.insert(m_vec_ema12.end(), r.get<double>("ema12"));
			m_vec_ema26.insert(m_vec_ema26.end(), r.get<double>("ema26"));
			m_vec_dif.insert(m_vec_dif.end(), r.get<double>("dif"));
			m_vec_dea.insert(m_vec_dea.end(), r.get<double>("dea"));

			cnt++;
		}

		ss.str("");
		ss << "MACD 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
