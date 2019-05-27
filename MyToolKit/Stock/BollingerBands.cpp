#include "StdAfx.h"
#include "Stock.h"
#include "BollingerBands.h"

CBollingerBands::CBollingerBands(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CBollingerBands::~CBollingerBands()
{
}


void CBollingerBands::BeginCalculate()
{
	stringstream ss;


	try{
		ss << "create table if not exists daily_bollinger (code varchar(32), tradedate date, "
			<< " standard double,  middle double, up double, down double, remark varchar(64), "
			<< " close double,	  close_1 double , close_2 double, close_3 double, close_4 double, "
			<< " close_5 double,  close_6 double , close_7 double, close_8 double, close_9 double, "
			<< " close_10 double, close_11 double , close_12 double, close_13 double, close_14 double, "
			<< " close_15 double, close_16 double , close_17 double, close_18 double, close_19 double, "
			<< " PRIMARY KEY ( code, tradedate))";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select close from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close,";
		ss << "( select close_1 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_1,";
		ss << "( select close_2 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_2,";
		ss << "( select close_3 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_3,";
		ss << "( select close_4 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_4,";
		ss << "( select close_5 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_5,";
		ss << "( select close_6 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_6,";
		ss << "( select close_7 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_7,";
		ss << "( select close_8 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_8,";
		ss << "( select close_9 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_9,";
		ss << "( select close_10 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_10,";
		ss << "( select close_11 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_11,";
		ss << "( select close_12 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_12,";
		ss << "( select close_13 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_13,";
		ss << "( select close_14 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_14,";
		ss << "( select close_15 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_15,";
		ss << "( select close_16 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_16,";
		ss << "( select close_17 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_17,";
		ss << "( select close_18 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_18,";
		ss << "( select close_19 from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as close_19,";

		ss << "( select middle from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as middle, ";
		ss << "( select up from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as up, ";
		ss << "( select down from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as down, ";
		ss << "( select standard from daily_bollinger b where b.code = a.code and b.tradedate = a.max_date ) as standard ";

		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_bollinger  group by code ) a ";

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
			m_mapIndex[strCode].close[0] = r.get<double>("close");
			m_mapIndex[strCode].close[1] = r.get<double>("close_1");
			m_mapIndex[strCode].close[2] = r.get<double>("close_2");
			m_mapIndex[strCode].close[3] = r.get<double>("close_3");
			m_mapIndex[strCode].close[4] = r.get<double>("close_4");
			m_mapIndex[strCode].close[5] = r.get<double>("close_5");
			m_mapIndex[strCode].close[6] = r.get<double>("close_6");
			m_mapIndex[strCode].close[7] = r.get<double>("close_7");
			m_mapIndex[strCode].close[8] = r.get<double>("close_8");
			m_mapIndex[strCode].close[9] = r.get<double>("close_9");

			m_mapIndex[strCode].close[10] = r.get<double>("close_10");
			m_mapIndex[strCode].close[11] = r.get<double>("close_11");
			m_mapIndex[strCode].close[12] = r.get<double>("close_12");
			m_mapIndex[strCode].close[13] = r.get<double>("close_13");
			m_mapIndex[strCode].close[14] = r.get<double>("close_14");
			m_mapIndex[strCode].close[15] = r.get<double>("close_15");
			m_mapIndex[strCode].close[16] = r.get<double>("close_16");
			m_mapIndex[strCode].close[17] = r.get<double>("close_17");
			m_mapIndex[strCode].close[18] = r.get<double>("close_18");
			m_mapIndex[strCode].close[19] = r.get<double>("close_19");

			m_mapIndex[strCode].middle = r.get<double>("middle");
			m_mapIndex[strCode].up = r.get<double>("up");
			m_mapIndex[strCode].down = r.get<double>("down");
			m_mapIndex[strCode].standard = r.get<double>("standard");

		}
	}
	catch(...)
	{

	}
}


void CBollingerBands::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

void   CBollingerBands::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
												string strTradedate, string strCode, string strPlate)
{

	if(today.close<0 )
	{
		// 本日停牌
		if(m_mapIndex.count(strCode)>0)
		{
			m_mapIndex[strCode].remark ="";

			// 本日停牌, 但是 转增、分红、红股发生,指标进行修正
			if(today.bonus_cash >0 || today.bonus_share >0 || today.reserve_to_common_share >0)
			{
				for(int i= 19; i>0; i--)
				{
					m_mapIndex[strCode].close[i] = RecoverPrice(m_mapIndex[strCode].close[i], today);
				}
			}
		}
		return;
	}

	// 逐个计算 每股的情况
	if(m_mapIndex.count(strCode)==0)
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].close[0] = today.close;
	}
	else
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		for(int i= 19; i>0; i--)
		{
			m_mapIndex[strCode].close[i] = RecoverPrice(m_mapIndex[strCode].close[i-1], today);
		}
		
		m_mapIndex[strCode].close[0] = today.close;
	}

	if(m_mapIndex[strCode].close[19] >0)
	{
		//	存在 20 个交易日数据，计算指标
		CBollingerBandsData bbd = m_mapIndex[strCode];
		double middle = 0;
		for(int i=0; i<20; i++)
			middle += bbd.close[i];
		middle = middle /20;

		double standard = 0;
		for(int i=0; i<20; i++)
		{
			standard += (bbd.close[i] - middle) * (bbd.close[i] - middle );
		}
		standard = sqrtf(standard/20);

		m_mapIndex[strCode].up = middle + 2 * standard;
		m_mapIndex[strCode].down = middle - 2 * standard;
		m_mapIndex[strCode].middle = middle;
		m_mapIndex[strCode].standard = standard;
		m_mapIndex[strCode].remark = "";

		// 计算极限宽度指标
		double width_yesterday = 4 * bbd.standard / bbd.middle;
		double width = 4 * standard / middle;
		if(width < 0.09)
		{
			if(width_yesterday > 0.09)
				m_mapIndex[strCode].remark = _T("====> 进入买入卖出时机");
		}
		else 
		{
			if(width_yesterday < 0.09)
				m_mapIndex[strCode].remark = _T("====< 退出买入卖出时机");
		}

		//	转增 、红股 、分红等 也在 boll 指标中发出 事件，以便 模拟账户进行 交易核算
		if(today.bonus_cash >0 || today.bonus_share >0 || today.reserve_to_common_share >0)
		{
			m_mapIndex[strCode].bonus_cash = today.bonus_cash;
			m_mapIndex[strCode].bonus_share = today.bonus_share;
			m_mapIndex[strCode].reserve_to_common_share = today.reserve_to_common_share;
			// m_mapIndex[strCode].extra_desc = "转增 、红股 、分红";
		}

		
	}
	else
	{

	}
	
}

void CBollingerBands::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		ss << "insert into daily_bollinger (code , tradedate, standard, middle, up, down, remark, "
			<< " close, close_1, close_2, close_3, close_4, close_5, close_6, close_7, close_8, close_9, " 
			<< " close_10, close_11, close_12, close_13, close_14, close_15, close_16, close_17, close_18, close_19 " 
			<< " ) values ";

		map<string, CBollingerBandsData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CBollingerBandsData rsi = it->second;
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << Double2String(rsi.standard)
				<< ", " << Double2String(rsi.middle)
				<< ", " << Double2String(rsi.up)
				<< ", " << Double2String(rsi.down)
				<< ", \'" << rsi.remark << "\'";
			for(int i=0; i< 20; i++)
				ss << ", " << Double2String(rsi.close[i]);
			ss << ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;

		
		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_bollinger", strTradedate);
	}
	catch(...)
	{

	}
}

void CBollingerBands::EndCalculate()
{
	
}



void CBollingerBands::ExportFromDatabase(string strCode, double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CBollingerBands 开始从 MYSQL 获取数据......";
		//ss << "【" << m_nCode << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			<< " standard, middle, up, down "
			<< " from daily_bollinger where code=\'" << strCode << "\' ";
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
			CBollingerBandsData  bbd;
			bbd.tradedate = r.get<string>("tradedate");
			bbd.down = r.get<double>("down");
			bbd.up = r.get<double>("up");
			bbd.middle = r.get<double>("middle");
			bbd.standard = r.get<double>("standard");

			m_vec_index.insert(m_vec_index.end(), bbd);
		}

		ss.str("");
		ss << "OK, CBollingerBands 从 MYSQL 获取数据完毕.";
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
}
