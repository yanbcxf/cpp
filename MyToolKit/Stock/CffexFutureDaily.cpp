#include "StdAfx.h"
#include "CffexFutureDaily.h"

#include "UrlUtils.h"

CCffexFutureDaily::CCffexFutureDaily(void)
{
}

CCffexFutureDaily::~CCffexFutureDaily(void)
{
}


string CCffexFutureDaily::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists " 
		<< " CffexFutureDaily ( trade_date date, agreement_code varchar(32), " 
		<< " open DECIMAL(10,2),  high DECIMAL(10,0),  low DECIMAL(10,2),  volume int,  "
		<< " turnover DECIMAL(15,2), holding int, close DECIMAL(10,2), settlement DECIMAL(10,2),"
		<< " change1 DECIMAL(10,2), change2 DECIMAL(10,2), "
		<< " PRIMARY KEY ( trade_date, agreement_code))";
	return ss.str();
}

string  CCffexFutureDaily::SaveAsCsv()
{
	stringstream ss;

	m_vec_agreement_code.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_volume.clear();

	m_vec_turnover.clear();
	m_vec_holding.clear();
	m_vec_close.clear();
	m_vec_settlement.clear();
	m_vec_change1.clear();
	m_vec_change2.clear();

	string request_date = ReplaceString(m_request_trade_date, "-", "");
	
	char tmp[256] = { 0 };
	snprintf (tmp, 255, "http://www.cffex.com.cn/fzjy/mrhq/%s/%s/%s_1.csv",
		request_date.substr(0,6).c_str(), request_date.substr(6,2).c_str(), request_date.c_str());	
	string strMonth = string(tmp);

	string htmlPage =CUrlUtils::PostUrlOfCffex(strMonth,NULL);

	if(htmlPage.find("error_404")!=string::npos)
	{

		m_vec_agreement_code.insert(m_vec_agreement_code.end(), "error_404");
		m_vec_open.insert(m_vec_open.end(), 0);
		m_vec_high.insert(m_vec_high.end(), 0);
		m_vec_low.insert(m_vec_low.end(), 0);
		m_vec_volume.insert(m_vec_volume.end(), 0);

		m_vec_turnover.insert(m_vec_turnover.end(), 0);
		m_vec_holding.insert(m_vec_holding.end(), 0);
		m_vec_close.insert(m_vec_close.end(), 0);
		m_vec_settlement.insert(m_vec_settlement.end(), 0);
		m_vec_change1.insert(m_vec_change1.end(), 0);
		m_vec_change2.insert(m_vec_change2.end(), 0);

		return "AlreadyInMemory";
	}

	vector<string> vecLine;
	splitString(htmlPage,"\r\n" , vecLine);
	if(vecLine.size()>2)
	{
		for(int i=1; i<vecLine.size(); i++)
		{
			vector<string>	vecYield;
			splitString(vecLine[i],",\t\s" , vecYield);
			if(vecYield.size()==13 && (
				vecYield[0].find("IC")!=string::npos ||
				vecYield[0].find("IF")!=string::npos ||
				vecYield[0].find("IH")!=string::npos ||
				vecYield[0].find("TF")!=string::npos ||
				vecYield[0].find("T")!=string::npos ))
			{
				m_vec_agreement_code.insert(m_vec_agreement_code.end(), trimString(vecYield[0]));
				m_vec_open.insert(m_vec_open.end(), String2Double(vecYield[1]));
				m_vec_high.insert(m_vec_high.end(), String2Double(vecYield[2]));
				m_vec_low.insert(m_vec_low.end(), String2Double(vecYield[3]));
				m_vec_volume.insert(m_vec_volume.end(), atoi(vecYield[4].c_str()));

				m_vec_turnover.insert(m_vec_turnover.end(), String2Double(vecYield[5]));
				m_vec_holding.insert(m_vec_holding.end(), atoi(vecYield[6].c_str()));
				m_vec_close.insert(m_vec_close.end(), String2Double(vecYield[7]));
				m_vec_settlement.insert(m_vec_settlement.end(), String2Double(vecYield[8]));
				m_vec_change1.insert(m_vec_change1.end(), String2Double(vecYield[9]));
				m_vec_change2.insert(m_vec_change2.end(), String2Double(vecYield[10]));
			}
		}

		if(m_vec_agreement_code.size()>0)
		{
			return "AlreadyInMemory";
		}
	}
	return "";
}



void CCffexFutureDaily::ImportToDatabase(void)
{
	
	stringstream ss;
	try
	{
		ss << "【" << m_request_trade_date << "】";
		ss << "CCffexFutureDaily::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		ss.str("");
		ss << "insert into CffexFutureDaily ("
			<< "trade_date, agreement_code, open, high, low, volume, turnover, holding, close, settlement, " 
			<< " change1, change2 ) values ";

		for (int row = 0; row < m_vec_agreement_code.size(); row++)
		{
			ss	<< "( \'" << m_request_trade_date << "\' , \'" 
				<< m_vec_agreement_code[row] << "\', "
				<< Double2String(m_vec_open[row]) << ","
				<< Double2String(m_vec_high[row]) << ", "
				<< Double2String(m_vec_low[row]) << ","
				<< Int2String(m_vec_volume[row]) << ","
				<< Double2String(m_vec_turnover[row]) << ","
				<< Int2String(m_vec_holding[row]) << ","
				<< Double2String(m_vec_close[row]) << ","
				<< Double2String(m_vec_settlement[row]) << ","
				<< Double2String(m_vec_change1[row]) << ","
				<< Double2String(m_vec_change2[row]) << " )";

			if(row< m_vec_agreement_code.size() - 1)
				ss << ",";
		}

		string test = ss.str();
		//test = Gbk2Utf8(test);
		sql << test; 

		ss.str("");
		ss << "【"  << m_request_trade_date << "】";
		ss << "共有数据 " << m_vec_agreement_code.size() ;
		ss << " OK, CCffexFutureDaily::ImportToDatabase 数据导入完毕. \r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



void CCffexFutureDaily::NotifyDatabaseImportedToWindow()
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

void CCffexFutureDaily::ExportFromDatabase(double startTime, double endTime)
{
	
}

vector<string>	CCffexFutureDaily::NeedDownload()
{
	stringstream ss;

	map<string ,string> mapRequestDate;

	bool bContinue = true;
	time_t time_end = CBussinessUtils::CurrentTradingDate(0);
	time_t time_start = convert_string_to_time_t("2010-04-16 01:00:00");	
	while(bContinue)
	{
		char tmp[256] = { 0 };
		/*struct tm * curr_tm;
		curr_tm = gmtime (&time_start);
		snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm->tm_year, curr_tm->tm_mon+1, curr_tm->tm_mday);  */

		struct tm curr_tm;
		localtime_s(&curr_tm, &time_start);
		snprintf (tmp, 255, "%4d-%02d-%02d", 1900 + curr_tm.tm_year, curr_tm.tm_mon+1, curr_tm.tm_mday);
		
		if(curr_tm.tm_wday!=0 && curr_tm.tm_wday!=6)
		{
			mapRequestDate[string(tmp)] = string(tmp);
		}

		time_start = time_start +  24 * 3600;

		if(time_end<=time_start)
			bContinue = false;
	}

	try
	{
		ss.str("");
		ss << " select DATE_FORMAT(trade_date, '%Y-%m-%d') as TradeDate , count(*) as cnt  ";
		ss << " from CffexFutureDaily  group by trade_date order by trade_date ";

		string test = ss.str();
		test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int lastCnt = 0;
		while (st.fetch())
		{
			string	strDate = r.get<string>("TradeDate");
			int cnt = r.get<long long>("cnt");

			if(cnt >0)   
			{
				//	
				mapRequestDate.erase(strDate);
			}
		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

	vector<string>  vecRequestDate;
	map<string, string>::iterator it = mapRequestDate.begin();
	for(; it!=mapRequestDate.end(); it++)
	{
		vecRequestDate.insert(vecRequestDate.end(), it->first);
	}
	return vecRequestDate;
}

