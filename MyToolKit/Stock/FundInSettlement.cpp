#include "StdAfx.h"
#include "FundInSettlement.h"

#include "UrlUtils.h"
#include "HtmlParser.h"

map<string, string>	CFundInSettlement::m_month_url;


//////////////////////////////////////////////////////////////////////////

class CFundInSettlementDirectoryHtmlParser: public CHtmlParser
{
public:
	CFundInSettlementDirectoryHtmlParser(tree<HTML::Node> &dom, CFundInSettlement & userContext)
		:CHtmlParser(dom),m_FundInSettlement(userContext)
	{

	}

	~CFundInSettlementDirectoryHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int div, ul, li, a;
		if(sscanf_s(xpath.c_str(), "/div[%d]/ul[%d]/li[%d]/a[%d]", &div, &ul, &li, &a)==4)
		{
			int nYear, nMonth;
			if(sscanf_s(val.c_str(), "%d年%d月统计数据", &nYear, &nMonth)==2)
			{
				char  tmpCode[256] = { 0 };
				sprintf_s(tmpCode, 256, "%4d%02d01", nYear, nMonth);
				string url = "http://www.sipf.com.cn/" + attrsOfParent["href"];
				CFundInSettlement::m_month_url[string(tmpCode)] = url;
			}			
		}
	}

private:
	int		m_tr;
	CFundInSettlement & m_FundInSettlement;

};

//////////////////////////////////////////////////////////////////////////
//	旧的格式，不包含 “资金类别”栏

class CFundInSettlementOldHtmlParser: public CHtmlParser
{
public:
	CFundInSettlementOldHtmlParser(tree<HTML::Node> &dom, CFundInSettlement & userContext)
		:CHtmlParser(dom),m_FundInSettlement(userContext)
	{
	}

	~CFundInSettlementOldHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int tr, td, p, font, strong;

		if(sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]", &tr, &td)==2)
		{
			if(tr>=3)
			{
				switch(td)
				{
				case 0:
					m_FundInSettlement.m_vec_week.insert(m_FundInSettlement.m_vec_week.end(), val);
					m_FundInSettlement.m_vec_fund_type.insert(m_FundInSettlement.m_vec_fund_type.end(), "证券交易");
					break;
				case 1:
					m_FundInSettlement.m_vec_final_fund.insert(m_FundInSettlement.m_vec_final_fund.end(), String2Double(val));
					break;
				case 2:
					m_FundInSettlement.m_vec_average_fund.insert(m_FundInSettlement.m_vec_average_fund.end(), String2Double(val));
					break;
				case 3:
					m_FundInSettlement.m_vec_input_fund.insert(m_FundInSettlement.m_vec_input_fund.end(), String2Double(val));
					break;
				case 4:
					m_FundInSettlement.m_vec_output_fund.insert(m_FundInSettlement.m_vec_output_fund.end(), String2Double(val));
					break;
				case 5:
					m_FundInSettlement.m_vec_net_fund.insert(m_FundInSettlement.m_vec_net_fund.end(), String2Double(val));
					break;
				case 6:
					//m_FundInSettlement.m_vec_remark.insert(m_FundInSettlement.m_vec_remark.end(), val);
					break;
				}
			}
		}
	}

private:
	CFundInSettlement & m_FundInSettlement;

};

//////////////////////////////////////////////////////////////////////////
//	新的格式，包含 “资金类别”栏

class CFundInSettlementNewHtmlParser: public CHtmlParser
{
public:
	CFundInSettlementNewHtmlParser(tree<HTML::Node> &dom, CFundInSettlement & userContext)
		:CHtmlParser(dom),m_FundInSettlement(userContext)
	{
	}

	~CFundInSettlementNewHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int tr, td, p, font, strong;

		if(sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]", &tr, &td)==2)
		{
			if(tr>=2)
			{
				if(td==0)
				{
					int n1,n2,n3,n4,n5;
					if(sscanf_s(val.c_str(), "%4d.%d.%d-%d.%d", &n1, &n2, &n3, &n4, &n5)==5 ||
						sscanf_s(val.c_str(), "%4d.%d", &n1, &n2)==2 )
					{
						//	日期栏
						m_rowType = 0;
						m_recentDate = val;
						m_FundInSettlement.m_vec_week.insert(m_FundInSettlement.m_vec_week.end(), val);
					}
					else if(sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]/P[%d]", &tr, &td,  &p)==3 && p==0)
					{
						//	资金类别栏
						m_rowType = 1;
						m_FundInSettlement.m_vec_week.insert(m_FundInSettlement.m_vec_week.end(), m_recentDate);
						m_FundInSettlement.m_vec_fund_type.insert(m_FundInSettlement.m_vec_fund_type.end(), val);
					}
				}
				else if(td==1 && m_rowType==0)
				{
					if(sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]/P[%d]", &tr, &td,  &p)==3 && p==0)
					{
						//	资金类别栏
						m_FundInSettlement.m_vec_fund_type.insert(m_FundInSettlement.m_vec_fund_type.end(), val);
					}
				}
				else
				{
					if(sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]/P[%d]/FONT[%d]", &tr, &td, &p, &font)==4 ||
						sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]/P[%d]", &tr, &td, &p)==3 )
					{
						if(m_recentDate == "2016.05.03-05.06")
						{
							int test = 1;
						}

						vector<string> vecMatch;
						if(Pcre2Grep(_T("[\\s\\t\\-\\d]+"), val.c_str() ,vecMatch )>0)
						{
							//	资金数额栏
							switch(m_rowType + td)
							{
							case 2:
								m_FundInSettlement.m_vec_final_fund.insert(m_FundInSettlement.m_vec_final_fund.end(), String2Double(val));
								break;
							case 3:
								m_FundInSettlement.m_vec_average_fund.insert(m_FundInSettlement.m_vec_average_fund.end(), String2Double(val));
								break;
							case 4:
								m_FundInSettlement.m_vec_input_fund.insert(m_FundInSettlement.m_vec_input_fund.end(), String2Double(val));
								break;
							case 5:
								m_FundInSettlement.m_vec_output_fund.insert(m_FundInSettlement.m_vec_output_fund.end(), String2Double(val));
								break;
							case 6:
								m_FundInSettlement.m_vec_net_fund.insert(m_FundInSettlement.m_vec_net_fund.end(), String2Double(val));
								break;
							}
						}
					}
				}
			}
		}
	}

private:
	int		m_rowType;			//	
	string	m_recentDate;
	CFundInSettlement & m_FundInSettlement;

};

//////////////////////////////////////////////////////////////////////////

CFundInSettlement::CFundInSettlement(void)
{
}

CFundInSettlement::~CFundInSettlement(void)
{
}



string CFundInSettlement::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists FundInSettlement (begin_date date , end_date date, fund_type varchar(32), " 
		<< " final_fund DECIMAL(12,2),  average_fund DECIMAL(12,2),  input_fund DECIMAL(12,2), "
		<< " output_fund DECIMAL(12,2), net_fund DECIMAL(12,2),  remark varchar(128) , "
		<< " PRIMARY KEY ( begin_date, fund_type))";
	return ss.str();
}

string  CFundInSettlement::SaveAsCsv()
{
	stringstream ss;

	m_vec_week.clear();
	m_vec_final_fund.clear();
	m_vec_fund_type.clear();
	m_vec_average_fund.clear();
	m_vec_input_fund.clear();

	m_vec_output_fund.clear();
	m_vec_net_fund.clear();
	m_vec_remark.clear();

	
	/*如果目录索引未获取到，则解析下载的目录页*/
	if(CFundInSettlement::m_month_url.size()<=0)
	{
		for(int i=1; i<10; i++)
		{
			char tmpCode[256] = {0};
			if(i==1)
				sprintf(tmpCode, "http://www.sipf.com.cn/zjjk/tjsj/zsj/index.shtml");
			else
				sprintf(tmpCode, "http://www.sipf.com.cn/zjjk/tjsj/zsj/index_%d.shtml", i);

			string htmlPage =CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);
			htmlPage = Utf8_GBK(htmlPage);

			if(htmlPage.find("404--Not")!=string::npos)
				break;

			HTML::ParserDom parser;
			parser.parse(htmlPage);
			tree<HTML::Node> dom = parser.getTree();

			tree<HTML::Node>::iterator it = dom.begin();
			tree<HTML::Node>::iterator end = dom.end();
			for (; it != end; ++it)
			{
				if (it->tagName() =="div")
				{
					it->parseAttributes();

					map<string,string> attrs = it->attributes();
					if(attrs.size()>0 && attrs["class"] == "main-auto")
					{
						break;
					}

				}
			}

			if(it!=end)
			{
				CFundInSettlementDirectoryHtmlParser parser(dom, *this);
				parser.Parse(it, "");
			}
		}

	}

	//	开始按照月份下载统计数据
	char tmp[256] = { 0 };
	snprintf (tmp, 255, "%8d", m_nCode);	
	string strMonth = string(tmp);
	
	if(CFundInSettlement::m_month_url.count(strMonth)>0)
	{
		string url = CFundInSettlement::m_month_url[strMonth];
		string htmlPage =CUrlUtils::PostUrlOfSouhu(url, NULL);
		htmlPage = Utf8_GBK(htmlPage);
		
		int nLoop = 0;
		HTML::ParserDom parser;
		parser.parse(htmlPage);
		tree<HTML::Node> dom = parser.getTree();

		tree<HTML::Node>::iterator it = dom.begin();
		tree<HTML::Node>::iterator end = dom.end();
		for (; it != end; ++it)
		{
			if (it->tagName() =="TBODY")
			{
				nLoop++;
				if(m_nCode>=20150201)
				{
					//	新表格，第二个的 table 才是需要的
					if(nLoop==2)
						break;
				}
				else
				{
					//	旧表格
					break;
				}
			}
		}

		if(it!=end)
		{
			if(nLoop==1)
			{
				// 旧的表格
				CFundInSettlementOldHtmlParser parser(dom, *this);
				parser.Parse(it, "");
			}
			else
			{
				CFundInSettlementNewHtmlParser parser(dom, *this);
				parser.Parse(it, "");
			}
			int recordSize = m_vec_average_fund.size();
			if(recordSize == m_vec_final_fund.size() && 
				recordSize == m_vec_fund_type.size() &&
				recordSize == m_vec_input_fund.size() &&
				recordSize == m_vec_net_fund.size() &&
				recordSize == m_vec_output_fund.size() &&
				recordSize == m_vec_week.size() &&
				recordSize > 1)
			{
				return "AlreadyInMemory";
			}
			else
			{
				int error = 1;
			}
		}
	}

	return "";

}



void CFundInSettlement::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CFundInSettlement::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		for (int row = 0; row < m_vec_week.size(); row++)
		{
			int n1, n2, n3, n4, n5, n6;
			string strBegin = "", strEnd = "";
			if(sscanf_s(m_vec_week[row].c_str(), "%4d.%d.%d-%4d.%d.%d", &n1, &n2, &n3, &n4, &n5, &n6)==6)
			{
				char tmp[32] = { 0 };
				sprintf_s(tmp, 32, "%4d-%02d-%02d", n1, n2, n3);
				strBegin = string(tmp);
				sprintf_s(tmp, 32, "%4d-%02d-%02d", n4, n5, n6);
				strEnd = string(tmp);
			}
			else if(sscanf_s(m_vec_week[row].c_str(), "%4d.%d.%d-%d.%d", &n1, &n2, &n3, &n4, &n5)==5)
			{
				char tmp[32] = { 0 };
				sprintf_s(tmp, 32, "%4d-%02d-%02d", n1, n2, n3);
				strBegin = string(tmp);
				sprintf_s(tmp, 32, "%4d-%02d-%02d", n1, n4, n5);
				strEnd = string(tmp);
			}
			
			if(strEnd.empty())
				continue;


			ss.str("");
			ss << "insert into FundInSettlement ("
				<< "begin_date, end_date, fund_type, final_fund, average_fund, " 
				<< "input_fund, output_fund, net_fund )"
				<< " select \'" << strBegin << "\', \'" 
				<< strEnd << "\',\'"
				<< m_vec_fund_type[row].substr(0,8) << "\', "
				<< Double2String(m_vec_final_fund[row]) << ","
				<< Double2String(m_vec_average_fund[row]) << ", "
				<< Double2String(m_vec_input_fund[row]) << ","
				<< Double2String(m_vec_output_fund[row]) << ","
				<< Double2String(m_vec_net_fund[row]) << " "
				
				<< " from dual where not exists ( SELECT 1 from FundInSettlement " 
				<< " where begin_date=\'" << strBegin << "\' "
				<< " and fund_type=\'" << m_vec_fund_type[row] << "\'  )" ;

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 


			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << "共有数据 " << m_vec_week.size() ;
		ss << " OK, CFundInSettlement::ImportToDatabase 数据导入完毕. \r\n";
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



void CFundInSettlement::NotifyDatabaseImportedToWindow()
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

void CFundInSettlement::ExportFromDatabase(double startTime, double endTime)
{
	
}
