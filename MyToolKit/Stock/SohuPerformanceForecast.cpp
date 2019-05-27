#include "StdAfx.h"
#include "Stock.h"
#include "SohuPerformanceForecast.h"
#include "UrlUtils.h"
#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CXHtmlParserForecast: public CHtmlParser
{
public:
	CXHtmlParserForecast(tree<HTML::Node> &dom, CSohuPerformanceForecast & userContext):CHtmlParser(dom), m_incomeComposition(userContext)
	{
		m_report_type = "";
		m_report_date = "";
		m_change_rate = "";
	}

	~CXHtmlParserForecast()
	{
		if(m_report_date.empty()==false)
		{
			//m_targetFile.Close();
		}
	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int div, ul, li;
		ul = 0;
		if(sscanf_s(xpath.c_str(), "/div[0]/div[%d]/div[0]/ul[%d]/li[%d]", &div, &ul, &li)!=3)
			sscanf_s(xpath.c_str(), "/div[0]/div[%d]/div[0]/ul[%d]/li[%d]", &div, &ul, &li);

		if(xpath.find("span")!=string::npos)
			return ;

		/*ss << xpath << " <------> " << val << " >>>>  ";
		LOG4CPLUS_DEBUG(g_logger, ss.str());*/

		if(ul==1)
		{
			if(li==0)
				m_report_date = val;
			else if(li==1)
				m_report_type = val;
			else if(li==2)
			{
				m_change_rate = val;

				bool bRepeat = false;
				int row = m_incomeComposition.m_vec_report_date.size();
				if(row>0)
				{
					if(m_incomeComposition.m_vec_report_date[row-1]==m_report_date)
						bRepeat = true;
				}
				if(bRepeat==false)
				{
					m_incomeComposition.m_vec_report_date[row]=m_report_date;
					m_incomeComposition.m_vec_report_type[row]= m_report_type;
					m_incomeComposition.m_vec_change_rate[row] = String2Double(m_change_rate.substr(0, m_change_rate.length()-1));
				}
				
			}
		}

	}

private:
	string m_report_date;
	string m_report_type;
	string m_change_rate;

	CSohuPerformanceForecast & m_incomeComposition;

};


//////////////////////////////////////////////////////////////////////////
// 业绩预告

CSohuPerformanceForecast::CSohuPerformanceForecast(void)
{
}

CSohuPerformanceForecast::~CSohuPerformanceForecast(void)
{
	m_vec_report_date.clear();
	m_vec_report_type.clear();
	m_vec_change_rate.clear();
}



string CSohuPerformanceForecast::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/SohuPerformanceForecast/SohuPerformanceForecast(%06d)_.csv" ,g_strCurrentDir.c_str(), m_nCode);
	return string(tmp.GetBuffer());
}


string CSohuPerformanceForecast::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SohuPerformanceForecast (Code int ,report_date date,  "
		<< " report_type varchar(32),  change_rate DECIMAL(10,4), PRIMARY KEY ( Code, report_date))";
	return ss.str();
}



int CSohuPerformanceForecast::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"预告报告期",
		"财报预告类型",
		"幅度"
	};

	if(arrayHeader.size()!=3)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<3; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}

	if(arrayData.size()==0)
		return CSV_CHECK_DATA_EXIST_ERROR;

	return CSV_CHECK_NO_ERROR;
}



void CSohuPerformanceForecast::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	m_vec_report_date.clear();
	m_vec_report_type.clear();
	m_vec_change_rate.clear();
	
	try
	{
		ss << "SohuPerformanceForecast ParseCsvFile。。。。。。\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			m_vec_report_date[row]=arrayData[row][0];
			m_vec_report_type[row]= 1<colnum ? (arrayData[row][1]):"0";
			m_vec_change_rate[row]= 2<colnum ? String2Double(arrayData[row][2].substr(0, arrayData[row][2].length()-1)):0;

		}

		ss.str("");
		ss << "OK, SohuPerformanceForecast ParseCsvFile 完毕.\r\n";
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

bool CSohuPerformanceForecast::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SohuPerformanceForecast\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(24, pos1-24);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CSohuPerformanceForecast::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;

	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.SohuPerformanceForecast;


		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			if(m_vec_report_date[row] > sdd.SohuPerformanceForecast)
			{
				ss.str("");
				ss << "insert into SohuPerformanceForecast "
					<< " select " <<  Int2String(m_nCode) << ", \'" 
					<< m_vec_report_date[row] << "\', \'"
					<< m_vec_report_type[row]  << "\', "
					<< Double2String(m_vec_change_rate[row]) << "  "

					<< " from dual where not exists (SELECT 1 from SohuPerformanceForecast " 
					<< " where report_date=\'" << m_vec_report_date[row] << "\' " 
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

			for(int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "SohuPerformanceForecast", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, SohuPerformanceForecast::ImportToDatabase 数据导入完毕. \r\n";
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

bool testUpper(char ch)
{
	if(ch>=97 && ch<=122)
		return true;
	else
		return false;
}

bool testLower(char ch)
{
	if(ch>=65 && ch<=90)
		return true;
	else
		return false;
}

string CSohuPerformanceForecast::SaveAsCsv()
{
	stringstream ss;
	time_t time_start;
	time_t time_end;

	//	计算要下载的开始时间
	CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
	string strLastTradeDate = sdd.SohuPerformanceForecast;
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
	//	当前最后一个交易日 与 数据库中最后的预告时间差值 < 3 个 月，则一般预告还没有来
	if(time_end - time_start < 90 * 24 * 3600)
		return "NoMoreData";

	
	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://q.stock.sohu.com/cn/%06d/yjyg.shtml", m_nCode);

	string test = string(tmpCode);
	string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);


	char tmpHtml[300000] = { 0};
	memcpy(tmpHtml, strGBK.c_str(), strGBK.length());
	for(int k =0; k<strGBK.length(); k++)
	{
		if(tmpHtml[k]=='<' && testUpper(tmpHtml[k+1])==false \
			&& testLower(tmpHtml[k+1]) == false && tmpHtml[k+1]!='/' && tmpHtml[k+1]!='!')
		{
			tmpHtml[k]='X';
		}
	}
	strGBK = string(tmpHtml);

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
			if(attrs.size()>0 && attrs["class"] == "BIZ_innerContent")
			{
				break;
			}

		}
	}

	if(it!=end)
	{
		CXHtmlParserForecast parser(dom, *this);
		parser.Parse(it, "");
		return "AlreadyInMemory";
	}

	return "";
	
}


void CSohuPerformanceForecast::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSohuPerformanceForecast 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, report_date, report_type,  change_rate  "
			<< " from SohuPerformanceForecast where Code=" << Int2String(m_nCode)
			<< " order by report_date";

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_report_type[cnt] = r.get<string>("report_type");
			m_vec_change_rate[cnt] = r.get<double>("change_rate");

			cnt++;
		}


		if(cnt>0)
		{
			m_nRowNum = cnt;
		}

		ss.str("");
		ss << "OK, CSohuPerformanceForecast 从 MYSQL 获取数据完毕.";
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


string CSohuPerformanceForecast::LastTradeDateFromDatabase()
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	string lastTradeDate = "";
	try
	{

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select max(report_date) as maxdate "
			<< " from SohuPerformanceForecast where Code=" << Int2String(m_nCode) ;

		string test = ss.str();

		row r;
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

// baseDate 参数即作为输入， 也作为输出
double CSohuPerformanceForecast::NetProfitBelongingToParentCompany(string currDate, string & baseDate)
{
	double nProfit = -1;
	COleDateTime odt_curr, odt_base, odt_k;
	odt_curr.ParseDateTime(currDate.c_str());
	odt_base.ParseDateTime(baseDate.c_str());

	int k = m_vec_report_date.size()-1;
	for(; k>=0; k--)
	{
		odt_k.ParseDateTime(m_vec_report_date[k].c_str());
		if(odt_k.m_dt >  odt_base.m_dt)
		{
			break;
		}
	}
	if(k>=0)
	{
		//	找到比 baseDate （来自利润表）更晚的业绩预告
		if(odt_curr.m_dt>=odt_k.m_dt)
		{
			// 获取去年同期的 利润
			string tmpDate;
			bool isLast;
			int mon = odt_k.GetMonth();
			int day = odt_k.GetDay();
			int year = odt_k.GetYear();
			char tmp[255] = {0};
			sprintf(tmp, "%d-%02d-%02d", year-1, mon, day);

			nProfit = CBussinessUtils::NetProfitBelongingToParentCompanyFromIncome(m_nCode, string(tmp), tmpDate, isLast);
			//	乘以 预告的增幅
			nProfit = nProfit * (1 + m_vec_change_rate[k]/100); 

			baseDate = m_vec_report_date[k];
		}
	}

	return nProfit;
}