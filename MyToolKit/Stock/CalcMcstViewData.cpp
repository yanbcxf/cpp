#include "StdAfx.h"
#include "CalcMcstViewData.h"

CCalcMcstViewData::CCalcMcstViewData(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcMcstViewData::~CCalcMcstViewData(void)
{
}



void CCalcMcstViewData::Execute1(void* firstArg)
{
	CCalcMcstViewData * sink = (CCalcMcstViewData *)firstArg;
	sink->Execute();
}



void CCalcMcstViewData::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_trade_date.clear();
	//m_vec_price.clear();
	m_vec_cost_price.clear();
	m_vec_mcst_price.clear();
	
	try{
		/* 计算最近的市盈率 PE */
		session sql(g_MysqlPool);
		sql << "call CalculateAllStockRecentMCST()";

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select date_format(trade_date, '%Y-%m-%d') as trade_date," 
			<< " cost_price, mcst_price  from MCST where code =";
		ss << m_nCode << " ";

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
				m_vec_trade_date.insert(m_vec_trade_date.end(), r.get<string>("trade_date"));
			}
			catch(...)
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), "");
			}

			//m_vec_price.insert(m_vec_price.end(), r.get<double>("price"));
			m_vec_cost_price.insert(m_vec_cost_price.end(), r.get<double>("cost_price"));
			m_vec_mcst_price.insert(m_vec_mcst_price.end(), r.get<double>("mcst_price"));
		
			cnt++;
		}

		ss.str("");
		ss << "MCST 成本价计算完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

