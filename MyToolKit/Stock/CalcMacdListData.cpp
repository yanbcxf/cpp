#include "StdAfx.h"
#include "CalcMacdListData.h"

CCalcMacdListData::CCalcMacdListData(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMacdListData::~CCalcMacdListData(void)
{
}




void CCalcMacdListData::Execute1(void* firstArg)
{
	CCalcMacdListData * sink = (CCalcMacdListData *)firstArg;
	sink->Execute();
}



void CCalcMacdListData::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_date1.clear();
	m_vec_date2.clear();
	m_vec_dif1.clear();
	m_vec_dif2.clear();
	m_vec_dea1.clear();
	m_vec_dea2.clear();

	try{
		/* MACD 统计  */
		session sql(g_MysqlPool);
		// sql << "call CalculateAllStockRecentMACD()";

		/*计算金叉和死叉*/
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select d.code, d.abbreviation, d.date1, d.date2," 
			"(select dif from macd a where a.code = d.code  and a.trade_date = d.date1 ) as  dif1,"
			"(select dea from macd a where a.code = d.code  and a.trade_date = d.date1 ) as  dea1,"
			"(select dif from macd a where a.code = d.code  and a.trade_date = d.date2 ) as  dif2,"
			"(select dea from macd a where a.code = d.code  and a.trade_date = d.date2 ) as  dea2 "
			"from  (select c.code, c.abbreviation, c.date1, "
			"RecentTradeDateForMACD(c.code, date_sub(c.date1, interval 1 day)) as date2 "
			"from (select code , abbreviation, RecentTradeDateForMACD(a.code, now()) as date1 "
			"from stockinfo a ) c ) d ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{

			try{
				r.get<double>("dif1");
				r.get<double>("dif2");
				r.get<double>("dea1");
				r.get<double>("dea2");

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
				std::tm  tm = r.get<std::tm>("date1");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_date1.insert(m_vec_date1.end(), ss.str());
			}
			catch(...)
			{
				m_vec_date1.insert(m_vec_date1.end(), "");
			}

			try
			{
				std::tm  tm = r.get<std::tm>("date2");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_date2.insert(m_vec_date2.end(), ss.str());
			}
			catch(...)
			{
				m_vec_date2.insert(m_vec_date2.end(), "");
			}

			m_vec_dif1.insert(m_vec_dif1.end(), r.get<double>("dif1"));
			m_vec_dif2.insert(m_vec_dif2.end(), r.get<double>("dif2"));
			m_vec_dea1.insert(m_vec_dea1.end(), r.get<double>("dea1"));
			m_vec_dea2.insert(m_vec_dea2.end(), r.get<double>("dea2"));

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
