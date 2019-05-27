#include "StdAfx.h"
#include "Stock.h"
#include "TechAnalysisEvent.h"
#include "SimulatedAccountMacd.h"
#include "SimulatedAccountAnalysis.h"

CSimulatedAccountAnalysis * CSimulatedAccountAnalysis::instance = NULL;

CSimulatedAccountAnalysis::CSimulatedAccountAnalysis(void)
{
	m_nLateset = -1;
}

CSimulatedAccountAnalysis::~CSimulatedAccountAnalysis(void)
{
}

CSimulatedAccountAnalysis& CSimulatedAccountAnalysis::getInstance()
{
	if(instance==NULL)
		instance = new CSimulatedAccountAnalysis;
	return *instance;
}


void CSimulatedAccountAnalysis::FetchInit()
{

	m_vecCode.clear();
	m_vecPlate.clear();
	m_vecTradedate.clear();
	m_mapSimulatedAccount.clear();
	m_mapPlate.clear();

	stringstream ss;
	try
	{
		session sql(g_MysqlPool);

		row r;
		//////////////////////////////////////////////////////////////////////////
		// 获取当前所有的 股票, 建立股票账户
		ss.str("");
		ss << "select  code, abbreviation, stock_plate, DATE_FORMAT(a_time_to_market, '%Y-%m-%d') as a_time_to_market  "
			<< " from stockinfo   ORDER BY a_time_to_market  ";

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			string strCode = Int2String( r.get<int>("code") ,"%06d" );
			CSimulatedAccount * pAccount = new CSimulatedAccountMacd(strCode);
			if(pAccount)
			{
				pAccount->m_time_to_market = r.get<string>("a_time_to_market", "");
				m_mapSimulatedAccount[strCode] = pAccount;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//	获取股票账户 关联的板块
		ss.str("");
		ss << "select Code, Name, Company_Number, Total_Volume, Total_Turnover, Parent_Code, Is_Stock"
			<< " from SouhuStockPlates order by Is_Stock, Parent_Code, Code " ;	/*order by Is_Stock, Parent_Code, Code*/

		string test = ss.str();

		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			string strCode  = Int2String(r.get<int>("Code"), "%06d");
			string strName = /*Utf8_GBK*/(r.get<string>("Name", ""));
			string strParentCode = Int2String(r.get<int>("Parent_Code"),  "%06d");
			int	isStock = r.get<int>("Is_Stock");

			if(isStock && m_mapSimulatedAccount.count(strCode)>0)
			{
				//	记录该股所属的 所有板块
				m_mapSimulatedAccount[strCode]->vecParent.push_back(strParentCode);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//	建立 模拟账户操作明细表
		ss.str("");
		ss << "create table if not exists simulated_account ( code varchar(32), tradedate date, "
			<< " capital double, shares int, cost double, loss_earning double, action varchar(64), remark varchar(256),  "
			<< "desc_dmi varchar(32), desc_macd varchar(32), desc_kdj varchar(32),  desc_k varchar(32),"
			<< "trend_boll varchar(32), obv_accum double, trend_flag varchar(32),  serial_num int,"
			<< "PRIMARY KEY ( code, tradedate, serial_num ) ) ";
		sql << ss.str();

		ss.str("");
		ss << " create TEMPORARY table tmp_table  as "
			<< " select a.code, a.tradedate, "
			<< " (select max(serial_num) from simulated_account b where a.code=b.code and a.tradedate=b.tradedate) as serial_num "
			<< " from ( select code, max(tradedate) as tradedate  "
			<< " from simulated_account group by  code, tradedate ) a ";
		test = ss.str();
		sql << ss.str();

		//	获取每个账户最后的状态
		ss.str("");
		ss << " select a.code, DATE_FORMAT(a.tradedate, '%Y-%m-%d') as tradedate, a.serial_num,  "
			<< " (select trend_flag from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as trend_flag, "
			<< " (select trend_boll from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as trend_boll, "
			<< " (select obv_accum from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as obv_accum, "

			<< " (select cost from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as cost , "
			<< " (select loss_earning from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as loss_earning , "

			<< " (select capital from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as capital, "
			<< " (select shares from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as shares, "
			<< " (select desc_dmi from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as desc_dmi, "
			<< " (select desc_kdj from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as desc_kdj, "
			<< " (select desc_k from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as desc_k, "
			<< " (select desc_macd from simulated_account b where a.code=b.code and a.tradedate=b.tradedate "
			<< " and a.serial_num=b.serial_num ) as desc_macd "
			
			<< " from tmp_table a  ";
		
		string beginTradedate = "2016-01-01";
		
		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			string strCode = r.get<string>("code", "");
			string strTradedate = r.get<string>("tradedate", "");

			CSimulatedAccountTradeDetail satd;
			satd.trend_flag = r.get<string>("trend_flag", "");
			satd.trend_boll = r.get<string>("trend_boll", "");
			//satd.obv_accum = r.get<double>("obv_accum", 0);

			satd.dbCost = r.get<double>("cost", 0);
			satd.dbLossEarning = r.get<double>("loss_earning", 0);

			satd.dbCapital = r.get<double>("capital", 0);
			satd.nShares = r.get<int>("shares", 0);
			satd.desc_dmi = r.get<string>("desc_dmi", "");
			satd.desc_kdj = r.get<string>("desc_kdj", "");
			satd.desc_k = r.get<string>("desc_k", "");
			satd.desc_macd = r.get<string>("desc_macd", "");

			if(m_mapSimulatedAccount.count(strCode) > 0)
				m_mapSimulatedAccount[strCode]->SetLastStatus(satd);

			if(beginTradedate < strTradedate)
				beginTradedate = strTradedate;

		}

		sql << "drop table tmp_table";
		//////////////////////////////////////////////////////////////////////////

		ss.str("");
		ss << "select  DATE_FORMAT(TradeDate, '%Y-%m-%d') as TradeDate "
			<< " from sohuindexdaily where code = 1  and TradeDate > \'"    // HuShen_GetTradeDate(
			<< beginTradedate 
			<<"\' order by TradeDate  ";
			
		test = ss.str();

		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			m_vecTradedate.insert(m_vecTradedate.end(), r.get<string>("TradeDate") );

		}

	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

int CSimulatedAccountAnalysis::FetchNext()
{
	stringstream ss;
	
	int nTradedate = m_nLateset + 1;
	if(nTradedate>= m_vecTradedate.size())
		return -1;

	int nEvent = -1;
	try
	{
		session sql(g_MysqlPool);
		row r;

		//////////////////////////////////////////////////////////////////////////
		//	获取该日 所有板块的信息
		m_mapPlate.clear();

		ss.str("");
		ss << "select code, name, circulating_value, repair_value, division, point, ema12, ema26, dif, dea ,remark, "
			<< " dif/point as ranking  "
			<< " from  daily_hushen_plate where tradedate = \'" << m_vecTradedate[nTradedate] << "\'"
			<< " order by ranking desc ";

		string test = ss.str();

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int ranking = 1;
		while (st.fetch())
		{
			int nCode = r.get<int>("code");
			string strCode = Int2String(nCode, "%06d");
			m_mapPlate[strCode].tradedate = m_vecTradedate[nTradedate];
			m_mapPlate[strCode].name = r.get<string>("name", "");

			m_mapPlate[strCode].circulating_value = r.get<double>("circulating_value", 0);
			m_mapPlate[strCode].repair_value = r.get<double>("repair_value", 0);
			m_mapPlate[strCode].division = r.get<double>("division", 0);
			m_mapPlate[strCode].point = r.get<double>("point", 0);

			m_mapPlate[strCode].ema12 = r.get<double>("ema12", 0);
			m_mapPlate[strCode].ema26 = r.get<double>("ema26", 0);
			m_mapPlate[strCode].dif = r.get<double>("dif", 0);
			m_mapPlate[strCode].dea = r.get<double>("dea", 0);

			m_mapPlate[strCode].remark = r.get<string>("remark", "");
			m_mapPlate[strCode].ranking = ranking;
			ranking++;
		}
		
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			
			<< " (select CONCAT(standard,'|',up,'|',down,'|',middle, '|', remark) from daily_bollinger b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) boll, "

			<< " (select CONCAT(index_value1,'|',index_value2,'|',index_value3 ) from daily_rsi b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) rsi, "

			<< " (select CONCAT(ema12,'|',ema26,'|',dif,'|',dea, '|', remark) from daily_macd b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) macd, "

			<< " (select CONCAT(k_value,'|',d_value,'|',j_value,'|',remark ) from daily_kdj b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) kdj, "

			<< " (select CONCAT(positive_di,'|',negative_di,'|',ema_dx, '|', remark ) from daily_dmi b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) dmi, "

			<< " (select remark  from daily_candlesticks b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) candlesticks, "

			<< " (select remark  from daily_obv b "
			<< " where a.tradedate = b.tradedate and a.code = b.code ) obv, "

			<< " (select concat(circulating_value, '|', change_rate, '|', close, '|', lclose) "
			<< " from netease_trade_daily b where b.Code = a.code and b.TradeDate = a.tradedate ) as circulating_value, "

			<< " (select CONCAT(max(bonus_share),'|',max(bonus_cash), '|', max(reserve_to_common_share))"
			<< " from cninfo_bonus b where b.code = a.Code and b.exit_right_date =a.tradedate ) as bonus_share, "

			<< " (select CONCAT(static_pe,'|',dynamic_pe,'|', pb,'|', dividend_yield_ratio, '|', "
			<< " (select dynamic_pe from csipedaily c where c.code=b.parent_code and c.is_stock=0 and c.TradeDate=b.TradeDate) )"
			<< " from csipedaily b where b.code = a.code "
			<< " and  b.is_stock = 1 and b.TradeDate = a.tradedate ) as pe,"

			<< " index_value1, index_value2, index_value3, index_value4 ,remark "
			<< " from daily_ma a where 1=1 ";
		ss << " and tradedate = \'" << m_vecTradedate[nTradedate] << "\'";

		test = ss.str();

		st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		nEvent = 0;
		while (st.fetch())
		{
			CTechAnalysisEventData  bbd;
			bbd.tradedate = r.get<string>("tradedate");
			bbd.ma5 =  r.get<double>("index_value1");
			bbd.ma10 = r.get<double>("index_value2");
			bbd.ma20 = r.get<double>("index_value3", 0);
			bbd.ma60 = r.get<double>("index_value4", 0);
			bbd.ma = r.get<string>("remark");

			string neteaseStr = r.get<string>("circulating_value", "");
			if(neteaseStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(neteaseStr, "|", vecYield);

				bbd.circulating_value = String2Double(vecYield[0]);
				bbd.change_rate = String2Double(vecYield[1]);
				bbd.close = String2Double(vecYield[2]);
				bbd.lclose = String2Double(vecYield[3]);
			}
			else
			{
				//	已退市、暂停上市、或者还未上市
				bbd.circulating_value = 0;
				bbd.change_rate = 0;
				bbd.close = -1;
				bbd.lclose = -1;
			}

			string bollStr = r.get<string>("boll", "");
			if(bollStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(bollStr, "|", vecYield);

				bbd.standard = String2Double(vecYield[0]);
				bbd.up = String2Double(vecYield[1]);
				bbd.down =  String2Double(vecYield[2]);
				bbd.middle =  String2Double(vecYield[3]);
				bbd.boll =  Utf8_GBK(vecYield[4]);
			}
			
			string rsiStr = r.get<string>("rsi", "");
			if(rsiStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(rsiStr, "|", vecYield);

				bbd.rsi1 = String2Double(vecYield[0]);
				bbd.rsi2 = String2Double(vecYield[1]);
				bbd.rsi3 =  String2Double(vecYield[2]);
			}

			string macdStr = r.get<string>("macd", "");
			if(macdStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(macdStr, "|", vecYield);

				bbd.ema12 = String2Double(vecYield[0]);
				bbd.ema26 = String2Double(vecYield[1]);
				bbd.dif =  String2Double(vecYield[2]);
				bbd.dea =  String2Double(vecYield[3]);
				bbd.macd =  Utf8_GBK(vecYield[4]);

			}

			string kdjStr = r.get<string>("kdj", "");
			if(kdjStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(kdjStr, "|", vecYield);

				bbd.k_value = String2Double(vecYield[0]);
				bbd.d_value = String2Double(vecYield[1]);
				bbd.j_value =  String2Double(vecYield[2]);
				bbd.kdj =  Utf8_GBK(vecYield[3]);
			}

			string dmiStr = r.get<string>("dmi", "");
			if(dmiStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(dmiStr, "|", vecYield);

				bbd.positive_di = String2Double(vecYield[0]);
				bbd.negative_di = String2Double(vecYield[1]);
				bbd.ema_dx =  String2Double(vecYield[2]);
				bbd.dmi =  Utf8_GBK(vecYield[3]);
			}

			string peStr = r.get<string>("pe", "");
			if(peStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(peStr, "|", vecYield);

				bbd.static_pe = String2Double(vecYield[0]);
				bbd.dynamic_pe = String2Double(vecYield[1]);
				bbd.pb =  String2Double(vecYield[2]);
				bbd.dividend_yield_ratio =  String2Double(vecYield[3]);
				bbd.parent_dynamic_pe =  String2Double(vecYield[4]);
			}

			bbd.candlesticks = r.get<string>("candlesticks", "");
			bbd.obv = r.get<string>("obv", "");

			string bonusStr = r.get<string>("bonus_share", "");
			if(bonusStr.empty()==false)
			{
				vector<string>	vecYield;
				splitString(bonusStr, "|", vecYield);

				bbd.bonus_share = String2Double(vecYield[0]);
				bbd.bonus_cash = String2Double(vecYield[1]);
				bbd.reserve_to_common_share =  String2Double(vecYield[2]);
			}

			string strCode = r.get<string>("code", "");
			if(m_mapSimulatedAccount.count(strCode) > 0)
				m_mapSimulatedAccount[strCode]->PostEvent(bbd);

			nEvent++;
		}

		m_nLateset = nTradedate;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
	return nEvent;
}

void CSimulatedAccountAnalysis::SaveDetail(string strTradedate)
{
	stringstream ss;
	
	vector<string>	vecSqlStr;
	map<string, CSimulatedAccount *>::iterator it = m_mapSimulatedAccount.begin();
	for(; it!=m_mapSimulatedAccount.end(); it++)
	{
		CSimulatedAccount * pAccount = it->second;
		if(pAccount && pAccount->m_vec_detail.size()> 0 )
		{
			vector<CSimulatedAccountTradeDetail> vec = pAccount->m_vec_detail;

			for(int k = 0; k< vec.size(); k++)
			{
				ss.str("");
				ss << "(" ;
				ss << "\'" << pAccount->m_strCode << "\'";
				ss << ",\'" << strTradedate << "\'";
				ss << "," << (k + 1);
				ss << "," << Double2String(0.0) ;
				ss << ",\'" << vec[k].trend_flag << "\'";
				ss << ",\'" << vec[k].trend_boll << "\'";
				ss << "," << Double2String(vec[k].dbCapital);
				ss << "," << vec[k].nShares;
				ss << "," << Double2String(vec[k].dbCost);
				ss << "," << Double2String(vec[k].dbLossEarning);
				ss << ",\'" << vec[k].action << "\'";
				ss << ",\'" << vec[k].remark << "\'";
				ss << ",\'" << vec[k].desc_ma << "\'";
				ss << ",\'" << vec[k].desc_kdj << "\'";
				ss << ",\'" << vec[k].desc_k << "\'";
				ss << ",\'" << vec[k].desc_macd << "\' )";
				
				vecSqlStr.insert(vecSqlStr.end(), ss.str());
			}
		}
		pAccount->m_vec_detail.clear();
	}

	if(vecSqlStr.size()>0)
	{
		ss.str("");
		ss << "insert simulated_account ( code, tradedate, serial_num, obv_accum, trend_flag, trend_boll, capital, shares," 
			<< "cost, loss_earning, action, remark, desc_dmi, desc_kdj, desc_k, desc_macd ) values ";
		for(int i=0; i< vecSqlStr.size(); i++)
		{
			ss << vecSqlStr[i];
			if( (i+1)!=vecSqlStr.size())
				ss << ",";
		}

		string test  = ss.str();
		session sql(g_MysqlPool);
		sql << ss.str();
	}
}

DWORD WINAPI CSimulatedAccountAnalysis::CommandHandler1(void *data)
{
	CSimulatedAccountAnalysis * pThis = (CSimulatedAccountAnalysis *)data;

	pThis->CommandHandler();
	return 0 ;
}


DWORD CSimulatedAccountAnalysis::CommandHandler()
{
	
	unsigned long ticks =  GetTickCount();

		
	//	生成所有的模拟账户, 确定演算的时间范围
	FetchInit();
	
	//	从数据库中读入指标事件，并分发到各个 账户
	int nEvent ;
	int nDay = 0;
	while((nEvent =FetchNext())>=0)
	{
		if(nEvent==0)
			continue;
		string strTradedate =  m_vecTradedate[m_nLateset];

		LOG4CPLUS_DEBUG(g_logger, (strTradedate + " ------ 开始模拟交易分析 ------"));

		//	根据当日计算结果，决定股票的买卖

		map<string, CSimulatedAccount *>::iterator it = m_mapSimulatedAccount.begin();
		for(; it!=m_mapSimulatedAccount.end(); it++)
		{
			CSimulatedAccount * pAccount = it->second;
			pAccount->ProccessEvent(strTradedate, m_mapPlate);
		}

		//	保存处理的结果
		SaveDetail(strTradedate);

		nDay++;
		Sleep(10);
	}

	if(nDay==0)
	{
		LOG4CPLUS_DEBUG(g_logger, "------ 模拟交易分析：无新数据 ------");

		map<string, CSimulatedAccount *>::iterator it = m_mapSimulatedAccount.begin();
		for(; it!=m_mapSimulatedAccount.end(); it++)
		{
			CSimulatedAccount * pAccount = it->second;
			delete pAccount;
		}
		m_mapSimulatedAccount.clear();

		return 0;
	}
	
	//  统计账户信息，并删除账户
	int nLost = 0;
	int nEarning = 0;
	double dbLost = 0;
	double dbEarning = 0;

	map<string, CSimulatedAccount *>::iterator it = m_mapSimulatedAccount.begin();
	for(; it!=m_mapSimulatedAccount.end(); it++)
	{
		CSimulatedAccount * pAccount = it->second;
		double total = pAccount->m_status.dbCapital;
		if(pAccount->m_status.nShares>0)
		{
			if(pAccount->m_Event.close>0)
				total += pAccount->m_status.nShares * pAccount->m_Event.close;
			else
				total += pAccount->m_status.nShares * pAccount->m_Event.lclose;
		}
		
		/*if(pAccount->m_status.nShares==0)*/
		{
			if(total > 10 * 10000)
			{
				dbEarning += (total - 10 *10000);
				nEarning++;
			}
			else if(total < 10 * 10000)
			{
				dbLost += (10 * 10000 - total);
				nLost ++;
			}
		}
		delete pAccount;
	}
	m_mapSimulatedAccount.clear();

	stringstream ss;
	ss << "账户汇总， Lost = " << nLost << ", Earning = " << nEarning;
	ss << ", 金额， Lost = " << Double2String(dbLost) << ", Earning = " << Double2String(dbEarning);
	LOG4CPLUS_DEBUG(g_logger, ss.str());

	ticks = GetTickCount() - ticks;
	return 0;
}
