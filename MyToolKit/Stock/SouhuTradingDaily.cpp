#include "StdAfx.h"
#include "Stock.h"

#include "SouhuTradingDaily.h"

#include "UrlUtils.h"



CSouhuTradingDaily::CSouhuTradingDaily(void)
{
}

CSouhuTradingDaily::~CSouhuTradingDaily(void)
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
	m_vec_turnover_rate.clear();
}



int CSouhuTradingDaily::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"Date",
		"Open",
		"High",
		"Low",
		"Close",
		"Volume",
		"Change",
		"Change_Rate",
		"Turnover",
		"Turnover_Rate"
	};

	if(arrayHeader.size()!=10)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<10; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}

	stringstream ss;
	try{
		ss << "select count(*) as cnt from SouhuTradingDaily "
			<< " where TradeDate>=\'" << arrayData[arrayData.size()-1][0] << "\' " 
			<< " and TradeDate<=\'" << arrayData[0][0] << "\' "
			<< " and Code=" << Int2String(m_nCode) << "  ";
		string test = ss.str();

		row r;
		session sql(g_MysqlPool);
		sql << ss.str(), into(r);
		assert(r.size() == 1);
		long long existCnt = r.get<long long>("cnt") ;
		if(existCnt == arrayData.size())
		{
			ss.str("");
			ss << "CSouhuTradingDaily::CheckReport 已存在相关记录，导入取消.";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			return CSV_CHECK_DATA_EXIST_ERROR;
		}
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

	return CSV_CHECK_NO_ERROR;
}



void CSouhuTradingDaily::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	/*string connectString = "dbname=stock  port=3306   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();*/

	m_vec_report_date.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_close.clear();
	m_vec_volume.clear();
	
	m_vec_change.clear();
	m_vec_change_rate.clear();
	m_vec_turnover.clear();
	m_vec_turnover_rate.clear();

	try
	{
		ss << "CSouhuTradingDaily ParseCsvFile。。。。。。\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			m_vec_report_date[row]=arrayData[row][0];
			m_vec_open[row]= 1<colnum ? String2Double(arrayData[row][1]):0;
			m_vec_high[row]= 2<colnum ? String2Double(arrayData[row][2]):0;
			m_vec_low[row]= 3<colnum ? String2Double(arrayData[row][3]):0;
			m_vec_close[row]= 4<colnum ? String2Double(arrayData[row][4]):0;
			m_vec_volume[row]=5<colnum ? String2Double(arrayData[row][5]):0;
			m_vec_change[row]=6<colnum ? String2Double(arrayData[row][6]):0;
			m_vec_change_rate[row]=7<colnum ? String2Double(arrayData[row][7].substr(0, arrayData[row][7].length()-1)):0;
			m_vec_turnover[row]=8<colnum ? String2Double(arrayData[row][8]):0;
			m_vec_turnover_rate[row]=9<colnum ? String2Double(arrayData[row][9].substr(0,arrayData[row][9].length()-1)):0;

		}

		ss.str("");
		ss << "OK, CSouhuTradingDaily ParseCsvFile 完毕.\r\n";
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

void CSouhuTradingDaily::ParseXmlElement(void)
{
}

bool CSouhuTradingDaily::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SouhuTradingDaily\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(18, pos1-18);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CSouhuTradingDaily::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.SouhuTradingDaily;
		
		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			if(m_vec_report_date[row] > sdd.SouhuTradingDaily)
			{
				ss.str("");
				ss << "insert into SouhuTradingDaily "
					<< "(Code, TradeDate, Open, High, Low, Close, Volume, Change_Amount, Change_Rate, Turnover, Turnover_Rate)"
					<< " select " <<  Int2String(m_nCode) << ", \'" 
					<< m_vec_report_date[row] << "\', "
					<< Double2String(m_vec_open[row])  << ", "
					<< Double2String(m_vec_high[row]) << ", "
					<< Double2String(m_vec_low[row]) << ", "
					<< Double2String(m_vec_close[row]) << ", "
					<< Double2String(m_vec_volume[row]) << ", "
					<< Double2String(m_vec_change[row]) << ", "
					<< Double2String(m_vec_change_rate[row]) << ", "
					<< Double2String(m_vec_turnover[row]) << ", "
					<< Double2String(m_vec_turnover_rate[row]) << " "

					<< " from dual where not exists (SELECT 1 from SouhuTradingDaily " 
					<< " where TradeDate=\'" << m_vec_report_date[row] << "\' " 
					<< " and Code=" << Int2String(m_nCode) << "  ) ";

				string test = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), test);

				if(maxDate < m_vec_report_date[row])
					maxDate = m_vec_report_date[row];
			}
		}

		if(vecSqlStr.size()>0)
		{
			session sql(g_MysqlPool);

			ss.str("");
			ss << SqlScript();
			sql << ss.str();

			ss.str("");
			ss << "delete from SouhuTradingDaily "
				<< " where TradeDate>\'" << sdd.SouhuTradingDaily << "\' " 
				<< " and Code=" << Int2String(m_nCode) ;
			sql << ss.str();

			for(int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "SouhuTradingDaily", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CSouhuTradingDaily::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CSouhuTradingDaily::NewCopy()
{
	CSouhuTradingDaily * pTrading = new CSouhuTradingDaily();
	pTrading->m_nRowNum = m_nRowNum;

	pTrading->m_vec_open = m_vec_open;
	pTrading->m_vec_high = m_vec_high;
	pTrading->m_vec_low = m_vec_low;
	pTrading->m_vec_close = m_vec_close;
	pTrading->m_vec_volume = m_vec_volume;

	pTrading->m_vec_change = m_vec_change;
	pTrading->m_vec_change_rate = m_vec_change_rate;
	pTrading->m_vec_turnover = m_vec_turnover;
	pTrading->m_vec_turnover_rate = m_vec_turnover_rate;

	pTrading->m_vec_report_date = m_vec_report_date;

	return pTrading;
}



string CSouhuTradingDaily::GetDownloadUrl()
{
	return "";
}


string CSouhuTradingDaily::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SouhuTradingDaily (Code int ,TradeDate date, Open DECIMAL(10,2), High DECIMAL(10,2), "
		<< "Low DECIMAL(10,2), Close DECIMAL(10,2), Volume int, Change_Amount DECIMAL(10,2), Change_Rate DECIMAL(10,2), "
		<< " Turnover DECIMAL(15,4),   Turnover_Rate DECIMAL(10,2),  PRIMARY KEY ( Code, TradeDate))";
	return ss.str();
}



string  CSouhuTradingDaily::SaveAsCsv()
{
	stringstream ss;

	time_t time_start;
	time_t time_end;

	if(m_nCode == 300181)
	{
		int wwww = 1;

	}

	//	计算要下载的开始时间和 结束时间

	CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
	string strLastTradeDate = sdd.SouhuTradingDaily;
	if(strLastTradeDate.empty())
		strLastTradeDate = LastTradeDateFromDatabase();

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
	bool bWriteTitle = true;
	int  nTry = 0;
	int  nTotalRow = 0;
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
			//curr_tm = gmtime (&time_middle);
			localtime_s(&curr_tm, &time_middle);
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
			ss << "http://q.stock.sohu.com/hisHq?code=cn_" << string(tmpCode);
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
					ss << "搜狐交易数据，解析 JSON 错误 ：";
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
							if(bWriteTitle)
							{
								bWriteTitle = false;
							}	

							for(int index = 0; index< size; index++)
							{
								Json::Value  struction = json_result[index];
								int colSize = struction.size();

								m_vec_report_date[nTotalRow]= struction[0].asString();
								m_vec_open[nTotalRow]=  String2Double(struction[1].asString());
								m_vec_high[nTotalRow]=  String2Double(struction[6].asString());
								m_vec_low[nTotalRow]=  String2Double(struction[5].asString());
								m_vec_close[nTotalRow]=  String2Double(struction[2].asString());
								m_vec_volume[nTotalRow]= String2Double(struction[7].asString());
								m_vec_change[nTotalRow]= String2Double(struction[3].asString());
								m_vec_change_rate[nTotalRow]= String2Double(struction[4].asString());
								m_vec_turnover[nTotalRow]= String2Double(struction[8].asString());
								m_vec_turnover_rate[nTotalRow]= String2Double(struction[9].asString());

								nTotalRow++;
							}

							ss.str("");
							ss << "搜狐交易数据，获取 JSON 成功. ";
							ss << "【" << m_nCode << "】" << strStart << "-" << strEnd;
							// sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						}

					}
					else
					{
						ss.str("");
						ss << "搜狐交易数据，获取 JSON 中的 hq 数据失败. \r\n";
						sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);

						nTry++;
						if(nTry>=5)
						{
							//	连续 5次无数据 ，说明可能时退市股票，没有数据
							bContinue = false;
						}
					}
				}
			}
			else
			{
				nTry++;
				if(nTry>=20)
				{
					//	每次 90 天，连续 20次无数据 （约为 5 年），则判定为到达最早交易日
					bContinue = false;
				}
			}
		}
		catch (std::exception const & e)
		{
			ss.str("");
			ss << "搜狐交易数据,异常【" << m_nCode << "】" << strStart << "-" << strEnd;
			ss << e.what() << ' \r\n';
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
	}

	if(bWriteTitle==false)
	{
		//	bWriteTitle == false, 说明本次成功下载了数据
		
		return "AlreadyInMemory";
	}
	return "";
}

string  CSouhuTradingDaily::SaveAsCsv_Old()
{
	stringstream ss;

	time_t time_start;
	time_t time_end;
	
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
	bool bWriteTitle = true;
	int  nTry = 0;
	while(bContinue)
	{
		char tmp[256] = { 0 };
		struct tm * curr_tm;
		curr_tm = gmtime (&time_end);
		snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm->tm_year, curr_tm->tm_mon+1, curr_tm->tm_mday);	
		string strEnd = string(tmp);
		string strStart ;
		if(time_end - time_start > 90 * 24 * 3600)
		{
			time_t time_middle = time_end - 90 * 24 * 3600;
			curr_tm = gmtime (&time_middle);
			snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm->tm_year, curr_tm->tm_mon+1, curr_tm->tm_mday);	
			strStart = string(tmp);
			time_end = time_middle;
		}
		else
		{
			bContinue = false;
			curr_tm = gmtime (&time_start);
			snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm->tm_year, curr_tm->tm_mon+1, curr_tm->tm_mday);	
			strStart = string(tmp);
		}


		try
		{
			char tmpCode[10] = {0};
			sprintf(tmpCode, "%06d" ,m_nCode);
			ss.str("");
			ss << "http://q.stock.sohu.com/hisHq?code=cn_" << string(tmpCode);
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
					ss << "搜狐交易数据，解析 JSON 错误 ：";
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
							if(bWriteTitle)
							{
								string sssss = GetCsvFileName();
								BOOL b = targetFile.Open(GetCsvFileName().c_str(),\
									CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);

								if(b==FALSE)
									return "";

								//	第一次循环时，才输出 CSV 文件的标题头
								ss.str("");
								ss << "Date," << "Open," << "High," << "Low," << "Close,"; 
								ss << "Volume," << "Change," << "Change_Rate," << "Turnover," << "Turnover_Rate" << "\n";
								targetFile.WriteString(ss.str().c_str());
								bWriteTitle = false;
							}	
							

							for(int index = 0; index< size; index++)
							{
								Json::Value  struction = json_result[index];
								int colSize = struction.size();

								ss.str("");
								ss << struction[0] << "," << struction[1] << "," << struction[6] << "," << struction[5] << ",";
								ss << struction[2] << "," << struction[7] << "," << struction[3] << "," << struction[4] << ",";
								ss << struction[8] << "," << struction[9] << "\n";
								string test = ss.str();
								targetFile.WriteString(ss.str().c_str());
							}

							ss.str("");
							ss << "搜狐交易数据，获取 JSON 成功. ";
							ss << "【" << m_nCode << "】" << strStart << "-" << strEnd;
							sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
						}

					}
					else
					{
						ss.str("");
						ss << "搜狐交易数据，获取 JSON 中的 hq 数据失败. \r\n";
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
			ss << "搜狐交易数据,异常【" << m_nCode << "】" << strStart << "-" << strEnd;
			ss << e.what() << ' \r\n';
			sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
		}
	}
	
	if(bWriteTitle==false)
	{
		//	bWriteTitle == false, 说明本次成功下载了数据
		targetFile.Flush();
		targetFile.Close();
		return GetCsvFileName();
	}
	return "";
}

string CSouhuTradingDaily::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/SouhuTradingDaily/SouhuTradingDaily(%06d)_.csv" ,g_strCurrentDir.c_str(),  m_nCode);
	return string(tmp.GetBuffer());
}


void CSouhuTradingDaily::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSouhuTradingDaily 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, TradeDate, Open, High, Low, Close, Volume, Change_Amount,"
			<< " Change_Rate, Turnover, Turnover_Rate "
			<< " from SouhuTradingDaily where Code=" << Int2String(m_nCode);
		if(endTime > startTime)
		{
			COleDateTime start, end;
			start.m_dt = startTime;
			end.m_dt = endTime;
			ss << " and TradeDate >= \'" << start.Format("%Y-%m-%d") << "\'";
			ss << " and TradeDate <= \'" << end.Format("%Y-%m-%d") << "\'";
		}
		ss	<< " order by TradeDate";

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
			m_vec_turnover_rate[cnt] = r.get<double>("Turnover_Rate");

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


string CSouhuTradingDaily::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select max(TradeDate) as maxdate "
			<< " from SouhuTradingDaily where Code=" << Int2String(m_nCode) ;

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

void CSouhuTradingDaily::NotifyDatabaseImportedToWindow()
{
	/*stringstream ss;
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

	sendToOutput(ss.str().c_str(), m_hWnd);*/
}


void  CSouhuTradingDaily::Reorganize()
{
	for(int i=0; i<m_vec_report_date.size(); i++)
	{
		CSouhuTradingDailyData data;
		data.open = m_vec_open[i];
		data.high = m_vec_high[i];
		data.low = m_vec_low[i];
		data.close = m_vec_close[i];
		data.volume = m_vec_volume[i];
		data.change = m_vec_change[i];
		data.change_rate = m_vec_change_rate[i];
		data.turnover = m_vec_turnover[i];
		data.turnover_rate = m_vec_turnover_rate[i];

		m_vec_reorganize[m_vec_report_date[i]] = data;
	}

	/*m_vec_report_date.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_close.clear();
	m_vec_volume.clear();

	m_vec_change.clear();
	m_vec_change_rate.clear();
	m_vec_turnover.clear();
	m_vec_turnover_rate.clear();*/
}