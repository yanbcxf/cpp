#pragma once


//////////////////////////////////////////////////////////////////////////
//	该类用来 查询、排序、分类、过滤等功能

class CCalculateStockInfoEx : public CCalculateTask 
{
public:
	CCalculateStockInfoEx(HWND hwnd, int logType);
	~CCalculateStockInfoEx(void);

	string ClassName(){ return "CCalculateStockInfoEx"; }

	string SqlScript();
	void ImportToDatabase();
	void RecentStockSummary();
	void RecentMainIncomeComposition();
	void RecentLiquidityRatio();
	void RecentNetProfitsRatio();
	void RecentBalanceAndIncomestatement();
	void RecentFundHold();
	void ZombieStockList();
	void MainProductQueryList();

	static void ImportToDatabase1(void* firstArg);
	static void RecentStockSummary1(void* firstArg);
	static void RecentMainIncomeComposition1(void* firstArg);
	static void RecentLiquidityRatio1(void* firstArg);
	static void RecentNetProfitsRatio1(void* firstArg);
	static void RecentBalanceAndIncomestatement1(void* firstArg);
	static void RecentFundHold1(void* firstArg);
	static void ZombieStockList1(void* firstArg);
	static void MainProductQueryList1(void* firstArg);
	
	

private:
	void Calculate();

public :
	vector<CStockInfoSummary>  m_vec_StockSummary;	//	保存计算的结果

public:
	map<int, int>		m_vec_code			;	//	"公司代码",
	map<int, string>	m_vec_calculate_date;	//	最后计算的交易日
	map<int, double>	m_vec_a_total_shares;	//	A 股股数
	map<int, double>	m_vec_a_total_value;		//	总市值
	map<int, double>	m_vec_earnings_per_share;	//	每股收益
	map<int, double>	m_vec_pe;				//	市盈率


	map<int, string>	m_vec_abbreviation;		//	"公司简称",
	map<int, string>	m_vec_province;			//	"省份
	map<int, string>	m_vec_belong_to_industry;	//	 "所属行业"
	map<int, string>	m_vec_stock_plate;		//	股票板块： 创业版、中小版

public:
	map<int, double>	m_vec_open;
	map<int, double>	m_vec_high;
	map<int, double>	m_vec_low;
	map<int, double>	m_vec_close;		//	收盘价
	map<int, double>	m_vec_volume;		//	成交量（手）

	map<int, double>	m_vec_change;			//	涨跌额
	map<int, double>	m_vec_change_rate;		//	涨跌幅
	map<int, double>	m_vec_turnover;			//	成交额
	map<int, double>	m_vec_turnover_rate;	//	换手率

public:
	map<int, string>	m_vec_composition_name;
	map<int, double>	m_vec_income;
	map<int, double>	m_vec_cost;
	map<int, double>	m_vec_gross_profit_rate;

public:
	map<int, string>	m_vec_report_date;
	map<int, double>	m_vec_liquidity_ratio;
public:
	map<int, double>	m_vec_net_profits_ratio;

public:
	map<int, double>	m_vec_sales_from_operations;		//	营业收入
	map<int, double>	m_vec_cost_of_operations;			//	营业成本
	map<int, double>	m_vec_net_profit_belonging_to_parent_company;		//	归属于母公司所有者的净利润

	map<int, double>	m_vec_inventories;				//		"存货"
	map<int, double>	m_vec_fixed_assets_net_value;	//		"固定资产净值"
	map<int, double>	m_vec_total_current_assets;		//		"流动资产合计"
	map<int, double>	m_vec_total_non_current_assets;  //		"非流动资产合计"
	map<int, double>	m_vec_total_current_liabilities;  //		"流动负债合计"
	map<int, double>	m_vec_total_non_current_liabilities;	 //		"非流动负债合计"

public:
	map<int, double>	m_vec_number_fund_hold;			//	基金持有量
	map<int, double>	m_vec_number_AShares;			//	流通股数
	map<int, double>	m_vec_number_A_total_shares;	//	总流通股数

public:
	map<int, double>	m_vec_registered_capital;		//	实收资本
	map<int, double>	m_vec_total_owners_equity_belongs_to_parent_company;	//	归属于母公司股东权益合计
	map<int ,double>	m_vec_undistributed_profits;	//	未分配利润
	map<int ,double>	m_vec_total_liability;			//	负债总计
	map<int ,double>	m_vec_total_assets;				//	资产总计
	map<int ,double>	m_vec_intangibel_assets;		//	无形资产
	map<int, double>	m_vec_goodwill;					//	商誉

public:
	string		m_main_product_query;					//	主营产品查询


};
