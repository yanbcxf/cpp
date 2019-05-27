#include "StdAfx.h"
#include "CalcMarketMarginGuaranteeFund.h"

CCalcMarketMarginGuaranteeFund::CCalcMarketMarginGuaranteeFund(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMarketMarginGuaranteeFund::~CCalcMarketMarginGuaranteeFund(void)
{
}





void CCalcMarketMarginGuaranteeFund::Execute1(void* firstArg)
{
	CCalcMarketMarginGuaranteeFund * sink = (CCalcMarketMarginGuaranteeFund *)firstArg;
	sink->Execute();
}



void CCalcMarketMarginGuaranteeFund::Execute()
{
	/*融资融券担保资金*/


	stringstream ss;
	m_vec_begin_date.clear();
	m_vec_end_date.clear();
	m_vec_average_fund.clear();

	m_vec_final_fund.clear();
	m_vec_input_fund.clear();
	m_vec_output_fund.clear();
	m_vec_net_fund.clear();
	m_vec_sum_volume.clear();
	m_vec_sum_turnover.clear();
	m_vec_begin_index.clear();
	m_vec_end_index.clear();

	try{

		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select date_format(begin_date, '%Y-%m-%d') as begin_date, date_format(end_date, '%Y-%m-%d') as end_date, "
			<< " average_fund, final_fund, input_fund, output_fund, net_fund, "
			<< " (select sum(volume)    from sohuindexdaily  b where b.TradeDate >= a.begin_date and b.TradeDate <=a.end_date and b.Code = 1 ) as sum_volume, "
			<< " (select sum(turnover)  from sohuindexdaily  b where b.TradeDate >= a.begin_date and b.TradeDate <=a.end_date and b.Code = 1 ) as sum_turnover, "
			<< " (select close  from sohuindexdaily  b where b.TradeDate = a.begin_date  and b.Code = 1 ) as begin_index, "
			<< " (select close  from sohuindexdaily  b where b.TradeDate = a.end_date and b.Code = 1 ) as end_index"
			<< " from fundinsettlement a where fund_type='融资融券' order by end_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("input_fund");
				r.get<double>("output_fund");
				r.get<double>("sum_volume");
				r.get<double>("sum_turnover");
				r.get<double>("begin_index");
				r.get<double>("end_index");
			}
			catch(...)
			{
				continue;
			}

			m_vec_begin_date.insert(m_vec_begin_date.end(),  r.get<string>("begin_date"));
			m_vec_end_date.insert(m_vec_end_date.end(), r.get<string>("end_date"));

			m_vec_final_fund.insert(m_vec_final_fund.end(), r.get<double>("final_fund"));
			m_vec_average_fund.insert(m_vec_average_fund.end(), r.get<double>("average_fund"));

			m_vec_input_fund.insert(m_vec_input_fund.end(), r.get<double>("input_fund"));
			m_vec_output_fund.insert(m_vec_output_fund.end(), r.get<double>("output_fund"));
			m_vec_net_fund.insert(m_vec_net_fund.end(), r.get<double>("net_fund"));
			m_vec_sum_turnover.insert(m_vec_sum_turnover.end(), r.get<double>("sum_turnover"));
			m_vec_sum_volume.insert(m_vec_sum_volume.end(),r.get<double>("sum_volume"));
			m_vec_begin_index.insert(m_vec_begin_index.end(),r.get<double>("begin_index"));
			m_vec_end_index.insert(m_vec_end_index.end(),r.get<double>("end_index"));

			cnt++;
		}

		ss.str("");
		ss << "CCalcMarketMarginGuaranteeFund 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
