#include "StdAfx.h"
#include "Stock.h"
#include "RelativeStrengthIndex.h"

CRelativeStrengthIndex::CRelativeStrengthIndex(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CRelativeStrengthIndex::~CRelativeStrengthIndex(void)
{
}


void CRelativeStrengthIndex::BeginCalculate()
{
	stringstream ss;


	try{
		ss << "create table if not exists daily_rsi (code varchar(32), tradedate date, serial_num int, "
			<< " index_value1 double, index_value2 double, index_value3 double, "
			<< " close double, advance_average1 double , decline_average1 double,   "
			<< " advance_average2 double , decline_average2 double,   "
			<< " advance_average3 double , decline_average3 double,   "
			<< " PRIMARY KEY ( code, tradedate))";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select serial_num from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as serial_num,";
		ss << "( select close from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as close,";
		ss << "( select advance_average1 from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as advance_average1,";
		ss << "( select decline_average1 from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as decline_average1, ";

		ss << "( select advance_average2 from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as advance_average2,";
		ss << "( select decline_average2 from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as decline_average2,";

		ss << "( select advance_average3 from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as advance_average3,";
		ss << "( select decline_average3 from daily_rsi b where b.code = a.code and b.tradedate = a.max_date ) as decline_average3 ";

		ss << "from ( select  code,  date_format(max(tradedate), '%Y-%m-%d') as max_date ";
		ss << "from daily_rsi  group by code ) a ";

		test = ss.str();

		row r;
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();
		while(st.fetch())
		{
			//	取所有股票中最大的交易日，因为可能有股票停牌
			string tmp = r.get<string>("max_date");
			if(tmp!="0000-00-00" && tmp > m_strLatest)
			{
				m_strLatest = tmp;

				ss.str("");
				ss << IndexName() << ", m_strLatest = " << m_strLatest;
				LOG4CPLUS_DEBUG(g_logger, ss.str() );
			}  

			string strCode = r.get<string>("code");
			m_mapIndex[strCode].serial_num = r.get<int>("serial_num");
			m_mapIndex[strCode].close = r.get<double>("close");
			m_mapIndex[strCode].advance_average1 = r.get<double>("advance_average1");
			m_mapIndex[strCode].decline_average1 = r.get<double>("decline_average1");

			m_mapIndex[strCode].advance_average2 = r.get<double>("advance_average2");
			m_mapIndex[strCode].decline_average2 = r.get<double>("decline_average2");

			m_mapIndex[strCode].advance_average3 = r.get<double>("advance_average3");
			m_mapIndex[strCode].decline_average3 = r.get<double>("decline_average3");
		}
	}
	catch(...)
	{

	}
}


void CRelativeStrengthIndex::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

void   CRelativeStrengthIndex::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterdy,
												string strTradedate, string strCode, string strPlate)
{

	if(today.close<0 )
	{
		// 本日停牌
		if(m_mapIndex.count(strCode)>0)
		{
			m_mapIndex[strCode].basic_desc ="";
			m_mapIndex[strCode].extra_desc ="";

			// 本日停牌, 但是 转增、分红、红股发生,指标进行修正
			if(today.bonus_cash >0 || today.bonus_share >0 || today.reserve_to_common_share >0)
			{
				CRelativeStrengthIndexData yesterday  = m_mapIndex[strCode];

				//	先对之前价格按照现有 价位 进行前复权处理
				yesterday.close = RecoverPrice(yesterday.close, today);
				yesterday.advance_average1 = RecoverPrice(yesterday.advance_average1, today, false);
				yesterday.decline_average1 = RecoverPrice(yesterday.decline_average1, today, false);

				yesterday.advance_average2 = RecoverPrice(yesterday.advance_average2, today, false);
				yesterday.decline_average2 = RecoverPrice(yesterday.decline_average2, today, false);

				yesterday.advance_average3 = RecoverPrice(yesterday.advance_average3, today, false);
				yesterday.decline_average3 = RecoverPrice(yesterday.decline_average3, today, false);

				m_mapIndex[strCode] = yesterday;
			}
		}
		return;
	}

	// 逐个计算 每股的情况
	if(m_mapIndex.count(strCode)==0)
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].close = today.close;
		m_mapIndex[strCode].serial_num = 0;
		m_mapIndex[strCode].advance_average1 = 0;
		m_mapIndex[strCode].decline_average1 = 0;
		m_mapIndex[strCode].advance_average2 = 0;
		m_mapIndex[strCode].decline_average2 = 0;
		m_mapIndex[strCode].advance_average3 = 0;
		m_mapIndex[strCode].decline_average3 = 0;
		m_mapIndex[strCode].index1 = 0;
		m_mapIndex[strCode].index2 = 0;
		m_mapIndex[strCode].index3 = 0;

	}
	else
	{
		CRelativeStrengthIndexData yesterday  = m_mapIndex[strCode];

		//	先对之前价格按照现有 价位 进行前复权处理
		yesterday.close = RecoverPrice(yesterday.close, today);
		yesterday.advance_average1 = RecoverPrice(yesterday.advance_average1, today, false);
		yesterday.decline_average1 = RecoverPrice(yesterday.decline_average1, today, false);

		yesterday.advance_average2 = RecoverPrice(yesterday.advance_average2, today, false);
		yesterday.decline_average2 = RecoverPrice(yesterday.decline_average2, today, false);

		yesterday.advance_average3 = RecoverPrice(yesterday.advance_average3, today, false);
		yesterday.decline_average3 = RecoverPrice(yesterday.decline_average3, today, false);

		m_mapIndex[strCode].serial_num++;
		m_mapIndex[strCode].close = today.close;

		double advance_change = 0;
		double decline_change = 0;
		if(today.close > yesterday.close)
			advance_change = today.close - yesterday.close;
		else
			decline_change = yesterday.close - today.close;

		if(m_mapIndex[strCode].serial_num == 1)
		{
			m_mapIndex[strCode].advance_average1 = advance_change;
			m_mapIndex[strCode].decline_average1 = decline_change;

			m_mapIndex[strCode].advance_average2 = advance_change;
			m_mapIndex[strCode].decline_average2 = decline_change;

			m_mapIndex[strCode].advance_average3 = advance_change;
			m_mapIndex[strCode].decline_average3 = decline_change;

			m_mapIndex[strCode].index1 = 0;
			m_mapIndex[strCode].index2 = 0;
			m_mapIndex[strCode].index3 = 0;
		}
		else if(m_mapIndex[strCode].serial_num < RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 )
		{
			m_mapIndex[strCode].advance_average3 = 
				(yesterday.advance_average3 * yesterday.serial_num + advance_change)/m_mapIndex[strCode].serial_num;

			m_mapIndex[strCode].decline_average3 = 
				(yesterday.decline_average3 * yesterday.serial_num + decline_change)/m_mapIndex[strCode].serial_num;

			m_mapIndex[strCode].advance_average1 = m_mapIndex[strCode].advance_average2 = m_mapIndex[strCode].advance_average3;
			m_mapIndex[strCode].decline_average1 = m_mapIndex[strCode].decline_average2 = m_mapIndex[strCode].decline_average3;

			m_mapIndex[strCode].index1 = 0;
			m_mapIndex[strCode].index2 = 0;
			m_mapIndex[strCode].index3 = 0;
		}
		else 
		{
			//	从第  RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY  天开始计算指数
			if(m_mapIndex[strCode].serial_num == RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 )
			{
				m_mapIndex[strCode].advance_average3 = 
					(yesterday.advance_average3 * yesterday.serial_num + advance_change)/m_mapIndex[strCode].serial_num;

				m_mapIndex[strCode].decline_average3 = 
					(yesterday.decline_average3 * yesterday.serial_num + decline_change)/m_mapIndex[strCode].serial_num;

				m_mapIndex[strCode].advance_average1 = m_mapIndex[strCode].advance_average2 = m_mapIndex[strCode].advance_average3;
				m_mapIndex[strCode].decline_average1 = m_mapIndex[strCode].decline_average2 = m_mapIndex[strCode].decline_average3;
			}
			else
			{
				//	采用简单 算术平均算法
				m_mapIndex[strCode].advance_average1 
					= yesterday.advance_average1 * (RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1 - 1)/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1 
					+ advance_change *  1/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1;

				m_mapIndex[strCode].decline_average1 
					= yesterday.decline_average1 * (RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1 - 1)/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1 
					+ decline_change *  1/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1;


				m_mapIndex[strCode].advance_average2 
					= yesterday.advance_average2 * (RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2 - 1)/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2 
					+ advance_change *  1/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2;

				m_mapIndex[strCode].decline_average2 
					= yesterday.decline_average2 * (RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2 - 1)/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2 
					+ decline_change *  1/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2;


				m_mapIndex[strCode].advance_average3 
					= yesterday.advance_average3 * (RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 - 1)/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 
					+ advance_change *  1/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3;

				m_mapIndex[strCode].decline_average3 
					= yesterday.decline_average3 * (RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 - 1)/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 
					+ decline_change *  1/RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3;

			}

			if(m_mapIndex[strCode].decline_average1 == 0)
				m_mapIndex[strCode].index1 = 100;
			else
				m_mapIndex[strCode].index1 = 100 - 100/(1 + m_mapIndex[strCode].advance_average1 / m_mapIndex[strCode].decline_average1);

			if(m_mapIndex[strCode].decline_average2 == 0)
				m_mapIndex[strCode].index2 = 100;
			else
				m_mapIndex[strCode].index2 = 100 - 100/(1 + m_mapIndex[strCode].advance_average2 / m_mapIndex[strCode].decline_average2);

			if(m_mapIndex[strCode].decline_average3 == 0)
				m_mapIndex[strCode].index3 = 100;
			else
				m_mapIndex[strCode].index3 = 100 - 100/(1 + m_mapIndex[strCode].advance_average3 / m_mapIndex[strCode].decline_average3);
		}

	}
	
}

void CRelativeStrengthIndex::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		ss << "insert into daily_rsi (code , tradedate, serial_num, close, "
			<< "advance_average1, decline_average1, index_value1,"
			<< "advance_average2, decline_average2, index_value2,"
			<< "advance_average3, decline_average3, index_value3 ) values ";

		map<string, CRelativeStrengthIndexData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CRelativeStrengthIndexData rsi = it->second;
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << rsi.serial_num
				<< ", " << Double2String(rsi.close, "%.3f")
				<< ", " << Double2String(rsi.advance_average1, "%.3f")
				<< ", " << Double2String(rsi.decline_average1, "%.3f")
				<< ", " << Double2String(rsi.index1, "%.3f")
				<< ", " << Double2String(rsi.advance_average2, "%.3f")
				<< ", " << Double2String(rsi.decline_average2, "%.3f")
				<< ", " << Double2String(rsi.index2, "%.3f")
				<< ", " << Double2String(rsi.advance_average3, "%.3f")
				<< ", " << Double2String(rsi.decline_average3, "%.3f")
				<< ", " << Double2String(rsi.index3, "%.3f")
				<< ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;

		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_rsi", strTradedate);
	}
	catch(...)
	{

	}
}

void CRelativeStrengthIndex::EndCalculate()
{
	
}
