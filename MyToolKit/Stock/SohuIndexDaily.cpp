#include "StdAfx.h"
#include "SohuIndexDaily.h"

#include "UrlUtils.h"

CSohuIndexDaily::CSohuIndexDaily(void)
{
}

CSohuIndexDaily::~CSohuIndexDaily(void)
{
	m_vec_report_date.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_close.clear();
	m_vec_volume.clear();

	m_vec_change.clear();
	m_vec_change_rate.clear();
	m_vec_turnover.clear();
}


void CSohuIndexDaily::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "SohuIndexDaily::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		//	先从最早的日期开始插入
		for (int row = m_vec_report_date.size()-1; row >= 0; row--)
		{
			ss.str("");
			ss << "insert into SohuIndexDaily "
				<< " select " <<  Int2String(m_nCode) << ", \'" 
				<< m_vec_report_date[row] << "\', "
				<< Double2String(m_vec_open[row])  << ", "
				<< Double2String(m_vec_high[row]) << ", "
				<< Double2String(m_vec_low[row]) << ", "
				<< Double2String(m_vec_close[row]) << ", "
				<< Double2String(m_vec_volume[row]) << ", "
				<< Double2String(m_vec_change[row]) << ", "
				<< Double2String(m_vec_change_rate[row]) << ", "
				<< Double2String(m_vec_turnover[row]) << "  "
				//<< Double2String(m_vec_turnover_rate[row]) << " "

				<< " from dual where not exists (SELECT 1 from SohuIndexDaily " 
				<< " where TradeDate=\'" << m_vec_report_date[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ) ";

			//string sql = ss.str();
			sql << ss.str(); 

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_report_date.size() ;
		ss << " OK, SohuIndexDaily::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


string CSohuIndexDaily::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SohuIndexDaily (Code int ,TradeDate date, Open DECIMAL(10,2), High DECIMAL(10,2), "
		<< "Low DECIMAL(10,2), Close DECIMAL(10,2), Volume int, Change_Amount DECIMAL(10,2), Change_Rate DECIMAL(10,2), "
		<< " Turnover DECIMAL(15,4),   PRIMARY KEY ( Code, TradeDate))";
	return ss.str();
}

string  CSohuIndexDaily::SaveAsCsv()
{
	stringstream ss;

	time_t time_start;
	time_t time_end;

	m_vec_report_date.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_close.clear();
	m_vec_volume.clear();

	m_vec_change.clear();
	m_vec_change_rate.clear();
	m_vec_turnover.clear();
	
	//	计算要下载的开始时间和 结束时间
 	string strLastTradeDate = LastTradeDateFromDatabase();
	if(!strLastTradeDate.empty())
		time_start = convert_string_to_time_t(strLastTradeDate + string(" 17:30:00"));
	else
		time_start = convert_string_to_time_t("1990-01-01 00:00:00");

	time_end = time (NULL);
	if(time_end - time_start < 24 * 3600)
		return "NoMoreData";
	time_end = CBussinessUtils::CurrentTradingDate(0);
	if(time_end - time_start < 24 * 3600)
		return "NoMoreData";
	
	//	开始下载，以最大范围 90 天为限进行
	CStdioFile  targetFile;
	bool bContinue = true;
	int  nTry = 0;
	while(bContinue)
	{
		char tmp[256] = { 0 };
		/*struct tm * curr_tm;
		curr_tm = gmtime (&time_end);*/
		struct tm curr_tm;
		localtime_s(&curr_tm, &time_end);

		snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
		string strEnd = string(tmp);
		string strStart ;
		if(time_end - time_start > 90 * 24 * 3600)
		{
			time_t time_middle = time_end - 90 * 24 * 3600;
			localtime_s(&curr_tm, &time_middle);
			// curr_tm = gmtime (&time_middle);
			snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
			strStart = string(tmp);
			time_end = time_middle;
		}
		else
		{
			bContinue = false;
			//curr_tm = gmtime (&time_start);
			localtime_s(&curr_tm, &time_start);
			snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
			strStart = string(tmp);
		}

		try
		{
			char tmpCode[10] = {0};
			sprintf(tmpCode, "%06d" ,m_nCode);
			ss.str("");
			ss << "http://q.stock.sohu.com/hisHq?code=zs_" << string(tmpCode);
			ss << "&start=" << strStart;
			ss << "&end=" << strEnd;
			ss << "&stat=0&order=D&period=d&callback=historySearchHandler&rt=jsonp&r=0.9201545790436751&0.5669214910302679";
			string test = ss.str();

			string  strGBK=CUrlUtils::PostUrlOfSouhu(ss.str().c_str(),NULL);

			vector<string> vecMatch;
			if(Pcre2Grep(_T("historySearchHandler\\(\\["), strGBK.c_str() ,vecMatch )>0)
			{
				string::size_type pos = strGBK.find(vecMatch[0]);
				strGBK = strGBK.substr(pos + vecMatch[0].length());
				strGBK = strGBK.substr(0, strGBK.length()-3);

				Json::Value jsonRoot;
				Json::Reader reader(Json::Features::strictMode());
				bool parsingSuccessful = reader.parse(strGBK, jsonRoot);
				if (!parsingSuccessful) {
					ss.str("");
					ss << "指数【"<< string(tmpCode) <<"】，解析 JSON 错误 ：";
					ss << reader.getFormattedErrorMessages().c_str();
					ss << "\r\n";
					sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
				}
				else
				{
					Json::Value json_result = jsonRoot["hq"];
					if(json_result.type()== Json::arrayValue)
					{
						int size = json_result.size();
						if(size>0)
						{
							nTry = 0;
							
							for(int index = 0; index< size; index++)
							{
								Json::Value  struction = json_result[index];
								int colSize = struction.size();

								string strReportDate =  struction[0].asString();
								string strOpen =  struction[1].asString();
								string strClose =  struction[2].asString();
								string strChange =  struction[3].asString();
								string strChangeRate =  struction[4].asString();
								string strLow =  struction[5].asString();
								string strHigh =  struction[6].asString();
								string strVolume =  struction[7].asString();
								string strTurnover =  struction[8].asString();

								m_vec_report_date.insert(m_vec_report_date.end(), strReportDate);
								m_vec_open.insert(m_vec_open.end(), String2Double(strOpen));
								m_vec_high.insert(m_vec_high.end(), String2Double(strHigh));
								m_vec_low.insert(m_vec_low.end(), String2Double(strLow));
								m_vec_close.insert(m_vec_close.end(), String2Double(strClose));
								m_vec_volume.insert(m_vec_volume.end(), String2Double(strVolume));
								m_vec_change.insert(m_vec_change.end(), String2Double(strChange));
								m_vec_change_rate.insert(m_vec_change_rate.end(), String2Double(strChangeRate));
								m_vec_turnover.insert(m_vec_turnover.end(), String2Double(strTurnover));
							
							}

							ss.str("");
							ss << "指数【"<< string(tmpCode) <<"】，获取 JSON 成功. ";
							ss <<  strStart << "-" << strEnd;
							ss << "\r\n";
							sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						}

					}
					else
					{
						ss.str("");
						ss << "指数【"<< string(tmpCode) <<"】，获取 JSON 中的 hq 数据失败. \r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
					}
				}
			}
			else
			{
				nTry++;
				if(nTry>=4)
				{
					//	每次 90 天，连续 4次无数据 （约为 1 年），则判定为到达最早交易日
					bContinue = false;
				}
			}

		}
		catch (std::exception const & e)
		{
			ss.str("");
			ss << "指数,异常【" << m_nCode << "】" << strStart << "-" << strEnd;
			ss << e.what() << ' \r\n';
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
	}

	if(m_vec_report_date.size()>0)
		return "AlreadyInMemory";
	
	return "";
}



void CSohuIndexDaily::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSohuIndexDaily 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, TradeDate, Open, High, Low, Close, Volume, Change_Amount,"
			<< " Change_Rate, Turnover "
			<< " from SohuIndexDaily where Code=" << Int2String(m_nCode)
			<< " order by TradeDate";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			{
				std::tm  tm = r.get<std::tm>("TradeDate");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_open[cnt] = r.get<double>("Open");
			m_vec_high[cnt] = r.get<double>("High");
			m_vec_low[cnt] = r.get<double>("Low");
			m_vec_close[cnt] = r.get<double>("Close");
			m_vec_volume[cnt] = r.get<int>("Volume");
			m_vec_change[cnt] = r.get<double>("Change_Amount");
			m_vec_change_rate[cnt] = r.get<double>("Change_Rate");
			m_vec_turnover[cnt] = r.get<double>("Turnover");
			//m_vec_turnover_rate[cnt] = r.get<double>("Turnover_Rate");

			cnt++;
		}


		if(cnt>0)
		{
			m_nRowNum = cnt;
		}

		ss.str("");
		ss << "OK, 从 MYSQL 获取数据完毕.";
		ss << " 共 " << cnt << "条 \r\n" ;
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


string CSohuIndexDaily::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select date_sub(max(TradeDate), INTERVAL 10 day) as maxdate "
			<< " from SohuIndexDaily where Code=" << Int2String(m_nCode) ;

		string test = ss.str();

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
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	return lastTradeDate;
}

void CSohuIndexDaily::NotifyDatabaseImportedToWindow()
{
	stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
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
	ss << "</data></root>";

	sendToOutput(ss.str().c_str(), m_hWnd);
}
