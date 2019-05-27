#include "StdAfx.h"
#include "Stock.h"
#include "StochasticsOscillatorKDJ.h"

CStochasticsOscillatorKDJ::CStochasticsOscillatorKDJ(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CStochasticsOscillatorKDJ::~CStochasticsOscillatorKDJ(void)
{
}


void CStochasticsOscillatorKDJ::BeginCalculate()
{
	stringstream ss;

	try{
		ss << "create table if not exists daily_kdj (code varchar(32), tradedate date, serial_num int, remark varchar(64), "
			<< " k_value double, d_value double, j_value double,  "
			<< " close double, high double, low double, "
			<< " high_1 double , low_1 double, "
			<< " high_2 double , low_2 double, "
			<< " high_3 double , low_3 double, "
			<< " high_4 double , low_4 double, "
			<< " high_5 double , low_5 double, "
			<< " high_6 double , low_6 double, "
			<< " high_7 double , low_7 double, "
			<< " high_8 double , low_8 double, "
			<< " PRIMARY KEY ( code, tradedate)) ";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select serial_num from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as serial_num,";

		ss << "( select close from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as close,";
		ss << "( select high from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high,";
		ss << "( select low  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low,";

		ss << "( select high_1 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_1,";
		ss << "( select low_1  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_1,";

		ss << "( select high_2 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_2,";
		ss << "( select low_2  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_2,";

		ss << "( select high_3 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_3,";
		ss << "( select low_3  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_3,";

		ss << "( select high_4 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_4,";
		ss << "( select low_4  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_4,";

		ss << "( select high_5 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_5,";
		ss << "( select low_5  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_5,";

		ss << "( select high_6 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_6,";
		ss << "( select low_6  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_6,";

		ss << "( select high_7 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_7,";
		ss << "( select low_7  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_7,";

		ss << "( select high_8 from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as high_8,";
		ss << "( select low_8  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as low_8,";

		ss << "( select k_value from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as k_value,";
		ss << "( select d_value from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as d_value,";
		ss << "( select j_value  from daily_kdj b where b.code = a.code and b.tradedate = a.max_date ) as j_value ";

		
		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_kdj group by code ) a ";

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
			m_mapIndex[strCode].tradedate = tmp;
			m_mapIndex[strCode].serial_num = r.get<int>("serial_num");

			m_mapIndex[strCode].close = r.get<double>("close", 0);
			m_mapIndex[strCode].high[0] = r.get<double>("high", 0);
			m_mapIndex[strCode].low[0] = r.get<double>("low", 0);

			m_mapIndex[strCode].high[1] = r.get<double>("high_1", 0);
			m_mapIndex[strCode].low[1] = r.get<double>("low_1", 0);

			m_mapIndex[strCode].high[2] = r.get<double>("high_2", 0);
			m_mapIndex[strCode].low[2] = r.get<double>("low_2", 0);

			m_mapIndex[strCode].high[3] = r.get<double>("high_3", 0);
			m_mapIndex[strCode].low[3] = r.get<double>("low_3", 0);

			m_mapIndex[strCode].high[4] = r.get<double>("high_4", 0);
			m_mapIndex[strCode].low[4] = r.get<double>("low_4", 0);

			m_mapIndex[strCode].high[5] = r.get<double>("high_5", 0);
			m_mapIndex[strCode].low[5] = r.get<double>("low_5", 0);

			m_mapIndex[strCode].high[6] = r.get<double>("high_6", 0);
			m_mapIndex[strCode].low[6] = r.get<double>("low_6", 0);

			m_mapIndex[strCode].high[7] = r.get<double>("high_7", 0);
			m_mapIndex[strCode].low[7] = r.get<double>("low_7", 0);

			m_mapIndex[strCode].high[8] = r.get<double>("high_8", 0);
			m_mapIndex[strCode].low[8] = r.get<double>("low_8", 0);

			m_mapIndex[strCode].k_value = r.get<double>("k_value", 0);
			m_mapIndex[strCode].d_value = r.get<double>("d_value", 0);
			m_mapIndex[strCode].j_value = r.get<double>("j_value", 0);
		}
	}
	catch(...)
	{

	}
}


void CStochasticsOscillatorKDJ::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}


/*****************  计算公式
n日RSV=（Cn－Ln）/（Hn－Ln）×100

公式中，Cn为第n日收盘价；Ln为n日内的最低价；Hn为n日内的最高价。

其次，计算K值与D值：

当日K值=2/3×前一日K值+1/3×当日RSV

当日D值=2/3×前一日D值+1/3×当日K值

若无前一日K 值与D值，则可分别用50来代替。

J值=3*当日K值-2*当日D值

*/

void   CStochasticsOscillatorKDJ::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
												string strTradedate, string strCode, string strPlate)
{
	if(today.close<0)
	{
		// 本日停牌
		if(m_mapIndex.count(strCode)>0)
		{
			m_mapIndex[strCode].remark ="";

			// 本日停牌, 但是 转增、分红、红股发生,指标进行修正
			if(today.bonus_cash >0 || today.bonus_share >0 || today.reserve_to_common_share >0)
			{
				CStochasticsOscillatorKDJData macd = m_mapIndex[strCode];

				//	先对之前价格按照现有 价位 进行前复权处理
				for(int i= KDJ_DAY - 1; i>=0; i--)
				{
					macd.high[i] = RecoverPrice(macd.high[i], today);
					macd.low[i] = RecoverPrice(macd.low[i], today);
				}
				 m_mapIndex[strCode] = macd;
			}
		}

		return;
	}

	// 逐个计算 每股的情况
	if(m_mapIndex.count(strCode)==0)
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].serial_num = 1;
		m_mapIndex[strCode].close = today.close;
		m_mapIndex[strCode].high[0] = today.high;
		m_mapIndex[strCode].low[0] = today.low;
	}
	else
	{
		CStochasticsOscillatorKDJData macd = m_mapIndex[strCode];

		CStochasticsOscillatorKDJData macd_new = macd;
		macd_new.serial_num ++;
		macd_new.tradedate = strTradedate;
		macd_new.remark = "";

		macd_new.close = today.close;

		//	先对之前价格按照现有 价位 进行前复权处理
		for(int i= KDJ_DAY - 1; i>0; i--)
		{
			macd_new.high[i] = RecoverPrice(macd_new.high[i-1], today);
			macd_new.low[i] = RecoverPrice(macd_new.low[i-1], today);
		}
		macd_new.high[0] = today.high;
		macd_new.low[0] = today.low;

		//	计算 N 日内的最高价与 最低价
		double dbHigh = macd_new.high[0];
		double dbLow = macd_new.low[0];
		for(int i = 1 ; i< KDJ_DAY; i++)
		{
			if(macd_new.high[i] > dbHigh)
				dbHigh = macd_new.high[i];
			if(macd_new.low[i] < dbLow)
				dbLow = macd_new.low[i];
		}

		if(macd_new.serial_num> KDJ_DAY)
		{
			double rsv = (today.close - dbLow) / (dbHigh - dbLow) * 100.0;
			macd_new.k_value = macd.k_value * 2/3 + rsv /3; 
			macd_new.d_value = macd.d_value * 2/3 + macd_new.k_value /3; 
			macd_new.j_value = macd_new.k_value * 3 - macd_new.d_value * 2;

			if(macd_new.d_value > 80)
			{
				//	超买区
				if(macd_new.k_value < macd_new.d_value && macd.k_value > macd.d_value)
				{
					macd_new.remark = _T("超买区死叉卖出");
				}
				else if(macd.d_value < 80)
				{
					macd_new.remark = _T("D 线进入超买区");
				}
			}
			else if(macd_new.d_value < 20)
			{
				//	超卖区
				if(macd_new.k_value > macd_new.d_value && macd.k_value < macd.d_value)
				{
					macd_new.remark = _T("超卖区金叉买入");
				}
				else if(macd.d_value > 20)
				{
					macd_new.remark = _T("D 线进入超卖区");
				}
			}
		}

		m_mapIndex[strCode] = macd_new;

	}

}

void CStochasticsOscillatorKDJ::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		ss << "insert into daily_kdj (code , tradedate, serial_num,  "
			<< " close, high, low, "
			<< " high_1, low_1, high_2, low_2, high_3, low_3, high_4, low_4, high_5, low_5, "
			<< " high_6, low_6, high_7, low_7, high_8, low_8, "
			<< " k_value, d_value, j_value, remark "
			<< " ) values ";

		map<string, CStochasticsOscillatorKDJData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CStochasticsOscillatorKDJData rsi = it->second;
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << rsi.serial_num
				<< ", " << Double2String(rsi.close, "%.3f")
				<< ", " << Double2String(rsi.high[0], "%.3f")
				<< ", " << Double2String(rsi.low[0], "%.3f")
				<< ", " << Double2String(rsi.high[1], "%.3f")
				<< ", " << Double2String(rsi.low[1], "%.3f")
				<< ", " << Double2String(rsi.high[2], "%.3f")
				<< ", " << Double2String(rsi.low[2], "%.3f")
				<< ", " << Double2String(rsi.high[3], "%.3f")
				<< ", " << Double2String(rsi.low[3], "%.3f")
				<< ", " << Double2String(rsi.high[4], "%.3f")
				<< ", " << Double2String(rsi.low[4], "%.3f")
				<< ", " << Double2String(rsi.high[5], "%.3f")
				<< ", " << Double2String(rsi.low[5], "%.3f")
				<< ", " << Double2String(rsi.high[6], "%.3f")
				<< ", " << Double2String(rsi.low[6], "%.3f")
				<< ", " << Double2String(rsi.high[7], "%.3f")
				<< ", " << Double2String(rsi.low[7], "%.3f")
				<< ", " << Double2String(rsi.high[8], "%.3f")
				<< ", " << Double2String(rsi.low[8], "%.3f")

				<< ", " << Double2String(rsi.k_value, "%.3f")
				<< ", " << Double2String(rsi.d_value, "%.3f")
				<< ", " << Double2String(rsi.j_value, "%.3f")
				<< ", \'" << rsi.remark << "\'";
			ss << ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;

		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_kdj", strTradedate);
	}
	catch(...)
	{

	}
}

void CStochasticsOscillatorKDJ::EndCalculate()
{
	
}



void CStochasticsOscillatorKDJ::ExportFromDatabase(string strCode, double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CStochasticsOscillatorKDJ 开始从 MYSQL 获取数据......";
		//ss << "【" << m_nCode << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			<< " k_value, d_value, j_value "
			<< " from daily_kdj where code=\'" << strCode << "\' ";
		if(endTime > startTime)
		{
			COleDateTime start, end;
			start.m_dt = startTime;
			end.m_dt = endTime;
			ss << " and tradedate >= \'" << start.Format("%Y-%m-%d") << "\'";
			ss << " and tradedate <= \'" << end.Format("%Y-%m-%d") << "\'";
		}
		ss	<< " order by tradedate";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			CStochasticsOscillatorKDJData  bbd;
			bbd.tradedate = r.get<string>("tradedate");
			bbd.k_value = r.get<double>("k_value");
			bbd.d_value = r.get<double>("d_value");
			bbd.j_value = r.get<double>("j_value");

			m_vec_index.insert(m_vec_index.end(), bbd);
		}

		ss.str("");
		ss << "OK, CStochasticsOscillatorKDJ 从 MYSQL 获取数据完毕.";
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
}
