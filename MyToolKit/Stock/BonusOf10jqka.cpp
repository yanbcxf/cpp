#include "StdAfx.h"
#include "Stock.h"
#include "BonusOf10jqka.h"

#include "UrlUtils.h"


#include "HtmlParser.h"


//////////////////////////////////////////////////////////////////////////

class CBonusOf10jqkaHtmlParser: public CHtmlParser
{
public:
	CBonusOf10jqkaHtmlParser(tree<HTML::Node> &dom, CBonusOf10jqka & userContext):CHtmlParser(dom),m_cfiBonus(userContext)
	{

	}

	~CBonusOf10jqkaHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int tbody, tr, td;
		if(sscanf_s(xpath.c_str(), "/tbody[%d]/tr[%d]/td[%d]", &tbody, &tr, &td)!=3)
			sscanf_s(xpath.c_str(), "/tbody[%d]/tr[%d]/td[%d]", &tbody, &tr, &td);


		if(tbody==1)
		{
			switch(td)
			{
			case 0:		/*股东大会日期*/	
				m_cfiBonus.m_vec_bonus_year.insert(m_cfiBonus.m_vec_bonus_year.end(), val);
				m_cfiBonus.m_vec_plan_announcement_date.insert(m_cfiBonus.m_vec_plan_announcement_date.end(), val);
				break;
			case 3:		/*10送2股转5股派0.6元(含税)*/
				{
					vector<string> firstMatch;
					float nBonusShare = 0;
					if(Pcre2Grep(_T("送[\\d\\.\\s]+股"), val.c_str(), firstMatch )>0)
					{
						sscanf_s(firstMatch[0].c_str(), "送%f股", &nBonusShare);
					}
					m_cfiBonus.m_vec_bonus_share.insert(m_cfiBonus.m_vec_bonus_share.end(), nBonusShare);

					nBonusShare = 0;
					firstMatch.clear();
					if(Pcre2Grep(_T("转[\\d\\.\\s]+股"), val.c_str(), firstMatch )>0)
					{
						sscanf_s(firstMatch[0].c_str(), "转%f股", &nBonusShare);
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
			
			case 4:
				m_cfiBonus.m_vec_right_record_date.insert(m_cfiBonus.m_vec_right_record_date.end(), val);
				break;
			case 5:
				m_cfiBonus.m_vec_exit_right_date.insert(m_cfiBonus.m_vec_exit_right_date.end(), val);
				break;
			case 7:
				m_cfiBonus.m_vec_event_state.insert(m_cfiBonus.m_vec_event_state.end(), val);
				break;
			default:
				break;
			}
		}
	}

private:


	int		m_tr;
	CBonusOf10jqka & m_cfiBonus;

};

//////////////////////////////////////////////////////////////////////////

CBonusOf10jqka::CBonusOf10jqka(void)
{
}

CBonusOf10jqka::~CBonusOf10jqka(void)
{
}




string CBonusOf10jqka::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists 10jqka_bonus (Code int , bonus_year date, bonus_share DECIMAL(10,2), "
		<< " reserve_to_common_share DECIMAL(10,2), bonus_cash DECIMAL(10,2), plan_announcement_date date,  "
		<< " right_record_date date, exit_right_date date, event_state varchar(64), "
		<< " PRIMARY KEY ( Code, bonus_year))";
	return ss.str();
}



string  CBonusOf10jqka::SaveAsCsv()
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
		sprintf(tmpCode, "http://stockpage.10jqka.com.cn/%06d/bonus/#bonuslist", m_nCode);

		string test = string(tmpCode);
	
		string  strGBK=CUrlUtils::PostUrlOfSouhu(tmpCode,NULL);
		strGBK = Utf8_GBK(strGBK);
		strGBK = ReplaceString(strGBK, "value=显示", "value=\'显示\'");
		

		HTML::ParserDom parser;
		parser.parse(strGBK);
		tree<HTML::Node> dom = parser.getTree();

		tree<HTML::Node>::iterator it = dom.begin();
		tree<HTML::Node>::iterator end = dom.end();
		int nLoopLimit  = 0;
		for (; it != end; ++it)
		{
			if (it->tagName() =="table")
			{
				it->parseAttributes();

				map<string,string> attrs = it->attributes();
				if(attrs.size()>0 && attrs["class"] == "m_table m_hl mt15")
				{
					break;
				}

			}
			nLoopLimit++;
			if(nLoopLimit%100 == 0)
				Sleep(10);
		}

		if(it!=end)
		{
			CBonusOf10jqkaHtmlParser parser(dom, *this);
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


void CBonusOf10jqka::ImportToDatabase(void)
{
	stringstream ss;

	if(m_nCode<=0)
		return;

	int nSize = m_vec_bonus_year.size();
	if(nSize != m_vec_bonus_share.size()
		|| nSize !=m_vec_reserve_to_common_share.size()
		|| nSize !=m_vec_bonus_cash.size()
		|| nSize != m_vec_plan_announcement_date.size()
		|| nSize != m_vec_right_record_date.size()
		|| nSize != m_vec_exit_right_date.size()
		|| nSize != m_vec_event_state.size())
	{
		ss << "【" <<  Int2String(m_nCode, "%06d") << "】 CBonusOf10jqka::ImportToDatabase " 
			<< " 数据长度不一致！";
		LOG4CPLUS_DEBUG(g_logger, ss.str());

		return ;
	}


	try
	{
		vector<string>  vecSqlStr;
		CStockDownloadData sdd = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchStockByCode(m_nCode);
		string maxDate = sdd._10jqka_bonus;

		for (int row = 0; row < m_vec_bonus_year.size(); row++)
		{
			if(m_vec_bonus_year[row] > sdd._10jqka_bonus)
			{
				ss.str("");
				ss << "insert into 10jqka_bonus ("
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

					<< " from dual where not exists (SELECT 1 from 10jqka_bonus " 
					<< " where bonus_year=\'" << m_vec_bonus_year[row] << "\' " 
					<< " and Code=" << Int2String(m_nCode) << "  ) ";

				string test = ss.str();
				vecSqlStr.insert(vecSqlStr.end(), test);

				ss.str("");
				ss << "update 10jqka_bonus set "
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

			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateStockByCode(m_nCode, "_10jqka_bonus", maxDate);

			ss.str("");
			ss << "共有数据 " << vecSqlStr.size() ;
			ss << " OK, CBonusOf10jqka::ImportToDatabase 数据导入完毕. \r\n";
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



void CBonusOf10jqka::NotifyDatabaseImportedToWindow()
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
