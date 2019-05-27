#include "StdAfx.h"
#include "Stock.h"
#include "MAConvergenceDivergence.h"
#include "MACDOfPlate.h"

CMACDOfPlate::CMACDOfPlate(bool bAnalysisUsed)
:CMAConvergenceDivergence(bAnalysisUsed)
{
}

CMACDOfPlate::~CMACDOfPlate(void)
{
}


void CMACDOfPlate::BeginCalculate()
{
	stringstream ss;

	try{
		ss << "create table if not exists daily_macd_plate (code varchar(32), tradedate date, serial_num int, remark varchar(64), "
			<< " ema12 double, ema26 double, dif double, dea double,   "
			<< " price varchar(100), peak_date varchar(256), peak_dif varchar(256), "
			<< " peak_high varchar(256), peak_low varchar(256), peak_sign varchar(256), " 
			<< " cross_date varchar(256), cross_dif varchar(256), cross_sign varchar(256), "
			<< " PRIMARY KEY ( code, tradedate)) ";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		
		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select serial_num from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as serial_num,";

		ss << "( select ema12 from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as ema12,";
		ss << "( select ema26 from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as ema26,";
		ss << "( select dif   from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as dif,";
		ss << "( select dea   from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as dea, ";

		ss << "( select price from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as price,";
		ss << "( select peak_date  from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as peak_date,";
		ss << "( select peak_dif   from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as peak_dif,";
		ss << "( select peak_high  from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as peak_high,";
		ss << "( select peak_low   from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as peak_low,";
		ss << "( select peak_sign  from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as peak_sign, ";

		ss << "( select cross_date  from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as cross_date,";
		ss << "( select cross_dif   from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as cross_dif,";
		ss << "( select cross_sign  from daily_macd_plate b where b.code = a.code and b.tradedate = a.max_date ) as cross_sign ";

		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_macd_plate group by code ) a ";

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
			m_mapIndex[strCode].ema12 = r.get<double>("ema12");
			m_mapIndex[strCode].ema26 = r.get<double>("ema26");
			m_mapIndex[strCode].dif = r.get<double>("dif");
			m_mapIndex[strCode].dea = r.get<double>("dea");


			string peak = r.get<string>("price", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				
				m_mapIndex[strCode].close = String2Double(vecYield[0]);
				m_mapIndex[strCode].high = String2Double(vecYield[1]);
				m_mapIndex[strCode].low = String2Double(vecYield[2]);
			}

			//	dif 波峰与波谷
			peak = r.get<string>("peak_date", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].peak_date[i] = vecYield[i];
				}
			}

			peak = r.get<string>("peak_dif", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].peak_dif[i] = String2Double(vecYield[i]);
				}
			}

			peak = r.get<string>("peak_high", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].peak_high[i] = String2Double(vecYield[i]);
				}
			}

			peak = r.get<string>("peak_low", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].peak_low[i] = String2Double(vecYield[i]);
				}
			}

			peak = r.get<string>("peak_sign", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].peak_sign[i] = atoi(vecYield[i].c_str());
				}
			}

			//	聚散交叉
			peak = r.get<string>("cross_date", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].cross_date[i] = vecYield[i];
				}
			}

			peak = r.get<string>("cross_dif", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].cross_dif[i] = String2Double(vecYield[i]);
				}
			}

			peak = r.get<string>("cross_sign", "");
			if(peak.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peak, ",", vecYield);
				for(int i=0; i< vecYield.size(); i++)
				{
					m_mapIndex[strCode].cross_sign[i] = atoi(vecYield[i].c_str());
				}
			}
		}
	}
	catch(...)
	{

	}
}


void CMACDOfPlate::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		vector<string>  vecSqlStr;
		map<string, CMAConvergenceDivergenceData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CMAConvergenceDivergenceData rsi = it->second;
			ss.str("");
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << rsi.serial_num
				<< ", " << Double2String(rsi.ema12, "%.3f")
				<< ", " << Double2String(rsi.ema26, "%.3f")
				<< ", " << Double2String(rsi.dif, "%.3f")
				<< ", " << Double2String(rsi.dea, "%.3f")
				<< ", \'" << rsi.remark << "\'";

			ss << ",\'";
			ss << Double2String(rsi.close) << "," << Double2String(rsi.high) << "," << Double2String(rsi.low);
			ss << "\'";

			//	dif 波峰与波谷
			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << rsi.peak_date[i];
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << Double2String(rsi.peak_dif[i], "%.3f");
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << Double2String(rsi.peak_high[i]);
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << Double2String(rsi.peak_low[i]);
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << rsi.peak_sign[i];
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			//	聚散交叉
			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << rsi.cross_date[i];
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << Double2String(rsi.cross_dif[i], "%.3f");
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ",\'";
			for(int i=0; i< 10; i++)
			{
				ss << rsi.cross_sign[i];
				if(i<9)
					ss << "," ;
			}
			ss << "\'";

			ss << ")";

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
			ss << "insert into daily_macd_plate (code , tradedate, serial_num, ema12, ema26, dif, dea, remark, "
				<< " price, peak_date, peak_dif, peak_high, peak_low, peak_sign, "
				<< " cross_date, cross_dif, cross_sign "
				<< " ) values ";

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

		if(nLoop > 0)
			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_macd_plate", strTradedate);
	}
	catch(...)
	{

	}
}
