#include "StdAfx.h"
#include "CalcTechnicalAnalysisRoadMap.h"

CCalcTechnicalAnalysisRoadMap::CCalcTechnicalAnalysisRoadMap(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcTechnicalAnalysisRoadMap::~CCalcTechnicalAnalysisRoadMap(void)
{
}

void CCalcTechnicalAnalysisRoadMap::Execute1(void* firstArg)
{
	CCalcTechnicalAnalysisRoadMap * sink = (CCalcTechnicalAnalysisRoadMap *)firstArg;
	sink->Execute();
}



void CCalcTechnicalAnalysisRoadMap::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	

	try{
		
		session sql(g_MysqlPool);
				
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select  code, date_format(tradedate, '%Y-%m-%d') as tradedate,  "
			" index_name, basic_desc, extra_desc, close "
			" from  tech_analysis_event  a where  a.code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << "\'" << m_vec_request_code[m] << "\'" ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ";
		if(m_StartDate.m_dt>0 && m_EndDate.m_dt > m_StartDate.m_dt)
		{
			ss << " and a.tradedate between \'" << m_StartDate.Format("%Y-%m-%d") 
				<< "\' and \'" << m_EndDate.Format("%Y-%m-%d") << "\' " ;
		}
		if(m_str_request_zb_code.empty()==false)
		{
			ss << " and a.basic_desc like \'%" << m_str_request_zb_code << "%\'";
		}
		ss << " ORDER BY code, tradedate ";
		
		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			
			//	标准的输出
			string secCode = r.get<string>("code");
			string report_date = r.get<string>("tradedate");
			string strKey = secCode+ " 元_" + report_date;

			CTipRecord tipRecord;
			tipRecord.secCode = secCode;
			tipRecord.report_date = report_date;
			tipRecord.id = r.get<string>("index_name");
			tipRecord.title = r.get<string>("basic_desc");
			tipRecord.url = r.get<string>("extra_desc");
			
			map<string,list<CTipRecord>>::iterator it_tip = tip.find(strKey);
			if(it_tip!=tip.end())
				it_tip->second.push_back(tipRecord);
			else
			{
				list<CTipRecord>  lst;
				lst.push_back(tipRecord);
				tip[strKey] = lst;
			}
	
			cnt++;
		}


		//////////////////////////////////////////////////////////////////////////

		ss.str("");
		ss << "select right(concat('000000',a.code),6) as code, date_format(a.TradeDate, '%Y-%m-%d') as TradeDate, a.Close, "
			" a.Open, a.Low, a.High "
			" from netease_trade_daily  a where  a.code in ( ";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m] ;

			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss << " ) ";
		if(m_StartDate.m_dt>0 && m_EndDate.m_dt > m_StartDate.m_dt)
		{
			ss << " and a.TradeDate between \'" << m_StartDate.Format("%Y-%m-%d") 
				<< "\' and \'" << m_EndDate.Format("%Y-%m-%d") << "\' " ;
		}
		ss << " ORDER BY code, TradeDate ";

		test = ss.str();
		//test = Gbk2Utf8(test);

		st = (sql.prepare << test,into(r)) ;
		st.execute();

		cnt = 0;
		while (st.fetch())
		{
			AddPoint( r.get<string>("code"), r.get<string>("code"),  r.get<string>("TradeDate"), "元", 
				r.get<double>("Close"), r.get<double>("Open"), r.get<double>("High"), r.get<double>("Low"));
		}

		ss.str("");
		ss << "CCalcTechnicalAnalysisRoadMap 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
