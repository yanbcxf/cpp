#include "StdAfx.h"
#include "ShenzhenMarketQuotation.h"

CShenzhenMarketQuotation::CShenzhenMarketQuotation(void)
{
}



CShenzhenMarketQuotation::~CShenzhenMarketQuotation(void)
{
	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_yesterday_close.clear();
	m_vec_today_close.clear();
	m_vec_up_down.clear();
	m_vec_turnover_amount.clear();
	m_vec_price_earnings_ratio.clear();

	m_vec_report_date.clear();
}


int CShenzhenMarketQuotation::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"交易日期",
		"证券代码",
		"证券简称",
		"前收",
		"今收",
		"升跌(%)",
		"成交金额(元)",
		"市盈率"
	};

	if(arrayHeader.size()!=8)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<8; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}
	return CSV_CHECK_NO_ERROR;
}



void CShenzhenMarketQuotation::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	/*string connectString = "dbname=stock  port=3306   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();*/

	m_vec_code.clear();
	m_vec_name.clear();
	m_vec_yesterday_close.clear();
	m_vec_today_close.clear();
	m_vec_up_down.clear();
	m_vec_turnover_amount.clear();
	m_vec_price_earnings_ratio.clear();

	try
	{
		ss << "CShenzhenMarketQuotation ParseCsvFile。。。。。。\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			//m_vec_report_date[row]=arrayData[row][0];
			m_vec_code[row]= 1<colnum ? atoi(arrayData[row][1].c_str()):0;
			m_vec_name[row]= 2<colnum ? arrayData[row][2]:"";
			m_vec_yesterday_close[row]= 3<colnum ? String2Double(arrayData[row][3]):0;
			m_vec_today_close[row]= 4<colnum ? String2Double(arrayData[row][4]):0;
			m_vec_up_down[row]=5<colnum ? String2Double(arrayData[row][5]):0;
			m_vec_turnover_amount[row]=6<colnum ? String2Double(arrayData[row][6]):0;
			m_vec_price_earnings_ratio[row]=7<colnum ? String2Double(arrayData[row][7]):0;
		}

		ss.str("");
		ss << "OK, CShenzhenMarketQuotation ParseCsvFile 完毕.\r\n";
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

void CShenzhenMarketQuotation::ParseXmlElement(void)
{
}

bool CShenzhenMarketQuotation::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("table\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(6, pos1-6);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CShenzhenMarketQuotation::ImportToDatabase(void)
{
	if(m_str_report_date.length()< 10)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CShenzhenMarketQuotation::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		for (int row = 0; row < m_vec_code.size(); row++)
		{
			ss.str("");
			ss << "insert into MarketQuotation "
				<< " select " <<  Int2String( m_vec_code[row]) << ", \'" 
				<< m_str_report_date << "\', \'"
				<< (m_vec_name[row])  << "\', "
				<< Double2String(m_vec_yesterday_close[row]) << ", "
				<< Double2String(m_vec_today_close[row]) << ", "
				<< Double2String(m_vec_up_down[row]) << ", "
				<< Double2String(m_vec_turnover_amount[row]) << ", "
				<< Double2String(m_vec_price_earnings_ratio[row]) << " "

				<< " from dual where not exists (SELECT 1 from MarketQuotation " 
				<< " where TradeDate=\'" << m_str_report_date << "\' " 
				<< " and Code=" << Int2String(m_vec_code[row]) << "  ) ";

			string sql1 = ss.str();
			//string sqlStr = Gbk2Utf8(ss.str());
			sql << sql1; 

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_code.size() ;
		ss << " OK, CShenzhenMarketQuotation::ImportToDatabase 数据导入完毕. \r\n";
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

CStockDataModel * CShenzhenMarketQuotation::NewCopy()
{
	CShenzhenMarketQuotation * pTrading = new CShenzhenMarketQuotation();
	pTrading->m_nRowNum = m_nRowNum;
	

	return pTrading;
}



string CShenzhenMarketQuotation::GetDownloadUrl()
{
	string url = "";
	char cmdline[512]; 
	CString tmp;    
	string::size_type pos = GetCsvFileName().find(".csv");
	string xlsFileName = GetCsvFileName().substr(0, pos);
	xlsFileName += ".html";

	//	对于本日已经成功下载的 ，忽略该任务
	if(CheckDownloadFileInfo(xlsFileName)>0)
		return url;

	//	从“深圳交易所”直接下载网页
	tmp.Format("wget.exe http://www.szse.cn//szseWeb/ShowReport.szse?SHOWTYPE=EXCEL&CATALOGID=1815_stock&tab1PAGENUM=1&txtBeginDate=%s&txtEndDate=%s&ENCODE=1&TABKEY=tab1 -O %s", \
		m_str_report_date.c_str(), m_str_report_date.c_str(), xlsFileName.c_str());    

	sprintf(cmdline,"%s",tmp);    
	url = string(cmdline);
	return url;
}


string CShenzhenMarketQuotation::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists MarketQuotation (Code int ,TradeDate date, Name varchar(256), "
		<< " Yesterday_Close DECIMAL(10,2), Today_Close DECIMAL(10,2), Up_Down DECIMAL(10,2), "
		<< " Turnover_Amount DECIMAL(15,2), Price_Earnings_Ratio DECIMAL(10,2), PRIMARY KEY ( Code, TradeDate))";
	return ss.str();
}



string CShenzhenMarketQuotation::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/MarketQuotation/MarketQuotation(%s)_.csv" ,g_strCurrentDir.c_str(), m_str_report_date.c_str());
	return string(tmp.GetBuffer());
}


void CShenzhenMarketQuotation::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "\n 开始从 MYSQL 获取数据......";
		ss << "【" << m_str_report_date << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, TradeDate, Name, Yesterday_Close, Today_Close, Up_Down, Turnover_Amount, Price_Earnings_Ratio"
			<< " from MarketQuotation where TradeDate=\'" << m_str_report_date << "\'"
			<< " order by TradeDate";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("Code");
			m_vec_name[cnt] = r.get<string>("Name");
			m_vec_yesterday_close[cnt] = r.get<double>("Yesterday_Close");
			m_vec_today_close[cnt] = r.get<double>("Today_Close");
			m_vec_up_down[cnt] = r.get<double>("Up_Down");
			m_vec_turnover_amount[cnt] = r.get<double>("Turnover_Amount");
			m_vec_price_earnings_ratio[cnt] = r.get<double>("Price_Earnings_Ratio");

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



void CShenzhenMarketQuotation::ExportFromDatabaseByCode()
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
		ss << "select Code, TradeDate, Name, Yesterday_Close, Today_Close, Up_Down, Turnover_Amount, Price_Earnings_Ratio"
			<< " from MarketQuotation where Code=" << Int2String(m_nCode) << " "
			<< " order by TradeDate";

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
				std::tm  tm = r.get<std::tm>("TradeDate");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_name[cnt] = /*Utf8_GBK*/(r.get<string>("Name"));
			m_vec_yesterday_close[cnt] = r.get<double>("Yesterday_Close");
			m_vec_today_close[cnt] = r.get<double>("Today_Close");
			m_vec_up_down[cnt] = r.get<double>("Up_Down");
			m_vec_turnover_amount[cnt] = r.get<double>("Turnover_Amount");
			m_vec_price_earnings_ratio[cnt] = r.get<double>("Price_Earnings_Ratio");

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


string  CShenzhenMarketQuotation::SaveAsCsv()
{
	stringstream ss;
	string html;
	tree<HTML::Node> dom;

	try
	{

		string::size_type pos = GetCsvFileName().find(".csv");
		string xlsFileName = GetCsvFileName().substr(0, pos);
		xlsFileName += ".html";
		//////////////////////////////////////////////////////////////////////////

		ifstream file(xlsFileName.c_str());
		if (!file.is_open()) 
		{
			ss.str("");
			ss << "Error! Open " << xlsFileName; 
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			return "";
		}
		else
		{
			while (1)
			{
				char buf[BUFSIZ];
				file.read(buf, BUFSIZ);
				if(file.gcount() == 0) {
					break;
				}
				html.append(buf, file.gcount());
			}
			file.close();
		}


		HTML::ParserDom parser;
		parser.parse(html);
		dom = parser.getTree();

		tree<HTML::Node>::iterator it = dom.begin();
		tree<HTML::Node>::iterator end = dom.end();
		for (; it != end; ++it)
		{
			if (it->tagName() =="table")
			{
				it->parseAttributes();

				map<string,string> attrs = it->attributes();
				if(attrs["id"] == "REPORTID_tab1")
				{
					break;
				}

			}
		}

		if(it!=end)
		{
			int nChild = dom.number_of_children(it);
			CStdioFile  targetFile(GetCsvFileName().c_str(), CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
			ss.str("");
			ss << "交易日期," << "证券代码," << "证券简称," << "前收," << "今收,"; 
			ss << "升跌(%)," << "成交金额(元)," << "市盈率" << "\n";
			targetFile.WriteString(ss.str().c_str());

			tree<HTML::Node>::sibling_iterator tr_it = dom.begin(it);
			tree<HTML::Node>::sibling_iterator tr_end = dom.end(it);
			for( ; tr_it!=tr_end; tr_it++)
			{
				

				
				string tagName = tr_it->tagName();
				if(tagName=="tr")
				{
					tr_it->parseAttributes();

					map<string,string> attrs = tr_it->attributes();
					if(attrs["class"] == "cls-data-tr")
					{
						int td_index = 0;
						string strTradeDate = "";
						string strCode = "0";
						string strName = "0";
						string strYesterdayClose = "";
						string strTodayClose = "";
						string strUpDown = "0";
						string strTurnoverAmount = "0";
						string strPE = "";

						tree<HTML::Node>::sibling_iterator td_it = dom.begin(tr_it);
						tree<HTML::Node>::sibling_iterator td_end = dom.end(tr_it);
						for(; td_it!=td_end; td_it++)
						{
							tree<HTML::Node>::sibling_iterator val_it = dom.begin(td_it);
							tree<HTML::Node>::sibling_iterator val_end = dom.end(td_it);
							for(; val_it!=val_end; val_it++)
							{
								string text = val_it->text();
								if(td_index==0)
									strTradeDate = text;
								if(td_index==1)
									strCode = text;	
								if(td_index==2)
									strName = text;		
								if(td_index==3)
									strYesterdayClose = text;		
								if(td_index==4)
									strTodayClose = text;	
								if(td_index==5)
									strUpDown = text;	
								if(td_index==6)
									strTurnoverAmount = text;	
								if(td_index==7)
									strPE = text;	

								td_index++ ;
								//sendToOutput(text.c_str(), m_hWnd);
							}
						}

						ss.str("");
						ss << strTradeDate << "," << strCode << "," << strName << ",\"" << strYesterdayClose << "\",\"";
						ss << strTodayClose << "\"," << strUpDown << ",\"" << strTurnoverAmount << "\",\"" << strPE << "\"\n";
						targetFile.WriteString(ss.str().c_str());
					}

				}
				
			}
			targetFile.Flush();
			targetFile.Close();
			return GetCsvFileName();
		}

	}
	catch(...)
	{

	}
	return "";

}