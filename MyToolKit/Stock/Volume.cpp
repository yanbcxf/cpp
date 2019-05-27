#include "StdAfx.h"
#include "Stock.h"
#include "volume.h"

CVolume::CVolume(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CVolume::~CVolume()
{
}


void CVolume::BeginCalculate()
{
	stringstream ss;


	try{
		ss << "create table if not exists daily_volume (code varchar(32), tradedate date, "
			<< " volume_index1 double,  volume_index2 double, volume_index3 double, remark varchar(64), "
			<< " volume double,	  volume_1 double , volume_2 double, volume_3 double, volume_4 double, "
			<< " volume_5 double,  volume_6 double , volume_7 double, volume_8 double, volume_9 double, "
			<< " volume_10 double, volume_11 double , volume_12 double, volume_13 double, volume_14 double, "
			<< " volume_15 double, volume_16 double , volume_17 double, volume_18 double, volume_19 double, "
			<< " PRIMARY KEY ( code, tradedate))";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select volume from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume,";
		ss << "( select volume_1 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_1,";
		ss << "( select volume_2 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_2,";
		ss << "( select volume_3 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_3,";
		ss << "( select volume_4 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_4,";
		ss << "( select volume_5 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_5,";
		ss << "( select volume_6 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_6,";
		ss << "( select volume_7 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_7,";
		ss << "( select volume_8 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_8,";
		ss << "( select volume_9 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_9,";
		ss << "( select volume_10 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_10,";
		ss << "( select volume_11 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_11,";
		ss << "( select volume_12 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_12,";
		ss << "( select volume_13 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_13,";
		ss << "( select volume_14 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_14,";
		ss << "( select volume_15 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_15,";
		ss << "( select volume_16 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_16,";
		ss << "( select volume_17 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_17,";
		ss << "( select volume_18 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_18,";
		ss << "( select volume_19 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_19,";

		ss << "( select volume_index1 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_index1, ";
		ss << "( select volume_index2 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_index2, ";
		ss << "( select volume_index3 from daily_volume b where b.code = a.code and b.tradedate = a.max_date ) as volume_index3 ";

		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_volume  group by code ) a ";

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
			m_mapIndex[strCode].volume[0] = r.get<double>("volume");
			m_mapIndex[strCode].volume[1] = r.get<double>("volume_1");
			m_mapIndex[strCode].volume[2] = r.get<double>("volume_2");
			m_mapIndex[strCode].volume[3] = r.get<double>("volume_3");
			m_mapIndex[strCode].volume[4] = r.get<double>("volume_4");
			m_mapIndex[strCode].volume[5] = r.get<double>("volume_5");
			m_mapIndex[strCode].volume[6] = r.get<double>("volume_6");
			m_mapIndex[strCode].volume[7] = r.get<double>("volume_7");
			m_mapIndex[strCode].volume[8] = r.get<double>("volume_8");
			m_mapIndex[strCode].volume[9] = r.get<double>("volume_9");

			m_mapIndex[strCode].volume[10] = r.get<double>("volume_10");
			m_mapIndex[strCode].volume[11] = r.get<double>("volume_11");
			m_mapIndex[strCode].volume[12] = r.get<double>("volume_12");
			m_mapIndex[strCode].volume[13] = r.get<double>("volume_13");
			m_mapIndex[strCode].volume[14] = r.get<double>("volume_14");
			m_mapIndex[strCode].volume[15] = r.get<double>("volume_15");
			m_mapIndex[strCode].volume[16] = r.get<double>("volume_16");
			m_mapIndex[strCode].volume[17] = r.get<double>("volume_17");
			m_mapIndex[strCode].volume[18] = r.get<double>("volume_18");
			m_mapIndex[strCode].volume[19] = r.get<double>("volume_19");

			m_mapIndex[strCode].volume_index1 = r.get<double>("volume_index1");
			m_mapIndex[strCode].volume_index2 = r.get<double>("volume_index2");
			m_mapIndex[strCode].volume_index3 = r.get<double>("volume_index3");

		}
	}
	catch(...)
	{

	}
}


void CVolume::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}

void   CVolume::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
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
		m_mapIndex[strCode].volume[0] = today.volume;
	}
	else
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		for(int i= 19; i>0; i--)
		{
			m_mapIndex[strCode].volume[i] = m_mapIndex[strCode].volume[i-1];
		}
		
		m_mapIndex[strCode].volume[0] = today.volume;
	}

	m_mapIndex[strCode].remark = "";
	m_mapIndex[strCode].close = 0;

	if(m_mapIndex[strCode].volume[19] >0)
	{
		//	存在 20 个交易日数据，计算指标
		CVolumeData bbd = m_mapIndex[strCode];
		double volume_index1 = 0;
		for(int i=0; i<VOLUME_SMOOTHING_DAY_1; i++)
			volume_index1 += bbd.volume[i];
		volume_index1 = volume_index1 /VOLUME_SMOOTHING_DAY_1;

		double volume_index2 = 0;
		for(int i=0; i<VOLUME_SMOOTHING_DAY_2; i++)
			volume_index2 += bbd.volume[i];
		volume_index2 = volume_index2 /VOLUME_SMOOTHING_DAY_2;

		double volume_index3 = 0;
		for(int i=0; i<VOLUME_SMOOTHING_DAY_3; i++)
			volume_index3 += bbd.volume[i];
		volume_index3 = volume_index3 /VOLUME_SMOOTHING_DAY_3;

		
		// 
		if(volume_index1 > volume_index2 && bbd.volume_index1 < bbd.volume_index2 )
		{
			m_mapIndex[strCode].remark = _T("成交量上升，5日线超过 10日线");
		}
		else if(volume_index1 < volume_index2 && bbd.volume_index1 > bbd.volume_index2 )
		{
			m_mapIndex[strCode].remark = _T("成交量下降，5日线跌破 10日线");
		}
		
		m_mapIndex[strCode].volume_index1 = volume_index1;
		m_mapIndex[strCode].volume_index2 = volume_index2;
		m_mapIndex[strCode].volume_index3 = volume_index3;
	}
	else
	{

	}
	
}

void CVolume::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		ss << "insert into daily_volume (code , tradedate, volume_index1, volume_index2, volume_index3, remark, "
			<< " volume, volume_1, volume_2, volume_3, volume_4, volume_5, volume_6, volume_7, volume_8, volume_9, " 
			<< " volume_10, volume_11, volume_12, volume_13, volume_14, volume_15, volume_16, volume_17, volume_18, volume_19 " 
			<< " ) values ";

		map<string, CVolumeData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CVolumeData rsi = it->second;
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << Double2String(rsi.volume_index1, "%.1f")
				<< ", " << Double2String(rsi.volume_index2, "%.1f")
				<< ", " << Double2String(rsi.volume_index3, "%.1f")
				<< ", \'" << rsi.remark << "\'";
			for(int i=0; i< 20; i++)
				ss << ", " << Double2String(rsi.volume[i], "%.1f");
			ss << ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;
		
		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_volume", strTradedate);
	}
	catch(...)
	{

	}
}

void CVolume::EndCalculate()
{
	
}



void CVolume::ExportFromDatabase(string strCode, double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CVolume 开始从 MYSQL 获取数据......";
		//ss << "【" << m_nCode << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			<< " volume_index1, volume_index2, volume_index3  "
			<< " from daily_volume where code=\'" << strCode << "\' ";
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
			CVolumeData  bbd;
			bbd.tradedate = r.get<string>("tradedate");
			bbd.volume_index1 = r.get<double>("volume_index1");
			bbd.volume_index2 = r.get<double>("volume_index2");
			bbd.volume_index3 = r.get<double>("volume_index3");

			m_vec_index.insert(m_vec_index.end(), bbd);
		}

		ss.str("");
		ss << "OK, CVolume 从 MYSQL 获取数据完毕.";
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
}
