#include "StdAfx.h"
#include "CfiPlacementOfShare.h"
#include "UrlUtils.h"


#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CCfiPlacementOfShareHtmlParser: public CHtmlParser
{
public:
	CCfiPlacementOfShareHtmlParser(tree<HTML::Node> &dom, CCfiPlacementOfShare & userContext):CHtmlParser(dom),m_cfiBonus(userContext)
	{

	}

	~CCfiPlacementOfShareHtmlParser()
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
			case 3:	
				m_cfiBonus.m_vec_placement_year.insert(m_cfiBonus.m_vec_placement_year.end(), val);
				break;
			case 4:
				m_cfiBonus.m_vec_placement_ratio.insert(m_cfiBonus.m_vec_placement_ratio.end(), String2Double(val));
				break;
			case 5:
				m_cfiBonus.m_vec_placement_price.insert(m_cfiBonus.m_vec_placement_price.end(), String2Double(val));
				break;
			case 7:
				m_cfiBonus.m_vec_placement_object.insert(m_cfiBonus.m_vec_placement_object.end(), val);
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
				m_cfiBonus.m_vec_market_date.insert(m_cfiBonus.m_vec_market_date.end(), val);
				break;
			default:
				break;
			}
		}
	}

private:


	int		m_tr;
	CCfiPlacementOfShare & m_cfiBonus;

};

//////////////////////////////////////////////////////////////////////////

CCfiPlacementOfShare::CCfiPlacementOfShare(void)
{
}

CCfiPlacementOfShare::~CCfiPlacementOfShare(void)
{
}


string CCfiPlacementOfShare::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists cfi_placement_of_share (Code int , placement_year date,  "
		<< " placement_ratio DECIMAL(10,2), placement_price DECIMAL(10,2),  plan_announcement_date date,  "
		<< " right_record_date date, exit_right_date date, placement_object varchar(256), "
		<< " market_date date, PRIMARY KEY ( Code, placement_year))";
	return ss.str();
}



string  CCfiPlacementOfShare::SaveAsCsv()
{
	stringstream ss;

	m_vec_placement_year.clear();
	m_vec_placement_ratio.clear();
	m_vec_placement_price.clear();
	m_vec_plan_announcement_date.clear();
	m_vec_right_record_date.clear();
	m_vec_exit_right_date.clear();
	m_vec_placement_object.clear();
	m_vec_market_date.clear();

	
	try
	{
		char tmpCode[256] = {0};
		sprintf(tmpCode, "http://data.cfi.cn/cfidata.aspx?sortfd=&sortway=&curpage=1&fr=content&ndk=A0A1934A1939A1957A1967A1984&xztj=&mystock=&searchstock=%06d", m_nCode);

		string test = string(tmpCode);
	
		string  strGBK=CUrlUtils::PostUrlOfCfi(tmpCode,NULL);
		strGBK = ReplaceString(strGBK, "value=显示", "value=\'显示\'");
		strGBK = ReplaceString(strGBK, "title=点击按公告日期排序", "title=\'点击按公告日期排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按代码排序", "title=\'点击按代码排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按证券简称排序", "title=\'点击按证券简称排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按配股年度排序", "title=\'点击按配股年度排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按配股比例*排序", "title=\'点击按配股比例*排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按配股价格排序", "title=\'点击按配股价格排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按配股代码排序", "title=\'点击按配股代码排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按配股对象排序", "title=\'点击按配股对象排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按预案公布日排序", "title=\'点击按预案公布日排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按股权登记日排序", "title=\'点击按股权登记日排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按除权日排序", "title=\'点击按除权日排序\'");
		strGBK = ReplaceString(strGBK, "title=点击按配股上市日排序", "title=\'点击按配股上市日排序\'");


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
			CCfiPlacementOfShareHtmlParser parser(dom, *this);
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


void CCfiPlacementOfShare::ImportToDatabase(void)
{
	if(m_nCode<=0 || m_vec_placement_year.size() ==0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCfiPlacementOfShare::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		for (int row = 0; row < m_vec_placement_year.size(); row++)
		{
			ss.str("");
			ss << "insert into cfi_placement_of_share ("
				<< "code, placement_year, placement_ratio, placement_price, plan_announcement_date, " 
				<< "right_record_date, exit_right_date, placement_object, market_date )"
				<< " select " <<  Int2String(m_nCode) << ", \'" 
				<< m_vec_placement_year[row] << "\',"
				<< m_vec_placement_ratio[row] << ", "
				<< m_vec_placement_price[row] << ",\'"
				<< m_vec_plan_announcement_date[row] << "\',\'"
				<< m_vec_right_record_date[row] << "\',\'"
				<< m_vec_exit_right_date[row] << "\',\'"
				<< m_vec_placement_object[row] << "\',\'"
				<< m_vec_market_date[row] << "\'"

				<< " from dual where not exists (SELECT 1 from cfi_placement_of_share " 
				<< " where placement_year=\'" << m_vec_placement_year[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ) ";

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			ss.str("");
			ss << "update cfi_placement_of_share set "
				<< " placement_ratio=" <<  m_vec_placement_ratio[row] << ","
				<< " placement_price=" << m_vec_placement_price[row] <<","
				<< " market_date=\'" << m_vec_market_date[row] <<"\',"
				<< " plan_announcement_date=\'" << m_vec_plan_announcement_date[row] << "\',"
				<< " right_record_date=\'" << m_vec_right_record_date[row] << "\',"
				<< " exit_right_date=\'" << m_vec_exit_right_date[row] << "\', "
				<< " placement_object=\'" << m_vec_placement_object[row] <<"\' "

				<< " where placement_year=\'" << m_vec_placement_year[row] << "\' " 
				<< " and Code=" << Int2String(m_nCode) << "  ";

			test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_placement_year.size() ;
		ss << " OK, CCfiPlacementOfShare::ImportToDatabase 数据导入完毕. \r\n";
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



void CCfiPlacementOfShare::NotifyDatabaseImportedToWindow()
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
