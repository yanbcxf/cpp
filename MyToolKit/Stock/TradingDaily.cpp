#include "StdAfx.h"
#include "TradingDaily.h"

CTradingDaily::CTradingDaily(void)
{
}

CTradingDaily::~CTradingDaily(void)
{
	m_vec_report_date.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_close.clear();
	m_vec_volume.clear();
	m_vec_adj_close.clear();
}


int CTradingDaily::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"Date",
		"Open",
		"High",
		"Low",
		"Close",
		"Volume",
		"Adj Close"
	};

	if(arrayHeader.size()!=7)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<7; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}

	stringstream ss;
	try{
		ss << "select count(*) as cnt from TradingDaily "
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
			ss << "CTradingDaily::CheckReport 已存在相关记录，导入取消.";
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



void CTradingDaily::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
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
	m_vec_adj_close.clear();

	try
	{
		ss << "CTradingDaily ParseCsvFile。。。。。。\r\n";
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
			m_vec_adj_close[row]=6<colnum ? String2Double(arrayData[row][6]):0;
		}

		ss.str("");
		ss << "OK, CTradingDaily ParseCsvFile 完毕.\r\n";
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

void CTradingDaily::ParseXmlElement(void)
{
}

bool CTradingDaily::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("table\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(6, pos1-6);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CTradingDaily::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CTradingDaily::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			ss.str("");
			ss << "insert into TradingDaily "
				<< " select " <<  Int2String(m_nCode) << ", \'" 
				<< m_vec_report_date[row] << "\', "
				<< Double2String(m_vec_open[row])  << ", "
				<< Double2String(m_vec_high[row]) << ", "
				<< Double2String(m_vec_low[row]) << ", "
				<< Double2String(m_vec_close[row]) << ", "
				<< Double2String(m_vec_volume[row]) << ", "
				<< Double2String(m_vec_adj_close[row]) << " "

				<< " from dual where not exists (SELECT 1 from TradingDaily " 
				<< " where TradeDate=\'" << m_vec_report_date[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ) ";

			//string sql = ss.str();
			sql << ss.str(); 

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_report_date.size() ;
		ss << " OK, CTradingDaily::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CTradingDaily::NewCopy()
{
	CTradingDaily * pTrading = new CTradingDaily();
	pTrading->m_nRowNum = m_nRowNum;
	pTrading->m_vec_open = m_vec_open;
	pTrading->m_vec_high = m_vec_high;
	pTrading->m_vec_low = m_vec_low;
	pTrading->m_vec_close = m_vec_close;
	pTrading->m_vec_volume = m_vec_volume;
	pTrading->m_vec_adj_close = m_vec_adj_close;
	pTrading->m_vec_report_date = m_vec_report_date;

	return pTrading;
}



string CTradingDaily::GetDownloadUrl()
{
	string url = "";
	char cmdline[512]; 
	CString tmp;   

	if(m_nCode == 600004)
	{
		int wwww = 1;
	}

	//	对于本日已经成功下载的 ，忽略该任务
	if(CheckDownloadFileInfo(GetCsvFileName())>0)
		return url;

	
	//	根据数据库，确定开始的下载时间点
	string mysqlLastTradeDate = LastTradeDateFromDatabase();
	if(mysqlLastTradeDate.empty())
	{
		//	数据库中无记录，则下载所有交易记录
		if(m_nCode<600000)
		{
			tmp.Format("wget.exe  http://table.finance.yahoo.com/table.csv?s=%06d.sz -t 3 -T 10  -O %s", \
				m_nCode, GetCsvFileName().c_str());    
		}
		else
		{
			tmp.Format("wget.exe  http://table.finance.yahoo.com/table.csv?s=%06d.ss -t 3 -T 10  -O %s", \
				m_nCode, GetCsvFileName().c_str());    
		}
	}
	else
	{
		COleDateTime odt ;
		odt.ParseDateTime(mysqlLastTradeDate.c_str());
		// http://table.finance.yahoo.com/table.csv?s=300067.sz&a=3&b=21&c=2015

		if(m_nCode<600000)
		{
			tmp.Format("wget.exe  http://table.finance.yahoo.com/table.csv?s=%06d.sz&a=%d&b=%d&c=%d -t 3 -T 10  -O %s", \
				m_nCode, odt.GetMonth()-1, odt.GetDay(),odt.GetYear(), GetCsvFileName().c_str());    
		}
		else
		{
			tmp.Format("wget.exe  http://table.finance.yahoo.com/table.csv?s=%06d.ss&a=%d&b=%d&c=%d -t 3 -T 10  -O %s", \
				m_nCode, odt.GetMonth()-1, odt.GetDay(),odt.GetYear(), GetCsvFileName().c_str());    
		}
	}
	
	
	sprintf(cmdline,"%s",tmp);    
	url = string(cmdline);
	return url;
}


string CTradingDaily::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists TradingDaily (Code int ,TradeDate date, Open DECIMAL(10,2), High DECIMAL(10,2), "
		<< "Low DECIMAL(10,2), Close DECIMAL(10,2), Volume int, Adj_Close DECIMAL(10,2),  PRIMARY KEY ( Code, TradeDate))";
	return ss.str();
}

string  CTradingDaily::SaveAsCsv()
{
	return GetCsvFileName();
}

string CTradingDaily::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/TradingDaily/TradingDaily(%06d)_.csv" ,g_strCurrentDir.c_str(),  m_nCode);
	return string(tmp.GetBuffer());
}


void CTradingDaily::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "\n 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, TradeDate, Open, High, Low, Close, Volume, Adj_Close"
			<< " from TradingDaily where Code=" << Int2String(m_nCode)
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
			m_vec_adj_close[cnt] = r.get<double>("Adj_Close");

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


string CTradingDaily::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select max(TradeDate) as maxdate "
			<< " from TradingDaily where Code=" << Int2String(m_nCode) ;

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

void CTradingDaily::NotifyDatabaseImportedToWindow()
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