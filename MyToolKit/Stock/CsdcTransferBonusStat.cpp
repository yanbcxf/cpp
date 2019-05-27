#include "StdAfx.h"
#include "CsdcTransferBonusStat.h"
#include "UrlUtils.h"
#include "HtmlParser.h"

//////////////////////////////////////////////////////////////////////////

class CCsdcTransferBonusStatHtmlParser: public CHtmlParser
{
public:
	CCsdcTransferBonusStatHtmlParser(tree<HTML::Node> &dom, CCsdcTransferBonusStat & userContext)
		:CHtmlParser(dom),m_FundInSettlement(userContext)
	{
	}

	~CCsdcTransferBonusStatHtmlParser()
	{

	}

	void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent)
	{
		stringstream ss;
		int tr, td, p, font, strong;

		if(sscanf_s(xpath.c_str(), "/TR[%d]/TD[%d]", &tr, &td)==2)
		{

		}
	}

private:
	int		m_rowType;			//	
	string	m_recentDate;
	CCsdcTransferBonusStat & m_FundInSettlement;

};

//////////////////////////////////////////////////////////////////////////

CCsdcTransferBonusStat::CCsdcTransferBonusStat(void)
{
}

CCsdcTransferBonusStat::~CCsdcTransferBonusStat(void)
{
}



string CCsdcTransferBonusStat::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists CsdcTransferBonusStat (begin_date date , end_date date, fund_type varchar(32), " 
		<< " final_fund DECIMAL(12,2),  average_fund DECIMAL(12,2),  input_fund DECIMAL(12,2), "
		<< " output_fund DECIMAL(12,2), net_fund DECIMAL(12,2),  remark varchar(128) , "
		<< " PRIMARY KEY ( begin_date, fund_type))";
	return ss.str();
}

string  CCsdcTransferBonusStat::SaveAsCsv()
{
	stringstream ss;

	/*m_vec_week.clear();
	m_vec_final_fund.clear();
	m_vec_fund_type.clear();
	m_vec_average_fund.clear();
	m_vec_input_fund.clear();

	m_vec_output_fund.clear();
	m_vec_net_fund.clear();
	m_vec_remark.clear();*/



	return "";

}



void CCsdcTransferBonusStat::ImportToDatabase(void)
{
	if(m_nCode<=0)
		return;

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CCsdcTransferBonusStat::ImportToDatabase 开始导入数据.....\r\n";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		ss.str("");
		ss << SqlScript();
		sql << ss.str();


		ss.str("");
		ss << "【"  << Int2String(m_nCode) << "】";
		//ss << "共有数据 " << m_vec_week.size() ;
		ss << " OK, CCsdcTransferBonusStat::ImportToDatabase 数据导入完毕. \r\n";
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
