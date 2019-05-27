#include "StdAfx.h"
#include "CalcGrossProfit.h"

CCalcGrossProfit::CCalcGrossProfit(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcGrossProfit::~CCalcGrossProfit(void)
{
}

void CCalcGrossProfit::Execute1(void* firstArg)
{
	CCalcGrossProfit * sink = (CCalcGrossProfit *)firstArg;
	sink->Execute();
}



void CCalcGrossProfit::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_trade_date.clear();

	m_vec_company_number.clear();
	m_vec_is_stock.clear();
	m_vec_pb.clear();
	m_vec_static_pe.clear();
	m_vec_dynamic_pe.clear();
	m_vec_dividend_yield_ratio.clear();

	try{
		
		session sql(g_MysqlPool);
		sql << "Call CalculateAllStockRecentGrossProfit()";
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select right(concat('000000',code),6) as code, DATE_FORMAT(report_date,'%Y-%m-%d') as report_date,  "
			" (select abbreviation from stockinfo  where code = a.code ) as name,  "
			" profit_quarter, profit_rate_quarter,profit_rate_4_quarter, profit_rate_change_for_last, profit_rate_change_for_average "
			" from gross_profit a where code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m] ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ";
		if(m_StartDate.m_dt>0 && m_EndDate.m_dt > m_StartDate.m_dt)
		{
			ss << " and report_date between \'" << m_StartDate.Format("%Y-%m-%d") << "\' and \'" << m_EndDate.Format("%Y-%m-%d") << "\' " ;
		}
		ss << " ORDER BY code, report_date ";
		
		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			
			//	标准的输出

			if(m_vec_request_code.size()>1)
			{
				//	多股的单个指标比较
				if(title_code.empty())
				{
					title_code = m_str_request_zb_code;
					title_name = "序号";
				}

				char tmp[128];
				sprintf_s(tmp, 128, "%s(%s)", /*Utf8_GBK*/( r.get<string>("name")).c_str(), r.get<string>("code").c_str());

				sj_code.insert(sj_code.end(), r.get<string>("report_date"));
				zb_code.insert(zb_code.end(), r.get<string>("code"));
				zb_name.insert(zb_name.end(), /*Utf8_GBK*/( r.get<string>("name")));
				if(m_str_request_zb_code=="profit_quarter")
					data.insert(data.end(), GetDoubleValue(r, "profit_quarter"));
				if(m_str_request_zb_code=="profit_rate_quarter")
					data.insert(data.end(), GetDoubleValue(r, "profit_rate_quarter"));
				if(m_str_request_zb_code=="profit_rate_4_quarter")
					data.insert(data.end(), GetDoubleValue(r, "profit_rate_4_quarter"));
				if(m_str_request_zb_code=="profit_rate_change_for_last")
					data.insert(data.end(), GetDoubleValue(r, "profit_rate_change_for_last"));
				if(m_str_request_zb_code=="profit_rate_change_for_average")
					data.insert(data.end(), GetDoubleValue(r, "profit_rate_change_for_average"));

				unit.insert(unit.end(), "%");
			}
			else
			{
				//	单股的多个指标展示
				if(title_code.empty())
				{
					title_code =  r.get<string>("code");
					title_name =  /*Utf8_GBK*/(r.get<string>("name"));
				}

				AddPoint( r.get<string>("code"),"本季度毛利率",  r.get<string>("report_date"),  "%", GetDoubleValue(r, "profit_rate_quarter"));
				AddPoint( r.get<string>("code"),"前4季度（含本季）平均毛利率",  r.get<string>("report_date"),  "%", GetDoubleValue(r, "profit_rate_4_quarter"));
				AddPoint( r.get<string>("code"),"同上一季度毛利增长比率",  r.get<string>("report_date"),  "%", GetDoubleValue(r, "profit_rate_change_for_last"));
				AddPoint( r.get<string>("code"),"同前4个季度毛利增长比率", r.get<string>("report_date"),  "%", GetDoubleValue(r, "profit_rate_change_for_average"));
			}
			

			cnt++;
		}

		ss.str("");
		ss << "CCalcGrossProfit 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
