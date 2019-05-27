#include "StdAfx.h"
#include "CsdcFundInSettlementStat.h"

#include "UrlUtils.h"
#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class CCsdcFundInSettlementStatTitleHtmlParser: public CHtmlParser
{
public:
	CCsdcFundInSettlementStatTitleHtmlParser(tree<HTML::Node> &dom, CCsdcFundInSettlementStat & userContext)
		:CHtmlParser(dom),m_Stat(userContext)
	{
	}

	~CCsdcFundInSettlementStatTitleHtmlParser()
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
	CCsdcFundInSettlementStat & m_Stat;

};

//////////////////////////////////////////////////////////////////////////

class CCsdcFundInSettlementStatHtmlParser: public CHtmlParser
{
public:
	CCsdcFundInSettlementStatHtmlParser(tree<HTML::Node> &dom, CCsdcFundInSettlementStat & userContext)
		:CHtmlParser(dom),m_Stat(userContext)
	{
	}

	~CCsdcFundInSettlementStatHtmlParser()
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
					/*m_Stat.m_vec_new_stock_num.clear();
					m_Stat.m_vec_new_stock_num.insert(m_Stat.m_vec_new_stock_num.end(),
						atoi(val.c_str()));*/
					break;
				case 2:		/*  新增发行股本（万股） */
					/*m_Stat.m_vec_new_stock_capital.clear();
					m_Stat.m_vec_new_stock_capital.insert(m_Stat.m_vec_new_stock_capital.end(),
						String2Double(val.c_str()));*/
					break;
				case 3:		/* 新增流通股本（万股） */
					/*m_Stat.m_vec_new_stock_circulation_capital.clear();
					m_Stat.m_vec_new_stock_circulation_capital.insert(m_Stat.m_vec_new_stock_circulation_capital.end(),
						String2Double(val.c_str()));*/
					break;
				case 4:		/* 新增债券发行登记只数（只） */
					/*m_Stat.m_vec_new_bond_num.clear();
					m_Stat.m_vec_new_bond_num.insert(m_Stat.m_vec_new_bond_num.end(),
						atoi(val.c_str()));*/
					break;
			
				}
			}
		}
	}

private:
	CCsdcFundInSettlementStat & m_Stat;

};


//////////////////////////////////////////////////////////////////////////

CCsdcFundInSettlementStat::CCsdcFundInSettlementStat(void)
{
}

CCsdcFundInSettlementStat::~CCsdcFundInSettlementStat(void)
{
}


string CCsdcFundInSettlementStat::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists FundInSettlement (begin_date date , end_date date, fund_type varchar(32), " 
		<< " final_fund DECIMAL(12,2),  average_fund DECIMAL(12,2),  input_fund DECIMAL(12,2), "
		<< " output_fund DECIMAL(12,2), net_fund DECIMAL(12,2),  remark varchar(128) , "
		<< " PRIMARY KEY ( begin_date, fund_type))";
	return ss.str();
}

string  CCsdcFundInSettlementStat::SaveAsCsv()
{
	stringstream ss;

	//m_vec_week.clear();
	//m_vec_final_fund.clear();
	//m_vec_fund_type.clear();
	//m_vec_average_fund.clear();
	//m_vec_input_fund.clear();

	//m_vec_output_fund.clear();
	//m_vec_net_fund.clear();
	//m_vec_remark.clear();



	return "";

}



void CCsdcFundInSettlementStat::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCsdcFundInSettlementStat::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();


		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		//ss << "共有数据 " << m_vec_week.size() ;
		ss << " OK, CCsdcFundInSettlementStat::ImportToDatabase 数据导入完毕. \r\n";
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
