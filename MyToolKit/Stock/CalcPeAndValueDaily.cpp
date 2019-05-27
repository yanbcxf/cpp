#include "StdAfx.h"
#include "CalcPeAndValueDaily.h"

CCalcPeAndValueDaily::CCalcPeAndValueDaily(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcPeAndValueDaily::~CCalcPeAndValueDaily(void)
{
}




void CCalcPeAndValueDaily::Execute1(void* firstArg)
{
	CCalcPeAndValueDaily * sink = (CCalcPeAndValueDaily *)firstArg;
	sink->Execute();
}



void CCalcPeAndValueDaily::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	

	try{
		
		session sql(g_MysqlPool);
		//////////////////////////////////////////////////////////////////////////

		try{
			ss.str("");
			ss << "delete from pedaily where code in (";
			for(int m=0; m< m_vec_request_code.size(); m++)
			{
				ss <<  m_vec_request_code[m]   ;

				if(m!=m_vec_request_code.size()-1)
					ss << ",";
			}
			ss << " ) ";
			sql << ss.str();
		}catch(...)
		{

		}
		

		//////////////////////////////////////////////////////////////////////////

		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss.str("");
			ss << "call CalculatePe(" <<  m_vec_request_code[m] << ",'2000-01-01')";
			sql << ss.str();
		}
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "  select right(concat('000000',a.code),6) as code, "
			<< "( select b.abbreviation from stockinfo b where a.code = b.code ) as name, "
			<< " date_format(trade_date, '%Y-%m-%d') as trade_date, pe, pe_gross, total_value "
			<< " from pedaily a where code  in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss <<  m_vec_request_code[m] ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ";
		if(m_StartDate.m_dt>0 && m_EndDate.m_dt > m_StartDate.m_dt)
		{
			ss << " and tradedate between \'" << m_StartDate.Format("%Y-%m-%d").GetBuffer() << "\' and \'" << m_EndDate.Format("%Y-%m-%d").GetBuffer() << "\' " ;
		}
		ss << " ORDER BY code, trade_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{
				r.get<double>("pe");
				r.get<double>("total_value");
			}
			catch(...)
			{
				continue;
			}

			
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
				if(m_str_request_zb_code=="pe")
					data.insert(data.end(),r.get<double>("pe"));
				if(m_str_request_zb_code=="pe_gross")
					data.insert(data.end(),r.get<double>("pe_gross"));
				if(m_str_request_zb_code=="total_value")
					data.insert(data.end(),r.get<double>("total_value")/10000);
				
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
				AddPoint(r.get<string>("code"),"净利润PE", r.get<string>("trade_date"), "", r.get<double>("pe"));
				AddPoint(r.get<string>("code"),"毛利润PE", r.get<string>("trade_date"), "", r.get<double>("pe_gross"));
				AddPoint(r.get<string>("code"),"总市值", r.get<string>("trade_date"), "", r.get<double>("total_value")/10000 );
				
			}
			cnt++;
		}

		ss.str("");
		ss << "CCalcPeAndValueDaily 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
