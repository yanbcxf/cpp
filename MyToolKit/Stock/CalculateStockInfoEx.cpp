#include "StdAfx.h"
#include "Stock.h"
#include "CalculateStockInfoEx.h"

CCalculateStockInfoEx::CCalculateStockInfoEx(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalculateStockInfoEx::~CCalculateStockInfoEx(void)
{
}



string CCalculateStockInfoEx::SqlScript()
{
	stringstream ss;

	ss << " create table if not exists StockInfoEx ( code int, " 
		<< " calculate_date date, a_total_shares DECIMAL(15,4) ,a_total_value DECIMAL(15,4), "
		<< " earnings_per_share DECIMAL(15,4), pe DECIMAL(15,4), close DECIMAL(15,4), abbreviation varchar(256),"
		<< " province varchar(256), belong_to_industry varchar(256), stock_plate  varchar(256), "
		<< " PRIMARY KEY(code))";

	return ss.str();
}

void CCalculateStockInfoEx::ImportToDatabase1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->ImportToDatabase();
}

//该代码已经不用
void CCalculateStockInfoEx::RecentStockSummary1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->RecentStockSummary();
}

void CCalculateStockInfoEx::RecentMainIncomeComposition1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->RecentMainIncomeComposition();
}

void CCalculateStockInfoEx::RecentLiquidityRatio1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->RecentLiquidityRatio();
}

void CCalculateStockInfoEx::RecentNetProfitsRatio1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->RecentNetProfitsRatio();
}


void CCalculateStockInfoEx::RecentBalanceAndIncomestatement1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->RecentBalanceAndIncomestatement();
}


void CCalculateStockInfoEx::RecentFundHold1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->RecentFundHold();
}

void CCalculateStockInfoEx::ZombieStockList1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->ZombieStockList();
}


void CCalculateStockInfoEx::MainProductQueryList1(void* firstArg)
{
	CCalculateStockInfoEx * sink = (CCalculateStockInfoEx *)firstArg;
	sink->MainProductQueryList();
}


void CCalculateStockInfoEx::MainProductQueryList()
{
	stringstream ss;
	if(m_main_product_query.empty())
		return ;

	m_vec_code.clear();
	m_vec_report_date.clear();
	m_vec_abbreviation.clear();
	m_vec_composition_name.clear();
	m_vec_income.clear();

	try{

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		
		ss << "select a.code, ( select d.abbreviation  from stockinfo d where d.code=a.code) as abbreviation, "
			<< "a.report_date, a.composition_name , a.income * 10000 as income "
			<< " from sohumainincomecomposition a , "
			<< "( SELECT code , composition_name, max(report_date) as max_report_date "
			<< "FROM sohumainincomecomposition "
			<< " where composition_type='主营产品构成（万）' and composition_name like '%" 
			<< m_main_product_query
			<< "%' group by code, composition_name ) b "
			<< " where a.code = b.code and a.composition_name = b.composition_name "
			<< "and a.report_date = b.max_report_date and a.composition_type = '主营产品构成（万）' order by a.code";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("code");
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_abbreviation[cnt] = /*Utf8_GBK*/( r.get<string>("abbreviation"));
			m_vec_income[cnt] = r.get<double>("income");
			m_vec_composition_name[cnt] = /*Utf8_GBK*/(r.get<string>("composition_name"));

			cnt++;
		}
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

}

void CCalculateStockInfoEx::ZombieStockList()
{
	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;
	
	m_vec_code.clear();
	m_vec_report_date.clear();
	m_vec_registered_capital.clear();
	m_vec_total_owners_equity_belongs_to_parent_company.clear();
	m_vec_undistributed_profits.clear();
	m_vec_total_liability.clear();
	m_vec_total_assets.clear();
	m_vec_intangibel_assets.clear();
	m_vec_goodwill.clear();
	m_vec_net_profit_belonging_to_parent_company.clear();
	m_vec_abbreviation.clear();

	try{

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		/*	从资产负债表、利润表读入各种字段，以便生成各种比率	*/
		ss << "select a.code as code, a.report_date as report_date,  "
			<< " ( select d.abbreviation  from stockinfo d where d.code=a.code) as abbreviation, "
			<< " a.colume_99  as registered_capital, "		//	实收资本
			<< " a.colume_109 as total_owners_equity_belongs_to_parent_company, "						//	归属于母公司股东权益合计
			<< " a.colume_106 as undistributed_profits, "	//	未分配利润
			<< " a.colume_97  as total_liability, "			//	负债合计
			<< " a.colume_54  as total_assets, "			//	资产总计
			<< " a.colume_46  as intangibel_assets, "		//	无形资产
			<< " a.colume_48  as goodwill, "				//	商誉
			<< " b.colume_41  as net_profit_belonging_to_parent_company, "	//	归属母公司利润
			<< " a.colume_106/a.colume_99 as per_undistributed "
			<< " from balance a,  incomestatement b "
			<< " where a.code=b.code and a.report_date=b.report_date "
			<< " and  a.report_date= ( select max(c.report_date) from balance c where c.code=a.code ) "
			<< " and  a.code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< " order by per_undistributed ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("code");
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_abbreviation[cnt] = /*Utf8_GBK*/( r.get<string>("abbreviation"));
			m_vec_registered_capital[cnt] = r.get<double>("registered_capital");
			m_vec_undistributed_profits[cnt] = r.get<double>("undistributed_profits");
			m_vec_total_owners_equity_belongs_to_parent_company[cnt] = r.get<double>("total_owners_equity_belongs_to_parent_company");
			m_vec_total_liability[cnt] = r.get<double>("total_liability");
			m_vec_total_assets[cnt] = r.get<double>("total_assets");
			m_vec_intangibel_assets[cnt] = r.get<double>("intangibel_assets");
			m_vec_goodwill[cnt] = r.get<double>("goodwill");
			m_vec_net_profit_belonging_to_parent_company[cnt] = r.get<double>("net_profit_belonging_to_parent_company");

			cnt++;
		}
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}

}

void CCalculateStockInfoEx::RecentFundHold()
{
	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_report_date.clear();
	m_vec_number_fund_hold.clear();
	m_vec_number_AShares.clear();
	m_vec_number_A_total_shares.clear();
	
	try
	{
		ss.str("");
		/*	获取截止每个季度的基金持有量	*/
		ss << "SELECT code, report_date , sum(number_hold) as sum_number_hold  "
			<< " FROM sohufundhold   "
			<< " where  code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< "  GROUP BY code, report_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("code");
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_number_fund_hold[cnt] = r.get<double>("sum_number_hold");
			cnt++;
		}

		//	获取截止每个季度的A股流通股数及总股数

		for(int m=0; m<m_vec_code.size(); m++)
		{
			ss.str("");
			ss << "SELECT code , real_date, AShares, A_total_shares  "
				<< " FROM capitalstructure  where code = " << m_vec_code[m]
				<< " and real_date= "
				<< " (select max(real_date) from capitalstructure  where code = " << m_vec_code[m]
				<< " and real_date<=\"" <<  m_vec_report_date[m] << "\")";

			test = ss.str();
			//test = Gbk2Utf8(test);

			statement st = (sql.prepare << test,into(r)) ;
			st.execute();

			while (st.fetch())
			{
				m_vec_number_AShares[m] = r.get<double>("AShares") * 10000;
				m_vec_number_A_total_shares[m] = r.get<double>("A_total_shares") * 10000;
			}

		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

void CCalculateStockInfoEx::RecentBalanceAndIncomestatement()
{
	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_report_date.clear();
	m_vec_inventories.clear();
	m_vec_total_current_assets.clear();
	m_vec_total_non_current_assets.clear();
	m_vec_total_current_liabilities.clear();
	m_vec_total_non_current_liabilities.clear();
	m_vec_fixed_assets_net_value.clear();

	m_vec_sales_from_operations.clear();
	m_vec_cost_of_operations.clear();
	m_vec_net_profit_belonging_to_parent_company.clear();

	try
	{
		ss.str("");
		/*	从资产负债表、利润表读入各种字段，以便生成各种比率	*/
		ss << "select a.code as code, a.report_date as report_date , a.colume_21 as inventories, "
			<< " a.colume_26 as total_current_assets, "
			<< " a.colume_53 as total_non_current_assets, "
			<< " a.colume_87 as total_current_liabilities, "
			<< " a.colume_96 as total_non_current_liabilities, "
			<< " a.colume_39 as fixed_assets_net_value, "
			<< " b.colume_2  as sales_from_operations, "
			<< " b.colume_9  as cost_of_operations, "
			<< " b.colume_41 as net_profit_belonging_to_parent_company, "
			<< " ( select d.abbreviation  from stockinfo d where d.code=a.code) as abbreviation "
			<< " from balance a,  incomestatement b "
			<< " where a.code=b.code and a.report_date=b.report_date "
			<< " and  a.code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< " order by a.code, a.report_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("code");
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_inventories[cnt] = r.get<double>("inventories");
			m_vec_abbreviation[cnt] = /*Utf8_GBK*/(r.get<string>("abbreviation"));
			m_vec_total_current_assets[cnt] = r.get<double>("total_current_assets");
			m_vec_total_non_current_assets[cnt] = r.get<double>("total_non_current_assets");
			m_vec_total_current_liabilities[cnt] = r.get<double>("total_current_liabilities");
			m_vec_total_non_current_liabilities[cnt] = r.get<double>("total_non_current_liabilities");
			m_vec_sales_from_operations[cnt] = r.get<double>("sales_from_operations");
			m_vec_cost_of_operations[cnt] = r.get<double>("cost_of_operations");
			m_vec_net_profit_belonging_to_parent_company[cnt] = r.get<double>("net_profit_belonging_to_parent_company");
			m_vec_fixed_assets_net_value[cnt] = r.get<double>("fixed_assets_net_value");
			cnt++;
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

void CCalculateStockInfoEx::RecentNetProfitsRatio()
{
	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_report_date.clear();
	m_vec_net_profits_ratio.clear();

	try
	{
		ss.str("");
		/*	归属母公司净利润/(总资产 - 流动负债)	*/
		ss << "select code, report_date, colume_109/(colume_54 - colume_87) as net_profits_ratio ,"
			<< " ( select d.abbreviation  from stockinfo d where d.code=code) as abbreviation "
			<< " from balance "
			<< " where code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< " order by code, report_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("code");
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_net_profits_ratio[cnt] = r.get<double>("net_profits_ratio");

			m_vec_abbreviation[cnt] = /*Utf8_GBK*/(r.get<string>("abbreviation"));

			cnt++;
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

void CCalculateStockInfoEx::RecentLiquidityRatio()
{
	stringstream ss;
	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_report_date.clear();
	m_vec_liquidity_ratio.clear();

	try
	{
		ss.str("");
		ss << "select code, report_date, colume_26/colume_87 as liquidity, "
			<< " ( select d.abbreviation  from stockinfo d where d.code=a.code) as abbreviation "
			<< " from balance a "
			<< " where code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< " order by code, report_date ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{

			try
			{
				r.get<int>("code");
				r.get<std::tm>("report_date");
				r.get<double>("liquidity");
				r.get<string>("abbreviation");
			}
			catch(...)
			{
				continue;
			}

			m_vec_code[cnt] = r.get<int>("code");
			{
				std::tm  tm = r.get<std::tm>("report_date");
				ss.str("");
				ss << (tm.tm_year + 1900) << "-" << (tm.tm_mon + 1) << "-" << tm.tm_mday ;
				m_vec_report_date[cnt] = ss.str();
			}
			m_vec_liquidity_ratio[cnt] = r.get<double>("liquidity");
			m_vec_abbreviation[cnt] = /*Utf8_GBK*/(r.get<string>("abbreviation"));
			
			cnt++;
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

void CCalculateStockInfoEx::RecentMainIncomeComposition()
{
	stringstream ss;
	time_t tradingDate;
	string strTradingDate = "2015-06-30";

	if(m_vec_request_code.size()<=0)
		return;

	m_vec_code.clear();
	m_vec_composition_name.clear();
	m_vec_cost.clear();
	m_vec_income.clear();
	m_vec_gross_profit_rate.clear();

	try
	{
		ss.str("");
		ss << "select Code, report_date, composition_name, income, cost, gross_profit_rate "
			<< " from SohuMainIncomeComposition where report_date=\"" << strTradingDate  << "\""
			<< " and composition_type=\"主营产品构成（万）\""
			<< " and Code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< " order by Code";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{

			try
			{
				r.get<int>("Code");
				r.get<string>("composition_name");
				r.get<double>("income");
				r.get<double>("cost");
				r.get<double>("gross_profit_rate");
			}
			catch(...)
			{
				continue;
			}

			m_vec_code[cnt] = r.get<int>("Code");
			m_vec_composition_name[cnt] = r.get<string>("composition_name");
			m_vec_income[cnt] = r.get<double>("income");
			m_vec_cost[cnt] = r.get<double>("cost");
			m_vec_gross_profit_rate[cnt] = r.get<double>("gross_profit_rate");
			
			cnt++;
		}
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

//该代码已经不用
void CCalculateStockInfoEx::RecentStockSummary()
{
	stringstream ss;
	time_t tradingDate;
	string strTradingDate;

	if(m_vec_request_code.size()<=0)
		return;

	m_vec_StockSummary.clear();

	m_vec_code.clear();
	m_vec_open.clear();
	m_vec_high.clear();
	m_vec_low.clear();
	m_vec_close.clear();
	m_vec_change.clear();
	m_vec_change_rate.clear();
	m_vec_turnover.clear();
	m_vec_turnover_rate.clear();
	m_vec_pe.clear();
	m_vec_a_total_shares.clear();

	try
	{

		ss.str("");
		ss << "任务【" << m_nTaskID << "】";
		ss << "开始执行......";
		sendToOutput(ss.str().c_str(), NULL, m_nLogType);
		//////////////////////////////////////////////////////////////////////////
		int k=0;
		for(; k<15; k++)
		{
			tradingDate = CBussinessUtils::CurrentTradingDate(k);
			//struct tm *ret = gmtime (&tradingDate);
			struct tm ret;
			localtime_s(&ret, &tradingDate);
			char tmp[256] = { 0 };
			snprintf (tmp, 255, "%d-%02d-%02d", 1900 + ret.tm_year, ret.tm_mon+1, ret.tm_mday);	
			strTradingDate = string(tmp);

			ss.str("");
			ss << "select count(*) as cnt "
				<< " from netease_trade_daily where TradeDate=\"" << strTradingDate
				<< "\" ";

			session sql(g_MysqlPool);
			long long nRowNum = 0LL;
			sql << ss.str(), into(nRowNum);
			if(nRowNum>0)
				break;
		}
		if(k==15)
		{
			// 未找到 10 个交易日内数据，任务取消
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "未找到 15 个交易日内数据，任务取消";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);
			return ;
		}

		ss.str("");
		ss << "select Code, TradeDate, Open, High, Low, Close, Volume, Change_Amount,"
			<< " Change_Rate, Turnover, Turnover_Rate "
			<< " from netease_trade_daily where TradeDate=\"" << strTradingDate  << "\""
			<< " and Code in (";
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			ss << m_vec_request_code[m];
			if(m!=m_vec_request_code.size()-1)
				ss << ",";
		}
		ss  << ")";
		ss	<< " order by Code";

		string test = ss.str();

		row r;
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			m_vec_code[cnt] = r.get<int>("Code");
			m_vec_open[cnt] = r.get<double>("Open");
			m_vec_high[cnt] = r.get<double>("High");
			m_vec_low[cnt] = r.get<double>("Low");
			m_vec_close[cnt] = r.get<double>("Close");
			m_vec_volume[cnt] = r.get<int>("Volume");
			m_vec_change[cnt] = r.get<double>("Change_Amount");
			m_vec_change_rate[cnt] = r.get<double>("Change_Rate");
			m_vec_turnover[cnt] = r.get<double>("Turnover");
			m_vec_turnover_rate[cnt] = r.get<double>("Turnover_Rate");

			cnt++;
		}

		if(cnt>0)
		{
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "开始股票概况的汇总";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);

			for(int k=0; k<m_vec_code.size(); k++)
			{
				CStockInfoSummary spd;
				spd.code = m_vec_code[k];
				spd.change_rate = m_vec_change_rate[k];
				spd.turnover_rate = m_vec_turnover_rate[k];
				spd.turnover = m_vec_turnover[k];
				spd.volume = m_vec_volume[k];

				spd.estimate_profit = CBussinessUtils::EstimateYearProfits(spd.code);
				spd.a_total_share = CBussinessUtils::CurrentATotalShares(spd.code);
				if(spd.a_total_share>0)
				{
					spd.earning_per_share = spd.estimate_profit/(spd.a_total_share * 10000.0);
				}
				else
				{
					spd.earning_per_share = -1;
				}

				spd.close = m_vec_close[k]; /*CBussinessUtils::CurrentClosePrice(spd.code);*/
				if(spd.close>0 && spd.earning_per_share>0)
				{
					spd.pe = spd.close/spd.earning_per_share;
				}
				else
				{
					spd.pe = -1;
				}

				double calculateShares = CBussinessUtils::CurrentACalculateshares(spd.code);
				if(spd.close>0 && calculateShares>0)
				{
					spd.a_total_value = spd.close * calculateShares / 10000.0;

				}
				else
					spd.a_total_value = -1;

				m_vec_StockSummary.push_back(spd);
			}
		}
		else
		{
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "未找到股票的每日交易数据";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);
			return ;
		}

		if(m_vec_StockSummary.size()>0)
		{
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "开始股票资金流向的汇总";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);

			for(int k=0; k< m_vec_StockSummary.size(); k++)
			{

				ss.str("");
				ss << "select Code, TradeDate, Close, Change_Rate, Turnover_Rate, FlowIn, FlowOut, NetFlow,"
					<< " MainFlowIn, MainFlowOut, NetMainFlow "
					<< " from CapitalFlows163 where Code=" << m_vec_StockSummary[k].code
					<< " and TradeDate=\"" << strTradingDate  << "\"";

				session sql(g_MysqlPool);
				row r;
				statement st = (sql.prepare << ss.str(),into(r)) ;
				st.execute();

				int cnt = 0;
				while (st.fetch())
				{
					m_vec_StockSummary[k].MainFlowIn = r.get<double>("MainFlowIn");
					m_vec_StockSummary[k].MainFlowOut = r.get<double>("MainFlowOut");
					double main_turnover = m_vec_StockSummary[k].MainFlowIn + m_vec_StockSummary[k].MainFlowOut;
					
					m_vec_StockSummary[k].main_turnover_rate = (main_turnover/m_vec_StockSummary[k].turnover);
					m_vec_StockSummary[k].main_turnover_rate *= m_vec_StockSummary[k].turnover_rate;
					cnt++;
				}

			}
		}


		if(m_vec_StockSummary.size()>0)
		{
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "开始业绩预告的汇总";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);

			for(int k=0; k< m_vec_StockSummary.size(); k++)
			{

				ss.str("");
				ss << "select Code, report_date, report_type, change_rate "
					<< " from SohuPerformanceForecast where Code=" << m_vec_StockSummary[k].code
					<< " order by report_date desc";

				session sql(g_MysqlPool);
				row r;
				statement st = (sql.prepare << ss.str(),into(r)) ;
				st.execute();

				while (st.fetch())
				{
					{
						std::tm  tm = r.get<std::tm>("report_date");
						CString str;
						str.Format("%4d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1,  tm.tm_mday);
						m_vec_StockSummary[k].performanceForecast_date = str.GetBuffer();
					}
					m_vec_StockSummary[k].performanceForecast_rate = r.get<double>("change_rate");
					break;
				}

			}
		}

		if(m_vec_StockSummary.size()>0)
		{
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "开始财务报表时间的汇总";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);

			for(int k=0; k< m_vec_StockSummary.size(); k++)
			{

				ss.str("");
				ss << "select code, report_date "
					<< " from balance where code=" << m_vec_StockSummary[k].code
					<< " order by report_date desc ";

				session sql(g_MysqlPool);
				row r;
				statement st = (sql.prepare << ss.str(),into(r)) ;
				st.execute();

				while (st.fetch())
				{
					{
						std::tm  tm = r.get<std::tm>("report_date");
						CString str;
						str.Format("%4d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1,  tm.tm_mday);
						m_vec_StockSummary[k].recent_finance_report_date = str.GetBuffer();
					}
					break;
				}

			}
		}

		if(m_vec_StockSummary.size()>0)
		{
			ss.str("");
			ss << "任务【" << m_nTaskID << "】";
			ss << "开始补充基本信息";
			sendToOutput(ss.str().c_str(), NULL, m_nLogType);

			for(int k=0; k< m_vec_StockSummary.size(); k++)
			{

				ss.str("");
				ss << "select abbreviation, a_time_to_market "
					<< " from StockInfo where Code=" << m_vec_StockSummary[k].code
					<< " ";

				session sql(g_MysqlPool);
				row r;
				statement st = (sql.prepare << ss.str(),into(r)) ;
				st.execute();

				while (st.fetch())
				{
					{
						std::tm  tm = r.get<std::tm>("a_time_to_market");
						CString str;
						str.Format("%4d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1,  tm.tm_mday);
						m_vec_StockSummary[k].a_time_to_market = str.GetBuffer();
					}
					m_vec_StockSummary[k].name = /*Utf8_GBK*/(r.get<string>("abbreviation"));
					break;
				}

			}
		}

		//	补充停牌公司的信息
		for(int m=0; m< m_vec_request_code.size(); m++)
		{
			int req_code = atoi(m_vec_request_code[m].c_str());
			int k=0;
			for(; k< m_vec_StockSummary.size(); k++)
			{
				if(m_vec_StockSummary[k].code== req_code)
					break;
			}
			if(k==m_vec_StockSummary.size())
			{
				CStockInfoSummary spd;
				spd.code = req_code;
				spd.name = "可能停牌";
				m_vec_StockSummary.push_back(spd);
			}
		}

		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}


void CCalculateStockInfoEx::Calculate()
{
	//CStockApp *  pApp = (CStockApp *)AfxGetApp();
	//for(int j=0; j<m_vec_code.size(); j++)
	//{
	//	int index = pApp->m_CEntrepreneurship.SearchStockByCode(m_vec_code[j]);
	//	if(index>=0)
	//	{
	//		m_vec_abbreviation[j] = pApp->m_CEntrepreneurship.m_vec_abbreviation[index];
	//		m_vec_belong_to_industry[j] = pApp->m_CEntrepreneurship.m_vec_belong_to_industry[index];
	//		m_vec_province[j] = pApp->m_CEntrepreneurship.m_vec_province[index];
	//		m_vec_stock_plate[j]="创业板";
	//	}

	//	index = pApp->m_CSmallMidEnterprise.SearchStockByCode(m_vec_code[j]);
	//	if(index>=0)
	//	{
	//		m_vec_abbreviation[j] = pApp->m_CSmallMidEnterprise.m_vec_abbreviation[index];
	//		m_vec_belong_to_industry[j] = pApp->m_CSmallMidEnterprise.m_vec_belong_to_industry[index];
	//		m_vec_province[j] = pApp->m_CSmallMidEnterprise.m_vec_province[index];
	//		m_vec_stock_plate[j]="中小板";
	//	}
	//	index = pApp->m_CShenzhenAshares.SearchStockByCode(m_vec_code[j]);
	//	if(index>=0)
	//	{
	//		m_vec_abbreviation[j] = pApp->m_CShenzhenAshares.m_vec_abbreviation[index];
	//		m_vec_belong_to_industry[j] = pApp->m_CShenzhenAshares.m_vec_belong_to_industry[index];
	//		m_vec_province[j] = pApp->m_CShenzhenAshares.m_vec_province[index];
	//		m_vec_stock_plate[j]="深圳A股";
	//	}
	//	index = pApp->m_CShanghaiStock.SearchStockByCode(m_vec_code[j]);
	//	if(index>=0)
	//	{
	//		m_vec_abbreviation[j] = pApp->m_CShanghaiStock.m_vec_abbreviation[index];
	//		m_vec_belong_to_industry[j] = pApp->m_CShanghaiStock.m_vec_belong_to_industry[index];
	//		m_vec_province[j] = pApp->m_CShanghaiStock.m_vec_province[index];
	//		m_vec_stock_plate[j]="上海股票板";
	//	}

	//	map<int, CCapitalStructOf10jqka>::iterator it_cs = pApp->m_mapCapitalStruct.find(m_vec_code[j]);
	//	if(it_cs!=pApp->m_mapCapitalStruct.end())
	//	{
	//		//	计算总市值
	//		double nShare = -1;
	//		CCapitalStructOf10jqka cap = it_cs->second;
	//		m_vec_a_total_shares[j] = cap.m_vec_a_total_shares[cap.m_vec_a_total_shares.size()-1];
	//		nShare = m_vec_a_total_shares[j]* 10000;
	//		m_vec_a_total_value[j] = m_vec_a_total_shares[j] * m_vec_close[j];

	//		// 计算市盈率
	//		map<int, CIncomeStatement>::iterator it_in = pApp->m_mapIncomeStatement.find(m_vec_code[j]);
	//		if(it_in!=pApp->m_mapIncomeStatement.end())
	//		{
	//			CIncomeStatement income = it_in->second;
	//			// 寻找最近的 净利润, 估计全年的利润, 
	//			double nProfit = -1;
	//			COleDateTime odt_in ;
	//			odt_in.ParseDateTime( income.m_vec_report_date[income.m_vec_report_date.size()-1].c_str());
	//			int mon = odt_in.GetMonth();
	//			int day = odt_in.GetDay();

	//			nProfit = income.m_vec_net_profit[income.m_vec_report_date.size()-1];
	//			if(mon==3)
	//				nProfit = 4 * nProfit;
	//			else if(mon==6)
	//				nProfit = 2 * nProfit;
	//			else if(mon==9)
	//				nProfit = 4 * nProfit / 3;

	//			if(nProfit> 0 && nShare >0)
	//			{
	//				double nPe = nProfit/nShare;
	//				m_vec_earnings_per_share[j] = nPe;
	//				nPe =  m_vec_close[j]/nPe;
	//				m_vec_pe[j] = nPe;
	//			}
	//		}
	//	}
	//}
}

void CCalculateStockInfoEx::ImportToDatabase(void)
{

	Calculate();

	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();
	ss << "CStockInfoEx 开始导入数据。。。。。。\r\n";
	//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

	string connectString = "dbname=stock  port=3366   host=127.0.0.1 user=root  charset=gbk ";
	//session sql(backEnd, connectString);
	session sql(g_MysqlPool);

	try
	{
		for (int row = 0; row < m_vec_code.size(); row++)
		{
			if(m_vec_code[row]==0)
				continue;
			ss.str("");

			ss << "insert into StockInfoEx "
				<< " select " <<  Int2String(m_vec_code[row]) << ", " 
				<< " \'" << m_vec_calculate_date[row] << "\', "
				<< Double2String(m_vec_a_total_shares[row]) << ", "
				<< Double2String(m_vec_a_total_value[row]) << ", "
				<< Double2String(m_vec_earnings_per_share[row]) << ", "
				<< Double2String(m_vec_pe[row]) << ", "
				<< Double2String(m_vec_close[row]) << ", "
				<< " \'" << m_vec_abbreviation[row] << "\', "
				<< " \'" << m_vec_province[row] << "\', "
				<< " \'" << m_vec_belong_to_industry[row] << "\', "
				<< " \'" << m_vec_stock_plate[row] << "\'  "
				<< " from dual where not exists (SELECT 1 from StockInfoEx " 
				<< " where code =" << Int2String(m_vec_code[row]) << "  ) ";

			string sql1 = /*Gbk2Utf8*/(ss.str());

			ss.str("");
			ss << "update StockInfoEx "
				<< " set calculate_date=\'" << m_vec_calculate_date[row] << "\', "
				<< " a_total_shares=" << Double2String(m_vec_a_total_shares[row]) << ", "
				<< " a_total_value=" << Double2String(m_vec_a_total_value[row]) << ", "
				<< " earnings_per_share=" << Double2String(m_vec_earnings_per_share[row]) << ", "
				<< " pe=" << Double2String(m_vec_pe[row]) << ", "
				<< " close=" << Double2String(m_vec_close[row]) << ", "
				<< " abbreviation=\'" << m_vec_abbreviation[row] << "\', "
				<< " province=\'" << m_vec_province[row] << "\', "
				<< " belong_to_industry=\'" << m_vec_belong_to_industry[row] << "\', "
				<< " stock_plate=\'" << m_vec_stock_plate[row] << "\'  "
				<< " where code =" << Int2String(m_vec_code[row]) ;

			string sql2 = /*Gbk2Utf8*/(ss.str());

			sql << sql1;
			sql << sql2;

			//sendToOutput(ss.str().c_str());
		}
		ss.str("");
		ss << "共有数据 " << m_vec_code.size() ;
		ss << " OK,  CStockInfoEx 数据导入完毕.";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << "code【" << m_vec_code[0] << "】";
		ss << e.what() << '\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	
	
}
