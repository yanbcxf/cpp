#include "StdAfx.h"
#include "CfiBonus.h"

#include "UrlUtils.h"


#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CCfiBonusHtmlParser: public CHtmlParser
{
public:
	CCfiBonusHtmlParser(tree<HTML::Node> &dom, CCfiBonus & userContext):CHtmlParser(dom),m_cfiBonus(userContext)
	{
		
	}

	~CCfiBonusHtmlParser()
	{
		
	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int tr, td;
		if(sscanf_s(xpath.c_str(), "/tr[%d]/td[%d]", &tr, &td)!=2)
			sscanf_s(xpath.c_str(), "/tr[%d]/td[%d]", &tr, &td);


		if(tr>=1)
		{
			switch(td)
			{
			case 2:	
				m_cfiBonus.m_vec_bonus_year.insert(m_cfiBonus.m_vec_bonus_year.end(), val);
				break;
			case 4:
				m_cfiBonus.m_vec_bonus_share.insert(m_cfiBonus.m_vec_bonus_share.end(), String2Double(val));
				break;
			case 5:
				m_cfiBonus.m_vec_reserve_to_common_share.insert(m_cfiBonus.m_vec_reserve_to_common_share.end(), 
					String2Double(val));
				break;
			case 7:
				m_cfiBonus.m_vec_bonus_cash.insert(m_cfiBonus.m_vec_bonus_cash.end(), String2Double(val));
				break;
			case 8:
				m_cfiBonus.m_vec_plan_announcement_date.insert(m_cfiBonus.m_vec_plan_announcement_date.end(), val);
				break;
			case 9:
				m_cfiBonus.m_vec_right_record_date.insert(m_cfiBonus.m_vec_right_record_date.end(), val);
				break;
			case 10:
				m_cfiBonus.m_vec_exit_right_date.insert(m_cfiBonus.m_vec_exit_right_date.end(), val);
				break;
			case 11:
				m_cfiBonus.m_vec_event_state.insert(m_cfiBonus.m_vec_event_state.end(), val);
				break;
			default:
				break;
			}
		}
	}

private:
	

	int		m_tr;
	CCfiBonus & m_cfiBonus;

};

//////////////////////////////////////////////////////////////////////////

CCfiBonus::CCfiBonus(void)
{
}

CCfiBonus::~CCfiBonus(void)
{
}



string CCfiBonus::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists cfi_bonus (Code int , bonus_year date, bonus_share DECIMAL(10,2), "
		<< " reserve_to_common_share DECIMAL(10,2), bonus_cash DECIMAL(10,2), plan_announcement_date date,  "
		<< " right_record_date date, exit_right_date date, event_state varchar(64), "
		<< " PRIMARY KEY ( Code, bonus_year))";
	return ss.str();
}



string  CCfiBonus::SaveAsCsv()
{
	stringstream ss;

	m_vec_bonus_year.clear();
	m_vec_bonus_share.clear();
	m_vec_reserve_to_common_share.clear();
	m_vec_bonus_cash.clear();
	m_vec_plan_announcement_date.clear();
	m_vec_right_record_date.clear();
	m_vec_exit_right_date.clear();
	m_vec_event_state.clear();

	
	try
	{
		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://data.cfi.cn/cfidata.aspx?sortfd=&sortway=&curpage=1&fr=content&ndk=A0A1934A1939A1957A1966A1983&xztj=&mystock=&searchstock=%06d", m_nCode);

		string test = string(tmpCode);
	
		string  strGBK=CUrlUtils::PostUrlOfCfi(tmpCode,NULL);
		strGBK = ReplaceString(strGBK, "value=显示", "value=\'显示\'");
		strGBK = ReplaceString(strGBK, "title=点击按代码排序", "title=\'点击按代码排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按证券简称排序", "title=\'点击按证券简称排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按分配年度排序", "title=\'点击按分配年度排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按是否分配排序", "title=\'点击按是否分配排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按送股*排序", "title=\'点击按送股*排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按转增股*排序", "title=\'点击按转增股*排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按送转总数*排序", "title=\'点击按送转总数*排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按派现/元*排序", "title=\'点击按派现/元*排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按预案公布日排序", "title=\'点击按预案公布日排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按股权登记日排序", "title=\'点击按股权登记日排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按除权除息日排序", "title=\'点击按除权除息日排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按事件进程排序", "title=\'点击按事件进程排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按公告日期排序", "title=\'点击按公告日期排序\'");


		HTML::ParserDom parser;
		parser.parse(strGBK);
		tree<HTML::Node> dom = parser.getTree();

		tree<HTML::Node>::iterator it = dom.begin();
		tree<HTML::Node>::iterator end = dom.end();
		for (; it != end; ++it)
		{
			if (it->tagName() =="table")
			{
				it->parseAttributes();

				map<string,string> attrs = it->attributes();
				if(attrs.size()>0 && attrs["class"] == "table_data")
				{
					break;
				}

			}
		}

		if(it!=end)
		{
			CCfiBonusHtmlParser parser(dom, *this);
			parser.Parse(it, "");
			return "AlreadyInMemory";
		}

		return "AlreadyInMemory";
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(),m_hWnd, m_nLogType);
	}

	return "";
}


void CCfiBonus::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCfiBonus::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		for (int row = 0; row < m_vec_bonus_year.size(); row++)
		{
			ss.str("");
			ss << "insert into cfi_bonus ("
				<< "code, bonus_year, bonus_share, reserve_to_common_share, bonus_cash, " 
				<< "plan_announcement_date, right_record_date, exit_right_date, event_state )"
				<< " select " <<  Int2String(m_nCode) << ", \'" 
				<< m_vec_bonus_year[row] << "\',"
				<< m_vec_bonus_share[row] << ", "
				<< m_vec_reserve_to_common_share[row] << ","
				<< m_vec_bonus_cash[row] << ", \'"
				<< m_vec_plan_announcement_date[row] << "\',\'"
				<< m_vec_right_record_date[row] << "\',\'"
				<< m_vec_exit_right_date[row] << "\',\'"
				<< m_vec_event_state[row] << "\' "

				<< " from dual where not exists (SELECT 1 from cfi_bonus " 
				<< " where bonus_year=\'" << m_vec_bonus_year[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ) ";

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			ss.str("");
			ss << "update cfi_bonus set "
				<< " bonus_share=" <<  m_vec_bonus_share[row] << ","
				<< " reserve_to_common_share=" << m_vec_reserve_to_common_share[row] <<","
				<< " bonus_cash=" << m_vec_bonus_cash[row] <<","
				<< " plan_announcement_date=\'" << m_vec_plan_announcement_date[row] << "\',"
				<< " right_record_date=\'" << m_vec_right_record_date[row] << "\',"
				<< " exit_right_date=\'" << m_vec_exit_right_date[row] << "\', "
				<< " event_state=\'" << m_vec_event_state[row] <<"\' "

				<< " where bonus_year=\'" << m_vec_bonus_year[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ";

			test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_bonus_year.size() ;
		ss << " OK, CCfiBonus::ImportToDatabase 数据导入完毕. \r\n";
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



void CCfiBonus::NotifyDatabaseImportedToWindow()
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
