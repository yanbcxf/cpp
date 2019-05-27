#include "StdAfx.h"
#include "HexunResearchPaper.h"
#include "stock.h"
#include "UrlUtils.h"


#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CHexunResearchPaperHtmlParser: public CHtmlParser
{
public:
	CHexunResearchPaperHtmlParser(tree<HTML::Node> &dom):CHtmlParser(dom)
	{
		m_ok =false;
		m_abstract = "";
	}

	~CHexunResearchPaperHtmlParser()
	{
		
	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int div, table, tr, td, p, script;
		p = -1;
		script = -1;
		sscanf_s(xpath.c_str(), "/p[%d]/script[%d]", &p, &script);
			
		if(val.find("查看本报告全文")!=string::npos)
			m_ok = true;

		if(m_ok==false && p>=3 && script<0)
			m_abstract += val;		
	}

public:
	string m_abstract;
	
	bool	m_ok;
	
	
};


//////////////////////////////////////////////////////////////////////////

CHexunResearchPaper::CHexunResearchPaper(void)
{
}

CHexunResearchPaper::~CHexunResearchPaper(void)
{
}



string CHexunResearchPaper::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists hexun_research_paper (Code int , report_date date, title varchar(256), "
		<< " author varchar(32), source varchar(32), grade varchar(32), abstract varchar(2000),  "
		<< " url varchar(256), initial_year int, first_eps decimal(10,3), second_eps decimal(10,3)," 
		<< " third_eps decimal(10,3) , PRIMARY KEY ( Code, report_date, title))";
	return ss.str();
}


string CHexunResearchPaper::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select max(report_date) as maxdate "
			<< " from hexun_research_paper where Code=" << Int2String(m_nCode) ;

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			{
				std::tm  tm = r.get<std::tm>("maxdate");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				lastTradeDate = ss.str();
				break;
			}

			cnt++;
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << "和讯研究报告不存在. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	return lastTradeDate;
}


string  CHexunResearchPaper::SaveAsCsv()
{
	stringstream ss;

	m_vec_source.clear();
	m_vec_report_date.clear();
	m_vec_grade.clear();
	m_vec_title.clear();
	m_vec_abstract.clear();
	m_vec_url.clear();

	
	try
	{
		//	根据数据库，确定开始的下载时间点

		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string mysqlLastTradeDate = sdd.hexun_research_paper;
		if(mysqlLastTradeDate.empty() || mysqlLastTradeDate == "0000-00-00")
			mysqlLastTradeDate = LastTradeDateFromDatabase();
		if(mysqlLastTradeDate.empty())
			mysqlLastTradeDate = "2014-01-01";

		ss.str("");
		ss << "http://yanbao.stock.hexun.com/Handle/Json_dzyb_list.aspx?code=";
		ss << Int2String(m_nCode) << "&page=1";
		string test = ss.str();
		string  strGBK=CUrlUtils::PostUrl(ss.str().c_str(),NULL);

		vector<string> vecMatch;
		if(1 /*Pcre2Grep(_T("\\("), strGBK.c_str() ,vecMatch )>0*/)
		{
			strGBK = strGBK.substr(1);
			strGBK = strGBK.substr(0, strGBK.length()-1);
			strGBK = ReplaceString(strGBK, "list:", "\"list\":");
			Sleep(10);
			strGBK = ReplaceString(strGBK, "title:", "\"title\":");
			strGBK = ReplaceString(strGBK, "date:", "\"date\":");
			strGBK = ReplaceString(strGBK, "jgurl:", "\"jgurl\":");
			Sleep(10);
			strGBK = ReplaceString(strGBK, "jgname:", "\"jgname\":");
			strGBK = ReplaceString(strGBK, "type:", "\"type\":");
			strGBK = ReplaceString(strGBK, "id:", "\"id\":");
			Sleep(10);
			strGBK = ReplaceString(strGBK, "more:", "\"more\":");
			strGBK = ReplaceString(strGBK, "url:", "\"url\":");


			Json::Value jsonRoot;
			Json::Reader reader(Json::Features::strictMode());
			bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
			if (!parsingSuccessful) {
				ss.str("");
				ss << "和讯研究报告，解析 JSON 错误 ：";
				ss << reader.getFormattedErrorMessages().c_str();
				ss << "\r\n";
				sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
			}
			else
			{
				Json::Value json_result = jsonRoot["list"];
				if(json_result.type()== Json::arrayValue)
				{
					int size = json_result.size();
					if(size>0)
					{
						for(int index = size-1; index>=0; index--)
						{
							Json::Value  struction = json_result[index];
							string title = struction["title"].asString();
							string url = "http://yanbao.stock.hexun.com/" + struction["url"].asString();
							string grade = struction["type"].asString();
							string source = struction["jgname"].asString();
							string report_date = struction["date"].asString();
							string abstract1 = "";

							/*倒数第一条记录有可能时错误的*/
							if(index==size-1 && grade=="--")
								continue;

							COleDateTime odt ;
							odt.ParseDateTime(mysqlLastTradeDate.c_str());

							COleDateTime odt1 ;
							odt1.ParseDateTime(report_date.c_str());

							if(odt1.m_dt<= odt.m_dt) 
								continue;

							ss.str("");
							ss << "和讯研究报告.下载摘要 ： " << title;
							ss << " \r\n";
							sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
									
							/*获取研报的摘要*/
							try
							{
								string  strGBK=CUrlUtils::PostUrlOfSouhu(url,NULL);

								HTML::ParserDom parser;
								parser.parse(strGBK);
								tree<HTML::Node> dom = parser.getTree();

								tree<HTML::Node>::iterator it = dom.begin();
								tree<HTML::Node>::iterator end = dom.end();
								for (; it != end; ++it)
								{
									if (it->tagName() =="div")
									{
										it->parseAttributes();

										map<string,string> attrs = it->attributes();
										if(attrs.size()>0 && attrs["class"] == "yj_bglc")
										{
											break;
										}

									}
								}

								if(it!=end)
								{
									CHexunResearchPaperHtmlParser parser(dom);
									parser.Parse(it, "");
									abstract1 = parser.m_abstract;
									abstract1 = ReplaceChar(abstract1, '\'', "\"");
								}
								else
								{
									ss.str("");
									ss << "和讯研究报告.解析摘要错误 ： " << title;
									ss << " \r\n";
									sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
									continue;
								}
							}
							catch(...)
							{
								ss.str("");
								ss << "和讯研究报告.解析摘要错误 ： " << title;
								ss << " \r\n";
								sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
								continue;
							}

							m_vec_report_date.push_back(report_date);
							m_vec_source.push_back(source);
							m_vec_grade.push_back(grade);
							m_vec_title.push_back(title);
							m_vec_url.push_back(url);
							m_vec_abstract.push_back(abstract1);
							
						}
						ss.str("");
						ss << "和讯研究报告，获取 JSON 数据成功. \r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						return "AlreadyInMemory";
					}
				}
				else
				{
					ss.str("");
					ss << "和讯研究报告，获取 JSON 中的 Result 数据失败. \r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
				}

			}
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}

	return "";
}


void CHexunResearchPaper::ImportToDatabase(void)
{
	if(m_nCode<=0|| m_vec_report_date.size() ==0)
		return;

	stringstream ss;
	
	try
	{
		ss << "CHexunResearchPaper::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.hexun_research_paper;

		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			ss.str("");
			ss << "insert into hexun_research_paper ("
				<< "code, report_date, title, source, grade, abstract, url )"
				<< " select " <<  Int2String(m_nCode) << ", \'" 
				<< m_vec_report_date[row] << "\', \'"
				<< m_vec_title[row] << "\', \'"
				<< m_vec_source[row] << "\', \'"
				<< m_vec_grade[row] << "\', \'"
				<< m_vec_abstract[row] << "\',\'"
				<< m_vec_url[row] << "\'  "

				<< " from dual where not exists (SELECT 1 from hexun_research_paper " 
				<< " where report_date=\'" << m_vec_report_date[row] << "\' " 
				<< " and title=\'" << m_vec_title[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ) ";

			string test = ss.str();
			vecSqlStr.insert(vecSqlStr.end(), test);

			if(maxDate < m_vec_report_date[row])
				maxDate = m_vec_report_date[row];
		}
		
		if(vecSqlStr.size()>0)
		{
			session sql(g_MysqlPool);

			ss.str("");
			ss << SqlScript();
			sql << ss.str();

			ss.str("");
			ss << "delete from hexun_research_paper "
				<< " where report_date>\'" << sdd.hexun_research_paper << "\' " 
				<< " and Code=" << Int2String(m_nCode) ;
			sql << ss.str();

			for(int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "hexun_research_paper", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CHexunResearchPaper::ImportToDatabase 数据导入完毕. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



void CHexunResearchPaper::NotifyDatabaseImportedToWindow()
{
	stringstream ss;
	/*ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ss << "<root>";
	ss << "<msg_type code=\"" << m_nCode << "\" ";
	ss << " model=\"" << ModelType() << "\" ";
	ss << " >DatabaseImported</msg_type>";
	ss << "<data " ;
	if(m_vec_close.size()>0)
	{
		ss << " param1=\"" << m_vec_report_date[0] << "\" ";
		ss << " param2=\"" << Double2String(m_vec_close[0]) << "\" ";
	}
	ss << " >";
	ss << "</data></root>";*/

	//sendToOutput(ss.str().c_str(), m_hWnd);
}
