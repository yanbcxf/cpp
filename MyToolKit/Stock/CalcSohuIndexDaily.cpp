#include "StdAfx.h"
#include "CalcSohuIndexDaily.h"

CCalcSohuIndexDaily::CCalcSohuIndexDaily(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcSohuIndexDaily::~CCalcSohuIndexDaily(void)
{
}


void CCalcSohuIndexDaily::Execute1(void* firstArg)
{
	CCalcSohuIndexDaily * sink = (CCalcSohuIndexDaily *)firstArg;
	sink->Execute();
}



void CCalcSohuIndexDaily::Execute()
{
	/*汇总各个股指*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	m_vec_code.clear();
	m_vec_trade_date.clear();

	m_vec_turnover.clear();
	m_vec_close.clear();
	m_vec_volume.clear();

	try{
		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, date_format(TradeDate, '%Y-%m-%d') as trade_date, "
			<< " Turnover, Close, Volume "
			<< " from sohuindexdaily a where code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) and TradeDate >'2007-1-1' ORDER BY trade_date, code ";

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
				r.get<double>("Close");
				r.get<int>("Volume");
				r.get<int>("Code");

			}
			catch(...)
			{
				continue;
			}

			int code = r.get<int>("Code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_trade_date.insert(m_vec_trade_date.end(), r.get<string>("trade_date"));

			m_vec_turnover.insert(m_vec_turnover.end(), r.get<double>("Turnover")/10000);
			m_vec_close.insert(m_vec_close.end(), r.get<double>("Close"));
			m_vec_volume.insert(m_vec_volume.end(), r.get<int>("Volume"));

			cnt++;
		}

		ss.str("");
		ss << "CCalcSohuIndexDaily 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
