#pragma once
#include "stockdatamodel.h"

class CCashFlows :
	public CFinanceReport
{
public:
	CCashFlows(void);
	//	因为 m_vec_index 中包含了指针，故重写 赋值函数和拷贝构造函数
	CCashFlows(const CCashFlows& rhs);
	CCashFlows& operator=(const CCashFlows& rhs);

	~CCashFlows(void);


	CStockDataModel * NewCopy();
	string GetDownloadUrl();
	int CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData);

private:
	void Initialize();

public:
	// map<int, string>	m_vec_report_date;
	vector<double> m_vec_cash_flows_from_operating_activities ;		//		"一、经营活动产生的现金流量",
	vector<double> m_vec_cash_received_from_sales_of_goods_or_rending_of_services ;		//		"销售商品、提供劳务收到的现金",
	vector<double> m_vec_net_increase_of_customers_deposit_and_deposit_taking_of_interbank ;		//		"客户存款和同业存放款项净增加额",
	vector<double> m_vec_net_increase_borrowings_from_central_bank  ;		//		"向中央银行借款净增加额",
	vector<double> m_vec_net_increase_borrowing_funds_to_other_financing_institution  ;		//		"向其他金融机构拆入资金净增加额",
	vector<double> m_vec_cash_from_original_insurance_contract_premium ;		//		"收到原保险合同保费取得的现金",
	vector<double> m_vec_net_cash_from_reinsurance_business  ;		//		"收到再保险业务现金净额",
	vector<double> m_vec_net_increase_of_insured_deposit_and_investment  ;		//		"保户储金及投资款净增加额",
	vector<double> m_vec_net_increase_in_disposal_of_financial_assets ;		//		"处置交易性金融资产净增加额",
	vector<double> m_vec_interest_handling_charges_and_commissions_received  ;		//		"收取利息、手续费及佣金的现金",
	vector<double> m_vec_net_increase_of_loans  ;		//		"拆入资金净增加额",
	vector<double> m_vec_net_increase_of_repurchasing_business_funds  ;		//		"回购业务资金净增加额",
	vector<double> m_vec_refund_of_tax_and_levies  ;		//		"收到的税费返还",
	vector<double> m_vec_cash_relating_to_other_business_activities  ;		//		"收到的其他与经营活动有关的现金",
	vector<double> m_vec_cash_inflow_from_business_activities  ;		//		"经营活动现金流入小计",
	vector<double> m_vec_cash_from_purchasement_and_receiving_service_payments  ;		//		"购买商品、接受劳务支付的现金",
	vector<double> m_vec_net_increase_of_customer_loans_and_advances  ;		//		"客户贷款及垫款净增加额",
	vector<double> m_vec_net_increase_depositing_in_central_bank_and_other_banks  ;		//		"存放中央银行和同业款项净增加额",
	vector<double> m_vec_cash_paid_for_original_insurance_contract_compensation_payment  ;		//		"支付原保险合同赔付款项的现金",
	vector<double> m_vec_interests_handling_charges_and_commissions_paid  ;		//		"支付利息、手续费及佣金的现金",
	vector<double> m_vec_cash_paid_for_policy_dividend ;		//		"支付保单红利的现金",
	vector<double> m_vec_cash_paid_to_employee_and_for_employee  ;		//		"支付给职工以及为职工支付的现金",
	vector<double> m_vec_payments_of_all_types_of_taxes  ;		//		"支付的各项税费",
	vector<double> m_vec_cash_paid_for_others_relating_to_business_activities  ;		//		"支付的其他与经营活动有关的现金",
	vector<double> m_vec_sub_total_of_cash_outflows_relating_to_business_activities ;		//		"经营活动现金流出小计",
	vector<double> m_vec_net_cash_flow_from_business_activities  ;		//		"经营活动产生的现金流量净额",

	vector<double> m_vec_cash_flows_from_investing_activities ;		//		"二、投资活动产生的现金流量：",
	vector<double> m_vec_cash_received_from_return_of_investment ;		//		"收回投资所收到的现金",
	vector<double> m_vec_cash_received_from_investments ;		//		"取得投资收益所收到的现金",
	vector<double> m_vec_net_cash_received_from_disposal_of_fixed_assets_intangible_assets_and_other_long_term_assets ;		//		"处置固定资产、无形资产和其他长期资产所收回的现金净额",
	vector<double> m_vec_net_cash_received_from_disposals_of_subsidiaries_and_other_business_units ;		//		"处置子公司及其他营业单位收到的现金净额",
	vector<double> m_vec_other_cash_received_relating_to_operating_activities ;		//		"收到的其他与投资活动有关的现金",
	vector<double> m_vec_unknown_1 ;		//		"减少质押和定期存款所收到的现金",
	vector<double> m_vec_sub_total_of_cash_inflows_relating_to_investment_activities ;		//		"投资活动现金流入小计",
	vector<double> m_vec_cash_paid_to_acquire_fixed_assets_intangible_assets_and_other_long_term_assents ;		//		"购建固定资产、无形资产和其他长期资产所支付的现金",
	vector<double> m_vec_cash_paid_for_investments ;		//		"投资所支付的现金",
	vector<double> m_vec_net_increase_of_hypothecated_loan  ;		//		"质押贷款净增加额",
	vector<double> m_vec_net_cash_from_subsidiary_corporation_and_other_business_entity  ;		//		"取得子公司及其他营业单位支付的现金净额",
	vector<double> m_vec_other_cash_paid_relating_to_operating_activities ;		//		"支付的其他与投资活动有关的现金",
	vector<double> m_vec_unknown_2 ;		//		"增加质押和定期存款所支付的现金",
	vector<double> m_vec_sub_total_of_cash_outflows_relating_to_investment_activities ;		//		"投资活动现金流出小计",
	vector<double> m_vec_net_cash_flows_from_investing_activities ;		//		"投资活动产生的现金流量净额",

	vector<double> m_vec_cash_flows_from_financing_activities  ;		//		"三、筹资活动产生的现金流量：",
	vector<double> m_vec_cash_received_from_investors ;		//		"吸收投资收到的现金",
	vector<double> m_vec_cash_from_minority_stockholders_by_subsidiary_corporation  ;		//		"其中：子公司吸收少数股东投资收到的现金",
	vector<double> m_vec_cash_from_borrowings ;		//		"取得借款收到的现金",
	vector<double> m_vec_proceeds_from_issuance_of_bonds  ;		//		"发行债券收到的现金",
	vector<double> m_vec_cash_from_other_relevant_financing_activities  ;		//		"收到其他与筹资活动有关的现金",
	vector<double> m_vec_sub_total_of_cash_inflows ;		//		"筹资活动现金流入小计",
	vector<double> m_vec_cash_repayments_of_amount_borrowed ;		//		"偿还债务支付的现金",
	vector<double> m_vec_cash_payments_for_distrbution_of_dividends_or_profits ;		//		"分配股利、利润或偿付利息所支付的现金",
	vector<double> m_vec_other_cash_payment_relating_to_financing_activities ;		//		"支付其他与筹资活动有关的现金",
	vector<double> m_vec_sub_total_of_cash_outflows_relating_to_financing_activities ;	//		"筹资活动现金流出小计",
	vector<double> m_vec_net_cash_flows_from_financing_activities ;		//		"筹资活动产生的现金流量净额",

	vector<double> m_vec_supplemental_information ;		//		"附注",
	vector<double> m_vec_effect_of_foreign_exchang_rate_changes_on_cash ;		//		"汇率变动对现金及现金等价物的影响",
	vector<double> m_vec_net_increase_in_cash_and_cash_equivalents ;		//		"现金及现金等价物净增加额",
	vector<double> m_vec_cash_and_cash_equivalents_at_beginning_of_period  ;		//		"期初现金及现金等价物余额",
	vector<double> m_vec_cash_and_cash_equivalents_at_end_of_period  ;		//		"期末现金及现金等价物余额",
	vector<double> m_vec_net_profit ;		//		"净利润",
	vector<double> m_vec_minority_stockholders_interest  ;		//		"少数股东权益",
	vector<double> m_vec_unaffirmed_investment_loss  ;		//		"未确认的投资损失",
	vector<double> m_vec_provision_for_assets ;		//		"资产减值准备",
	vector<double> m_vec_depreciation_for_fixed_assets_and_so_on ;		//		"固定资产折旧、油气资产折耗、生产性物资折旧",
	vector<double> m_vec_amortisation_of_intangible_assets ;		//		"无形资产摊销",
	vector<double> m_vec_amortisation_of_long_term_deffered_expenses ;		//		"长期待摊费用摊销",
	vector<double> m_vec_decrease_of_deffered_expenses ;		//		"待摊费用的减少",
	vector<double> m_vec_addition_of_accued_expense ;		//		"预提费用的增加",
	vector<double> m_vec_losses_on_disposal_of_fixed_assets_intangible_assets_and_other_long_term_assets ;		//		"处置固定资产、无形资产和其他长期资产的损失",
	vector<double> m_vec_losses_on_scrapping_of_fixed_assets ;		//		"固定资产报废损失",
	vector<double> m_vec_losses_on_fair_value ;		//		"公允价值变动损失",
	vector<double> m_vec_unknown_3 ;		//		"递延收益增加（减：减少）",
	vector<double> m_vec_anticipation_liabilities  ;		//		"预计负债",
	vector<double> m_vec_finance_expense ;		//		"财务费用",
	vector<double> m_vec_losses_arsing_from_investment ;		//		"投资损失",
	vector<double> m_vec_unknown_4 ;		//		"递延所得税资产减少",
	vector<double> m_vec_unknown_5 ;		//		"递延所得税负债增加",
	vector<double> m_vec_decrease_in_inventories ;		//		"存货的减少",
	vector<double> m_vec_decrease_in_operating_receivables ;		//		"经营性应收项目的减少",
	vector<double> m_vec_increase_in_operating_receivables ;		//		"经营性应付项目的增加",
	vector<double> m_vec_unknown_6 ;		//		"已完工尚未结算款的减少(减:增加)",
	vector<double> m_vec_unknown_7 ;		//		"已结算尚未完工款的增加(减:减少)",
	vector<double> m_vec_others ;		//		"其他",
	vector<double> m_vec_net_cash_flows_from_operating_activities ;		//		"经营活动产生现金流量净额",
	vector<double> m_vec_debts_transfer_to_capital ;		//		"债务转为资本",
	vector<double> m_vec_one_year_due_convertible_bonds ;		//		"一年内到期的可转换公司债券",
	vector<double> m_vec_financing_rent_to_the_fixed_asset ;		//		"融资租入固定资产",
	vector<double> m_vec_cash_at_the_end_of_period ;		//		"现金的期末余额",
	vector<double> m_vec_cash_at_the_beginning_of_the_period ;		//		"现金的期初余额",
	vector<double> m_vec_cash_equivalents_at_the_end_of_the_period ;		//		"现金等价物的期末余额",
	vector<double> m_vec_cash_equivalents_at_the_beginning_of_the_period ;		//		"现金等价物的期初余额",
	vector<double> m_vec_net_increase_in_cash_and_cash_equivalents_1 ;		//		"现金及现金等价物的净增加额"

};
