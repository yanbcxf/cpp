#include "StdAfx.h"
#include "CsdcSecurityIssueStat.h"

#include "UrlUtils.h"
#include "HtmlParser.h"


//////////////////////////////////////////////////////////////////////////

class CCsdcSecurityIssueStatTitleHtmlParser: public CHtmlParser
{
public:
	CCsdcSecurityIssueStatTitleHtmlParser(tree<HTML::Node> &dom, CCsdcSecurityIssueStat & userContext)
		:CHtmlParser(dom),m_Stat(userContext)
	{
	}

	~CCsdcSecurityIssueStatTitleHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int h, begin_year, begin_month, begin_date, end_year, end_month, end_date;

		if(sscanf_s(xpath.c_str(), "/h2[%d]", &h)==1)
		{
			if(sscanf_s(val.c_str(), "一周证券登记统计表（%d.%d.%d-%d.%d.%d）", 
				&begin_year, &begin_month, &begin_date, &end_year, &end_month, &end_date)==6 ||
				sscanf_s(val.c_str(), "一周证券发行登记统计表（%d.%d.%d-%d.%d.%d）", 
				&begin_year, &begin_month, &begin_date, &end_year, &end_month, &end_date)==6)
			{
				char tmp[32] = {0};
				sprintf_s(tmp,32, "%04d-%02d-%02d", begin_year, begin_month, begin_date);
				m_Stat.m_vec_begin_date.insert(m_Stat.m_vec_begin_date.end(), tmp);

				sprintf_s(tmp,32, "%04d-%02d-%02d", end_year, end_month, end_date);
				m_Stat.m_vec_end_date.insert(m_Stat.m_vec_end_date.end(), tmp);
			}
		}

	}

private:
	CCsdcSecurityIssueStat & m_Stat;

};

//////////////////////////////////////////////////////////////////////////

class CCsdcSecurityIssueStatHtmlParser: public CHtmlParser
{
public:
	CCsdcSecurityIssueStatHtmlParser(tree<HTML::Node> &dom, CCsdcSecurityIssueStat & userContext)
		:CHtmlParser(dom),m_Stat(userContext)
	{
	}

	~CCsdcSecurityIssueStatHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int temp, tr, td, p, span;

		transform(xpath.begin(), xpath.end(), xpath.begin(), ::toupper);  
		string::size_type pos1 = xpath.find("/TABLE");
		if(pos1!=string::npos) 
			xpath = xpath.substr(pos1);
		vector<string> vecMatch;
		if(Pcre2Grep(_T("/TR\\[[\\d]+\\]/TD\\[[\\d]+\\]/P\\[[\\d]+\\]/SPAN\\[[\\d]+\\]"), xpath.c_str() ,vecMatch )>0)
		{
			sscanf_s(vecMatch[0].c_str(), "/TR[%d]/TD[%d]/P[%d]/SPAN[%d]", &tr, &td, &p, &span);
		}
		else if(Pcre2Grep(_T("/TR\\[[\\d]+\\]/TD\\[[\\d]+\\]/SPAN\\[[\\d]+\\]"), xpath.c_str() ,vecMatch )>0)
		{
			sscanf_s(vecMatch[0].c_str(), "/TR[%d]/TD[%d]/SPAN[%d]", &tr, &td, &span);
		}


		if(vecMatch.size()>0)
		{
			if(td==3 && tr>=1)
			{
				switch(tr)
				{
				case 1:		/*  新增股票发行登记只数（只） */
					m_Stat.m_vec_new_stock_num.clear();
					m_Stat.m_vec_new_stock_num.insert(m_Stat.m_vec_new_stock_num.end(),
						atoi(val.c_str()));
					break;
				case 2:		/*  新增发行股本（万股） */
					m_Stat.m_vec_new_stock_capital.clear();
					m_Stat.m_vec_new_stock_capital.insert(m_Stat.m_vec_new_stock_capital.end(),
						String2Double(val.c_str()));
					break;
				case 3:		/* 新增流通股本（万股） */
					m_Stat.m_vec_new_stock_circulation_capital.clear();
					m_Stat.m_vec_new_stock_circulation_capital.insert(m_Stat.m_vec_new_stock_circulation_capital.end(),
						String2Double(val.c_str()));
					break;
				case 4:		/* 新增债券发行登记只数（只） */
					m_Stat.m_vec_new_bond_num.clear();
					m_Stat.m_vec_new_bond_num.insert(m_Stat.m_vec_new_bond_num.end(),
						atoi(val.c_str()));
					break;
				case 5:		/* 新增债券发行面额（亿元） */
					m_Stat.m_vec_new_bond_denomination.clear();
					m_Stat.m_vec_new_bond_denomination.insert(m_Stat.m_vec_new_bond_denomination.end(),
						String2Double(val.c_str()));
					break;
				case 6:		/* 新增基金发行登记只数（只）*/
					m_Stat.m_vec_new_fundation_num.clear();
					m_Stat.m_vec_new_fundation_num.insert(m_Stat.m_vec_new_fundation_num.end(),
						atoi(val.c_str()));
					break;
				case 7:		/* 新增基金发行面额（亿元） */
					m_Stat.m_vec_new_fundation_denomination.clear();
					m_Stat.m_vec_new_fundation_denomination.insert(m_Stat.m_vec_new_fundation_denomination.end(),
						String2Double(val.c_str()));
					break;
				}
			}
		}
	}

private:
	CCsdcSecurityIssueStat & m_Stat;

};


//////////////////////////////////////////////////////////////////////////

CCsdcSecurityIssueStat::CCsdcSecurityIssueStat(void)
{
}

CCsdcSecurityIssueStat::~CCsdcSecurityIssueStat(void)
{
}


string  CCsdcSecurityIssueStat::SaveAsCsv()
{
	if(m_nCode<20050603)
		return "";

	stringstream ss;

	m_vec_begin_date.clear();
	m_vec_end_date.clear();
	m_vec_new_stock_num.clear();
	m_vec_new_stock_capital.clear();
	m_vec_new_stock_circulation_capital.clear();
	m_vec_new_bond_num.clear();
	m_vec_new_bond_denomination.clear();
	m_vec_new_fundation_num.clear();
	m_vec_new_fundation_denomination.clear();

	char tmpCode[256] = {0};
	sprintf(tmpCode, "http://www.chinaclear.cn/cms-search/view.action?action=china");

	char tmpData[256] = {0};
	sprintf(tmpData, "dateType=&dateStr=%s.%s.%s&channelIdStr=535b821c09eb424a818b82a2364caf74",
		Int2String(m_nCode).substr(0,4).c_str(), 
		Int2String(m_nCode).substr(4,2).c_str(),
		Int2String(m_nCode).substr(6,2).c_str());

	/*必须使用 Post 方法，PostUrlOfSouhu 的第二个参数不为 NULL 时使用了 Post ，否则使用 Get 方法*/
	string htmlPage =CUrlUtils::PostUrlOfSouhu(tmpCode,tmpData);
	htmlPage = Utf8_GBK(htmlPage);

	/*解析标题，获取时间*/
	{
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
				if(attrs.size()>0 && attrs["class"] == "SettlementTitle")
				{
					break;
				}
			}
		}

		if(it!=end)
		{
			CCsdcSecurityIssueStatTitleHtmlParser parser(dom, *this);
			parser.Parse(it, "");
		}
		else
			return "";
	}

	/*解析表格内容*/
	{
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
				if(attrs.size()>0 && attrs["id"] == "settlementList")
				{
					break;
				}
			}
		}

		if(it!=end)
		{
			CCsdcSecurityIssueStatHtmlParser parser(dom, *this);
			parser.Parse(it, "");

			int recordSize = m_vec_begin_date.size();
			if(m_vec_new_bond_num.size()==recordSize &&
				m_vec_new_bond_denomination.size()==recordSize &&
				m_vec_new_fundation_denomination.size()==recordSize &&
				m_vec_new_fundation_num.size()==recordSize &&
				m_vec_new_stock_capital.size()==recordSize &&
				m_vec_new_stock_circulation_capital.size()==recordSize &&
				m_vec_new_stock_num.size()==recordSize)
				return "AlreadyInMemory";
			else
			{
				ss << "【"  << Int2String(m_nCode) << "】";
				ss << "CCsdcSecurityIssueStat::解析数据 Err \r\n";
				sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
			}
		}
	}

	return "";

}


string CCsdcSecurityIssueStat::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CsdcSecurityIssueStat (begin_date date , end_date date, new_stock_num int, " 
		<< " new_stock_capital DECIMAL(12,2),  new_stock_circulation_capital DECIMAL(12,2),  new_bond_num int, "
		<< " new_bond_denomination DECIMAL(12,2), new_fundation_num int ,new_fundation_denomination DECIMAL(12,2), "
		<< " PRIMARY KEY ( begin_date))";
	return ss.str();
}


void CCsdcSecurityIssueStat::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCsdcSecurityIssueStat::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();

		for(int i =0; i<m_vec_begin_date.size(); i++)
		{
			ss.str("");
			ss << "insert into CsdcSecurityIssueStat ("
				<< "begin_date, end_date,  " 
				<< " new_stock_num , new_stock_capital ,new_stock_circulation_capital, "
				<< " new_bond_num , new_bond_denomination , "
				<< " new_fundation_num , new_fundation_denomination ) "
				<< " select \'" << m_vec_begin_date[i] << "\', \'" 
				<< m_vec_end_date[i] << "\',"
				<< Double2String(m_vec_new_stock_num[i]) << ","
				<< Double2String(m_vec_new_stock_capital[i]) << ", "
				<< Double2String(m_vec_new_stock_circulation_capital[i]) << ","
				<< Double2String(m_vec_new_bond_num[i]) << ","
				<< Double2String(m_vec_new_bond_denomination[i]) << ", "
				<< Double2String(m_vec_new_fundation_num[i]) << ", "
				<< Double2String(m_vec_new_fundation_denomination[i]) << " "

				<< " from dual where not exists ( SELECT 1 from CsdcSecurityIssueStat " 
				<< " where begin_date=\'" <<  m_vec_begin_date[i] << "\' )";

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 
		}


		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		ss << "共有数据 " << m_vec_begin_date.size() ;
		ss << " OK, CCsdcSecurityIssueStat::ImportToDatabase 数据导入完毕. \r\n";
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
