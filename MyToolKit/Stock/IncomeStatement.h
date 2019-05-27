#pragma once

//////////////////////////////////////////////////////////////////////////
//	利润表

class CIncomeStatement :public CFinanceReport
{
public:
	CIncomeStatement(void);
	//	因为 m_vec_index 中包含了指针，故重写 赋值函数和拷贝构造函数
	CIncomeStatement(const CIncomeStatement& rhs);
	CIncomeStatement& operator=(const CIncomeStatement& rhs);

	~CIncomeStatement(void);


	CStockDataModel * NewCopy();
	string GetDownloadUrl();
	int CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);

	//	估算全年的利润
	double EstimateYearProfits();

	//	某日归属母公司所有者的净利润
	double NetProfitBelongingToParentCompany(string currDate, string & baseDate, bool & isLast);

private:
	void Initialize();

public:
	//map<int, string>	m_vec_report_date;
	vector<double>	m_vec_overall_sales;		//	营业总收入
	vector<double>	m_vec_sales_from_operations;		//	营业收入
	vector<double>	m_vec_interest_income;						//	利息收入
	vector<double>	m_vec_insurance_premiums_earned;						//	已赚保费
	vector<double>	m_vec_handling_charges_and_commissions_income;			//	手续费及佣金收入
	vector<double>	m_vec_real_estate_sales_revenue;						//	房地产销售收入
	vector<double>	m_vec_income_from_other_operations;						//	其他业务收入
	vector<double>	m_vec_overall_costs;						//	二、营业总成本
	vector<double>	m_vec_cost_of_operations;					//	营业成本
	vector<double>	m_vec_interest_expenses;						//	利息支出
	vector<double>	m_vec_handling_charges_and_commissions_expenses;						//	手续费及佣金支出
	vector<double>	m_vec_real_estate_sales_cost;						//	房地产销售成本
	vector<double>	m_vec_research_and_development_expenses;			//	研发费用
	vector<double>	m_vec_refund_of_insurance_premiums;						//	退保金
	vector<double>	m_vec_net_payments_for_insurance_claims;						//	赔付支出净额
	vector<double>	m_vec_net_provision_for_insurance_contracts;			//	提取保险合同准备金净额
	vector<double>	m_vec_commissions_on_insurance_policies;						//	保单红利支出
	vector<double>	m_vec_cession_charges;						//	分保费用
	vector<double>	m_vec_cost_of_other_operations;						//	其他业务成本
	vector<double>	m_vec_sales_tax_and_additions;						//	营业税金及附加
	vector<double>	m_vec_selling_and_distribution_expenses;						//	销售费用
	vector<double>	m_vec_general_and_administrative_expenses;						//	管理费用
	vector<double>	m_vec_financial_expenses;						//	财务费用
	vector<double>	m_vec_impairment_loss_on_assets;						//	资产减值损失
	vector<double>	m_vec_gain_or_loss_from_changes_in_fair_values;						//	公允价值变动收益
	vector<double>	m_vec_investment_income;						//	投资收益
	vector<double>	m_vec_investment_income_from_joint_ventures_and_affiliates;			//	对联营企业和合营企业的投资收益
	vector<double>	m_vec_gain_or_loss_on_foreign_exchange_transactions;						//	汇兑收益
	vector<double>	m_vec_futures_gains_and_losses;						//	期货损益
	vector<double>	m_vec_managed_revenue;						//	托管收益
	vector<double>	m_vec_subsidy_income;						//	补贴收入
	vector<double>	m_vec_other_business_profits;						//	其他业务利润
	vector<double>	m_vec_profit_from_operations;						//	三、营业利润
	vector<double>	m_vec_non_operating_profit;						//	营业外收入
	vector<double>	m_vec_non_operating_expenses;						//	营业外支出
	vector<double>	m_vec_losses_from_disposal_of_non_current_assets;			//	非流动资产处置损失
	vector<double>	m_vec_profit_before_tax;						//	利润总额
	vector<double>	m_vec_income_tax_expenses;						//	所得税费用
	vector<double>	m_vec_unrecognized_investment_losses;						//	未确认投资损失
	vector<double>	m_vec_net_profit;						//	四、净利润
	vector<double>	m_vec_net_profit_belonging_to_parent_company;						//	归属于母公司所有者的净利润
	vector<double>	m_vec_minority_interest;						//	少数股东损益
	vector<double>	m_vec_earnings_per_share;						//	五、每股收益
	vector<double>	m_vec_basic_earnings_per_share;					//	基本每股收益
	vector<double>	m_vec_diluted_earnings_per_share;						//	稀释每股收益
	vector<double>	m_vec_other_consolidated_income;						//	六、其他综合收益
	vector<double>	m_vec_total_consolidated_income;						//	七、综合收益总额
	vector<double>	m_vec_consolidated_income_belonging_to_parent_company;	//	归属于母公司所有者的综合收益总额
	vector<double>	m_vec_consolidated_income_belonging_to_minority_shareholders;	//	归属于少数股东的综合收益总额

};
