#include "StdAfx.h"
#include "Stock.h"
#include "TechnicalAnalysis.h"

CTechnicalAnalysis * CTechnicalAnalysis::instance = NULL;

CTechnicalAnalysis::CTechnicalAnalysis(void)
{
	m_nLateset = -1;
	m_nEarliest = -1;
}

CTechnicalAnalysis::~CTechnicalAnalysis(void)
{
}

CTechnicalAnalysis& CTechnicalAnalysis::getInstance()
{
	if(instance==NULL)
		instance = new CTechnicalAnalysis;
	return *instance;
}


void CTechnicalAnalysis::FetchInit(string beginTradedate)
{

	m_mapTechnicalAnalysisData.clear();
	m_vecCode.clear();
	m_vecPlate.clear();
	m_vecTradedate.clear();
	m_vecCode.clear();


	stringstream ss;
	try
	{
		
		//////////////////////////////////////////////////////////////////////////

		string  endTradedate = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchTableUpdated("NetEase_Trade_Daily");
		string  endTradedate1 = ((CStockApp *)AfxGetApp())->m_StockDataLog.SearchTableUpdated("cninfo_bonus");
		if(endTradedate > endTradedate1)
			endTradedate = endTradedate1;

		LOG4CPLUS_DEBUG(g_logger, (string(_T("技术分析日期范围 ：")) + beginTradedate +  " 到 " + endTradedate ));


		if(endTradedate == "0000-00-00")
			return ;

		ss.str("");
		ss << "select  DATE_FORMAT(TradeDate, '%Y-%m-%d') as TradeDate "
			<< " from sohuindexdaily where code = 1  and TradeDate > \'"    // HuShen_GetTradeDate(
			<< beginTradedate 
			<<"\' and  tradeDate <=\'" << endTradedate << "\' "
			<< " order by TradeDate  ";
			
		string test = ss.str();

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			m_vecTradedate.insert(m_vecTradedate.end(), r.get<string>("TradeDate") );

		}

		//////////////////////////////////////////////////////////////////////////
		// 获取当前所有的 股票
		ss.str("");
		ss << "select  code, abbreviation, stock_plate, DATE_FORMAT(a_time_to_market, '%Y-%m-%d') as a_time_to_market  "
			<< " from stockinfo   ORDER BY a_time_to_market  ";

		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			m_vecCode.insert(m_vecCode.end(), Int2String( r.get<int>("code") ,"%06d" ) );
			m_vecPlate.insert(m_vecPlate.end(), r.get<string>("stock_plate"));
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}



bool CTechnicalAnalysis::FetchNext()
{
	stringstream ss;
	
	int nTradedate = m_nLateset + 1;
	if(nTradedate>= m_vecTradedate.size())
		return false;

	try
	{
		ss << "CTechnicalAnalysis 开始从 MYSQL 获取数据......";
		ss << "【" << 1 << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		string  td = m_vecTradedate[nTradedate];
		string  td1 = td;
		if(nTradedate + 1 < m_vecTradedate.size())
			td1 = m_vecTradedate[nTradedate + 1];

		ss.str("");
		ss << "select code, "
			<< " date_format(a_time_to_delisting, '%Y-%m-%d') as a_time_to_delisting, "
			<< " date_format(a_time_to_suspend, '%Y-%m-%d') as a_time_to_suspend, "
			<< " date_format(a_time_to_market, '%Y-%m-%d') as a_time_to_market, "

			<< " (select CONCAT( Open,',',High,',',Low,',',Close,',',Volume,',', "
			<< " Change_Amount,',',Change_Rate,',',Turnover,',',Turnover_Rate,',',Circulating_Value,',',LClose) " 
			<< " from NetEase_Trade_Daily b where b.Code = a.code and b.TradeDate=\'"<< td << "\'"
			<< " ) as Open,"

			<< " (select CONCAT(change_reason_desc,'|',real_date,'|',AShares"
			<< " ) from capitalstructure b "
			<< " where b.code = a.Code and b.real_date <= \'"<< td <<"\' "
			<< " ORDER BY real_date desc limit 1) as capitalstructure ,"

			<< " (select CONCAT(change_reason_desc,'|',real_date,'|',AShares"
			<< " ) from capitalstructure b "
			<< " where b.code = a.Code and b.real_date = \'"<< td1 <<"\' "
			<< " ) as capitalstructure1 ,"

			<< " (select CONCAT(max(bonus_share),',',max(bonus_cash), ',', max(reserve_to_common_share))"
			<< " from cninfo_bonus b where b.code = a.Code and b.exit_right_date =\'"<< td << "\') as bonus_share"
			<< " from stockinfo a  ";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		string test = ss.str();
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			/*vector<string>	map_key;
			map_key.insert(map_key.end(), m_vecTradedate[nTradedate]);
			map_key.insert(map_key.end(), Int2String( r.get<int>("Code") ,"%06d" ));*/

			string map_key =  m_vecTradedate[nTradedate] + "_" + Int2String( r.get<int>("code") ,"%06d" );
			

			string openStr = r.get<string>("Open", "");
			string bonusStr = r.get<string>("bonus_share", "");
			CTechnicalAnalysisData tad;

			if(openStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(openStr, ",", vecYield);

				tad.open = String2Double(vecYield[0]);
				tad.high = String2Double(vecYield[1]);
				tad.low =  String2Double(vecYield[2]);
				tad.close = String2Double(vecYield[3]);
				tad.volume =  String2Double(vecYield[4]);
				tad.change =  String2Double(vecYield[5]);
				tad.change_rate =  String2Double(vecYield[6]);
				tad.turnover = String2Double(vecYield[7]);
				tad.turnover_rate =  String2Double(vecYield[8]);

				tad.circulating_value = String2Double(vecYield[9]);
				tad.lclose = String2Double(vecYield[10]);
			}

			if(bonusStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(bonusStr, ",", vecYield);

				tad.bonus_share = String2Double(vecYield[0]);
				tad.bonus_cash = String2Double(vecYield[1]);
				tad.reserve_to_common_share =  String2Double(vecYield[2]);
			}

			string capitalstructureStr = r.get<string>("capitalstructure", "");
			if(capitalstructureStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(capitalstructureStr, "|", vecYield);

				tad.change_reason_desc = Utf8_GBK(vecYield[0]);
				tad.real_date = vecYield[1];
				tad.circulating = String2Double(vecYield[2]);
			}

			// 如果存在 转增、送股等情况，可能需要进行调整 流通股数
			if(tad.change_reason_desc.find("送股")!=string::npos && tad.real_date==td)
			{
				if(tad.bonus_share > 0)
				{
					capitalstructureStr = r.get<string>("capitalstructure1", "");
					vector<string>	vecYield;
					splitString(capitalstructureStr, "|", vecYield);
					if(vecYield.size()>=3 && Utf8_GBK(vecYield[0]).find("送股")!=string::npos )
					{
						tad.circulating = String2Double(vecYield[2]);

						ss.str("");
						ss << map_key << ", 修正流通股数, 送股" ;
						LOG4CPLUS_DEBUG(g_logger, ss.str());
					}
				}
				else
				{

				}
				
			}
			else if(tad.change_reason_desc.find("转增股本")!=string::npos && tad.real_date==td)
			{
				if(tad.reserve_to_common_share > 0)
				{
					capitalstructureStr = r.get<string>("capitalstructure1", "");
					vector<string>	vecYield;
					splitString(capitalstructureStr, "|", vecYield);
					if(vecYield.size()>=3 && Utf8_GBK(vecYield[0]).find("转增股本")!=string::npos )
					{
						tad.circulating = String2Double(vecYield[2]);

						ss.str("");
						ss << map_key << ", 修正流通股数, 转增股本" ;
						LOG4CPLUS_DEBUG(g_logger, ss.str());
					}
				}
				else
				{

				}
			}


			string a_time_to_delisting = r.get<string>("a_time_to_delisting", "");
			string a_time_to_suspend = r.get<string>("a_time_to_suspend", "");
			if(a_time_to_delisting==m_vecTradedate[nTradedate] || a_time_to_suspend==m_vecTradedate[nTradedate] )
				tad.is_exit_from_market = true;
			else
				tad.is_exit_from_market = false;

			tad.a_time_to_market = r.get<string>("a_time_to_market", "");
										
			// if(tad.close >=0 || tad.bonus_share >=0 || tad.is_exit_from_market)
				m_mapTechnicalAnalysisData[map_key] = tad;

		}

		m_nLateset = nTradedate;
		if(m_nEarliest < 0) m_nEarliest = nTradedate;

		//	当内存中保存的 交易数据超过 n 天时，删除最早的数据
		if(m_nLateset - m_nEarliest > 10)
		{
			for(int k=0; k<m_vecCode.size(); k++)
			{
				/*vector<string>	map_key;
				map_key.insert(map_key.end(), m_vecTradedate[m_nEarliest]);
				map_key.insert(map_key.end(), m_vecCode[k]);*/

				string map_key = m_vecTradedate[m_nEarliest] + "_" + m_vecCode[k];

				m_mapTechnicalAnalysisData.erase(map_key);
			}

			m_nEarliest++;
		}


		ss.str("");
		ss << "OK, 从 MYSQL 获取数据完毕.";
		//ss << " 共 " << cnt << "条 \r\n" ;
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

	return true;
}



DWORD WINAPI CTechnicalAnalysis::CommandHandler1(void *data)
{
	CTechnicalAnalysis * pThis = (CTechnicalAnalysis *)data;

	pThis->CommandHandler();
	return 0 ;
}


DWORD CTechnicalAnalysis::CommandHandler()
{
	if(m_vecIndex.size()==0)
		return 0;


	unsigned long ticks =  GetTickCount();

	//	建立 指标事件表
	session sql(g_MysqlPool);
	stringstream ss;
	

	// 开始初始化各个 指标
	string beginTradedate = "2050-01-01";
	for(int k=0; k<m_vecIndex.size(); k++)
	{
		m_vecIndex[k]->BeginCalculate();
		string tmp = m_vecIndex[k]->m_strLatest;
		if(tmp.empty()==false && tmp < beginTradedate)
			beginTradedate = tmp;
	}
	
	//	beginTradedate 是最早指标的 最后日期
	FetchInit(beginTradedate);
	
	while(FetchNext())
	{
		string strTradedate =  m_vecTradedate[m_nLateset];

		LOG4CPLUS_DEBUG(g_logger, (strTradedate + " ------ 开始技术分析 ------"));

		if(m_nLateset == 0)
		{
			//	以便保证内存中已经读入昨日的数据
			// continue;
		}

		map<string, unsigned long>	indexTicks;
		map<string, unsigned long>	dbTicks;
		

		//	发送某交易日开始计算的通知
		for(int j=0; j<m_vecIndex.size(); j++)
		{
			if(strTradedate > m_vecIndex[j]->m_strLatest)
				m_vecIndex[j]->BeginTradedate(*this, strTradedate);
		}

		//	开始某个交易日逐个股票的计算
		for(int k= 0 ; k< m_vecCode.size(); k++)
		{
			CTechnicalAnalysisData  today;
			CTechnicalAnalysisData  yesterday;
			string strCode =  m_vecCode[k];
			string strPlate = m_vecPlate[k];

			/*vector<string>	map_key;
			map_key.insert(map_key.end(), strTradedate);
			map_key.insert(map_key.end(),strCode);*/

			string map_key = strTradedate + "_" + strCode;
			if(m_mapTechnicalAnalysisData.count(map_key)>0 )
				today = m_mapTechnicalAnalysisData[map_key];

			if(m_nLateset>0)
			{
				string map_key_yesterday = m_vecTradedate[m_nLateset-1] + "_" + strCode;
				if(m_mapTechnicalAnalysisData.count(map_key_yesterday)>0)
					yesterday = m_mapTechnicalAnalysisData[map_key_yesterday];
			}

			if(1 )
			{
				for(int j=0; j<m_vecIndex.size(); j++)
				{
					if(strTradedate > m_vecIndex[j]->m_strLatest)
					{
						unsigned long ticks = GetTickCount();
						
						//	假如该股票已经上市，并且今天停牌，则 today.close = -1
						if(m_mapTechnicalAnalysisData.count(map_key)>0 )
						{
							m_vecIndex[j]->Calculate(*this, today, yesterday, strTradedate, strCode, strPlate);

							ticks = GetTickCount() - ticks;
							if(indexTicks.count(m_vecIndex[j]->IndexName())==0)
								indexTicks[m_vecIndex[j]->IndexName()] = ticks;
							else
								indexTicks[m_vecIndex[j]->IndexName()] += ticks;
						}
					}
				}
			}
		}

		

		//	通知某个交易日计算 结束
		for(int j=0; j<m_vecIndex.size(); j++)
		{
			if(strTradedate > m_vecIndex[j]->m_strLatest)
			{
				unsigned long ticks = GetTickCount();
				m_vecIndex[j]->EndTradedate(*this, strTradedate);

				ticks = GetTickCount() - ticks;
				if(dbTicks.count(m_vecIndex[j]->IndexName())==0)
					dbTicks[m_vecIndex[j]->IndexName()] = ticks;
				else
					dbTicks[m_vecIndex[j]->IndexName()] += ticks;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//	打印处理的使用时间
		map<string , unsigned long >::iterator it = indexTicks.begin();
		for(; it!= indexTicks.end(); it++)
		{
			stringstream ss;
			ss << it->first << ", calculate = " << it->second << ", database = " << dbTicks[it->first] ;
			LOG4CPLUS_DEBUG(g_logger ,ss.str());

		}

		Sleep(10);
	}

	//	保存指标的计算结果
	for(int k=0; k<m_vecIndex.size(); k++)
	{
		m_vecIndex[k]->EndCalculate();
		delete m_vecIndex[k];
	}
	m_vecIndex.clear();


	ticks = GetTickCount() - ticks;
	return 0;
}

