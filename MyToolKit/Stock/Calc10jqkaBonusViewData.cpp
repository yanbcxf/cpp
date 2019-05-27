#include "StdAfx.h"
#include "Calc10jqkaBonusViewData.h"

CCalc10jqkaBonusViewData::CCalc10jqkaBonusViewData(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalc10jqkaBonusViewData::~CCalc10jqkaBonusViewData(void)
{
}




void CCalc10jqkaBonusViewData::Execute1(void* firstArg)
{
	CCalc10jqkaBonusViewData * sink = (CCalc10jqkaBonusViewData *)firstArg;
	sink->Execute();
}



void CCalc10jqkaBonusViewData::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_right_record_date.clear();
	m_vec_exit_right_date.clear();
	m_vec_bonus_cash.clear();
	m_vec_bonus_share.clear();
	m_vec_reserve_to_common_share.clear();

	try{
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select DATE_FORMAT(right_record_date, '%Y-%m-%d') as right_record_date, "
			<< " DATE_FORMAT(exit_right_date, '%Y-%m-%d') as exit_right_date, " 
			<< " bonus_cash, bonus_share, reserve_to_common_share from 10jqka_bonus " 
		    << " where code = " << m_nCode; 
		ss << " and exit_right_date > right_record_date " 
			<< " order by right_record_date " ; 
		
		string test = ss.str();
		//test = Gbk2Utf8(test);

		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{

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

			//m_vec_price.insert(m_vec_price.end(), r.get<double>("price"));
			m_vec_bonus_cash.insert(m_vec_bonus_cash.end(), r.get<double>("bonus_cash"));
			m_vec_bonus_share.insert(m_vec_bonus_share.end(), r.get<double>("bonus_share"));
			m_vec_reserve_to_common_share.insert(m_vec_reserve_to_common_share.end(), r.get<double>("reserve_to_common_share"));

			cnt++;
		}

		ss.str("");
		ss << "个股历年分红 10jqkaBonus 计算完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

