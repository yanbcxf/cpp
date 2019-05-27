#include "StdAfx.h"
#include "Stock.h"
#include "CninfoPlacementOfShare.h"
#include "UrlUtils.h"


#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CCninfoPlacementOfShareHtmlParser: public CHtmlParser
{
public:
	CCninfoPlacementOfShareHtmlParser(tree<HTML::Node> &dom, CCninfoPlacementOfShare & userContext):CHtmlParser(dom),m_cfiBonus(userContext)
	{

	}

	~CCninfoPlacementOfShareHtmlParser()
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
			case 0:	
				//	配股年度
				{
					int nYear = atoi(val.c_str());
					if(nYear>0)
					{
						string strDate = Int2String(nYear) + "-12-31"; 
						m_cfiBonus.m_vec_placement_year.insert(m_cfiBonus.m_vec_placement_year.end(), strDate);
					}
					else
					{
						m_cfiBonus.m_vec_placement_year.insert(m_cfiBonus.m_vec_placement_year.end(), "0000-00-00");
					}

				}
				break;
			case 1:
				//	配股方案
				{
					vector<string> firstMatch;
					float nBonusShare = 0;
					if(Pcre2Grep(_T("配[\\d\\.\\s]+股"), val.c_str(), firstMatch )>0)
					{
						sscanf_s(firstMatch[0].c_str(), "配%f股", &nBonusShare);
					}
					m_cfiBonus.m_vec_placement_ratio.insert(m_cfiBonus.m_vec_placement_ratio.end(), nBonusShare);
				}
				
				break;
			case 2:
				//	配股价
				m_cfiBonus.m_vec_placement_price.insert(m_cfiBonus.m_vec_placement_price.end(), String2Double(val));
				break;
			case 3:
				//	股权登记日
				m_cfiBonus.m_vec_right_record_date.insert(m_cfiBonus.m_vec_right_record_date.end(), val);
				break;
			case 4:
				//	除权基准日
				m_cfiBonus.m_vec_exit_right_date.insert(m_cfiBonus.m_vec_exit_right_date.end(), val);
				break;
			case 5:
				//	配股交款起止日
				
				break;
			case 6:
				//	配股可流通部分上市日
				m_cfiBonus.m_vec_market_date.insert(m_cfiBonus.m_vec_market_date.end(), val);
				break;
			default:
				break;
			}
		}
	}

private:


	int		m_tr;
	CCninfoPlacementOfShare & m_cfiBonus;

};

//////////////////////////////////////////////////////////////////////////

CCninfoPlacementOfShare::CCninfoPlacementOfShare(void)
{
}

CCninfoPlacementOfShare::~CCninfoPlacementOfShare(void)
{
}


string CCninfoPlacementOfShare::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists cninfo_placement_of_share (Code int , placement_year date,  "
		<< " placement_ratio DECIMAL(10,2), placement_price DECIMAL(10,2),  plan_announcement_date date,  "
		<< " right_record_date date, exit_right_date date, placement_object varchar(256), "
		<< " market_date date, PRIMARY KEY ( Code, placement_year))";
	return ss.str();
}



string  CCninfoPlacementOfShare::SaveAsCsv()
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
		if(m_nCode>=600000)
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/allotment/shmb%06d.html", m_nCode);
		else if(m_nCode>=300000)
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/allotment/szcn%06d.html", m_nCode);
		else if(m_nCode>=2000 && m_nCode < 4999)
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/allotment/szsme%06d.html", m_nCode);
		else
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/allotment/szmb%06d.html", m_nCode);
		string test = string(tmpCode);
	
		string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);
		

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
				if(attrs.size()>0 && attrs["width"] == "100%")
				{
					break;
				}

			}
		}

		if(it!=end)
		{
			CCninfoPlacementOfShareHtmlParser parser(dom, *this);
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


void CCninfoPlacementOfShare::ImportToDatabase(void)
{
	if(m_nCode<=0 || m_vec_placement_year.size() ==0)
		return;

	
	stringstream ss;
	try
	{
		
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.cninfo_placement_of_share;

		for (int row = 0; row < m_vec_placement_year.size(); row++)
		{
			if(m_vec_placement_year[row] > sdd.cninfo_placement_of_share)
			{
				ss.str("");
				ss << "insert into cninfo_placement_of_share ("
					<< "code, placement_year, placement_ratio, placement_price,  " 
					<< "right_record_date, exit_right_date, market_date )"
					<< " select " <<  Int2String(m_nCode) << ", \'" 
					<< m_vec_placement_year[row] << "\',"
					<< m_vec_placement_ratio[row] << ", "
					<< m_vec_placement_price[row] << ",\'"
					<< m_vec_right_record_date[row] << "\',\'"
					<< m_vec_exit_right_date[row] << "\',\'"
					<< m_vec_market_date[row] << "\'"

					<< " from dual where not exists (SELECT 1 from cninfo_placement_of_share " 
					<< " where placement_year=\'" << m_vec_placement_year[row] << "\' " 
					<< " and Code=" << Int2String(m_nCode) << "  ) ";

				string test = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), test);

				if(maxDate < m_vec_placement_year[row])
					maxDate = m_vec_placement_year[row];
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

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "cninfo_placement_of_share", maxDate);
		
			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CCninfoPlacementOfShare::ImportToDatabase 数据导入完毕. \r\n";
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



void CCninfoPlacementOfShare::NotifyDatabaseImportedToWindow()
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
