#include "StdAfx.h"
#include "Stock.h"
#include "OnBalanceVolume.h"

COnBalanceVolume::COnBalanceVolume(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

COnBalanceVolume::~COnBalanceVolume()
{
}


void COnBalanceVolume::BeginCalculate()
{
	stringstream ss;


	try{
		ss << "create table if not exists daily_obv (code varchar(32), tradedate date, close double, remark varchar(64), "
			<< " obv double,	obv_1 double ,  obv_2 double,  obv_3 double,  obv_4 double, "
			<< " obv_5 double,  obv_6 double ,  obv_7 double,  obv_8 double,  obv_9 double, "
			<< " obv_10 double, obv_11 double ,  "
			<< " vol double,	vol_1 double ,  vol_2 double,  vol_3 double,  vol_4 double, "
			<< " vol_5 double,  vol_6 double ,  vol_7 double,  vol_8 double,  vol_9 double, "
			<< " vol_10 double, vol_11 double ,  "
			<< " PRIMARY KEY ( code, tradedate))";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select close from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as close,";

		ss << "( select obv   from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv,";
		ss << "( select obv_1 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_1,";
		ss << "( select obv_2 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_2,";
		ss << "( select obv_3 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_3,";
		ss << "( select obv_4 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_4,";
		ss << "( select obv_5 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_5,";
		ss << "( select obv_6 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_6,";
		ss << "( select obv_7 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_7,";
		ss << "( select obv_8 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_8,";
		ss << "( select obv_9 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_9,";

		ss << "( select obv_10 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_10,";
		ss << "( select obv_11 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as obv_11,";
		
		ss << "( select vol   from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol,";
		ss << "( select vol_1 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_1,";
		ss << "( select vol_2 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_2,";
		ss << "( select vol_3 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_3,";
		ss << "( select vol_4 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_4,";
		ss << "( select vol_5 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_5,";
		ss << "( select vol_6 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_6,";
		ss << "( select vol_7 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_7,";
		ss << "( select vol_8 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_8,";
		ss << "( select vol_9 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_9,";

		ss << "( select vol_10 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_10,";
		ss << "( select vol_11 from daily_obv b where b.code = a.code and b.tradedate = a.max_date ) as vol_11 ";
		
		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_obv  group by code ) a ";

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
			m_mapIndex[strCode].close = r.get<double>("close");
			m_mapIndex[strCode].obv[0] = r.get<double>("obv");
			m_mapIndex[strCode].obv[1] = r.get<double>("obv_1");
			m_mapIndex[strCode].obv[2] = r.get<double>("obv_2");
			m_mapIndex[strCode].obv[3] = r.get<double>("obv_3");
			m_mapIndex[strCode].obv[4] = r.get<double>("obv_4");
			m_mapIndex[strCode].obv[5] = r.get<double>("obv_5");
			m_mapIndex[strCode].obv[6] = r.get<double>("obv_6");
			m_mapIndex[strCode].obv[7] = r.get<double>("obv_7");
			m_mapIndex[strCode].obv[8] = r.get<double>("obv_8");
			m_mapIndex[strCode].obv[9] = r.get<double>("obv_9");

			m_mapIndex[strCode].obv[10] = r.get<double>("obv_10");
			m_mapIndex[strCode].obv[11] = r.get<double>("obv_11");

			m_mapIndex[strCode].vol[0] = r.get<double>("vol");
			m_mapIndex[strCode].vol[1] = r.get<double>("vol_1");
			m_mapIndex[strCode].vol[2] = r.get<double>("vol_2");
			m_mapIndex[strCode].vol[3] = r.get<double>("vol_3");
			m_mapIndex[strCode].vol[4] = r.get<double>("vol_4");
			m_mapIndex[strCode].vol[5] = r.get<double>("vol_5");
			m_mapIndex[strCode].vol[6] = r.get<double>("vol_6");
			m_mapIndex[strCode].vol[7] = r.get<double>("vol_7");
			m_mapIndex[strCode].vol[8] = r.get<double>("vol_8");
			m_mapIndex[strCode].vol[9] = r.get<double>("vol_9");

			m_mapIndex[strCode].vol[10] = r.get<double>("vol_10");
			m_mapIndex[strCode].vol[11] = r.get<double>("vol_11");
		}
	}
	catch(...)
	{

	}
}


void COnBalanceVolume::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

void   COnBalanceVolume::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
												string strTradedate, string strCode, string strPlate)
{

	if(today.close<0)
	{
		// 本日停牌
		if(m_mapIndex.count(strCode)>0)
		{
			m_mapIndex[strCode].remark ="";
		}

		return;
	}


	// 逐个计算 每股的情况

	if(m_mapIndex.count(strCode)==0)
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].close = today.close;
		m_mapIndex[strCode].remark = "";
	}
	else
	{
		
		COnBalanceVolumeData yester = m_mapIndex[strCode];

		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].remark = "";
		m_mapIndex[strCode].close = today.close;

		// VA = V × [（C - L）- （H - C）]/（H - C）, 以下对该公式进行了变形
		double today_obv = 0;
		if(today.high > today.low)
		{
			today_obv = today.volume * ((today.close - today.low) - ( today.high - today.close )) / ( today.high - today.low);
		}
		else
		{
			//	跌停 或者涨停 一字板
			if(today.close > RecoverPrice(yester.close, today))
				today_obv = today.volume;
			else
				today_obv = -1 * today.volume;
		}
		if(today_obv == 0)  today_obv = 1;


		for(int i= OBV_SMOOTHING_DAY - 1; i>0; i--)
		{
			m_mapIndex[strCode].obv[i] = RecoverVolume(m_mapIndex[strCode].obv[i-1], today);
			m_mapIndex[strCode].vol[i] = RecoverVolume(m_mapIndex[strCode].vol[i-1], today);
		}
		m_mapIndex[strCode].obv[0] = today_obv;
		m_mapIndex[strCode].vol[0] = today.volume;

		// 计算前 10 个交易日累计 OBV 及平均成交量

		double accum_obv = 0;
		double ma_vol = 0;
		if(m_mapIndex[strCode].obv[OBV_SMOOTHING_DAY - 1] !=0)
		{
			for(int i = 2; i< OBV_SMOOTHING_DAY; i++)
			{
				accum_obv += m_mapIndex[strCode].obv[i];
			}

			for(int i = 1; i< OBV_SMOOTHING_DAY; i++)
			{
				ma_vol += m_mapIndex[strCode].vol[i];
			}
			ma_vol = ma_vol/ (OBV_SMOOTHING_DAY - 1);
		}

		if(strCode == "000626" && strTradedate =="2016-10-24" )
		{
			int kkk = 1;
		}

			
		// 统计 成交量突变事件
		if(accum_obv> 0 )
		{
			if( m_mapIndex[strCode].vol[0] > 2 * ma_vol 
				&& m_mapIndex[strCode].obv[1] > accum_obv
				&& today_obv < 0 
				&& abs(today_obv) > 1.5 * m_mapIndex[strCode].obv[1] )
			{
				m_mapIndex[strCode].remark = "股价放量大跌";
			}
			
		}
	}	
}

void COnBalanceVolume::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		vector<string>  vecSqlStr;
		map<string, COnBalanceVolumeData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			COnBalanceVolumeData rsi = it->second;
			ss.str("");
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << Double2String(rsi.close, "%.2f")
				<< ", \'" << rsi.remark << "\'";
			for(int i=0; i< OBV_SMOOTHING_DAY; i++)
				ss << ", " << Double2String(rsi.obv[i], "%.2f");
			for(int i=0; i< OBV_SMOOTHING_DAY; i++)
				ss << ", " << Double2String(rsi.vol[i], "%.2f");
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
			ss << "insert into daily_obv (code ,tradedate, close,  remark, "
				<< " obv,	 obv_1,  obv_2,  obv_3,  obv_4,  obv_5,  obv_6,  obv_7, obv_8, obv_9, " 
				<< " obv_10, obv_11, " 
				<< " vol,	 vol_1,  vol_2,  vol_3,  vol_4,  vol_5,  vol_6,  vol_7, vol_8, vol_9, " 
				<< " vol_10, vol_11 " 
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

		if(nLoop >0)
		{
			((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_obv", strTradedate);
		}
		
	}
	catch(...)
	{

	}
}

void COnBalanceVolume::EndCalculate()
{
	
}



void COnBalanceVolume::ExportFromDatabase(string strCode, double startTime, double endTime)
{
	stringstream ss;

	try
	{
		ss << "COnBalanceVolume 开始从 MYSQL 获取数据......";
		//ss << "【" << m_nCode << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			<< " obv_index1, obv_index2, obv_index3  "
			<< " from daily_obv where code=\'" << strCode << "\' ";
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
			COnBalanceVolumeData  bbd;
			bbd.tradedate = r.get<string>("tradedate");

			m_vec_index.insert(m_vec_index.end(), bbd);
		}

		ss.str("");
		ss << "OK, COnBalanceVolume 从 MYSQL 获取数据完毕.";
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
}
