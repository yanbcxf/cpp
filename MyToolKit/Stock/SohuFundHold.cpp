#include "StdAfx.h"
#include "SohuFundHold.h"

#include "UrlUtils.h"
#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CFundHoldHtmlParser: public CHtmlParser
{
public:
	CFundHoldHtmlParser(tree<HTML::Node> &dom, CStdioFile &  targetFile):CHtmlParser(dom), m_TargetFile(targetFile)
	{
		m_report_date == "";
	}

	~CFundHoldHtmlParser()
	{
		
	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int div, table, tr, td;

		sscanf_s(xpath.c_str(), "/div[%d]/table[%d]/tr[%d]/td[%d]", &div, &table, &tr, &td);
		
		if(table==0 && tr==1 && td==0)
		{
			m_report_date = ReplaceChar(val,'&' , "");
			m_report_date = ReplaceChar(m_report_date,'n' , "");
			m_report_date = ReplaceChar(m_report_date,'b' , "");
			m_report_date = ReplaceChar(m_report_date,'s' , "");
			m_report_date = ReplaceChar(m_report_date,'p' , "");
			m_report_date = ReplaceChar(m_report_date,';' , "");

			string::size_type pos = m_report_date.find("报告期：");
			if(pos!=string::npos)
			{
				m_report_date = m_report_date.substr(8);
			}

		}

		if(table==1 && tr>=1)
		{
			string strTmp = val;
			strTmp = ReplaceChar(strTmp,'&' , "");
			strTmp = ReplaceChar(strTmp,'n' , "");
			strTmp = ReplaceChar(strTmp,'b' , "");
			strTmp = ReplaceChar(strTmp,'s' , "");
			strTmp = ReplaceChar(strTmp,'p' , "");
			strTmp = ReplaceChar(strTmp,';' , "");

			if(td==1)
				m_fund_code = strTmp;
			if(td==2)
				m_fund_name = strTmp;
			if(td==3)
				m_value_hold = strTmp;
			if(td==4)
				m_number_hold = strTmp;
			if(td==5)
			{
				m_percent = strTmp;

				ss.str("");
				ss << "\"" << m_report_date << "\",\"" << m_fund_code << "\",\"" << m_fund_name << "\",\"" ;
				ss << m_value_hold << "\",\"" << m_number_hold << "\",\""  << m_percent << "\"\n";
				m_TargetFile.WriteString(ss.str().c_str());
			}
		}

		/*
		ss << xpath << " <------> " << val << " >>>>  ";
		LOG4CPLUS_DEBUG(g_logger, ss.str());*/

		
	}

private:
	string m_report_date;
	string m_fund_code;
	string m_fund_name;
	string m_value_hold;
	string m_number_hold;
	string m_percent;
	
	int		m_tr;
	CStdioFile & m_TargetFile;
	
};

//////////////////////////////////////////////////////////////////////////

CSohuFundHold::CSohuFundHold(void)
{

}

CSohuFundHold::~CSohuFundHold(void)
{
	m_vec_report_date.clear();
	m_vec_fund_code.clear();
	m_vec_fund_name.clear();
	m_vec_value_hold.clear();
	m_vec_number_hold.clear();
	m_vec_percent.clear();
}



string CSohuFundHold::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/SohuFundHold/SohuFundHold(%06d)_.csv" ,g_strCurrentDir.c_str(), m_nCode);
	return string(tmp.GetBuffer());
}


string CSohuFundHold::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SohuFundHold ( Code int ,report_date date, "
		<< " fund_code int, fund_name varchar(64), "
		<< " value_hold  DECIMAL(15,2), number_hold DECIMAL(15,2),  "
		<< " percent DECIMAL(10,2) ,  PRIMARY KEY ( Code, report_date, fund_code ) ) ";
	return ss.str();
}



int CSohuFundHold::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"报告日期",
		"基金代码",
		"基金简称",
		"持仓市值(元)",
		"持仓数量(股)",
		"占基金净值比例(%)"
	};

	if(arrayHeader.size()!=6)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<6; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}

	if(arrayData.size()==0)
		return CSV_CHECK_DATA_EXIST_ERROR;

	return CSV_CHECK_NO_ERROR;
}



void CSohuFundHold::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	/*string connectString = "dbname=stock  port=3306   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();*/

	m_vec_report_date.clear();
	m_vec_fund_code.clear();
	m_vec_fund_name.clear();
	m_vec_value_hold.clear();
	m_vec_number_hold.clear();
	m_vec_percent.clear();

	try
	{
		ss << "CSohuFundHold ParseCsvFile。。。。。。\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			m_vec_report_date[row]=arrayData[row][0];
			m_vec_fund_code[row]= 1<colnum ? atoi(arrayData[row][1].c_str()) : 0;
			m_vec_fund_name[row]= 2<colnum ? arrayData[row][2]:"";
			m_vec_value_hold[row] = 3<colnum ? String2Double(arrayData[row][3]) : 0;
			m_vec_number_hold[row] = 4<colnum ? String2Double(arrayData[row][4]) : 0;
			m_vec_percent[row] = 5<colnum ? String2Double(arrayData[row][5]) : 0;
		}

		ss.str("");
		ss << "OK, CSohuFundHold ParseCsvFile 完毕.\r\n";
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


bool CSohuFundHold::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SohuFundHold\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(13, pos1-13);
	m_nCode = atoi(codeStr.c_str());
	return true;
}


void CSohuFundHold::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSohuFundHold::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			ss.str("");
			ss << "insert into SohuFundHold "
				<< " select " <<  Int2String(m_nCode) << ", \'" 
				<< m_vec_report_date[row] << "\', "
				<< m_vec_fund_code[row] << ", \'"
				<< m_vec_fund_name[row] << "\', "
				<< Double2String(m_vec_value_hold[row]) << ", "
				<< Double2String(m_vec_number_hold[row]) << ", "
				<< Double2String(m_vec_percent[row]) << "  "
				
				<< " from dual where not exists (SELECT 1 from SohuFundHold " 
				<< " where report_date=\'" << m_vec_report_date[row] << "\' " 
				<< " and fund_code=" << m_vec_fund_code[row] << " "
				<< " and Code=" << Int2String(m_nCode) << "  ) ";

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_report_date.size() ;
		ss << " OK, CSohuFundHold::ImportToDatabase 数据导入完毕. \r\n";
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



string CSohuFundHold::SaveAsCsv()
{
	stringstream ss;
	vector<string>  lstTdata;

	string tdate[4] = { "12-31","09-30","06-30","03-31" };
	
	time_t curtime = time(NULL);
	struct tm rett;
	localtime_s(&rett, &curtime);
	for(int y = 0  ;  y <3; y++ )
	{
		int yy = 1900 + rett.tm_year - y;
		for(int i=0; i<4 ; i++)
		{
			char tmp[20];
			memset(tmp, 0 , 20);
			sprintf(tmp, "%4d-%s", yy, tdate[i].c_str());
			lstTdata.push_back(string(tmp));
		}

	}

	CStdioFile targetFile;
	BOOL b = targetFile.Open(GetCsvFileName().c_str(),\
		CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);

	if(b==FALSE)
		return "";

	ss.str("");
	ss << "报告日期," << "基金代码," << "基金简称," << "持仓市值(元)," << "持仓数量(股),";
	ss << "占基金净值比例(%)" << "\n";
	targetFile.WriteString(ss.str().c_str());

	b = FALSE;
	for(int k=0; k< lstTdata.size(); k++)
	{
		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://q.fund.sohu.com/q/hsl.php?code=%06d&tdate=%s", m_nCode, lstTdata[k].c_str());

		string test = string(tmpCode);

		string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);

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
				if(attrs.size()>0 && attrs["class"] == "Area" && attrs["id"]=="Content")
				{
					break;
				}

			}
		}

		if(it!=end)
		{
			CFundHoldHtmlParser parser(dom, targetFile);
			parser.Parse(it, "");
			b = TRUE;
		}
	}

	targetFile.Close();

	if(b)
		return GetCsvFileName();
	else
		return "NoMoreData";
}


void CSohuFundHold::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSohuFundHold 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, report_date, fund_code, fund_name, value_hold, number_hold,"
			<< " percent from SohuFundHold where Code=" << Int2String(m_nCode)
			<< " order by report_date";

		row r;
		//session sql(backEnd, m_ConnectString);
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
			m_vec_fund_code[cnt] = r.get<int>("fund_code");
			m_vec_fund_name[cnt] = r.get<string>("fund_name");
			m_vec_value_hold[cnt] = r.get<double>("value_hold");
			m_vec_number_hold[cnt] = r.get<double>("number_hold");
			m_vec_percent[cnt] = r.get<double>("percent");
			cnt++;
		}


		if(cnt>0)
		{
			m_nRowNum = cnt;
		}

		ss.str("");
		ss << "OK, CSohuFundHold 从 MYSQL 获取数据完毕.";
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

