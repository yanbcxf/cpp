#include "StdAfx.h"
#include "CalcCsindex.h"

CCalcCsindex::CCalcCsindex(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcCsindex::~CCalcCsindex(void)
{
}




void CCalcCsindex::Execute1(void* firstArg)
{
	CCalcCsindex * sink = (CCalcCsindex *)firstArg;
	sink->Execute();
}



void CCalcCsindex::Execute()
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
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, name, trade_date, is_stock,"
			<< "company_number,  pb, static_pe, dynamic_pe, dividend_yield_ratio from ( "
			<< "( select right(concat('000000',code),6) as code, name, "
			<< " date_format(tradedate, '%Y-%m-%d') as trade_date, is_stock, "
			<< " company_number,  pb, static_pe, dynamic_pe, dividend_yield_ratio "
			<< " from csipedaily a where ( code, is_stock )  in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << "(" << m_vec_request_code[m]  << "," << m_vec_request_is_stock[m] << " )" ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ";
		if(m_StartDate.m_dt>0 && m_EndDate.m_dt > m_StartDate.m_dt)
		{
			ss << " and tradedate between \'" << m_StartDate.Format("%Y-%m-%d") << "\' and \'" << m_EndDate.Format("%Y-%m-%d") << "\' " ;
		}
		ss << " ORDER BY code, is_stock, trade_date )";
		ss << " UNION "
			<< "( select code, name, date_format(tradedate, '%Y-%m-%d') as trade_date, is_stock, "
			<< " company_number,  pb, static_pe, dynamic_pe, dividend_yield_ratio "
			<< " from csicsrcpedaily a where ( code, is_stock )  in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << "(" << m_vec_request_code[m]  << "," << m_vec_request_is_stock[m] << " )" ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ";
		if(m_StartDate.m_dt>0 && m_EndDate.m_dt > m_StartDate.m_dt)
		{
			ss << " and tradedate between \'" << m_StartDate.Format("%Y-%m-%d").GetBuffer() << "\' and \'" << m_EndDate.Format("%Y-%m-%d").GetBuffer() << "\' " ;
		}
		ss << " ORDER BY code, is_stock, trade_date )"
			<< " ) e  ORDER BY e.code, e.is_stock, e.trade_date";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("pb");
				r.get<double>("static_pe");
				r.get<double>("dynamic_pe");
				r.get<double>("dividend_yield_ratio");
				r.get<int>("is_stock");
				r.get<int>("company_number");
			}
			catch(...)
			{
				continue;
			}

			m_vec_code.insert(m_vec_code.end(), r.get<string>("code"));
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("name")));

			try
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), r.get<string>("trade_date"));
			}
			catch(...)
			{
				m_vec_trade_date.insert(m_vec_trade_date.end(), "");
			}

			m_vec_pb.insert(m_vec_pb.end(), r.get<double>("pb"));
			m_vec_static_pe.insert(m_vec_static_pe.end(), r.get<double>("static_pe"));
			m_vec_dividend_yield_ratio.insert(m_vec_dividend_yield_ratio.end(), r.get<double>("dividend_yield_ratio"));
			m_vec_dynamic_pe.insert(m_vec_dynamic_pe.end(), r.get<double>("dynamic_pe"));
			m_vec_is_stock.insert(m_vec_is_stock.end(),r.get<int>("is_stock"));
			m_vec_company_number.insert(m_vec_company_number.end(),r.get<int>("company_number"));

			//	标准的输出

			if(m_vec_request_code.size()>1)
			{
				//	多股的单个指标比较
				if(title_code.empty())
				{
					title_code = m_str_request_zb_code;
					title_name = m_str_request_zb_code;
				}

				char tmp[128];
				sprintf_s(tmp, 128, "%s(%s)", /*Utf8_GBK*/( r.get<string>("name")).c_str(), r.get<string>("code").c_str());

				sj_code.insert(sj_code.end(), r.get<string>("trade_date"));
				zb_code.insert(zb_code.end(), r.get<string>("code"));
				zb_name.insert(zb_name.end(), string(tmp));
				if(m_str_request_zb_code=="DynamicPe")
					data.insert(data.end(),r.get<double>("dynamic_pe"));
				if(m_str_request_zb_code=="StaticPe")
					data.insert(data.end(),r.get<double>("static_pe"));
				if(m_str_request_zb_code=="Pb")
					data.insert(data.end(),r.get<double>("pb"));
				if(m_str_request_zb_code=="Company_Number")
					data.insert(data.end(),r.get<double>("company_number"));
				if(m_str_request_zb_code=="Dividend_Yield_Ratio")
					data.insert(data.end(),r.get<double>("dividend_yield_ratio"));

				unit.insert(unit.end(), "");
			}
			else
			{
				//	单股的多个指标展示
				if(title_code.empty())
				{
					title_code =  r.get<string>("code");
					title_name =  /*Utf8_GBK*/(r.get<string>("name"));
				}

				AddPoint( r.get<string>("code"),"滚动PE", r.get<string>("trade_date"), "", r.get<double>("dynamic_pe"));
				AddPoint( r.get<string>("code"),"静态PE", r.get<string>("trade_date"),  "", r.get<double>("static_pe") );
				AddPoint( r.get<string>("code"),"市净率", r.get<string>("trade_date"),  "", r.get<double>("pb"));
				AddPoint( r.get<string>("code"),"股息率", r.get<string>("trade_date"),  "", r.get<double>("dividend_yield_ratio"));
			}
			

			cnt++;
		}

		ss.str("");
		ss << "CCalcCsindex 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
