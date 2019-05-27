#include "StdAfx.h"
#include "Stock.h"

#include "NetEaseTradeDaily.h"

#include "UrlUtils.h"



CNetEaseTradeDaily::CNetEaseTradeDaily(void)
{
}

CNetEaseTradeDaily::~CNetEaseTradeDaily(void)
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

	m_vec_total_value.clear();
	m_vec_circulating_value.clear();
	m_vec_lclose.clear();
}



int CNetEaseTradeDaily::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	

	return CSV_CHECK_NO_ERROR;
}



void CNetEaseTradeDaily::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	
}

void CNetEaseTradeDaily::ParseXmlElement(void)
{
}

bool CNetEaseTradeDaily::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SouhuTradingDaily\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(18, pos1-18);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CNetEaseTradeDaily::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.NetEase_Trade_Daily;
		
		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			if(m_vec_report_date[row] > sdd.NetEase_Trade_Daily)
			{
				ss.str("");
				ss << "insert into NetEase_Trade_Daily "
					<< "(Code, TradeDate, Open, High, Low, Close, Volume, Change_Amount, Change_Rate, Turnover, Turnover_Rate, "
					<< " Total_Value, Circulating_Value, LClose )"
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
					<< Double2String(m_vec_turnover_rate[row]) << ", "

					<< Double2String(m_vec_total_value[row]) << ", "
					<< Double2String(m_vec_circulating_value[row]) << ", "
					<< Double2String(m_vec_lclose[row]) << " "

					<< " from dual where not exists (SELECT 1 from NetEase_Trade_Daily " 
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
			ss << "delete from NetEase_Trade_Daily "
				<< " where TradeDate>\'" << sdd.NetEase_Trade_Daily << "\' " 
				<< " and Code=" << Int2String(m_nCode) ;
			sql << ss.str();

			for(int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "NetEase_Trade_Daily", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CNetEaseTradeDaily::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CNetEaseTradeDaily::NewCopy()
{
	CNetEaseTradeDaily * pTrading = new CNetEaseTradeDaily();
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

	pTrading->m_vec_circulating_value = m_vec_circulating_value;
	pTrading->m_vec_total_value = m_vec_total_value;
	pTrading->m_vec_lclose = m_vec_lclose;


	return pTrading;
}



string CNetEaseTradeDaily::GetDownloadUrl()
{
	return "";
}


string CNetEaseTradeDaily::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists NetEase_Trade_Daily (Code int ,TradeDate date, Open DECIMAL(10,2), High DECIMAL(10,2), "
		<< "Low DECIMAL(10,2), Close DECIMAL(10,2), Volume int, Change_Amount DECIMAL(10,2), Change_Rate DECIMAL(10,2), "
		<< " Turnover DECIMAL(15,4),   Turnover_Rate DECIMAL(10,2), Total_Value DECIMAL(22,4), Circulating_Value DECIMAL(22,4), "
		<< " LClose DECIMAL(10,2), PRIMARY KEY ( Code, TradeDate))";
	return ss.str();
}



string  CNetEaseTradeDaily::SaveAsCsv()
{
	stringstream ss;

	time_t time_start;
	time_t time_end;

	//	计算要下载的开始时间和 结束时间

	CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
	string strLastTradeDate = sdd.NetEase_Trade_Daily;
	if(strLastTradeDate.empty() || strLastTradeDate == "0000-00-00")
		strLastTradeDate = "2014-01-01";  //LastTradeDateFromDatabase();

	if(!strLastTradeDate.empty())
		time_start = convert_string_to_time_t(strLastTradeDate + string(" 17:30:00"));
	else
		time_start = convert_string_to_time_t("1990-01-01 00:00:00");

	time_end = time (NULL);
	if(time_end - time_start < 24 * 3600)
		return "NoMoreData";

	char tmp[256] = { 0 };
	string strStart;
	struct tm curr_tm;
	localtime_s(&curr_tm, &time_start);
	snprintf (tmp, 255, "%4d%02d%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);	
	strStart = string(tmp);

	vector<string> vecTradedate  = ((CStockApp *)AfxGetApp())->m_StockDataLog.m_vecTradedate;
	string strEnd = vecTradedate[vecTradedate.size()-1];
	strEnd = ReplaceString(strEnd, "-", "");
	if(strEnd  <= strStart)
		return "NoMoreData";

	//	开始下载，以最大范围 360 天为限进行
	int  nTry = 0;
	do 
	{
		char tmpCode[10] = {0};
		sprintf(tmpCode, "%06d" ,m_nCode);
		ss.str("");
		ss << "http://quotes.money.163.com/service/chddata.html?code=";
		if(m_nCode>=600000)
			ss << "0";
		else
			ss << "1";
		ss << string(tmpCode);
		ss << "&start=" << strStart;
		ss << "&end=" << strEnd;
		ss << "&fields=TCLOSE;HIGH;LOW;TOPEN;LCLOSE;CHG;PCHG;TURNOVER;VOTURNOVER;VATURNOVER;TCAP;MCAP";
		ss << "&tttt=" << rand();
		string test = ss.str();

		string  strGBK=CUrlUtils::PostUrlOfSouhu(ss.str().c_str(),NULL);

		bool  isOK =false;
		vector<string> vecLine;
		splitString(strGBK,"\r\n" , vecLine);
		if(vecLine.size()>2)
		{
			for(int i=1; i<vecLine.size(); i++)
			{
				vector<string>	vecYield;
				splitString(vecLine[i],"," , vecYield);
				if(vecYield.size()==15)
				{
					isOK = true;
					m_vec_report_date.insert(m_vec_report_date.end(), vecYield[0]);
					m_vec_high.insert(m_vec_high.end(), String2Double(vecYield[4]));
					m_vec_low.insert(m_vec_low.end(), String2Double(vecYield[5]));
					m_vec_open.insert(m_vec_open.end(), String2Double(vecYield[6]));

					m_vec_lclose.insert(m_vec_lclose.end(), String2Double(vecYield[7]));

					//	停牌处理
					if(vecYield[8].find("None")!=string::npos)
					{
						m_vec_close.insert(m_vec_close.end(), -1);
						m_vec_change.insert(m_vec_change.end(), 0);
						m_vec_change_rate.insert(m_vec_change_rate.end(), String2Double(vecYield[9]));
					}
					else
					{
						m_vec_close.insert(m_vec_close.end(), String2Double(vecYield[3]));
						m_vec_change.insert(m_vec_change.end(), String2Double(vecYield[8]));
						m_vec_change_rate.insert(m_vec_change_rate.end(), String2Double(vecYield[9]));
					}
					m_vec_turnover_rate.insert(m_vec_turnover_rate.end(), String2Double(vecYield[10]));

					m_vec_volume.insert(m_vec_volume.end(), String2Double(vecYield[11]));
					m_vec_turnover.insert(m_vec_turnover.end(), String2Double(vecYield[12]));
					m_vec_total_value.insert(m_vec_total_value.end(), String2Double(vecYield[13]));
					m_vec_circulating_value.insert(m_vec_circulating_value.end(), String2Double(vecYield[14]));
				}
			}


		}

		if(isOK)
		{
			//	数据获取成功
			int k = m_vec_report_date.size();
			string kkk = m_vec_report_date[0];
			kkk = ReplaceString(kkk, "-", "");
			if(kkk == strEnd)
			{
				break;
			}
		}

		m_vec_report_date.clear();
		m_vec_high.clear();
		m_vec_low.clear();
		m_vec_open.clear();
		m_vec_lclose.clear();
		m_vec_close.clear();
		m_vec_change.clear();
		m_vec_change_rate.clear();
		m_vec_turnover_rate.clear();
		m_vec_volume.clear();
		m_vec_turnover.clear();
		m_vec_total_value.clear();
		m_vec_circulating_value.clear();

		Sleep(100);
		nTry++;
	} while (nTry<5);

	
	if(m_vec_report_date.size()>0)
	{
		return "AlreadyInMemory";
	}
	return "";
}


string CNetEaseTradeDaily::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/SouhuTradingDaily/SouhuTradingDaily(%06d)_.csv" ,g_strCurrentDir.c_str(),  m_nCode);
	return string(tmp.GetBuffer());
}


void CNetEaseTradeDaily::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CNetEaseTradeDaily 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, TradeDate, Open, High, Low, Close, Volume, Change_Amount,"
			<< " Change_Rate, Turnover, Turnover_Rate, LClose, Total_Value, Circulating_Value "
			<< " from NetEase_Trade_Daily where Code=" << Int2String(m_nCode);
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
				m_vec_report_date.push_back(ss.str());
			}
			m_vec_open.push_back(r.get<double>("Open", 0));
			m_vec_high.push_back(r.get<double>("High", 0));
			m_vec_low.push_back(r.get<double>("Low", 0));
			m_vec_close.push_back( r.get<double>("Close", 0));
			m_vec_volume.push_back( r.get<int>("Volume", 0));
			m_vec_change.push_back( r.get<double>("Change_Amount", 0));
			m_vec_change_rate.push_back( r.get<double>("Change_Rate", 0));
			m_vec_turnover.push_back( r.get<double>("Turnover", 0));
			m_vec_turnover_rate.push_back( r.get<double>("Turnover_Rate", 0));

			m_vec_lclose.push_back( r.get<double>("LClose", 0));
			m_vec_total_value.push_back( r.get<double>("Total_Value", 0));
			m_vec_circulating_value.push_back( r.get<double>("Circulating_Value", 0));

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


string CNetEaseTradeDaily::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "2010-01-01";
	try
	{
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select max(TradeDate) as maxdate "
			<< " from NetEase_Trade_Daily where Code=" << Int2String(m_nCode) ;

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

void CNetEaseTradeDaily::NotifyDatabaseImportedToWindow()
{
	
}


void  CNetEaseTradeDaily::Reorganize()
{
	for(int i=0; i<m_vec_report_date.size(); i++)
	{
		CNetEaseTradeDailyData data;
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