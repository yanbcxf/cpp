#include "StdAfx.h"
#include "stock.h"
#include "SohuMainIncomeComposition.h"
#include "UrlUtils.h"
#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CXHtmlParser: public CHtmlParser
{
public:
	CXHtmlParser(tree<HTML::Node> &dom, CSohuMainIncomeComposition & userContext):CHtmlParser(dom), m_incomeComposition(userContext)
	{
		m_cosposition_type = "";
		m_report_date == "";
	}

	~CXHtmlParser()
	{
		if(m_report_date.empty()==false)
		{
			//m_targetFile.Close();
		}
	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int table, tr, td;
		if(sscanf_s(xpath.c_str(), "/table[%d]/tr[%d]/th[%d]", &table, &tr, &td)!=3)
			sscanf_s(xpath.c_str(), "/table[%d]/tr[%d]/td[%d]", &table, &tr, &td);

		if(xpath=="/h4[0]")
		{
			m_report_date = val;
			/*m_report_date = val;
			BOOL b = m_targetFile.Open(m_incomeComposition.GetCsvFileName().c_str(),\
					CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);

			if(b==FALSE)
				return ;

			ss.str("");
			ss << "报告日期," << "构成类型," << "名称," << "占总收入比例," << "收入,";
			ss << "收入同比变化," << "成本," << "毛利率,";
			ss << "毛利率同比变化" << "\n";
			m_targetFile.WriteString(ss.str().c_str());*/
		}
		else if(val=="主营产品构成（百万）" || val=="行业收入构成（百万）" || val=="地区收入构成（百万）")
		{
			m_cosposition_type = val;
			string::size_type pos = m_cosposition_type.find_first_of("（百万）");
			m_cosposition_type = m_cosposition_type.substr(0, pos);
			m_cosposition_type +="（万）";
			m_tr = tr + 2;
		}
		else if(val=="总计")
		{
			m_cosposition_type = "";
		}
		else if(m_cosposition_type.empty()==false)
		{
			if(tr>=m_tr)
			{
				if(td==0)	m_composition_name = val;
				else if(td==1)	m_proportion_of_total_income = val;
				else if(td==2)	
				{	
					double income = String2Double(val) * 100.0;
					m_income = Double2String(income);
				}
				else if(td==3)	m_income_change_year_on_year = val;
				else if(td==4)	
				{
					double cost = String2Double(val) * 100.0;
					m_cost = Double2String(cost);
				}
				else if(td==5)	m_gross_profit_rate = val;
				else if(td==6)	
				{
					m_gross_profit_rate_change_year_on_year = val;

					/*ss.str("");
					ss << "\"" << m_report_date << "\",\"" << m_cosposition_type << "\",\"" << m_composition_name << "\",\"" ;
					ss << m_proportion_of_total_income << "\",\"" << m_income << "\",\""  << m_income_change_year_on_year << "\",\"";
					ss << m_cost << "\",\"" << m_gross_profit_rate <<  "\",\"" << m_gross_profit_rate_change_year_on_year << "\"\n";
					m_targetFile.WriteString(ss.str().c_str());*/

					int row = m_incomeComposition.m_vec_report_date.size();
					m_incomeComposition.m_vec_report_date[row]=m_report_date;
					m_incomeComposition.m_vec_composition_type[row]= m_cosposition_type;
					m_incomeComposition.m_vec_composition_name[row]= m_composition_name;
					m_incomeComposition.m_vec_proportion_of_total_income[row]= String2Double(m_proportion_of_total_income.substr(0, m_proportion_of_total_income.length()-1));
					m_incomeComposition.m_vec_income[row]=  String2Double(m_income);
					m_incomeComposition.m_vec_income_change_year_on_year[row]= String2Double(m_income_change_year_on_year);
					m_incomeComposition.m_vec_cost[row] = String2Double(m_cost);
					m_incomeComposition.m_vec_gross_profit_rate[row] = String2Double(m_gross_profit_rate.substr(0, m_gross_profit_rate.length()-1));
					m_incomeComposition.m_vec_gross_profit_rate_change_year_on_year[row]= String2Double(m_gross_profit_rate_change_year_on_year.substr(0, m_gross_profit_rate_change_year_on_year.length()-1));
				}	
			}
		}

		/*ss << xpath << " <------> " << val << " >>>>  ";
		ss << table << "," << tr << "," << td;
		LOG4CPLUS_DEBUG(g_logger, ss.str());*/
	}

private:
	string m_report_date;
	string m_cosposition_type;
	string m_composition_name;
	string m_proportion_of_total_income;
	string m_income;
	string m_income_change_year_on_year;
	string m_cost;
	string m_gross_profit_rate;
	string m_gross_profit_rate_change_year_on_year;

	int		m_tr;
	//CStdioFile  m_targetFile;
	CSohuMainIncomeComposition & m_incomeComposition;

};

//////////////////////////////////////////////////////////////////////////

CSohuMainIncomeComposition::CSohuMainIncomeComposition(void)
{
}

CSohuMainIncomeComposition::~CSohuMainIncomeComposition(void)
{
	m_vec_report_date.clear();
	m_vec_composition_type.clear();
	m_vec_composition_name.clear();
	m_vec_proportion_of_total_income.clear();
	m_vec_income.clear();
	m_vec_income_change_year_on_year.clear();
	m_vec_cost.clear();
	m_vec_gross_profit_rate.clear();
	m_vec_gross_profit_rate_change_year_on_year.clear();
}


string CSohuMainIncomeComposition::GetCsvFileName()
{
	CString tmp;
	tmp.Format("%s/SohuMainIncomeComposition/SohuMainIncomeComposition(%06d)_.csv" ,g_strCurrentDir.c_str(), m_nCode);
	return string(tmp.GetBuffer());
}


string CSohuMainIncomeComposition::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists SohuMainIncomeComposition (Code int ,report_date date, "
		<< " composition_type varchar(64), composition_name varchar(64), "
		<< " proportion_of_total_income  DECIMAL(10,2), income DECIMAL(15,4),  "
		<< " income_change_year_on_year  DECIMAL(10,2), cost   DECIMAL(15,4), gross_profit_rate DECIMAL(10,2), "
		<< " gross_profit_rate_change_year_on_year DECIMAL(10,2),  PRIMARY KEY ( Code, report_date, composition_type, composition_name))";
	return ss.str();
}



int CSohuMainIncomeComposition::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	string header[] = {
		"报告日期",
		"构成类型",
		"名称",
		"占总收入比例",
		"收入",
		"收入同比变化",
		"成本",
		"毛利率",
		"毛利率同比变化"
	};

	if(arrayHeader.size()!=9)
		return CSV_CHECK_FORMAT_ERROR;

	for(int i=0; i<9; i++)
	{
		if(arrayHeader[i]!=header[i])
			return CSV_CHECK_FORMAT_ERROR;
	}

	if(arrayData.size()==0)
		return CSV_CHECK_DATA_EXIST_ERROR;

	return CSV_CHECK_NO_ERROR;
}



void CSohuMainIncomeComposition::ParseCsvFile(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	stringstream ss;
	/*string connectString = "dbname=stock  port=3306   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();*/

	m_vec_report_date.clear();
	m_vec_composition_type.clear();
	m_vec_composition_name.clear();
	m_vec_proportion_of_total_income.clear();
	m_vec_income.clear();
	m_vec_income_change_year_on_year.clear();
	m_vec_cost.clear();
	m_vec_gross_profit_rate.clear();
	m_vec_gross_profit_rate_change_year_on_year.clear();

	try
	{
		ss << "CSohuMainIncomeComposition ParseCsvFile。。。。。。\r\n";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		for (int row = 0; row < arrayData.size(); row++)
		{
			int colnum = arrayData[row].size();
			
			m_vec_report_date[row]=arrayData[row][0];
			m_vec_composition_type[row]= 1<colnum ? arrayData[row][1]:"";
			m_vec_composition_name[row]= 2<colnum ? arrayData[row][2]:"";
			m_vec_proportion_of_total_income[row]= 3<colnum ? String2Double(arrayData[row][3].substr(0, arrayData[row][3].length()-1)):0;
			m_vec_income[row]= 4<colnum ? String2Double(arrayData[row][4]):0;
			m_vec_income_change_year_on_year[row]=5<colnum ? String2Double(arrayData[row][5]):0;
			m_vec_cost[row]=6<colnum ? String2Double(arrayData[row][6]):0;
			m_vec_gross_profit_rate[row]=7<colnum ? String2Double(arrayData[row][7].substr(0, arrayData[row][7].length()-1)):0;
			m_vec_gross_profit_rate_change_year_on_year[row]=8<colnum ? String2Double(arrayData[row][8].substr(0, arrayData[row][8].length()-1)):0;
		}

		ss.str("");
		ss << "OK, CSohuMainIncomeComposition ParseCsvFile 完毕.\r\n";
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

bool CSohuMainIncomeComposition::ParseCsvFileName(string szFileName)
{
	vector<string> firstMatch;
	if(Pcre2Grep(_T("SohuMainIncomeComposition\\(\\d+\\)"), szFileName,firstMatch )<=0)
		return false;

	string::size_type pos1 = firstMatch[0].find(")");
	string codeStr = firstMatch[0].substr(26, pos1-26);
	m_nCode = atoi(codeStr.c_str());
	return true;
}

void CSohuMainIncomeComposition::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;

	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.SohuMainIncomeComposition;

		
		for (int row = 0; row < m_vec_report_date.size(); row++)
		{
			if(m_vec_report_date[row] > sdd.SohuMainIncomeComposition)
			{
				ss.str("");
				ss << "insert into SohuMainIncomeComposition "
					<< " select " <<  Int2String(m_nCode) << ", \'" 
					<< m_vec_report_date[row] << "\', \'"
					<< m_vec_composition_type[row] << "\', \'"
					<< m_vec_composition_name[row] << "\', "
					<< Double2String(m_vec_proportion_of_total_income[row]) << ", "
					<< Double2String(m_vec_income[row]) << ", "
					<< Double2String(m_vec_income_change_year_on_year[row]) << ", "
					<< Double2String(m_vec_cost[row]) << ", "
					<< Double2String(m_vec_gross_profit_rate[row]) << ", "
					<< Double2String(m_vec_gross_profit_rate_change_year_on_year[row]) << "  "

					<< " from dual where not exists (SELECT 1 from SohuMainIncomeComposition " 
					<< " where report_date=\'" << m_vec_report_date[row] << "\' " 
					<< " and composition_type=\'" << m_vec_composition_type[row] << "\' "
					<< " and composition_name=\'" << m_vec_composition_name[row] << "\' "
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

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "SohuMainIncomeComposition", maxDate);

			ss.str("");
			ss << "共有数据 " << m_vec_report_date.size() ;
			ss << " OK, CSohuMainIncomeComposition::ImportToDatabase 数据导入完毕. \r\n";
			sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		}
		
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

string CSohuMainIncomeComposition::SaveAsCsv()
{

	m_vec_report_date.clear();
	m_vec_composition_type.clear();
	m_vec_composition_name.clear();
	m_vec_proportion_of_total_income.clear();
	m_vec_income.clear();
	m_vec_income_change_year_on_year.clear();
	m_vec_cost.clear();
	m_vec_gross_profit_rate.clear();
	m_vec_gross_profit_rate_change_year_on_year.clear();

	//////////////////////////////////////////////////////////////////////////
	//	对于本日已经成功下载的 ，忽略该任务
	/*if(CheckDownloadFileInfo(GetCsvFileName())>0)
		return "NoMoreData";*/

	stringstream ss;

	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://q.stock.sohu.com/cn/%06d/srgc.shtml", m_nCode);

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
			if(attrs.size()>0 && attrs["class"] == "BIZ_innerContent")
			{
				break;
			}

		}
	}

	if(it!=end)
	{
		CXHtmlParser parser(dom, *this);
		parser.Parse(it, "");
		return "AlreadyInMemory";
	}
		
	return "";
}


void CSohuMainIncomeComposition::ExportFromDatabase(double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CSohuMainIncomeComposition 开始从 MYSQL 获取数据......";
		ss << "【" << m_nCode << "】";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select Code, report_date, composition_type, composition_name, proportion_of_total_income, income,"
			<< " income_change_year_on_year, cost, gross_profit_rate, gross_profit_rate_change_year_on_year "
			<< " from SohuMainIncomeComposition where Code=" << Int2String(m_nCode)
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
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_composition_type[cnt] = r.get<string>("composition_type");
			m_vec_composition_name[cnt] = r.get<string>("composition_name");
			m_vec_proportion_of_total_income[cnt] = r.get<double>("proportion_of_total_income");
			m_vec_income[cnt] = r.get<double>("income");
			m_vec_income_change_year_on_year[cnt] = r.get<double>("income_change_year_on_year");
			m_vec_cost[cnt] = r.get<double>("cost");
			m_vec_gross_profit_rate[cnt] = r.get<double>("gross_profit_rate");
			m_vec_gross_profit_rate_change_year_on_year[cnt] = r.get<double>("gross_profit_rate_change_year_on_year");
			
			cnt++;
		}


		if(cnt>0)
		{
			m_nRowNum = cnt;
		}

		ss.str("");
		ss << "OK, CSohuMainIncomeComposition 从 MYSQL 获取数据完毕.";
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

