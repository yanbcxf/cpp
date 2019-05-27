#include "StdAfx.h"
#include "Stock.h"
#include "MAConvergenceDivergence.h"

CMAConvergenceDivergence::CMAConvergenceDivergence(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CMAConvergenceDivergence::~CMAConvergenceDivergence(void)
{
}


void CMAConvergenceDivergence::BeginCalculate()
{
	stringstream ss;

	try{
		ss << "create table if not exists daily_macd (code varchar(32), tradedate date, serial_num int, remark varchar(64), "
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
		ss << "( select serial_num from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as serial_num,";

		ss << "( select ema12 from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as ema12,";
		ss << "( select ema26 from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as ema26,";
		ss << "( select dif   from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as dif,";
		ss << "( select dea   from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as dea, ";

		ss << "( select price from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as price,";
		ss << "( select peak_date  from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as peak_date,";
		ss << "( select peak_dif   from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as peak_dif,";
		ss << "( select peak_high  from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as peak_high,";
		ss << "( select peak_low   from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as peak_low,";
		ss << "( select peak_sign  from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as peak_sign, ";

		ss << "( select cross_date  from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as cross_date,";
		ss << "( select cross_dif   from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as cross_dif,";
		ss << "( select cross_sign  from daily_macd b where b.code = a.code and b.tradedate = a.max_date ) as cross_sign ";

		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_macd group by code ) a ";

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


void CMAConvergenceDivergence::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

string	Cross(CMAConvergenceDivergenceData macd)
{
	string remark;
	if(macd.cross_sign[0]>0)
	{
		int k= 1;
		while(k<10)
		{
			if(macd.cross_sign[k]==0)
				break;
			if(macd.cross_dif[0] * macd.cross_dif[k] < 0)
				break;
			k ++;
		};

		if(k<10)
		{
			if(k%2==0)	k-=2;
			else	k -=1;

			k = (k/2) + 1;
			stringstream ss;
			if(macd.cross_dif[0]>0)
				ss << k << "次水上金叉";
			else if(macd.cross_dif[0]<0)
				ss << k << "次水下金叉";
			remark = ss.str();
		}
		else
		{
			if(macd.cross_dif[0]>0)
				remark = _T("多次水上金叉");
			else if(macd.cross_dif[0]<0)
				remark = _T("多次水下金叉");
		}
	}
	else if(macd.cross_sign[0]<0)
	{
		int k= 1;
		while(k<10)
		{
			if(macd.cross_sign[k]==0)
				break;
			if(macd.cross_dif[0] * macd.cross_dif[k] < 0)
				break;
			k ++;
		};

		if(k<10)
		{
			if(k%2==0)	k-=2;
			else	k -=1;

			k = (k/2) + 1;
			stringstream ss;
			if(macd.cross_dif[0]>0)
				ss << k << "次水上死叉";
			else if(macd.cross_dif[0]<0)
				ss << k << "次水下死叉";
			remark = ss.str();
		}
		else
		{
			if(macd.cross_dif[0]>0)
				remark = _T("多次水上死叉");
			else if(macd.cross_dif[0]<0)
				remark = _T("多次水下死叉");
		}
	}
	return remark;
}

//	计算分析 “顶背离”  和 “底背离”
string	Divergence(CMAConvergenceDivergenceData macd)
{
	string remark;
	if(macd.dif>0)
	{
		if(macd.peak_sign[0] >0)
		{
			//	寻找合适的前一个波峰
			int k = 2;
			while(k<10)
			{
				if(macd.peak_sign[k-1] < 0 && macd.peak_dif[k-1] < 0)
				{
					// 之前的波谷 越过了 0 轴，停止寻找
					k = 10;
					break;
				}
				if(macd.peak_sign[k]>0 && abs(macd.peak_dif[k] - macd.peak_dif[0]) > macd.close * 0.002 )
					break;
				k +=2;
			};

			if(k<10)
			{
				if(macd.peak_high[0] > macd.peak_high[k] && macd.peak_dif[0] < macd.peak_dif[k])
				{
					remark = "顶背离";
				}
			}
		}
	}
	if(macd.dif<0)
	{
		if( macd.peak_sign[0] <0 )
		{
			//	寻找合适的前一个波谷
			int k = 2;
			while(k<10)
			{
				if(macd.peak_sign[k-1] > 0 && macd.peak_dif[k-1] > 0)
				{
					// 之前的波峰 越过了 0 轴，停止寻找
					k = 10;
					break;
				}
				if(macd.peak_sign[k]<0 && abs(macd.peak_dif[k] - macd.peak_dif[0]) > macd.close * 0.002 )
					break;
				k +=2;
			};

			if(k<10)
			{
				if(macd.peak_low[0] < macd.peak_low[k] && macd.peak_dif[0] > macd.peak_dif[k])
				{
					remark = "底背离";
				}
			}
		}
	}
	return remark;
}

void   CMAConvergenceDivergence::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
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
				CMAConvergenceDivergenceData macd = m_mapIndex[strCode];
				//	先对之前价格按照现有 价位 进行前复权处理
				macd.ema12 = RecoverPrice(macd.ema12, today);
				macd.ema26 = RecoverPrice(macd.ema26, today);
				macd.dea = RecoverPrice(macd.dea, today);

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
		m_mapIndex[strCode].ema12 = today.close;
		m_mapIndex[strCode].ema26 = today.close;
		m_mapIndex[strCode].dif = 0;
		m_mapIndex[strCode].dea = 0;

		m_mapIndex[strCode].close = today.close;
		m_mapIndex[strCode].high = today.high;
		m_mapIndex[strCode].low = today.low;
	}
	else
	{
		CMAConvergenceDivergenceData macd = m_mapIndex[strCode];

		//	先对之前价格按照现有 价位 进行前复权处理
		macd.ema12 = RecoverPrice(macd.ema12, today);
		macd.ema26 = RecoverPrice(macd.ema26, today);
		macd.dea = RecoverPrice(macd.dea, today);

		CMAConvergenceDivergenceData macd_new = macd;
		macd_new.serial_num ++;
		macd_new.tradedate = strTradedate;
		macd_new.remark = "";

		macd_new.close = today.close;
		macd_new.high = today.high;
		macd_new.low = today.low;

		macd_new.ema12 = macd.ema12 * 11 /13 + today.close * 2 /13;
		macd_new.ema26 = macd.ema26 * 25 /27 + today.close * 2 /27;
		if(macd_new.serial_num < 26)
		{
			macd_new.dif = 0;
			macd_new.dea = 0;
		}
		else if(macd_new.serial_num == 26)
		{
			macd_new.dif = macd_new.ema12 - macd_new.ema26;
			macd_new.dea = macd_new.dif;
		}
		else
		{
			macd_new.dif = macd_new.ema12 - macd_new.ema26;
			macd_new.dea = macd.dea * 8 /10 + macd_new.dif * 2/10;

			//	进行 金叉 和 死叉的判断
			int nCrossSign = 0;
			if(macd.dif < macd.dea && macd_new.dea < macd_new.dif )
			{
				nCrossSign = 1;
			}
			else if(macd.dif > macd.dea && macd_new.dea > macd_new.dif )
			{
				nCrossSign = -1;
			}
			if(nCrossSign!=0)
			{
				for(int i=8; i >=0; i-- )
				{
					macd_new.cross_date[i+1] = macd_new.cross_date[i];
					macd_new.cross_sign[i+1] = macd_new.cross_sign[i];
					macd_new.cross_dif[i+1] = macd_new.cross_dif[i];
				}
				macd_new.cross_sign[0] = nCrossSign;
				macd_new.cross_date[0] = macd_new.tradedate;
				macd_new.cross_dif[0] = macd_new.dif;

				if(macd_new.remark.empty()==false) macd_new.remark += ","; 
				macd_new.remark = Cross(macd_new);
			}

			//	dif 穿越 0 轴测试
			if(macd.dif > 0 and macd_new.dif < 0)
			{
				if(macd_new.remark.empty()==false) macd_new.remark += ","; 
				macd_new.remark += _T("dif下穿0轴");
			}
			else if(macd.dif < 0 and macd_new.dif > 0)
			{
				if(macd_new.remark.empty()==false) macd_new.remark += ","; 
				macd_new.remark += _T("dif上穿0轴");
			}

			//	进行峰值 的判断，记录前 10 个波峰和波谷
			if(macd_new.peak_sign[0] ==0 )
			{
				if(macd_new.dif > macd.dif)
				{
					//	昨日的作为 波谷
					macd_new.peak_sign[0] = -1;
					macd_new.peak_date[0] = macd.tradedate;
					macd_new.peak_dif[0] = macd.dif;
					macd_new.peak_high[0] = macd.high;
					macd_new.peak_low[0] = macd.low;
				}
				else if(macd_new.dif < macd.dif)
				{
					//	昨日的作为 波峰
					macd_new.peak_sign[0] = 1;
					macd_new.peak_date[0] = macd.tradedate;
					macd_new.peak_dif[0] = macd.dif;
					macd_new.peak_high[0] = macd.high;
					macd_new.peak_low[0] = macd.low;
				}
			}
			else if(macd_new.peak_sign[0] > 0)
			{
				// 之前已经找出了 波峰
				if(macd_new.dif > macd.dif)
				{
					//	昨日为 新的波谷
					for(int i=8; i >=0; i-- )
					{
						macd_new.peak_date[i+1] = macd_new.peak_date[i];
						macd_new.peak_sign[i+1] = macd_new.peak_sign[i];
						macd_new.peak_dif[i+1] = macd_new.peak_dif[i];
						macd_new.peak_high[i+1] = macd_new.peak_high[i];
						macd_new.peak_low[i+1] = macd_new.peak_low[i];
					}
					macd_new.peak_sign[0] = -1;
					macd_new.peak_date[0] = macd.tradedate;
					macd_new.peak_dif[0] = macd.dif;
					macd_new.peak_high[0] = macd.high;
					macd_new.peak_low[0] = macd.low;

					// macd_new.remark += _T(",dif 拐点向上");
					if(macd_new.remark.empty()==false) macd_new.remark += ","; 
					macd_new.remark += Divergence(macd_new);
				}
			}
			else
			{
				// 之前已经找出了 波谷
				if(macd_new.dif < macd.dif  )
				{
					//	昨日为 新的波峰
					for(int i=8; i >=0; i-- )
					{
						macd_new.peak_date[i+1] = macd_new.peak_date[i];
						macd_new.peak_sign[i+1] = macd_new.peak_sign[i];
						macd_new.peak_dif[i+1] = macd_new.peak_dif[i];
						macd_new.peak_high[i+1] = macd_new.peak_high[i];
						macd_new.peak_low[i+1] = macd_new.peak_low[i];
					}
					macd_new.peak_sign[0] = 1;
					macd_new.peak_date[0] = macd.tradedate;
					macd_new.peak_dif[0] = macd.dif;
					macd_new.peak_high[0] = macd.high;
					macd_new.peak_low[0] = macd.low;

					//macd_new.remark += _T(",dif 拐点向下");
					if(macd_new.remark.empty()==false) macd_new.remark += ","; 
					macd_new.remark += Divergence(macd_new);
				}
			}

			// 对死叉做补充判断
			if(macd_new.cross_sign[0]<0)
			{
				string remark ;
				/*	如果前一个是金叉 且与本次死叉同时在水下 或者 水下 */
				if(macd_new.cross_sign[1]>0 && macd_new.cross_dif[0] * macd_new.cross_dif[1]> 0)
				{
					if( (macd_new.dif < macd_new.cross_dif[1] && macd.dif >= macd_new.cross_dif[1]) 
						|| (macd_new.dif <= macd_new.cross_dif[1] && macd.dif > macd_new.cross_dif[1]) )
					{
						if(macd_new.dif < 0)
							remark += "水下跌破前次金叉";
						else 
							remark += "水上跌破前次金叉";
					}
				}

				if(macd_new.remark.empty()==false && remark.empty() == false) 
					macd_new.remark += ","; 
				macd_new.remark += remark;
			}
		
		}
		m_mapIndex[strCode] = macd_new;
	}

}

void CMAConvergenceDivergence::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
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
			ss << "insert into daily_macd (code , tradedate, serial_num, ema12, ema26, dif, dea, remark, "
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
			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_macd", strTradedate);
	}
	catch(...)
	{

	}
}

void CMAConvergenceDivergence::EndCalculate()
{
	
}
