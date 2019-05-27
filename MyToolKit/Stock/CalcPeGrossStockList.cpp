#include "StdAfx.h"
#include "CalcPeGrossStockList.h"

CCalcPeGrossStockList::CCalcPeGrossStockList(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcPeGrossStockList::~CCalcPeGrossStockList(void)
{
}




void CCalcPeGrossStockList::Execute1(void* firstArg)
{
	CCalcPeGrossStockList * sink = (CCalcPeGrossStockList *)firstArg;
	sink->Execute();
}



void CCalcPeGrossStockList::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();


	try{
		/* 计算最近的市盈率 PE */
		session sql(g_MysqlPool);
		sql << "call CalculateAllStockRecentPe()";
		sql << "call add_ind('pedaily','pe_gross','idx_pe_gross')";

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select h.code, h.abbreviation, h.min_pe, h.min_date, h.recent_date, h.recent_pe ,(h.recent_pe - h.min_pe) as different " 
			<< "from " 
			<< "( select c.code , c.abbreviation, c.min_pe, " 
			<< "( select max(trade_date) from pedaily d where c.code = d.code and c.min_pe = d.pe_gross) as min_date, "
			<< "c.recent_date , "
			<< "( select pe_gross from pedaily d where c.code = d.code and d.trade_date = c.recent_date ) as recent_pe "
			<< " from "
			<< "( select code , abbreviation , " 
			<< "( select min(pe_gross) from pedaily b where b.code = a.code ) as min_pe ,"
			<< "( select max(e.trade_date) from pedaily e where e.code = a.code ) as recent_date "
			<< "from stockinfo a where  code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << ") ) c ) h   ORDER BY different ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("min_pe");
				r.get<double>("recent_pe");
				r.get<double>("different");

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
				std::tm  tm = r.get<std::tm>("min_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_min_date.insert(m_vec_min_date.end(), ss.str());
			}
			catch(...)
			{
				m_vec_min_date.insert(m_vec_min_date.end(), "");
			}
			
			try
			{
				std::tm  tm = r.get<std::tm>("recent_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_recent_date.insert(m_vec_recent_date.end(), ss.str());
			}
			catch(...)
			{
				m_vec_recent_date.insert(m_vec_recent_date.end(), "");
			}

			m_vec_min_pe.insert(m_vec_min_pe.end(), r.get<double>("min_pe"));
			m_vec_recent_pe.insert(m_vec_recent_pe.end(), r.get<double>("recent_pe"));
			m_vec_defferent.insert(m_vec_defferent.end(), r.get<double>("different"));

			cnt++;
		}

		ss.str("");
		ss << "毛利润市盈率 PE 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
