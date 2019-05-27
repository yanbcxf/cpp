#include "StdAfx.h"
#include "Stock.h"

#include "CninfoBonus.h"

#include "UrlUtils.h"


#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

/*

<tr>
<td width="87" class="zx_data font_weight">分红年度</td>
<td width="285" class="zx_data3 font_weight">分红方案</td>
<td width="101" class="zx_data3 font_weight">股权登记日</td>
<td width="101" class="zx_data3 font_weight">除权基准日</td>
<td width="102" class="zx_data3 font_weight">红股上市日</td>
</tr>

*/

class CCninfoBonusHtmlParser: public CHtmlParser
{
public:
	CCninfoBonusHtmlParser(tree<HTML::Node> &dom, CCninfoBonus & userContext):CHtmlParser(dom),m_cfiBonus(userContext)
	{
		
	}

	~CCninfoBonusHtmlParser()
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
				{
					int nYear = atoi(val.c_str());
					if(nYear>0)
					{
						string strDate = Int2String(nYear) + "-12-31";
						if(val.find("中期")!=string::npos)
							strDate = Int2String(nYear) + "-06-30";

						m_cfiBonus.m_vec_bonus_year.insert(m_cfiBonus.m_vec_bonus_year.end(), strDate);
					}
					else
					{
						m_cfiBonus.m_vec_bonus_year.insert(m_cfiBonus.m_vec_bonus_year.end(), "0000-00-00");
					}
				}
				break;
			case 1:
				{
					vector<string> firstMatch;
					float nBonusShare = 0;
					if(Pcre2Grep(_T("送[\\d\\.\\s]+"), val.c_str(), firstMatch )>0)
					{
						sscanf_s(firstMatch[0].c_str(), "送%f", &nBonusShare);
					}
					m_cfiBonus.m_vec_bonus_share.insert(m_cfiBonus.m_vec_bonus_share.end(), nBonusShare);

					nBonusShare = 0;
					firstMatch.clear();
					if(Pcre2Grep(_T("转增[\\d\\.\\s]+股"), val.c_str(), firstMatch )>0)
					{
						sscanf_s(firstMatch[0].c_str(), "转增%f股", &nBonusShare);
					}
					m_cfiBonus.m_vec_reserve_to_common_share.insert(m_cfiBonus.m_vec_reserve_to_common_share.end(),nBonusShare);

					nBonusShare = 0;
					firstMatch.clear();
					if(Pcre2Grep(_T("派[\\d\\.\\s]+元"), val.c_str(), firstMatch )>0)
					{
						sscanf_s(firstMatch[0].c_str(), "派%f元", &nBonusShare);
					}
					m_cfiBonus.m_vec_bonus_cash.insert(m_cfiBonus.m_vec_bonus_cash.end(), nBonusShare);
				}
				break;
			case 2:
				//	股权登记日
				m_cfiBonus.m_vec_right_record_date.insert(m_cfiBonus.m_vec_right_record_date.end(), val);
				break;
			case 3:
				//	除权基准日
				m_cfiBonus.m_vec_exit_right_date.insert(m_cfiBonus.m_vec_exit_right_date.end(), val);
				break;
			case 4:
				//	红股上市日
				m_cfiBonus.m_vec_plan_announcement_date.insert(m_cfiBonus.m_vec_plan_announcement_date.end(), val);
				break;
			default:
				break;
			}
		}
	}

private:
	

	int		m_tr;
	CCninfoBonus & m_cfiBonus;

};

//////////////////////////////////////////////////////////////////////////

CCninfoBonus::CCninfoBonus(void)
{
}

CCninfoBonus::~CCninfoBonus(void)
{
}



string CCninfoBonus::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists cninfo_bonus (Code int , bonus_year date, bonus_share DECIMAL(10,2), "
		<< " reserve_to_common_share DECIMAL(10,2), bonus_cash DECIMAL(10,2), plan_announcement_date date,  "
		<< " right_record_date date, exit_right_date date, event_state varchar(64), "
		<< " PRIMARY KEY ( Code, bonus_year))";
	return ss.str();
}



string  CCninfoBonus::SaveAsCsv()
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
		if(m_nCode>=600000)
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/dividend/shmb%06d.html", m_nCode);
		else if(m_nCode>=300000)
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/dividend/szcn%06d.html", m_nCode);
		else if(m_nCode>=2000 && m_nCode < 4999)
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/dividend/szsme%06d.html", m_nCode);
		else
			sprintf(tmpCode, "http://www.cninfo.com.cn//information/dividend/szmb%06d.html", m_nCode);

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
			CCninfoBonusHtmlParser parser(dom, *this);
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


void CCninfoBonus::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;

	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd.cninfo_bonus;

		for (int row = 0; row < m_vec_bonus_year.size(); row++)
		{
			if(m_vec_bonus_year[row] > sdd.cninfo_bonus)
			{
				ss.str("");
				ss << "insert into cninfo_bonus ("
					<< "code, bonus_year, bonus_share, reserve_to_common_share, bonus_cash, " 
					<< "right_record_date, exit_right_date )"
					<< " select " <<  Int2String(m_nCode) << ", \'" 
					<< m_vec_bonus_year[row] << "\',"
					<< m_vec_bonus_share[row] << ", "
					<< m_vec_reserve_to_common_share[row] << ","
					<< m_vec_bonus_cash[row] << ", \'"
					//<< m_vec_plan_announcement_date[row] << "\',\'"
					<< m_vec_right_record_date[row] << "\',\'"
					<< m_vec_exit_right_date[row] << "\' "
					//<< m_vec_event_state[row] << "\' "

					<< " from dual where not exists (SELECT 1 from cninfo_bonus " 
					<< " where bonus_year=\'" << m_vec_bonus_year[row] << "\' " 
					<< " and Code=" << Int2String(m_nCode) << "  ) ";

				string test = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), test);

				if(maxDate < m_vec_bonus_year[row])
					maxDate = m_vec_bonus_year[row];
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

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "cninfo_bonus", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CCninfoBonus::ImportToDatabase 数据导入完毕. \r\n";
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



void CCninfoBonus::NotifyDatabaseImportedToWindow()
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
