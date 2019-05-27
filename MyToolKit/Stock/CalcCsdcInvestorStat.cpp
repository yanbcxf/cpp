#include "StdAfx.h"
#include "CalcCsdcInvestorStat.h"

CCalcCsdcInvestorStat::CCalcCsdcInvestorStat(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCsdcInvestorStat::~CCalcCsdcInvestorStat(void)
{
}




void CCalcCsdcInvestorStat::Execute1(void* firstArg)
{
	CCalcCsdcInvestorStat * sink = (CCalcCsdcInvestorStat *)firstArg;
	sink->Execute();
}



void CCalcCsdcInvestorStat::Execute()
{

	stringstream ss;
	begin_date.clear();
	end_date.clear();

	final_a_natural_investor.clear();
	final_a_legal_investor.clear();
	final_a_position_investor.clear();
	period_a_trading_investor.clear();
	
	try{

		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select  date_format(begin_date, '%Y-%m-%d') as begin_date , date_format(end_date, '%Y-%m-%d') as end_date , "
			<< "final_a_natural_investor, final_a_legal_investor, "
			<< "final_a_position_investor, period_a_trading_investor " 
			<< "from csdcinvestorstat ORDER BY end_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<string>("begin_date");
				r.get<string>("end_date");
				r.get<double>("final_a_natural_investor");
				r.get<double>("final_a_legal_investor");
				r.get<double>("final_a_position_investor");
				r.get<double>("period_a_trading_investor");
			}
			catch(...)
			{
				continue;
			}

			begin_date.insert(begin_date.end(),  r.get<string>("begin_date"));
			end_date.insert(end_date.end(), r.get<string>("end_date"));

			final_a_natural_investor.insert(final_a_natural_investor.end(), r.get<double>("final_a_natural_investor"));
			final_a_legal_investor.insert(final_a_legal_investor.end(), r.get<double>("final_a_legal_investor"));

			final_a_position_investor.insert(final_a_position_investor.end(), r.get<double>("final_a_position_investor"));
			period_a_trading_investor.insert(period_a_trading_investor.end(), r.get<double>("period_a_trading_investor"));
			
			cnt++;
		}

		ss.str("");
		ss << "CCalcCsdcInvestorStat 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
