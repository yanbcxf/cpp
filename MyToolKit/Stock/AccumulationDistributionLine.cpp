#include "StdAfx.h"
#include "Stock.h"
#include "AccumulationDistributionLine.h"

CAccumulationDistributionLine::CAccumulationDistributionLine(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CAccumulationDistributionLine::~CAccumulationDistributionLine(void)
{
}


void CAccumulationDistributionLine::BeginCalculate()
{
	stringstream ss;


	try{
		ss << "create table if not exists accumulation_distribution_line (code varchar(32), tradedate date,  "
			<< " accumulation_distribution_line double, "
			<< " PRIMARY KEY ( code, tradedate))";

		session sql(g_MysqlPool);
		sql << ss.str();

		ss.str("");
		ss << "select  code,  date_format(max(tradedate), '%Y-%m-%d') as max_date, ";
		ss << " accumulation_distribution_line ";
		ss << "from accumulation_distribution_line  group by code ";

		row r;
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();
		while(st.fetch())
		{
			string tmp = r.get<string>("max_date");
			if(tmp!="0000-00-00" && tmp > m_strLatest)
			{
				m_strLatest = tmp;

				ss.str("");
				ss << IndexName() << ", m_strLatest = " << m_strLatest;
				LOG4CPLUS_DEBUG(g_logger, ss.str() );
			} 
			string strCode = r.get<string>("code");
			m_mapIndex[strCode] = r.get<double>("accumulation_distribution_line");
		}
	}
	catch(...)
	{

	}
}


void CAccumulationDistributionLine::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

void   CAccumulationDistributionLine::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
												string strTradedate, string strCode, string strPlate)
{
	if(today.close<0)
	{
		// 本日停牌
		return;
	}

	// 逐个计算 每股的涨跌情况
	if(today.high != today.low)
	{
		double db;
		db = today.high - today.close;
		db = today.close - today.low - db;
		db = db / (today.high - today.low);
		db = db * today.turnover;

		if(m_mapIndex.count(strCode) == 0)
			m_mapIndex[strCode] = 0;
		m_mapIndex[strCode] += db;
	}
	
}

void CAccumulationDistributionLine::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		ss << "insert into accumulation_distribution_line (code, tradedate, accumulation_distribution_line) values ";

		map<string, double>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\', " << Double2String(it->second) << ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;

		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("accumulation_distribution_line", strTradedate);
	}
	catch(...)
	{

	}
}

void CAccumulationDistributionLine::EndCalculate()
{
	
}
