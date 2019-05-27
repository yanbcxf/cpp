#include "StdAfx.h"
#include "CalcHexunResearchPaperQuery.h"

CCalcHexunResearchPaperQuery::CCalcHexunResearchPaperQuery(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcHexunResearchPaperQuery::~CCalcHexunResearchPaperQuery(void)
{
}



void CCalcHexunResearchPaperQuery::Execute1(void* firstArg)
{
	CCalcHexunResearchPaperQuery * sink = (CCalcHexunResearchPaperQuery *)firstArg;
	sink->Execute();
}


void CCalcHexunResearchPaperQuery::Execute()
{
	/*汇总需要下载财务报表的股票*/
	stringstream ss;
	m_vec_code.clear();
	m_vec_abbreviation.clear();
	m_vec_title.clear();
	m_vec_source.clear();
	m_vec_url.clear();
	m_vec_abstract.clear();
	m_vec_grade.clear();

	m_vec_initial_year.clear();
	m_vec_first_eps.clear();
	m_vec_second_eps.clear();
	m_vec_third_eps.clear();
	m_vec_recent_close.clear();

	try{
		//////////////////////////////////////////////////////////////////////////
		/*查询总的记录数*/
		{
			ss.str("");
			ss << "SELECT count(*) as cnt " 
				<< "FROM hexun_research_paper b " 
				<< "where 1=1 " ;
			if(m_vec_request_code.size()>0)
			{
				ss << " and code in ( ";
				for(int i=0; i< m_vec_request_code.size(); i++)
				{
					ss << m_vec_request_code[i];
					if(i<m_vec_request_code.size()-1)
						ss << ",";
				}
				ss << ")";
			}

			if(m_after_report_date.empty()==false)
			{
				ss << " and report_date >= \'" << m_after_report_date << "\' " ;
			}

			if(m_request_like.empty()==false)
			{
				ss << " and abstract like \'%" << m_request_like << "%\' "; 
			}
			ss << " ORDER BY report_date desc";

			string test = ss.str();
			//test = Gbk2Utf8(test);

			int cnt;
			session sql(g_MysqlPool);
			sql << test, into(cnt);
			m_max_page = cnt /1000;
			if(cnt%1000 >0)
				m_max_page++;

		}
	
		
		if(m_page_no>m_max_page) m_page_no = m_max_page;
		if(m_page_no<1) m_page_no =1;


		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "SELECT b.code, (select max(a.abbreviation) from stockinfo a where a.code = b.code ) as abbreviation ," 
			<< "b.report_date, b.title, b.source, b.grade,  b.url, b.abstract, "
			<< "b.initial_year, b.first_eps, b.second_eps, b.third_eps, "
			<< " (select e.close from  netease_trade_daily e where " 
			<< " e.code = b.code and e.TradeDate = (select max(d.TradeDate) "  
			<< " from netease_trade_daily d where b.code = d.code )) as recent_close "
			<< "FROM hexun_research_paper b " 
			<< "where 1=1 " ;
		if(m_vec_request_code.size()>0)
		{
			ss << " and code in ( ";
			for(int i=0; i< m_vec_request_code.size(); i++)
			{
				ss << m_vec_request_code[i];
				if(i<m_vec_request_code.size()-1)
					ss << ",";
			}
			ss << ")";
		}
		
		if(m_after_report_date.empty()==false)
		{
			ss << " and report_date >= \'" << m_after_report_date << "\' " ;
		}

		if(m_request_like.empty()==false)
		{
			ss << " and abstract like \'%" << m_request_like << "%\' "; 
		}
		ss << " ORDER BY report_date desc ";
		ss << " limit " << ((m_page_no - 1) * 1000) << ", 1000";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		vector<int>	vecCalculateEps;
		int cnt = 0;
		while (st.fetch())
		{
			int code = r.get<int>("code");
			m_vec_code.insert(m_vec_code.end(), code);
			m_vec_abbreviation.insert(m_vec_abbreviation.end(), /*Utf8_GBK*/( r.get<string>("abbreviation")));

			try
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date.insert(m_vec_report_date.end(), ss.str());
			}
			catch(...)
			{
				m_vec_report_date.insert(m_vec_report_date.end(), "");
			}

			m_vec_title.insert(m_vec_title.end(), /*Utf8_GBK*/( r.get<string>("title")));
			m_vec_source.insert(m_vec_source.end(), /*Utf8_GBK*/( r.get<string>("source")));
			m_vec_grade.insert(m_vec_grade.end(), /*Utf8_GBK*/( r.get<string>("grade")));
			m_vec_url.insert(m_vec_url.end(), /*Utf8_GBK*/( r.get<string>("url")));
			m_vec_abstract.insert(m_vec_abstract.end(), /*Utf8_GBK*/( r.get<string>("abstract")));

			try
			{
				m_vec_initial_year.insert(m_vec_initial_year.end(), r.get<int>("initial_year"));
			}
			catch(...)
			{
				m_vec_initial_year.insert(m_vec_initial_year.end(), 0);
				vecCalculateEps.push_back(cnt);
			}

			try
			{
				m_vec_first_eps.insert(m_vec_first_eps.end(), r.get<double>("first_eps"));
			}
			catch(...)
			{
				m_vec_first_eps.insert(m_vec_first_eps.end(), 0);
			}

			try
			{
				m_vec_second_eps.insert(m_vec_second_eps.end(), r.get<double>("second_eps"));
			}
			catch(...)
			{
				m_vec_second_eps.insert(m_vec_second_eps.end(), 0);
			}

			try
			{
				m_vec_third_eps.insert(m_vec_third_eps.end(), r.get<double>("third_eps"));
			}
			catch(...)
			{
				m_vec_third_eps.insert(m_vec_third_eps.end(), 0);
			}
		
			try
			{
				m_vec_recent_close.insert(m_vec_recent_close.end(), r.get<double>("recent_close"));
			}
			catch(...)
			{
				m_vec_recent_close.insert(m_vec_recent_close.end(), 0);
			}

			cnt++;
		}

		/*从研报中抽取 EPS （每股收益）*/
		for(int i=0; i<vecCalculateEps.size(); i++)
		{
			int k = vecCalculateEps[i];
			string val;
			vector<string> firstMatch;

			int nLastPosition;
			string str1 = FindLastString(m_vec_abstract[k],"每股收益分别", nLastPosition);
			if(str1.empty())
				str1 = FindLastString(m_vec_abstract[k],"每股收益为", nLastPosition);
			if(str1.empty())
				str1 = FindLastString(m_vec_abstract[k],"EPS分别", nLastPosition);
			if(str1.empty())
				str1 = FindLastString(m_vec_abstract[k],"EPS", nLastPosition);
			if(str1.empty())
				str1 = FindLastString(m_vec_abstract[k],"eps", nLastPosition);

			if(str1.empty()==false)
			{
				val = str1.substr(0,64);
				val =  ReplaceString(val, "元/股", "/");
				val =  ReplaceString(val, "、", "/");
				val =  ReplaceString(val, "和", "/");
				val =  ReplaceString(val, "及", "/");
				val =  ReplaceString(val, "与", "/");
				val =  ReplaceString(val, "，", "/");
				val =  ReplaceString(val, "元", "/");

				string left = "";
				firstMatch.clear();
				if(Pcre2Grep(_T("[\\d\\.\\-]+[元]?/+[\\d\\.\\-]+[元]?/+[\\d\\.\\-]+[元]?"), val.c_str(), firstMatch )>0)
				{
					val = firstMatch[0];
					left = m_vec_abstract[k].substr(0, nLastPosition);

				}
				else if(Pcre2Grep(_T("[\\d\\.\\-]+[元]?/+[\\d\\.\\-]+[元]?"), val.c_str(), firstMatch )>0)
				{
					val = firstMatch[0];
					left = m_vec_abstract[k].substr(0, nLastPosition);
				}
				else 
					val = "";

				if(left.empty()==false)
				{
					string left1 =  ReplaceString(left, "。", "!@#$");
					left1 = ReplaceString(left1, "预计", "!@#$");
					left1 =  FindLastString(left1,"!@#$", nLastPosition);
					if(left1.empty()==false) left = left1;

					left =  ReplaceString(left, "~", "_");
					left =  ReplaceString(left, "、", "_");
					left =  ReplaceString(left, "，", "_");
					left =  ReplaceString(left, "/", "_");
					left =  ReplaceString(left, "-", "_");  
					left =  ReplaceString(left, "～", "_");
					left =  ReplaceString(left, "年", "_");

					firstMatch.clear();
					if(Pcre2Grep(_T("[\\d]{2,}[_]{1}"), left.c_str(), firstMatch )>0)
					{
						val = (firstMatch[0].length()>=4 ? firstMatch[0] : ("20" + firstMatch[0])) + ":" + val;
					}

				}
			}
			else
			{
				val = "";
			}

			if(val.empty()==false)
			{
				val =  ReplaceString(val, "_", "");
				val =  ReplaceString(val, "///", "/");
				val =  ReplaceString(val, "//", "/");

				firstMatch.clear();
				if(Pcre2Split(_T("[\\:]+"), val.c_str(), firstMatch )>0)
				{
					m_vec_initial_year[k] = atoi(firstMatch[0].c_str());
					val = firstMatch[1];
				}

				firstMatch.clear();
				if(Pcre2Split(_T("[/]+"), val.c_str(), firstMatch )>0)
				{
					if(firstMatch.size()>=1)
						m_vec_first_eps[k] = String2Double(firstMatch[0]);
					if(firstMatch.size()>=2)
						m_vec_second_eps[k] = String2Double(firstMatch[1]);
					if(firstMatch.size()>=3)
						m_vec_third_eps[k] = String2Double(firstMatch[2]);
				}

				/*保存到数据库中*/
				if(m_vec_initial_year[k]>0)
				{
					session sql(g_MysqlPool);

					ss.str("");
					ss << "update hexun_research_paper set initial_year= " << Int2String(m_vec_initial_year[k]);
					ss << ", first_eps=" << Double2String(m_vec_first_eps[k]);
					ss << ", second_eps=" << Double2String(m_vec_second_eps[k]);
					ss << ", third_eps=" << Double2String(m_vec_third_eps[k]);
					ss << "  where report_date=\'" << m_vec_report_date[k] << "\' " 
						<< " and title=\'" << m_vec_title[k] << "\' " 
						<< " and Code=" << Int2String(m_vec_code[k]) << "  ";

					string test = ss.str();
					//test = Gbk2Utf8(test);
					sql << test; 
				}
			}
		}

		/*计算今年研报预测的 pe 值*/
		COleDateTime t = COleDateTime::GetCurrentTime();
		m_vec_today_pe.clear();
		int nowYear = t.GetYear();
		for(int i=0; i< m_vec_code.size(); i++)
		{
			if(m_vec_initial_year[i]==nowYear && m_vec_first_eps[i]>0)
			{
				m_vec_today_pe.insert(m_vec_today_pe.end(), m_vec_recent_close[i]/m_vec_first_eps[i]);

			}
			else if(m_vec_initial_year[i]==nowYear-1 && m_vec_second_eps[i]>0)
			{
				m_vec_today_pe.insert(m_vec_today_pe.end(), m_vec_recent_close[i]/m_vec_second_eps[i]);
			}
			else
			{
				m_vec_today_pe.insert(m_vec_today_pe.end(), 3000);
			}
		}

		ss.str("");
		ss << "和讯研报查询完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

