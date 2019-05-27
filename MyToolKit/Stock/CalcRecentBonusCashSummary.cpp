#include "StdAfx.h"
#include "CalcRecentBonusCashSummary.h"

CCalcRecentBonusCashSummary::CCalcRecentBonusCashSummary(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcRecentBonusCashSummary::~CCalcRecentBonusCashSummary(void)
{
}




void CCalcRecentBonusCashSummary::Execute1(void* firstArg)
{
	CCalcRecentBonusCashSummary * sink = (CCalcRecentBonusCashSummary *)firstArg;
	sink->Execute();
}



void CCalcRecentBonusCashSummary::Execute()
{

	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_bonus_year.clear();
	m_vec_bonus_cash.clear();
	m_vec_bonus_share.clear();
	m_vec_reserve_to_common_share.clear();
	m_vec_right_record_date.clear();
	m_vec_exit_right_date.clear();
	m_vec_recent_close.clear();
	m_vec_earning_percent.clear();
	

	try
	{
		ss.str("");
		ss << " select c.code, ( select d.abbreviation from stockinfo d where d.code = c.code ) as abbreviation,"
			<< " date_format(c.bonus_year,'%Y-%m-%d') as bonus_year ,c.bonus_cash, c.bonus_share, c.reserve_to_common_share, " 
			<< " date_format(c.right_record_date,'%Y-%m-%d') as right_record_date, date_format(c.exit_right_date,'%Y-%m-%d') as exit_right_date ,"
			<< " c.recent_close, c.bonus_cash / (c.recent_close * 10) as percent"
			<< " from (select a.code, a.bonus_year, a.bonus_cash, a.bonus_share, a.reserve_to_common_share,"
			<< " a.right_record_date, a.exit_right_date,"
			<< " (select b.close  from netease_trade_daily b where a.code = b.code order by TradeDate desc limit 0,1 ) as recent_close"
			<< " FROM (select code , bonus_year, bonus_cash , bonus_share, reserve_to_common_share, right_record_date, exit_right_date" 
			<< " from 10jqka_bonus where  date_format(right_record_date, '%Y-%m-%d')>=date_format(now(), '%Y-%m-%d') ) a ) c"
			<< " ORDER BY percent desc";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				m_vec_bonus_year.insert(m_vec_bonus_year.end(), r.get<string>("bonus_year"));
			}
			catch(...)
			{
				m_vec_bonus_year.insert(m_vec_bonus_year.end(), "");
			}

			m_vec_bonus_cash.insert(m_vec_bonus_cash.end(), r.get<double>("bonus_cash"));
			m_vec_bonus_share.insert(m_vec_bonus_share.end(), r.get<double>("bonus_share"));
			m_vec_reserve_to_common_share.insert(m_vec_reserve_to_common_share.end(), r.get<double>("reserve_to_common_share"));
			m_vec_recent_close.insert(m_vec_recent_close.end(), r.get<double>("recent_close"));
			m_vec_earning_percent.insert(m_vec_earning_percent.end(), r.get<double>("percent"));

			try
			{
				m_vec_right_record_date.insert(m_vec_right_record_date.end(), r.get<string>("right_record_date"));
			}
			catch(...)
			{
				m_vec_right_record_date.insert(m_vec_right_record_date.end(), "");
			}

			try
			{
				m_vec_exit_right_date.insert(m_vec_exit_right_date.end(), r.get<string>("exit_right_date"));
			}
			catch(...)
			{
				m_vec_exit_right_date.insert(m_vec_exit_right_date.end(), "");
			}
			cnt++;
		}

		int wwwww = 1;

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}


}