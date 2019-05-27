#include "StdAfx.h"
#include "StockDataLog.h"

//////////////////////////////////////////////////////////////////////////
//	股票基本信息基础资料

string CStockDataLog::SqlScript()
{
	stringstream ss;

	
	ss << " create table if not exists stock_data_log ( code int, "
		<< " cninfo_bonus date, cninfo_placement_of_share date, SouhuTradingDaily date, "
		<< " PRIMARY KEY(code))";

	return ss.str();
}

CStockDataLog::CStockDataLog()
{
	m_mutxMap.Init();
	m_UpdatedNum = 0;

	
}

CStockDataLog::~CStockDataLog()
{
	
}

void CStockDataLog::FetchFromDB()
{
	stringstream ss;
	try
	{
		JMutexAutoLock lock(m_mutxMap);

		m_TableUpdatedMap.clear();

		row r;
		session sql(g_MysqlPool);
		ss.str("");
		ss << " create table if not exists table_update_log ( table_name varchar(64), update_date date, "
			<< " PRIMARY KEY(table_name))";
		sql << ss.str();

		ss.str("");
		ss << "select table_name, date_format(update_date, '%Y-%m-%d') as update_date from table_update_log ";
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			string	table_name = r.get<string>("table_name", "");
			string	update_date = r.get<string>("update_date", "0000-00-00");
			
			if(table_name.empty()==false)
			{
				m_TableUpdatedMap[table_name] = update_date;
			}

		}

		//////////////////////////////////////////////////////////////////////////

		sql << SqlScript();
		sql << "call add_col('stock_data_log','SohuMainIncomeComposition','date', '')";
		sql << "call add_col('stock_data_log','SohuPerformanceForecast','date', '')";
		sql << "call add_col('stock_data_log','CapitalFlows163','date', '')";
		sql << "call add_col('stock_data_log','_10jqka_bonus','date', '')";
		sql << "call add_col('stock_data_log','CapitalStructure','date', '')";
		sql << "call add_col('stock_data_log','NetEase_Trade_Daily','date', '')";

		sql << "call add_col('stock_data_log','Balance','date', '')";
		sql << "call add_col('stock_data_log','IncomeStatement','date', '')";
		sql << "call add_col('stock_data_log','CashFlows','date', '')";

		sql << "call add_col('stock_data_log','Balance_Zip','int', 0)";
		sql << "call add_col('stock_data_log','IncomeStatement_Zip','int', 0)";
		sql << "call add_col('stock_data_log','CashFlows_Zip','int', 0)";

		sql << "call add_col('stock_data_log','hexun_research_paper','date', '')";

		m_CStockDownloadDataMap.clear();

		ss.str("");
		ss << "select code"
			<< ", date_format(cninfo_bonus, '%Y-%m-%d') as cninfo_bonus"
			<< ", date_format(cninfo_placement_of_share, '%Y-%m-%d') as cninfo_placement_of_share"
			<< ", date_format(SouhuTradingDaily, '%Y-%m-%d') as SouhuTradingDaily "
			<< ", date_format(SohuMainIncomeComposition, '%Y-%m-%d') as SohuMainIncomeComposition  "
			<< ", date_format(SohuPerformanceForecast, '%Y-%m-%d') as SohuPerformanceForecast  "
			<< ", date_format(CapitalFlows163, '%Y-%m-%d') as CapitalFlows163  "
			<< ", date_format(_10jqka_bonus, '%Y-%m-%d') as _10jqka_bonus  "
			<< ", date_format(CapitalStructure, '%Y-%m-%d') as CapitalStructure  "
			<< ", date_format(NetEase_Trade_Daily, '%Y-%m-%d') as NetEase_Trade_Daily  "

			<< ", date_format(Balance, '%Y-%m-%d') as Balance  "
			<< ", date_format(IncomeStatement, '%Y-%m-%d') as IncomeStatement  "
			<< ", date_format(CashFlows, '%Y-%m-%d') as CashFlows  "

			<< ", Balance_Zip  "
			<< ", IncomeStatement_Zip  "
			<< ", CashFlows_Zip  "

			<< ", date_format(hexun_research_paper, '%Y-%m-%d') as hexun_research_paper  "

			<< " from stock_data_log " ;
		ss	<< " order by code";

		

		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			CStockDownloadData sdd;
			sdd.code = r.get<int>("code", 0);
			sdd.cninfo_bonus = r.get<string>("cninfo_bonus", "");
			sdd.cninfo_placement_of_share = r.get<string>("cninfo_placement_of_share", "");
			sdd.SouhuTradingDaily = r.get<string>("SouhuTradingDaily", "");
			sdd.SohuMainIncomeComposition = r.get<string>("SohuMainIncomeComposition", "");
			sdd.SohuPerformanceForecast = r.get<string>("SohuPerformanceForecast", "");
			sdd.CapitalFlows163 = r.get<string>("CapitalFlows163", "");
			sdd._10jqka_bonus = r.get<string>("_10jqka_bonus", "");
			sdd.CapitalStructure = r.get<string>("CapitalStructure", "");
			sdd.NetEase_Trade_Daily = r.get<string>("NetEase_Trade_Daily", "");

			sdd.Balance = r.get<string>("Balance", "");
			sdd.CashFlows = r.get<string>("CashFlows", "");
			sdd.IncomeStatement = r.get<string>("IncomeStatement", "");

			sdd.Balance_Zip = r.get<int>("Balance_Zip", 0);
			sdd.CashFlows_Zip = r.get<int>("CashFlows_Zip", 0);
			sdd.IncomeStatement_Zip = r.get<int>("IncomeStatement_Zip", 0);

			sdd.hexun_research_paper = r.get<string>("hexun_research_paper", "");

			char tmp[32] = { 0 };
			sprintf_s(tmp, 32, "%06d", sdd.code);
			m_CStockDownloadDataMap[string(tmp)] = sdd;
			cnt++;
		}

		ss.str("");
		ss << "CStockDataLog::FetchFromDB, 读入了 " << cnt << "条记录";
		LOG4CPLUS_DEBUG(g_logger, ss.str());

		//////////////////////////////////////////////////////////////////////////
		
		m_vecTradedate.clear();

		ss.str("");
		ss << "select  DATE_FORMAT(TradeDate, '%Y-%m-%d') as TradeDate "
			<< " from sohuindexdaily where code = 1  and TradeDate >= \'2014-1-1\' "
			<< " order by TradeDate  ";

		string test = ss.str();

		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			m_vecTradedate.insert(m_vecTradedate.end(), r.get<string>("TradeDate") );

		}

		//////////////////////////////////////////////////////////////////////////
		//	才上市 一两天的股票也不参与数据校验
		m_vecExitStock.clear();

		ss.str("");
		ss << "select code, abbreviation from stockinfo where a_time_to_delisting is not null or a_time_to_suspend is not null";
		ss << " or a_time_to_market >= \'" << m_vecTradedate[m_vecTradedate.size()-3] << "\'";
		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			m_vecExitStock.insert(m_vecExitStock.end(), Int2String( r.get<int>("code", 0 ), "%06d"));
		}
		
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << "CStockDataLog::FetchFromDB, 读入 Error";
		LOG4CPLUS_DEBUG(g_logger, ss.str());
	}
}


CStockDownloadData	 CStockDataLog::SearchStockByCode(int code)
{
	char tmp[32];
	sprintf_s(tmp, 32, "%06d", code);
	JMutexAutoLock lock(m_mutxMap);
	CStockDownloadData ret;

	if(m_CStockDownloadDataMap.count(string(tmp))>0)
	{
		ret = m_CStockDownloadDataMap[string(tmp)];
	}
	
	return ret;
}

void CStockDataLog::UpdateStockByCode(int code, string tableName, string val)
{

	/*{
		stringstream ss;
		ss << "UpdateStockByCode code = " << code << ", tableName = " << tableName << ", val = " << val;
		LOG4CPLUS_DEBUG(g_logger, ss.str());

	}*/

	char tmp[32];
	sprintf_s(tmp, 32, "%06d", code);
	CStockDownloadData  sdd;

	vector<CStockDownloadData>	vecDownload;
	{
		JMutexAutoLock lock(m_mutxMap);

		if(code >= 0)
		{
			//	先更新内存中记录
			if(m_CStockDownloadDataMap.count(string(tmp))>0)
			{
				sdd = m_CStockDownloadDataMap[string(tmp)];
			}
			sdd.isUpdated = true;
			sdd.code = code;

			if(tableName == "cninfo_bonus")
				sdd.cninfo_bonus = val;
			else if(tableName == "cninfo_placement_of_share")
				sdd.cninfo_placement_of_share = val;
			else if(tableName == "SouhuTradingDaily")
				sdd.SouhuTradingDaily = val;
			else if(tableName == "NetEase_Trade_Daily")
				sdd.NetEase_Trade_Daily = val;
			else if(tableName == "SohuMainIncomeComposition")
				sdd.SohuMainIncomeComposition = val;
			else if(tableName == "SohuPerformanceForecast")
				sdd.SohuPerformanceForecast = val;
			else if(tableName == "CapitalFlows163")
				sdd.CapitalFlows163 = val;
			else if(tableName == "_10jqka_bonus")
				sdd._10jqka_bonus = val;
			else if(tableName == "CapitalStructure")
				sdd.CapitalStructure = val;

			else if(tableName == "Balance")
				sdd.Balance = val;
			else if(tableName == "CashFlows")
				sdd.CashFlows = val;
			else if(tableName == "IncomeStatement")
				sdd.IncomeStatement = val;

			else if(tableName == "Balance_Zip")
				sdd.Balance_Zip = atoi(val.c_str());
			else if(tableName == "CashFlows_Zip")
				sdd.CashFlows_Zip = atoi(val.c_str());
			else if(tableName == "IncomeStatement_Zip")
				sdd.IncomeStatement_Zip = atoi(val.c_str());

			else if(tableName == "hexun_research_paper")
				sdd.hexun_research_paper = val;

			m_CStockDownloadDataMap[string(tmp)] = sdd;

			m_UpdatedNum ++;
		}
		

		// code < 0 则立刻同步到数据库
		if(m_UpdatedNum > 100 || code < 0)
		{
			map<string, CStockDownloadData>::iterator it = m_CStockDownloadDataMap.begin();
			for(; it!=m_CStockDownloadDataMap.end(); it++)
			{
				if(it->second.isUpdated)
				{
					it->second.isUpdated = false;
					vecDownload.insert(vecDownload.end(), it->second);
				}
			}
			m_UpdatedNum = 0;

		}
	}

	//////////////////////////////////////////////////////////////////////////
	//	保存到数据库

	if(vecDownload.size()>0)
	{

		stringstream ss;
		session sql(g_MysqlPool);
		for(int i=0; i< vecDownload.size(); i++)
		{
			ss.str("");
			ss << "insert into stock_data_log ("
				<< "code, cninfo_bonus, cninfo_placement_of_share, SouhuTradingDaily, "
				<< "SohuMainIncomeComposition, SohuPerformanceForecast, CapitalFlows163, "
				<< "_10jqka_bonus, CapitalStructure, NetEase_Trade_Daily , Balance, CashFlows, IncomeStatement,"
				<< " Balance_Zip, CashFlows_Zip, IncomeStatement_Zip,"
				<< " hexun_research_paper )"
				<< " select " << Int2String(vecDownload[i].code)  
				<< ",\'" << vecDownload[i].cninfo_bonus << "\' "
				<< ",\'" <<vecDownload[i].cninfo_placement_of_share << "\' "
				<< ",\'" <<vecDownload[i].SouhuTradingDaily << "\' "
				<< ",\'" <<vecDownload[i].SohuMainIncomeComposition << "\' "
				<< ",\'" <<vecDownload[i].SohuPerformanceForecast << "\' "
				<< ",\'" <<vecDownload[i].CapitalFlows163 << "\' "
				<< ",\'" <<vecDownload[i]._10jqka_bonus << "\' "
				<< ",\'" <<vecDownload[i].CapitalStructure << "\' "
				<< ",\'" <<vecDownload[i].NetEase_Trade_Daily << "\' "

				<< ",\'" <<vecDownload[i].Balance << "\' "
				<< ",\'" <<vecDownload[i].CashFlows << "\' "
				<< ",\'" <<vecDownload[i].IncomeStatement << "\' "

				<< "," <<vecDownload[i].Balance_Zip << " "
				<< "," <<vecDownload[i].CashFlows_Zip << " "
				<< "," <<vecDownload[i].IncomeStatement_Zip << " "

				<< ",\'" <<vecDownload[i].hexun_research_paper << "\' "

				<< " from dual where not exists (SELECT 1 from stock_data_log " 
				<< " where code=" << Int2String(vecDownload[i].code) << "  ) ";

			string test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

			ss.str("");
			ss << "update stock_data_log set "
				<< " cninfo_bonus=\'" <<  vecDownload[i].cninfo_bonus << "\'"
				<< ", cninfo_placement_of_share=\'" << vecDownload[i].cninfo_placement_of_share <<"\'"
				<< ", SouhuTradingDaily=\'" << vecDownload[i].SouhuTradingDaily <<"\'"
				<< ", SohuMainIncomeComposition=\'" << vecDownload[i].SohuMainIncomeComposition <<"\'"
				<< ", SohuPerformanceForecast=\'" << vecDownload[i].SohuPerformanceForecast <<"\'"
				<< ", CapitalFlows163=\'" << vecDownload[i].CapitalFlows163 <<"\'"
				<< ", _10jqka_bonus=\'" << vecDownload[i]._10jqka_bonus <<"\'"
				<< ", CapitalStructure=\'" << vecDownload[i].CapitalStructure <<"\'"
				<< ", NetEase_Trade_Daily=\'" << vecDownload[i].NetEase_Trade_Daily <<"\'"

				<< ", Balance=\'" << vecDownload[i].Balance <<"\'"
				<< ", CashFlows=\'" << vecDownload[i].CashFlows <<"\'"
				<< ", IncomeStatement=\'" << vecDownload[i].IncomeStatement <<"\'"

				<< ", Balance_Zip=" << vecDownload[i].Balance_Zip <<" "
				<< ", CashFlows_Zip=" << vecDownload[i].CashFlows_Zip <<" "
				<< ", IncomeStatement_Zip=" << vecDownload[i].IncomeStatement_Zip <<" "

				<< ", hexun_research_paper=\'" << vecDownload[i].hexun_research_paper <<"\'"

				<< " where code=" << Int2String(vecDownload[i].code)  << " "; 

			test = ss.str();
			//test = Gbk2Utf8(test);
			sql << test; 

		}

		ss.str("");
		ss << "CStockDataLog::UpdateStockByCode, 更新了 " << vecDownload.size() << "记录";
		LOG4CPLUS_DEBUG(g_logger, ss.str());

	}

}

void CStockDataLog::UpdateTable(string tablename, string val)
{
	vector<string>	vecSqlStr;
	stringstream ss;
	{
		JMutexAutoLock lock(m_mutxMap);
		m_TableUpdatedMap[tablename] = val;

		map<string, string>::iterator it = m_TableUpdatedMap.begin();
		for(; it!=m_TableUpdatedMap.end(); it++)
		{
			ss.str("");
			ss << "insert into table_update_log ( table_name, update_date )"
				<< " select \'" << it->first << "\'  "   
				<< ",\'" << it->second << "\' "
				<< " from dual where not exists (SELECT 1 from table_update_log " 
				<< " where table_name=\'" << it->first << "\'  ) ";

			vecSqlStr.insert(vecSqlStr.end(), ss.str());

			ss.str("");
			ss << "update table_update_log "
				<< " set update_date =\'" << it->second << "\' "
				<< " where table_name=\'" << it->first << "\' ";

			vecSqlStr.insert(vecSqlStr.end(), ss.str());
		}
	}

	session sql(g_MysqlPool);
	for(int i=0; i<vecSqlStr.size(); i++)
	{
		sql << vecSqlStr[i];
	}
}

string CStockDataLog::SearchTableUpdated(string table_name)
{
	JMutexAutoLock lock(m_mutxMap);
	string ret = "0000-00-00";

	if(m_TableUpdatedMap.count(table_name)>0)
	{
		ret = m_TableUpdatedMap[table_name];
	}
	return ret;
}

string CStockDataLog::MaxDateByTable(string tableName)
{
	string ret = "0000-00-00";
	{
		JMutexAutoLock lock(m_mutxMap);

		map<string, CStockDownloadData>::iterator it = m_CStockDownloadDataMap.begin();
		for(; it!=m_CStockDownloadDataMap.end(); it++)
		{
			CStockDownloadData sdd = it->second;

			string val;
			if(tableName == "cninfo_bonus")
				val = sdd.cninfo_bonus;
			else if(tableName == "cninfo_placement_of_share")
				val = sdd.cninfo_placement_of_share;
			else if(tableName == "SouhuTradingDaily")
				val = sdd.SouhuTradingDaily;
			else if(tableName == "NetEase_Trade_Daily")
				val = sdd.NetEase_Trade_Daily;
			else if(tableName == "SohuMainIncomeComposition")
				val = sdd.SohuMainIncomeComposition;
			else if(tableName == "SohuPerformanceForecast")
				val = sdd.SohuPerformanceForecast;
			else if(tableName == "CapitalFlows163")
				val = sdd.CapitalFlows163;
			else if(tableName == "_10jqka_bonus")
				val = sdd._10jqka_bonus;
			else if(tableName == "CapitalStructure")
				val = sdd.CapitalStructure;

			else if(tableName == "Balance")
				val = sdd.Balance;
			else if(tableName == "CashFlows")
				val = sdd.CashFlows;
			else if(tableName == "IncomeStatement")
				val = sdd.IncomeStatement;

			else if(tableName == "hexun_research_paper")
				val = sdd.hexun_research_paper;

			if(val>ret)
				ret = val;
		}
	}
	return ret;

}

string CStockDataLog::CheckTable(string tableName)
{
	string bRet ;
	JMutexAutoLock lock(m_mutxMap);

	map<string, CStockDownloadData>::iterator it = m_CStockDownloadDataMap.begin();

	for(; it!=m_CStockDownloadDataMap.end(); it++)
	{
		string strCode = it->first;
		CStockDownloadData sdd = it->second;

		if(strCode == "600710")
		{
			int www = 1;
		}

		bool bContinue = true;
		for(int i=0; i< m_vecExitStock.size(); i++)
		{
			if(m_vecExitStock[i]==strCode)
			{
				bContinue = false;
				break;
			}
		}

		if(bContinue)
		{
			string val;
			if(tableName == "SouhuTradingDaily")
				val = sdd.SouhuTradingDaily;
			else if(tableName == "NetEase_Trade_Daily")
				val = sdd.NetEase_Trade_Daily;

			if(val != m_vecTradedate[m_vecTradedate.size()-1])
			{
				if(bRet.empty() == false)
					bRet += ",";
				bRet += strCode; 
			}
		}
	}
	return bRet;
}