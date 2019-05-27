#include "StdAfx.h"
#include "CashFlows.h"
#include "UrlUtils.h"

void CCashFlows::Initialize(void)
{
	m_strChineseName = _T("现金流量表");
	m_strEnglishName = "CashFlows";

	m_vec_index[1] = &m_vec_cash_flows_from_operating_activities ;		//		"一、经营活动产生的现金流量",
	m_vec_index[2] = &m_vec_cash_received_from_sales_of_goods_or_rending_of_services ;		//		"销售商品、提供劳务收到的现金",
	m_vec_index[3] = &m_vec_net_increase_of_customers_deposit_and_deposit_taking_of_interbank ;		//		"客户存款和同业存放款项净增加额",
	m_vec_index[4] = &m_vec_net_increase_borrowings_from_central_bank  ;		//		"向中央银行借款净增加额",
	m_vec_index[5] = &m_vec_net_increase_borrowing_funds_to_other_financing_institution  ;		//		"向其他金融机构拆入资金净增加额",
	m_vec_index[6] = &m_vec_cash_from_original_insurance_contract_premium ;		//		"收到原保险合同保费取得的现金",
	m_vec_index[7] = &m_vec_net_cash_from_reinsurance_business  ;		//		"收到再保险业务现金净额",
	m_vec_index[8] = &m_vec_net_increase_of_insured_deposit_and_investment  ;		//		"保户储金及投资款净增加额",
	m_vec_index[9] = &m_vec_net_increase_in_disposal_of_financial_assets ;		//		"处置交易性金融资产净增加额",
	m_vec_index[10] = &m_vec_interest_handling_charges_and_commissions_received  ;		//		"收取利息、手续费及佣金的现金",
	m_vec_index[11] = &m_vec_net_increase_of_loans  ;		//		"拆入资金净增加额",
	m_vec_index[12] = &m_vec_net_increase_of_repurchasing_business_funds  ;		//		"回购业务资金净增加额",
	m_vec_index[13] = &m_vec_refund_of_tax_and_levies  ;		//		"收到的税费返还",
	m_vec_index[14] = &m_vec_cash_relating_to_other_business_activities  ;		//		"收到的其他与经营活动有关的现金",
	m_vec_index[15] = &m_vec_cash_inflow_from_business_activities  ;		//		"经营活动现金流入小计",
	m_vec_index[16] = &m_vec_cash_from_purchasement_and_receiving_service_payments  ;		//		"购买商品、接受劳务支付的现金",
	m_vec_index[17] = &m_vec_net_increase_of_customer_loans_and_advances  ;		//		"客户贷款及垫款净增加额",
	m_vec_index[18] = &m_vec_net_increase_depositing_in_central_bank_and_other_banks  ;		//		"存放中央银行和同业款项净增加额",
	m_vec_index[19] = &m_vec_cash_paid_for_original_insurance_contract_compensation_payment  ;		//		"支付原保险合同赔付款项的现金",
	m_vec_index[20] = &m_vec_interests_handling_charges_and_commissions_paid  ;		//		"支付利息、手续费及佣金的现金",
	m_vec_index[21] = &m_vec_cash_paid_for_policy_dividend ;		//		"支付保单红利的现金",
	m_vec_index[22] = &m_vec_cash_paid_to_employee_and_for_employee  ;		//		"支付给职工以及为职工支付的现金",
	m_vec_index[23] = &m_vec_payments_of_all_types_of_taxes  ;		//		"支付的各项税费",
	m_vec_index[24] = &m_vec_cash_paid_for_others_relating_to_business_activities  ;		//		"支付的其他与经营活动有关的现金",
	m_vec_index[25] = &m_vec_sub_total_of_cash_outflows_relating_to_business_activities ;		//		"经营活动现金流出小计",
	m_vec_index[26] = &m_vec_net_cash_flow_from_business_activities  ;		//		"经营活动产生的现金流量净额",
	m_vec_index[27] = &m_vec_cash_flows_from_investing_activities ;		//		"二、投资活动产生的现金流量：",
	m_vec_index[28] = &m_vec_cash_received_from_return_of_investment ;		//		"收回投资所收到的现金",
	m_vec_index[29] = &m_vec_cash_received_from_investments ;		//		"取得投资收益所收到的现金",
	m_vec_index[30] = &m_vec_net_cash_received_from_disposal_of_fixed_assets_intangible_assets_and_other_long_term_assets ;		//		"处置固定资产、无形资产和其他长期资产所收回的现金净额",
	m_vec_index[31] = &m_vec_net_cash_received_from_disposals_of_subsidiaries_and_other_business_units ;		//		"处置子公司及其他营业单位收到的现金净额",
	m_vec_index[32] = &m_vec_other_cash_received_relating_to_operating_activities ;		//		"收到的其他与投资活动有关的现金",
	m_vec_index[33] = &m_vec_unknown_1 ;		//		"减少质押和定期存款所收到的现金",
	m_vec_index[34] = &m_vec_sub_total_of_cash_inflows_relating_to_investment_activities ;		//		"投资活动现金流入小计",
	m_vec_index[35] = &m_vec_cash_paid_to_acquire_fixed_assets_intangible_assets_and_other_long_term_assents ;		//		"购建固定资产、无形资产和其他长期资产所支付的现金",
	m_vec_index[36] = &m_vec_cash_paid_for_investments ;		//		"投资所支付的现金",
	m_vec_index[37] = &m_vec_net_increase_of_hypothecated_loan  ;		//		"质押贷款净增加额",
	m_vec_index[38] = &m_vec_net_cash_from_subsidiary_corporation_and_other_business_entity  ;		//		"取得子公司及其他营业单位支付的现金净额",
	m_vec_index[39] = &m_vec_other_cash_paid_relating_to_operating_activities ;		//		"支付的其他与投资活动有关的现金",
	m_vec_index[40] = &m_vec_unknown_2 ;		//		"增加质押和定期存款所支付的现金",
	m_vec_index[41] = &m_vec_sub_total_of_cash_outflows_relating_to_investment_activities ;		//		"投资活动现金流出小计",
	m_vec_index[42] = &m_vec_net_cash_flows_from_investing_activities ;		//		"投资活动产生的现金流量净额",
	m_vec_index[43] = &m_vec_cash_flows_from_financing_activities  ;		//		"三、筹资活动产生的现金流量：",
	m_vec_index[44] = &m_vec_cash_received_from_investors ;		//		"吸收投资收到的现金",
	m_vec_index[45] = &m_vec_cash_from_minority_stockholders_by_subsidiary_corporation  ;		//		"其中：子公司吸收少数股东投资收到的现金",
	m_vec_index[46] = &m_vec_cash_from_borrowings ;		//		"取得借款收到的现金",
	m_vec_index[47] = &m_vec_proceeds_from_issuance_of_bonds  ;		//		"发行债券收到的现金",
	m_vec_index[48] = &m_vec_cash_from_other_relevant_financing_activities  ;		//		"收到其他与筹资活动有关的现金",
	m_vec_index[49] = &m_vec_sub_total_of_cash_inflows ;		//		"筹资活动现金流入小计",
	m_vec_index[50] = &m_vec_cash_repayments_of_amount_borrowed ;		//		"偿还债务支付的现金",
	m_vec_index[51] = &m_vec_cash_payments_for_distrbution_of_dividends_or_profits ;		//		"分配股利、利润或偿付利息所支付的现金",
	m_vec_index[52] = &m_vec_other_cash_payment_relating_to_financing_activities ;		//		"支付其他与筹资活动有关的现金",
	m_vec_index[53] = &m_vec_sub_total_of_cash_outflows_relating_to_financing_activities ;	//		"筹资活动现金流出小计",
	m_vec_index[54] = &m_vec_net_cash_flows_from_financing_activities ;		//		"筹资活动产生的现金流量净额",
	m_vec_index[55] = &m_vec_supplemental_information ;		//		"附注",
	m_vec_index[56] = &m_vec_effect_of_foreign_exchang_rate_changes_on_cash ;		//		"汇率变动对现金及现金等价物的影响",
	m_vec_index[57] = &m_vec_net_increase_in_cash_and_cash_equivalents ;		//		"现金及现金等价物净增加额",
	m_vec_index[58] = &m_vec_cash_and_cash_equivalents_at_beginning_of_period  ;		//		"期初现金及现金等价物余额",
	m_vec_index[59] = &m_vec_cash_and_cash_equivalents_at_end_of_period  ;		//		"期末现金及现金等价物余额",
	m_vec_index[60] = &m_vec_net_profit ;		//		"净利润",
	m_vec_index[61] = &m_vec_minority_stockholders_interest  ;		//		"少数股东权益",
	m_vec_index[62] = &m_vec_unaffirmed_investment_loss  ;		//		"未确认的投资损失",
	m_vec_index[63] = &m_vec_provision_for_assets ;		//		"资产减值准备",
	m_vec_index[64] = &m_vec_depreciation_for_fixed_assets_and_so_on ;		//		"固定资产折旧、油气资产折耗、生产性物资折旧",
	m_vec_index[65] = &m_vec_amortisation_of_intangible_assets ;		//		"无形资产摊销",
	m_vec_index[66] = &m_vec_amortisation_of_long_term_deffered_expenses ;		//		"长期待摊费用摊销",
	m_vec_index[67] = &m_vec_decrease_of_deffered_expenses ;		//		"待摊费用的减少",
	m_vec_index[68] = &m_vec_addition_of_accued_expense ;		//		"预提费用的增加",
	m_vec_index[69] = &m_vec_losses_on_disposal_of_fixed_assets_intangible_assets_and_other_long_term_assets ;		//		"处置固定资产、无形资产和其他长期资产的损失",
	m_vec_index[70] = &m_vec_losses_on_scrapping_of_fixed_assets ;		//		"固定资产报废损失",
	m_vec_index[71] = &m_vec_losses_on_fair_value ;		//		"公允价值变动损失",
	m_vec_index[72] = &m_vec_unknown_3 ;		//		"递延收益增加（减：减少）",
	m_vec_index[73] = &m_vec_anticipation_liabilities  ;		//		"预计负债",
	m_vec_index[74] = &m_vec_finance_expense ;		//		"财务费用",
	m_vec_index[75] = &m_vec_losses_arsing_from_investment ;		//		"投资损失",
	m_vec_index[76] = &m_vec_unknown_4 ;		//		"递延所得税资产减少",
	m_vec_index[77] = &m_vec_unknown_5 ;		//		"递延所得税负债增加",
	m_vec_index[78] = &m_vec_decrease_in_inventories ;		//		"存货的减少",
	m_vec_index[79] = &m_vec_decrease_in_operating_receivables ;		//		"经营性应收项目的减少",
	m_vec_index[80] = &m_vec_increase_in_operating_receivables ;		//		"经营性应付项目的增加",
	m_vec_index[81] = &m_vec_unknown_6 ;		//		"已完工尚未结算款的减少(减:增加)",
	m_vec_index[82] = &m_vec_unknown_7 ;		//		"已结算尚未完工款的增加(减:减少)",
	m_vec_index[83] = &m_vec_others ;		//		"其他",
	m_vec_index[84] = &m_vec_net_cash_flows_from_operating_activities ;		//		"经营活动产生现金流量净额",
	m_vec_index[85] = &m_vec_debts_transfer_to_capital ;		//		"债务转为资本",
	m_vec_index[86] = &m_vec_one_year_due_convertible_bonds ;		//		"一年内到期的可转换公司债券",
	m_vec_index[87] = &m_vec_financing_rent_to_the_fixed_asset ;		//		"融资租入固定资产",
	m_vec_index[88] = &m_vec_cash_at_the_end_of_period ;		//		"现金的期末余额",
	m_vec_index[89] = &m_vec_cash_at_the_beginning_of_the_period ;		//		"现金的期初余额",
	m_vec_index[90] = &m_vec_cash_equivalents_at_the_end_of_the_period ;		//		"现金等价物的期末余额",
	m_vec_index[91] = &m_vec_cash_equivalents_at_the_beginning_of_the_period ;		//		"现金等价物的期初余额",
	m_vec_index[92] = &m_vec_net_increase_in_cash_and_cash_equivalents_1 ;		//		"现金及现金等价物的净增加额"

	/*m_vec_report_date.clear();
	for(int row=1; row<= m_vec_index.size(); row++)
		m_vec_index[row]->clear();*/

	string header[] = {
		"单位",
		"一、经营活动产生的现金流量",
		"销售商品、提供劳务收到的现金",
		"客户存款和同业存放款项净增加额",
		"向中央银行借款净增加额",
		"向其他金融机构拆入资金净增加额",
		"收到原保险合同保费取得的现金",
		"收到再保险业务现金净额",
		"保户储金及投资款净增加额",
		"处置交易性金融资产净增加额",
		"收取利息、手续费及佣金的现金",
		"拆入资金净增加额",
		"回购业务资金净增加额",
		"收到的税费返还",
		"收到的其他与经营活动有关的现金",
		"经营活动现金流入小计",
		"购买商品、接受劳务支付的现金",
		"客户贷款及垫款净增加额",
		"存放中央银行和同业款项净增加额",
		"支付原保险合同赔付款项的现金",
		"支付利息、手续费及佣金的现金",
		"支付保单红利的现金",
		"支付给职工以及为职工支付的现金",
		"支付的各项税费",
		"支付的其他与经营活动有关的现金",
		"经营活动现金流出小计",
		"经营活动产生的现金流量净额",
		"二、投资活动产生的现金流量",
		"收回投资所收到的现金",
		"取得投资收益所收到的现金",
		"处置固定资产、无形资产和其他长期资产所收回的现金净额",
		"处置子公司及其他营业单位收到的现金净额",
		"收到的其他与投资活动有关的现金",
		"减少质押和定期存款所收到的现金",
		"投资活动现金流入小计",
		"购建固定资产、无形资产和其他长期资产所支付的现金",
		"投资所支付的现金",
		"质押贷款净增加额",
		"取得子公司及其他营业单位支付的现金净额",
		"支付的其他与投资活动有关的现金",
		"增加质押和定期存款所支付的现金",
		"投资活动现金流出小计",
		"投资活动产生的现金流量净额",
		"三、筹资活动产生的现金流量",
		"吸收投资收到的现金",
		"其中：子公司吸收少数股东投资收到的现金",
		"取得借款收到的现金",
		"发行债券收到的现金",
		"收到其他与筹资活动有关的现金",
		"筹资活动现金流入小计",
		"偿还债务支付的现金",
		"分配股利、利润或偿付利息所支付的现金",
		"支付其他与筹资活动有关的现金",
		"筹资活动现金流出小计",
		"筹资活动产生的现金流量净额",
		"附注",
		"四、汇率变动对现金及现金等价物的影响",
		"五、现金及现金等价物净增加额",
		"加:期初现金及现金等价物余额",
		"六、期末现金及现金等价物余额",
		"净利润",
		"少数股东权益",
		"未确认的投资损失",
		"资产减值准备",
		"固定资产折旧、油气资产折耗、生产性物资折旧",
		"无形资产摊销",
		"长期待摊费用摊销",
		"待摊费用的减少",
		"预提费用的增加",
		"处置固定资产、无形资产和其他长期资产的损失",
		"固定资产报废损失",
		"公允价值变动损失",
		"递延收益增加（减：减少）",
		"预计负债",
		"财务费用",
		"投资损失",
		"递延所得税资产减少",
		"递延所得税负债增加",
		"存货的减少",
		"经营性应收项目的减少",
		"经营性应付项目的增加",
		"已完工尚未结算款的减少(减:增加)",
		"已结算尚未完工款的增加(减:减少)",
		"其他",
		"经营活动产生现金流量净额",
		"债务转为资本",
		"一年内到期的可转换公司债券",
		"融资租入固定资产",
		"现金的期末余额",
		"现金的期初余额",
		"现金等价物的期末余额",
		"现金等价物的期初余额",
		"现金及现金等价物的净增加额"
	};

	for(int i=0; i< 93; i++)
	{
		m_row_name_2_index[header[i]] = i;
	}
}


CCashFlows::CCashFlows(void)
{
	Initialize();
}

CCashFlows::CCashFlows(const CCashFlows& rhs)
{
	if (this != &rhs)
	{
		Initialize();
		ReleaseVector();

		m_vec_report_date = rhs.m_vec_report_date;
		m_nRowNum = rhs.m_nRowNum;
		m_nCode = rhs.m_nCode;
		m_ConnectString = rhs.m_ConnectString;
		m_initial_report_date = rhs.m_initial_report_date;

		for(int row=1; row<= m_vec_index.size(); row++)
		{
			if(rhs.m_vec_index.count(row)>0)
			{
				vector<double> * pMap = rhs.m_vec_index.find(row)->second;
				vector<double>::iterator it = pMap->begin();
				vector<double>::iterator it_end = pMap->end();
				for(; it!=it_end; it++)
				{
					//m_vec_index[row]->insert(*it);
					m_vec_index[row]->insert(m_vec_index[row]->end(),*it);
				}
			}

		}
	}

}

CCashFlows::~CCashFlows(void)
{
	Initialize();
	ReleaseVector();
}

CCashFlows& CCashFlows::operator=(const CCashFlows& rhs)
{
	if (this != &rhs)
	{
		Initialize();
		ReleaseVector();

		m_vec_report_date = rhs.m_vec_report_date;
		m_nRowNum = rhs.m_nRowNum;
		m_nCode = rhs.m_nCode;
		m_ConnectString = rhs.m_ConnectString;
		m_initial_report_date = rhs.m_initial_report_date;

		for(int row=1; row<= m_vec_index.size(); row++)
		{
			if(rhs.m_vec_index.count(row)>0)
			{
				vector<double> * pMap = rhs.m_vec_index.find(row)->second;
				vector<double>::iterator it = pMap->begin();
				vector<double>::iterator it_end = pMap->end();
				for(; it!=it_end; it++)
				{
					//m_vec_index[row]->insert(*it);
					m_vec_index[row]->insert(m_vec_index[row]->end(),*it);
				}
			}

		}
	}
	return *this;
}


int CCashFlows::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
{
	
	string strRet;
	for(int row = 0; row < arrayData.size(); row++)
	{
		//	根据 第一列的 报表项名称，匹配行坐标
		string  indexName = arrayData[row][0];
		if(m_row_name_2_index.count(indexName)==0)
		{
			//return CSV_CHECK_FORMAT_ERROR;
			if(!strRet.empty())
				strRet += ",";
			strRet += indexName;
		}
	}

	if(!strRet.empty())
	{
		if(strRet != "其中：子公司支付给少数股东的股利、利润")
		{
			stringstream ss;
			ss << "现金流量表 Format Error， code = " << m_nCode << ", " << strRet;
			LOG4CPLUS_DEBUG(g_logger, ss.str());

			return CSV_CHECK_FORMAT_ERROR;
		}
	}
	return CSV_CHECK_NO_ERROR;
}


CStockDataModel * CCashFlows::NewCopy()
{
	CCashFlows * pTrading = new CCashFlows();
	pTrading->m_nRowNum = m_nRowNum;
	
	pTrading->m_vec_report_date = m_vec_report_date;

	return pTrading;
}

string CCashFlows::GetDownloadUrl()
{
	string url ="";
	char cmdline[512]; 
	CString tmp;

	
	string::size_type pos = GetCsvFileName().find(".csv");
	string xlsFileName = GetCsvFileName().substr(0, pos);
	xlsFileName += ".xls";

	/*tmp.Format("http://money.finance.sina.com.cn/corp/go.php/vDOWN_CashFlow/displaytype/4/stockid/%06d/ctrl/all.phtml", \
		m_nCode);  
	string  strTmp = tmp.GetBuffer();
	string  strGBK1 =CUrlUtils::PostUrlOfSouhu(tmp.GetBuffer(),NULL);

	CStdioFile  targetFile(xlsFileName.c_str(), CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText);
	targetFile.WriteString(strGBK1.c_str());
	targetFile.Close();*/

	/*停止使用 wget.exe 进行下载*/
	//return "";

	//////////////////////////////////////////////////////////////////////////
	//	对于本日已经成功下载的 ，忽略该任务

	if(CheckDownloadFileInfo(xlsFileName)>0)
		return url;

	tmp.Format("wget.exe http://money.finance.sina.com.cn/corp/go.php/vDOWN_CashFlow/displaytype/4/stockid/%06d/ctrl/all.phtml -O %s", \
		m_nCode, xlsFileName.c_str());    

	sprintf(cmdline,"%s",tmp);    
	url = string(cmdline);
	return url;
}
