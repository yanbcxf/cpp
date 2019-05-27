#include "StdAfx.h"
#include "AbsoluteBreadthIndex.h"

CAbsoluteBreadthIndex::CAbsoluteBreadthIndex(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CAbsoluteBreadthIndex::~CAbsoluteBreadthIndex(void)
{
}


void CAbsoluteBreadthIndex::BeginCalculate()
{
	stringstream ss;
	
	
	try{
		ss << "create table if not exists absolute_breadth_index (tradedate date, stock_plate varchar(32), "
			<< " absolute_breadth_index int, "
			<< " advance_decline_line int, "
			<< " advance_decline_ratio double, "
			<< " PRIMARY KEY ( tradedate, stock_plate))";

		session sql(g_MysqlPool);
		sql << ss.str();

		ss.str("");
		ss << "select  date_format(max(tradedate), '%Y-%m-%d') as max_date from absolute_breadth_index ";

		row r;
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();
		if(st.fetch())
		{
			string tmp = r.get<string>("max_date");
			if(tmp!="0000-00-00")
			{
				m_strLatest = tmp;

				ss.str("");
				ss << IndexName() << ", m_strLatest = " << m_strLatest;
				LOG4CPLUS_DEBUG(g_logger, ss.str() );
			} 
		}
	}
	catch(...)
	{
		
	}
}

void CAbsoluteBreadthIndex::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{
	m_nAdvance = 0;		m_nDecline = 0;
	m_nAdvance_of_entrepreneurship = 0;		m_nDecline_of_entrepreneurship = 0;
	m_nAdvance_of_middle_small = 0;			m_nDecline_of_middle_small = 0;
	m_nAdvance_of_shanghai = 0;				m_nDecline_of_shanghai = 0;
	m_nAdvance_of_shenzhen_ashare = 0;		m_nDecline_of_shenzhen_ashare = 0;
}

void   CAbsoluteBreadthIndex::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
										string strTradedate, string strCode, string strPlate)
{
	// 逐个计算 每股的涨跌情况, 前一个交易日停牌股票不纳入计算
	if(yesterday.close <0 || today.close < 0)
		return;
	
	if(today.close - RecoverPrice(yesterday.close, today) > 0)
	{
		m_nAdvance ++;

		if(strPlate.find("创业")!=string::npos)
			m_nAdvance_of_entrepreneurship++;
		else if(strPlate.find("中小企业")!=string::npos)
			m_nAdvance_of_middle_small ++;
		else if(strPlate.find("深圳")!=string::npos)
			m_nAdvance_of_shenzhen_ashare ++;

		else if(strPlate.find("上海")!=string::npos)
			m_nAdvance_of_shanghai ++;
	}
	else if(today.close - RecoverPrice(yesterday.close, today) < 0)
	{
		m_nDecline ++;

		if(strPlate.find("创业")!=string::npos)
			m_nDecline_of_entrepreneurship++;
		else if(strPlate.find("中小企业")!=string::npos)
			m_nDecline_of_middle_small ++;
		else if(strPlate.find("深圳")!=string::npos)
			m_nDecline_of_shenzhen_ashare ++;

		else if(strPlate.find("上海")!=string::npos)
			m_nDecline_of_shanghai ++;
	}
}

void  CAbsoluteBreadthIndex::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	if(m_mapIndex.count(strTradedate)==0)
	{
		CTechnicalAnalysisMarketIndexData tai;
		m_mapIndex[strTradedate] = tai;		
	}

	string strYesterday = ta.m_vecTradedate[ta.m_nLateset - 1];
	if(m_mapIndex.count( strYesterday)==0)
	{
		CTechnicalAnalysisMarketIndexData tai;
		m_mapIndex[strYesterday] = tai;		
	}

	//////////////////////////////////////////////////////////////////////////
	if(m_nAdvance - m_nDecline > 0)
		m_mapIndex[strTradedate].total.absolute_breadth_index = m_nAdvance - m_nDecline;
	else
		m_mapIndex[strTradedate].total.absolute_breadth_index = m_nDecline - m_nAdvance;

	if(m_nAdvance_of_entrepreneurship - m_nDecline_of_entrepreneurship > 0)
		m_mapIndex[strTradedate].entrepreneurship.absolute_breadth_index = m_nAdvance_of_entrepreneurship - m_nDecline_of_entrepreneurship;
	else
		m_mapIndex[strTradedate].entrepreneurship.absolute_breadth_index = m_nDecline_of_entrepreneurship - m_nAdvance_of_entrepreneurship;

	if(m_nAdvance_of_middle_small - m_nDecline_of_middle_small > 0)
		m_mapIndex[strTradedate].middle_small.absolute_breadth_index = m_nAdvance_of_middle_small - m_nDecline_of_middle_small;
	else
		m_mapIndex[strTradedate].middle_small.absolute_breadth_index = m_nDecline_of_middle_small - m_nAdvance_of_middle_small;

	if(m_nAdvance_of_shenzhen_ashare - m_nDecline_of_shenzhen_ashare > 0)
		m_mapIndex[strTradedate].shenzhen_ashare.absolute_breadth_index = m_nAdvance_of_shenzhen_ashare - m_nDecline_of_shenzhen_ashare;
	else
		m_mapIndex[strTradedate].shenzhen_ashare.absolute_breadth_index = m_nDecline_of_shenzhen_ashare - m_nAdvance_of_shenzhen_ashare;


	if(m_nAdvance_of_shanghai - m_nDecline_of_shanghai > 0)
		m_mapIndex[strTradedate].shanghai.absolute_breadth_index = m_nAdvance_of_shanghai - m_nDecline_of_shanghai;
	else
		m_mapIndex[strTradedate].shanghai.absolute_breadth_index = m_nDecline_of_shanghai - m_nAdvance_of_shanghai;

	//////////////////////////////////////////////////////////////////////////
	m_mapIndex[strTradedate].total.advance_decline_line 
		= m_mapIndex[strYesterday].total.advance_decline_line + (m_nAdvance - m_nDecline);
	
	m_mapIndex[strTradedate].entrepreneurship.advance_decline_line = 
		m_mapIndex[strYesterday].entrepreneurship.advance_decline_line + (m_nAdvance_of_entrepreneurship - m_nDecline_of_entrepreneurship);
	
	m_mapIndex[strTradedate].shenzhen_ashare.advance_decline_line 
		= m_mapIndex[strYesterday].shenzhen_ashare.advance_decline_line + (m_nAdvance_of_shenzhen_ashare - m_nDecline_of_shenzhen_ashare);
	
	m_mapIndex[strTradedate].middle_small.advance_decline_line 
		= m_mapIndex[strYesterday].middle_small.advance_decline_line + (m_nAdvance_of_middle_small - m_nDecline_of_middle_small);
	
	m_mapIndex[strTradedate].shanghai.advance_decline_line 
		= m_mapIndex[strYesterday].shanghai.advance_decline_line + (m_nAdvance_of_shanghai - m_nDecline_of_shanghai);

	//////////////////////////////////////////////////////////////////////////
	m_mapIndex[strTradedate].total.advance_decline_ratio = 100;
	m_mapIndex[strTradedate].entrepreneurship.advance_decline_ratio = 100;
	m_mapIndex[strTradedate].shenzhen_ashare.advance_decline_ratio = 100;
	m_mapIndex[strTradedate].middle_small.advance_decline_ratio = 100;
	m_mapIndex[strTradedate].shanghai.advance_decline_ratio = 100;

	if(m_nDecline)
		m_mapIndex[strTradedate].total.advance_decline_ratio = m_nAdvance * 1.0 / m_nDecline;

	if(m_nDecline_of_entrepreneurship)
		m_mapIndex[strTradedate].entrepreneurship.advance_decline_ratio = m_nAdvance_of_entrepreneurship * 1.0 / m_nDecline_of_entrepreneurship;

	if(m_nDecline_of_shenzhen_ashare)
		m_mapIndex[strTradedate].shenzhen_ashare.advance_decline_ratio = m_nAdvance_of_shenzhen_ashare * 1.0 / m_nDecline_of_shenzhen_ashare;

	if(m_nDecline_of_middle_small)
		m_mapIndex[strTradedate].middle_small.advance_decline_ratio = m_nAdvance_of_middle_small * 1.0 / m_nDecline_of_middle_small;

	if(m_nDecline_of_shanghai)
		m_mapIndex[strTradedate].shanghai.advance_decline_ratio = m_nAdvance_of_shanghai * 1.0 / m_nDecline_of_shanghai;

}

void   CAbsoluteBreadthIndex::EndCalculate()
{
	stringstream ss;
	try
	{
		ss.str("");
		ss << "insert into absolute_breadth_index (tradedate, stock_plate, absolute_breadth_index,"
			<< "advance_decline_line, advance_decline_ratio ) values ";

		map<string, CTechnicalAnalysisMarketIndexData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CTechnicalAnalysisMarketIndexData tai = it->second;
			ss << "(\'" << it->first << "\'" 
				<< ", \'所有A股\'"
				<< "," << tai.total.absolute_breadth_index 
				<< "," << tai.total.advance_decline_line 
				<< "," << Double2String(tai.total.advance_decline_ratio) 
				<< ") ,";

			ss << "(\'" << it->first << "\'" 
				<< ", \'创业板\'"
				<< "," << tai.entrepreneurship.absolute_breadth_index 
				<< "," << tai.entrepreneurship.advance_decline_line 
				<< "," << Double2String(tai.entrepreneurship.advance_decline_ratio) 
				<< ") ,";

			ss << "(\'" << it->first << "\'" 
				<< ", \'中小企业板\'"
				<< "," << tai.middle_small.absolute_breadth_index 
				<< "," << tai.middle_small.advance_decline_line 
				<< "," << Double2String(tai.middle_small.advance_decline_ratio) 
				<< ") ,";

			ss << "(\'" << it->first << "\'" 
				<< ", \'深圳A股\'"
				<< "," << tai.shenzhen_ashare.absolute_breadth_index 
				<< "," << tai.shenzhen_ashare.advance_decline_line 
				<< "," << Double2String(tai.shenzhen_ashare.advance_decline_ratio) 
				<< ") ,";

			ss << "(\'" << it->first << "\'" 
				<< ", \'上海证券板\'"
				<< "," << tai.shanghai.absolute_breadth_index 
				<< "," << tai.shanghai.advance_decline_line 
				<< "," << Double2String(tai.shanghai.advance_decline_ratio) 
				<< ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;
	}
	catch(...)
	{

	}
	
}
