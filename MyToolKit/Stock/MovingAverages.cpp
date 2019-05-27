#include "StdAfx.h"
#include "stock.h"
#include "MovingAverages.h"

CMovingAverages::CMovingAverages(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CMovingAverages::~CMovingAverages(void)
{
}


void CMovingAverages::BeginCalculate()
{
	stringstream ss;


	try{
		ss << "create table if not exists daily_ma (code varchar(32), tradedate date, remark varchar(64),"
			<< " index_value1 double, index_value2 double, index_value3 double, index_value4 double,  "
			<< " period_5 int, period_10 int, period_20 int, period_60 int, "

			<< " close double,	  close_1 double , close_2 double, close_3 double, close_4 double, "
			<< " close_5 double,  close_6 double , close_7 double, close_8 double, close_9 double, "
			<< " close_10 double, close_11 double , close_12 double, close_13 double, close_14 double, "
			<< " close_15 double, close_16 double , close_17 double, close_18 double, close_19 double, "

			<< " close_20 double,	  close_21 double , close_22 double, close_23 double, close_24 double, "
			<< " close_25 double,  close_26 double , close_27 double, close_28 double, close_29 double, "
			<< " close_30 double, close_31 double , close_32 double, close_33 double, close_34 double, "
			<< " close_35 double, close_36 double , close_37 double, close_38 double, close_39 double, "

			<< " close_40 double,	  close_41 double , close_42 double, close_43 double, close_44 double, "
			<< " close_45 double,  close_46 double , close_47 double, close_48 double, close_49 double, "
			<< " close_50 double, close_51 double , close_52 double, close_53 double, close_54 double, "
			<< " close_55 double, close_56 double , close_57 double, close_58 double, close_59 double, "
			<< " PRIMARY KEY ( code, tradedate))";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		//	前次意外终止时，插入的部分数据要被删除
		string  endTradedate = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchTableUpdated("daily_ma");
		ss.str("");
		ss << "delete from daily_ma where tradedate >\'" << endTradedate << "\'";
		sql << ss.str();
		
		ss.str("");
		ss << "select code , max_date, ";

		ss << "( select close from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close,";
		ss << "( select close_1 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_1,";
		ss << "( select close_2 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_2,";
		ss << "( select close_3 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_3,";
		ss << "( select close_4 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_4,";
		ss << "( select close_5 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_5,";
		ss << "( select close_6 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_6,";
		ss << "( select close_7 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_7,";
		ss << "( select close_8 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_8,";
		ss << "( select close_9 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_9,";
		ss << "( select close_10 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_10,";
		ss << "( select close_11 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_11,";
		ss << "( select close_12 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_12,";
		ss << "( select close_13 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_13,";
		ss << "( select close_14 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_14,";
		ss << "( select close_15 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_15,";
		ss << "( select close_16 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_16,";
		ss << "( select close_17 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_17,";
		ss << "( select close_18 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_18,";
		ss << "( select close_19 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_19,";

		ss << "( select close_20 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_20,";
		ss << "( select close_21 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_21,";
		ss << "( select close_22 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_22,";
		ss << "( select close_23 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_23,";
		ss << "( select close_24 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_24,";
		ss << "( select close_25 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_25,";
		ss << "( select close_26 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_26,";
		ss << "( select close_27 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_27,";
		ss << "( select close_28 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_28,";
		ss << "( select close_29 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_29,";
		ss << "( select close_30 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_30,";
		ss << "( select close_31 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_31,";
		ss << "( select close_32 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_32,";
		ss << "( select close_33 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_33,";
		ss << "( select close_34 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_34,";
		ss << "( select close_35 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_35,";
		ss << "( select close_36 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_36,";
		ss << "( select close_37 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_37,";
		ss << "( select close_38 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_38,";
		ss << "( select close_39 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_39,";

		ss << "( select close_40 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_40,";
		ss << "( select close_41 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_41,";
		ss << "( select close_42 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_42,";
		ss << "( select close_43 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_43,";
		ss << "( select close_44 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_44,";
		ss << "( select close_45 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_45,";
		ss << "( select close_46 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_46,";
		ss << "( select close_47 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_47,";
		ss << "( select close_48 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_48,";
		ss << "( select close_49 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_49,";
		ss << "( select close_50 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_50,";
		ss << "( select close_51 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_51,";
		ss << "( select close_52 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_52,";
		ss << "( select close_53 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_53,";
		ss << "( select close_54 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_54,";
		ss << "( select close_55 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_55,";
		ss << "( select close_56 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_56,";
		ss << "( select close_57 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_57,";
		ss << "( select close_58 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_58,";
		ss << "( select close_59 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as close_59,";
		
		ss << "( select index_value1 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as index_value1,";
		ss << "( select index_value2 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as index_value2, ";
		ss << "( select index_value3 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as index_value3,";
		ss << "( select index_value4 from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as index_value4, ";

		ss << "( select period_5   from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as period_5,";
		ss << "( select period_10  from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as period_10,";
		ss << "( select period_20  from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as period_20,";
		ss << "( select period_60  from daily_ma b where b.code = a.code and b.tradedate = a.max_date ) as period_60 ";

		ss << "from ( select  code,  date_format(max(tradedate), '%Y-%m-%d') as max_date ";
		ss << "from daily_ma  group by code ) a ";

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
			m_mapIndex[strCode].index1 = r.get<double>("index_value1");
			m_mapIndex[strCode].index2 = r.get<double>("index_value2");

			m_mapIndex[strCode].index3 = r.get<double>("index_value3");
			m_mapIndex[strCode].index4 = r.get<double>("index_value4");

			m_mapIndex[strCode].period_5 = r.get<int>("period_5");
			m_mapIndex[strCode].period_10 = r.get<int>("period_10");
			m_mapIndex[strCode].period_20 = r.get<int>("period_20");
			m_mapIndex[strCode].period_60 = r.get<int>("period_60");

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

			m_mapIndex[strCode].close[20] = r.get<double>("close_20");
			m_mapIndex[strCode].close[21] = r.get<double>("close_21");
			m_mapIndex[strCode].close[22] = r.get<double>("close_22");
			m_mapIndex[strCode].close[23] = r.get<double>("close_23");
			m_mapIndex[strCode].close[24] = r.get<double>("close_24");
			m_mapIndex[strCode].close[25] = r.get<double>("close_25");
			m_mapIndex[strCode].close[26] = r.get<double>("close_26");
			m_mapIndex[strCode].close[27] = r.get<double>("close_27");
			m_mapIndex[strCode].close[28] = r.get<double>("close_28");
			m_mapIndex[strCode].close[29] = r.get<double>("close_29");

			m_mapIndex[strCode].close[30] = r.get<double>("close_30");
			m_mapIndex[strCode].close[31] = r.get<double>("close_31");
			m_mapIndex[strCode].close[32] = r.get<double>("close_32");
			m_mapIndex[strCode].close[33] = r.get<double>("close_33");
			m_mapIndex[strCode].close[34] = r.get<double>("close_34");
			m_mapIndex[strCode].close[35] = r.get<double>("close_35");
			m_mapIndex[strCode].close[36] = r.get<double>("close_36");
			m_mapIndex[strCode].close[37] = r.get<double>("close_37");
			m_mapIndex[strCode].close[38] = r.get<double>("close_38");
			m_mapIndex[strCode].close[39] = r.get<double>("close_39");

			m_mapIndex[strCode].close[40] = r.get<double>("close_40");
			m_mapIndex[strCode].close[41] = r.get<double>("close_41");
			m_mapIndex[strCode].close[42] = r.get<double>("close_42");
			m_mapIndex[strCode].close[43] = r.get<double>("close_43");
			m_mapIndex[strCode].close[44] = r.get<double>("close_44");
			m_mapIndex[strCode].close[45] = r.get<double>("close_45");
			m_mapIndex[strCode].close[46] = r.get<double>("close_46");
			m_mapIndex[strCode].close[47] = r.get<double>("close_47");
			m_mapIndex[strCode].close[48] = r.get<double>("close_48");
			m_mapIndex[strCode].close[49] = r.get<double>("close_49");

			m_mapIndex[strCode].close[50] = r.get<double>("close_50");
			m_mapIndex[strCode].close[51] = r.get<double>("close_51");
			m_mapIndex[strCode].close[52] = r.get<double>("close_52");
			m_mapIndex[strCode].close[53] = r.get<double>("close_53");
			m_mapIndex[strCode].close[54] = r.get<double>("close_54");
			m_mapIndex[strCode].close[55] = r.get<double>("close_55");
			m_mapIndex[strCode].close[56] = r.get<double>("close_56");
			m_mapIndex[strCode].close[57] = r.get<double>("close_57");
			m_mapIndex[strCode].close[58] = r.get<double>("close_58");
			m_mapIndex[strCode].close[59] = r.get<double>("close_59");

		}
	}
	catch(...)
	{

	}
}


void CMovingAverages::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

void   CMovingAverages::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterdy,
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
				for(int i= 59; i>0; i--)
				{
					m_mapIndex[strCode].close[i] = RecoverPrice(m_mapIndex[strCode].close[i], today);
				}
			}
		}
		return;
	}

	// 逐个计算 每股的情况

	CMovingAveragesData		yester;
	if(m_mapIndex.count(strCode)==0)
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].close[0] = today.close;
		m_mapIndex[strCode].index1 = today.close;
		m_mapIndex[strCode].index2 = today.close;
		m_mapIndex[strCode].index3 = today.close;
		m_mapIndex[strCode].index4 = today.close;

	}
	else
	{
		yester = m_mapIndex[strCode];

		m_mapIndex[strCode].tradedate = strTradedate;
		for(int i= 59; i>0; i--)
		{
			m_mapIndex[strCode].close[i] = RecoverPrice(m_mapIndex[strCode].close[i-1], today);
		}
		m_mapIndex[strCode].close[0] = today.close;
	}
	
	double ma5, ma10, ma20, ma60;

	if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_1 - 1] >0)
	{
		double total = 0;
		for(int i=0; i<MOVING_AVERAGES_SMOOTHING_DAY_1; i++)
		{
			total += m_mapIndex[strCode].close[i];
		}
		m_mapIndex[strCode].index1 = total/MOVING_AVERAGES_SMOOTHING_DAY_1 ;
		ma5 = m_mapIndex[strCode].index1;
	}
	else
		m_mapIndex[strCode].index1 = today.close;


	if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_2 - 1] >0)
	{
		double total = 0;
		for(int i=0; i<MOVING_AVERAGES_SMOOTHING_DAY_2; i++)
		{
			total += m_mapIndex[strCode].close[i];
		}
		m_mapIndex[strCode].index2 = total/MOVING_AVERAGES_SMOOTHING_DAY_2 ;
		ma10 = m_mapIndex[strCode].index2;
	}
	else
		m_mapIndex[strCode].index2 = m_mapIndex[strCode].index1;

	if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_3 - 1] >0)
	{
		double total = 0;
		for(int i=0; i<MOVING_AVERAGES_SMOOTHING_DAY_3; i++)
		{
			total += m_mapIndex[strCode].close[i];
		}
		m_mapIndex[strCode].index3 = total/MOVING_AVERAGES_SMOOTHING_DAY_3 ;
		ma20 = m_mapIndex[strCode].index3;
	}
	else
		m_mapIndex[strCode].index3 = m_mapIndex[strCode].index2;

	if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_4 - 1] >0)
	{
		double total = 0;
		for(int i=0; i<MOVING_AVERAGES_SMOOTHING_DAY_4; i++)
		{
			total += m_mapIndex[strCode].close[i];
		}
		m_mapIndex[strCode].index4 = total/MOVING_AVERAGES_SMOOTHING_DAY_4 ;
		ma60 = m_mapIndex[strCode].index4;
	}
	else
		m_mapIndex[strCode].index4 = m_mapIndex[strCode].index3;

	// 计算发生的事件

	m_mapIndex[strCode].remark = "";
	int short_period ,long_period , period_5, period_10;
	short_period = long_period = period_5 = period_10 = 0;
	if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_3] >0)
	{

		if(ma5 > yester.index1 && today.close > ma5 )
			period_5 = 1;
		else if(ma5 < yester.index1 && today.close < ma5 )
			period_5 = -1;
		else
			period_5 = 0;

		if(ma10 > yester.index2 && today.close > ma10 )
			period_10 = 1;
		else if(ma10 < yester.index2 && today.close < ma10 )
			period_10 = -1;
		else
			period_10 = 0;

		if(ma20 > yester.index3 && today.close > ma20 )
			short_period = 1;
		else if(ma20 < yester.index3 && today.close < ma20 )
			short_period = -1;
		else
			short_period = 0;

		if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_4 - 1] >0)
		{
			if(ma60 > yester.index4 && today.close > ma60 )
				long_period = 1;
			else if(ma60 < yester.index4 && today.close < ma60 )
				long_period = -1;
			else
				long_period = 0;
		}

		string remark = "";
		if(yester.period_20 != short_period || yester.period_60 != long_period)
		{
			if(short_period < 0)
				remark += "短期空头";
			else if(short_period==0)
				remark += "短期未知";
			else
				remark += "短期多头";

			remark += ",";
			if(long_period < 0)
				remark += "长期空头";
			else if(long_period==0)
				remark += "长期未知";
			else
				remark += "长期多头";
		}

		if(m_mapIndex[strCode].close[MOVING_AVERAGES_SMOOTHING_DAY_4 - 1] >0)
		{
			if(period_5 ==1 && period_10 ==1 && short_period ==1
				&& ma5 > ma10 && ma10 > ma20 )
			{

				bool bYesterday = false;
				double abs1 = abs(yester.index2 - yester.index1)/today.close;
				double abs2 = abs(yester.index3 - yester.index1)/today.close;
				if(abs1< 0.01 && abs2 < 0.01)
					bYesterday = true;

				if(bYesterday)
				{
					if(remark.empty()==false)
						remark += ",";

					remark += "向上3线开花";
				}
				
			}

			if(period_5 ==-1 && period_10 ==-1 && short_period ==-1
				&& ma5 < ma10 && ma10 < ma20 )
			{
				bool bYesterday = false;
				double abs1 = abs(yester.index2 - yester.index1)/today.close;
				double abs2 = abs(yester.index3 - yester.index1)/today.close;
				if(abs1< 0.01 && abs2 < 0.01)
					bYesterday = true;

				if(bYesterday)
				{
					if(remark.empty()==false)
						remark += ",";

					remark += "向下3线开花";
				}
				
			}

			if(today.close > today.open 
				&& ma5 > today.open && ma5 < today.close 
				&& ma10 > today.open && ma10 < today.close 
				&& ma20 > today.open && ma20 < today.close
				&& ma60 > today.open && ma60 < today.close)
			{
				if(remark.empty()==false)
					remark += ",";

				remark += "向上多线穿越";
			}


			if(today.close < today.open 
				&& ma5 > today.close && ma5 < today.open 
				&& ma10 > today.close && ma10 < today.open 
				&& ma20 > today.close && ma20 < today.open
				&& ma60 > today.close && ma60 < today.open)
			{
				if(remark.empty()==false)
					remark += ",";

				remark += "向下多线穿越";
			}
		}
		

		m_mapIndex[strCode].period_5 = period_5;
		m_mapIndex[strCode].period_10 = period_10;

		m_mapIndex[strCode].period_20 = short_period;
		m_mapIndex[strCode].period_60 = long_period;

		m_mapIndex[strCode].remark = remark;
	}
}

void CMovingAverages::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		vector<string>  vecSqlStr;
		map<string, CMovingAveragesData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CMovingAveragesData rsi = it->second;
			ss.str("");
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'";
			ss << ",\'" << rsi.remark << "\'";
			ss << ", " << rsi.period_5 ;
			ss << ", " << rsi.period_10 ;
			ss << ", " << rsi.period_20 ;
			ss << ", " << rsi.period_60;
			for(int i=0; i< 60; i++)
				ss << ", " << Double2String(rsi.close[i]);
			ss << ", "  << Double2String(rsi.index1, "%.3f")
				<< ", " << Double2String(rsi.index2, "%.3f")
				<< ", " << Double2String(rsi.index3, "%.3f")
				<< ", " << Double2String(rsi.index4, "%.3f")
				<< ")";

			vecSqlStr.insert(vecSqlStr.end(), ss.str());
			it++;
		}

		//	分组插入 ，每组 1000 个
		session sql(g_MysqlPool);

		int nLoop = vecSqlStr.size()/1000;
		if(nLoop * 1000 < vecSqlStr.size())
			nLoop++;

		for(int k=0; k< nLoop; k++)
		{
			ss.str("");
			ss << "insert into daily_ma (code , tradedate, remark, period_5, period_10, period_20, period_60, "
				<< " close, close_1, close_2, close_3, close_4, close_5, close_6, close_7, close_8, close_9, " 
				<< " close_10, close_11, close_12, close_13, close_14, close_15, close_16, close_17, close_18, close_19," 
				<< " close_20, close_21, close_22, close_23, close_24, close_25, close_26, close_27, close_28, close_29, " 
				<< " close_30, close_31, close_32, close_33, close_34, close_35, close_36, close_37, close_38, close_39, " 
				<< " close_40, close_41, close_42, close_43, close_44, close_45, close_46, close_47, close_48, close_49, " 
				<< " close_50, close_51, close_52, close_53, close_54, close_55, close_56, close_57, close_58, close_59, " 
				<< " index_value1, index_value2, index_value3, index_value4 ) values ";

			for(int i = k * 1000; i < (k+1) * 1000 && i <vecSqlStr.size(); i++)
			{
				ss << vecSqlStr[i];
				if( (i+1) != (k+1)*1000 && (i+1) != vecSqlStr.size() )
				{
					ss << ",";
				}
			}
			
			string test = ss.str();
			sql << ss.str();
		}
		
		if(nLoop >0)
		{
			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_ma", strTradedate);
		}
		
	}
	catch(std::exception const & e)
	{
		string test = e.what();
		test += "";
	}
	catch(...)
	{
		
	}
}

void CMovingAverages::EndCalculate()
{
	
}
