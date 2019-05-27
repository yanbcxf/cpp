#include "StdAfx.h"
#include "Balance.h"
#include "UrlUtils.h"

void CBalance::Initialize(void)
{
	m_strChineseName = _T("资产负债表");
	m_strEnglishName = "Balance";

	// m_vec_index 中数组下标 与数据库中字段 colume_x 中的 x 相对应
	m_vec_index[1] =	&m_vec_current_assets		;  //		"流动资产",                                                 
	m_vec_index[2] =	&m_vec_cash		;  //		"货币资金",                                                           
	m_vec_index[3] =	&m_vec_provision_of_settlement_fund		;  //		"结算备付金",                                 
	m_vec_index[4] =	&m_vec_funds_lent		;  //		"拆出资金",                                                     
	m_vec_index[5] =	&m_vec_financial_assets_held_for_trading		;  //		"交易性金融资产",                       
	m_vec_index[6] =	&m_vec_derivative_financial_assets		;  //		"衍生金融资产",                               
	m_vec_index[07] =	&m_vec_notes_receivable		;  //		"应收票据",                                               
	m_vec_index[8] =	&m_vec_accounts_receivable		;  //		"应收账款",                                           
	m_vec_index[9] =	&m_vec_advances_to_suppliers		;  //		"预付款项",                                         
	m_vec_index[10] =	&m_vec_insurance_premiums_receivable		;  //		"应收保费",                                 
	m_vec_index[11] =	&m_vec_cession_premiums_receivable		;  //		"应收分保账款",                               
	m_vec_index[12] =	&m_vec_provision_of_cession_receivable		;  //		"应收分保合同准备金",                     
	m_vec_index[13] =	&m_vec_interests_receivable		;  //		"应收利息",                                           
	m_vec_index[14] =	&m_vec_dividends_receivable		;  //		"应收股利",                                           
	m_vec_index[15] =	&m_vec_other_receivable		;  //		"其他应收款",                                             
	m_vec_index[16] =	&m_vec_export_tax_rebate		;  //		"应收出口退税",                                         
	m_vec_index[17] =	&m_vec_subsidy_receivable		;  //		"应收补贴款",                                           
	m_vec_index[18] =	&m_vec_receivable_deposit		;  //		"应收保证金",                                           
	m_vec_index[19] =	&m_vec_internal_receivables		;  //		"内部应收款",                                         
	m_vec_index[20] =	&m_vec_buying_and_selling_financial_assets		;  //		"买入返售金融资产",                   
	m_vec_index[21] =	&m_vec_inventories		;  //		"存货",                                                       
	m_vec_index[22] =	&m_vec_deferred_expenses 		;  //		"待摊费用",                                             
	m_vec_index[23] =	&m_vec_profit_and_loss_to_be_treated		;  //		"待处理流动资产损益",                       
	m_vec_index[24] =	&m_vec_non_current_assets_maturing_within_one_year		;  //		"一年内到期的非流动资产",     
	m_vec_index[25] =	&m_vec_other_current_assets		;  //		"其他流动资产",                                       
	m_vec_index[26] =	&m_vec_total_current_assets		;  //		"流动资产合计",                                       
	m_vec_index[27] =	&m_vec_non_current_assets		;  //		"非流动资产",                                           
	m_vec_index[28] =	&m_vec_loans_and_payments_on_behalf 		;  //		"发放贷款及垫款",                           
	m_vec_index[29] =	&m_vec_available_for_sale_financial_assets		;  //		"可供出售金融资产",                   
	m_vec_index[30] =	&m_vec_held_to_maturity_investments		;  //		"持有至到期投资",                             
	m_vec_index[31] =	&m_vec_long_term_receivables		;  //		"长期应收款",                                       
	m_vec_index[32] =	&m_vec_long_term_equity_investments		;  //		"长期股权投资",                               
	m_vec_index[33] =	&m_vec_other_long_term_investments		;  //		"其他长期投资",                               
	m_vec_index[34] =	&m_vec_investment_real_estates		;  //		"投资性房地产",                                   
	m_vec_index[35] =	&m_vec_original_value_of_fixed_assets 		;  //		"固定资产原值",                           
	m_vec_index[36] =	&m_vec_accumulated_depreciation		;  //		"累计折旧",                                       
	m_vec_index[37] =	&m_vec_fixed_assets_net_value		;  //		"固定资产净值",                                     
	m_vec_index[38] =	&m_vec_fixed_assets_impairment_provision		;  //		"固定资产减值准备",                     
	m_vec_index[39] =	&m_vec_fixed_assets_net_book_value		;  //		"固定资产净额",                               
	m_vec_index[40] =	&m_vec_construction_in_progress		;  //		"在建工程",                                       
	m_vec_index[41] =	&m_vec_construction_supplies		;  //		"工程物资",                                         
	m_vec_index[42] =	&m_vec_fixed_assets_pending_disposal		;  //		"固定资产清理",                             
	m_vec_index[43] =	&m_vec_bearer_biological_assets		;  //		"生产性生物资产",                                 
	m_vec_index[44] =	&m_vec_public_welfare_biological_assets		;  //		"公益性生物资产",                         
	m_vec_index[45] =	&m_vec_oil_and_natural_gas_assets		;  //		"油气资产",                                     
	m_vec_index[46] =	&m_vec_intangibel_assets		;  //		"无形资产",                                             
	m_vec_index[47] =	&m_vec_research_and_development_costs		;  //		"开发支出",                                 
	m_vec_index[48] =	&m_vec_goodwill		;  //		"商誉",                                                           
	m_vec_index[49] =	&m_vec_long_term_deferred_expenses		;  //		"长期待摊费用",                               
	m_vec_index[50] =	&m_vec_circulation_right_for_equity_separation 		;  //		"股权分置流通权",                 
	m_vec_index[51] =	&m_vec_deferred_tax_assets		;  //		"递延所得税资产",                                     
	m_vec_index[52] =	&m_vec_other_non_current_assets		;  //		"其他非流动资产",                                 
	m_vec_index[53] =	&m_vec_total_non_current_assets		;  //		"非流动资产合计",                                 
	m_vec_index[54] =	&m_vec_total_assets		;  //		"资产总计",                                                   
	m_vec_index[55] =	&m_vec_current_debt		;  //		"流动负债",                                                   
	m_vec_index[56] =	&m_vec_short_term_borrowings		;  //		"短期借款",                                         
	m_vec_index[57] =	&m_vec_borrowings_from_central_bank		;  //		"向中央银行借款",                             
	m_vec_index[58] =	&m_vec_deposits_from_customers_and_interbank		;  //		"吸收存款及同业存放",               
	m_vec_index[59] =	&m_vec_deposit_funds		;  //		"拆入资金",                                                 
	m_vec_index[60] =	&m_vec_financial_assets_held_for_liabilities		;  //		"交易性金融负债",                   
	m_vec_index[61] =	&m_vec_derivative_financial_liabilities		;  //		"衍生金融负债",                           
	m_vec_index[62] =	&m_vec_notes_payable		;  //		"应付票据",                                                 
	m_vec_index[63] =	&m_vec_accounts_payable		;  //		"应付账款",                                               
	m_vec_index[64] =	&m_vec_advances_from_customers		;  //		"预收款项",                                       
	m_vec_index[65] =	&m_vec_funds_from_sales_of_financial_assets_with_repurchasement_agreement		;  //		"卖出回购金融资产款",
	m_vec_index[66] =	&m_vec_handling_charges_and_commissions_payable		;  //		"应付手续费及佣金",               
	m_vec_index[67] =	&m_vec_employee_benefits_payable		;  //		"应付职工薪酬",                                 
	m_vec_index[68] =	&m_vec_taxes_and_surcharges_payable		;  //		"应交税费",                                   
	m_vec_index[69] =	&m_vec_interests_payable		;  //		"应付利息",                                             
	m_vec_index[70] =	&m_vec_dividends_payable		;  //		"应付股利",                                             
	m_vec_index[71] =	&m_vec_other_accounts_payable		;  //		"其他应交款",                                       
	m_vec_index[72] =	&m_vec_margin_payable		;  //		"应付保证金",                                               
	m_vec_index[73] =	&m_vec_internal_accounts_payable		;  //		"内部应付款",                                   
	m_vec_index[74] =	&m_vec_other_payables		;  //		"其他应付款",                                               
	m_vec_index[75] =	&m_vec_accrued_expenses 		;  //		"预提费用",                                             
	m_vec_index[76] =	&m_vec_expected_current_liabilities		;  //		"预计流动负债",                               
	m_vec_index[77] =	&m_vec_cession_insurance_premiums_payable		;  //		"应付分保账款",                         
	m_vec_index[78] =	&m_vec_provision_for_insurance_contracts		;  //		"保险合同准备金",                       
	m_vec_index[79] =	&m_vec_funds_received_as_agent_of_stock_exchange 		;  //		"代理买卖证券款",               
	m_vec_index[80] =	&m_vec_funds_received_as_stock_underwrite		;  //		"代理承销证券款",                       
	m_vec_index[81] =	&m_vec_international_ticket_clearing		;  //		"国际票证结算",                             
	m_vec_index[82] =	&m_vec_domestic_ticket_clearing		;  //		"国内票证结算",                                   
	m_vec_index[83] =	&m_vec_deferred_income		;  //		"递延收益",                                               
	m_vec_index[84] =	&m_vec_short_term_bonds_payable		;  //		"应付短期债券",                                   
	m_vec_index[85] =	&m_vec_non_current_liabilities_maturing_within_one_year		;  //		"一年内到期的非流动负债", 
	m_vec_index[86] =	&m_vec_other_current_liablities		;  //		"其他流动负债",                                   
	m_vec_index[87] =	&m_vec_total_current_liabilities		;  //		"流动负债合计",                                 
	m_vec_index[88] =	&m_vec_non_current_liabilities		;  //		"非流动负债",                                     
	m_vec_index[89] =	&m_vec_long_term_loans		;  //		"长期借款",                                               
	m_vec_index[90] =	&m_vec_debentures_payable		;  //		"应付债券",                                             
	m_vec_index[91] =	&m_vec_long_term_payables		;  //		"长期应付款",                                           
	m_vec_index[92] =	&m_vec_specific_payable		;  //		"专项应付款",                                             
	m_vec_index[93] =	&m_vec_expected_non_current_liabilities		;  //		"预计非流动负债",                         
	m_vec_index[94] =	&m_vec_deferred_tax_liabilities		;  //		"递延所得税负债",                                 
	m_vec_index[95] =	&m_vec_other_non_current_liablities		;  //		"其他非流动负债",                             
	m_vec_index[96] =	&m_vec_total_non_current_liabilities		;  //		"非流动负债合计",                           
	m_vec_index[97] =	&m_vec_total_liability		;  //		"负债合计",                                               
	m_vec_index[98] =	&m_vec_owners_equity		;  //		"所有者权益",                                               
	m_vec_index[99] =	&m_vec_registered_capital		;  //		"实收资本(或股本)"                                      
	m_vec_index[100] =	&m_vec_capital_surplus		;  //		"资本公积"                                                
	m_vec_index[101] =	&m_vec_treasury_stock		;  //		"库存股"                                                  
	m_vec_index[102] =	&m_vec_special_reserve		;  //		"专项储备"                                                
	m_vec_index[103] =	&m_vec_surplus_reserve		;  //		"盈余公积",                                               
	m_vec_index[104] =	&m_vec_provision_for_normal_risks		;  //		"一般风险准备",                               
	m_vec_index[105] =	&m_vec_not_sure_investment_losses		;  //		"未确定的投资损失",                           
	m_vec_index[106] =	&m_vec_undistributed_profits		;  //		"未分配利润",                                       
	m_vec_index[107] =	&m_vec_quasi_cash_dividend		;  //		"拟分配现金股利",                                     
	m_vec_index[108] =	&m_vec_exchange_differences_on_translating_foreign_operations 		;  //		"外币报表折算差额"
	m_vec_index[109] =	&m_vec_total_owners_equity_belongs_to_parent_company		;  //		"归属于母公司股东权益合计", 
	m_vec_index[110] =	&m_vec_minority_interest		;  //		"少数股东权益",                                         
	m_vec_index[111] =	&m_vec_total_owners_equity		;  //		"所有者权益(或股东权益)合计",                         
	m_vec_index[112] =	&m_vec_total_liabilities_and_owners_equity		;  //		"负债和所有者权益(或股东权益)总计"    

	/*m_vec_report_date.clear();
	for(int row=1; row<= m_vec_index.size(); row++)
		m_vec_index[row]->clear();*/

	string header[] = {
		"单位",
		"流动资产",
		"货币资金",
		"结算备付金",
		"拆出资金",
		"交易性金融资产",
		"衍生金融资产",
		"应收票据",
		"应收账款",
		"预付款项",
		"应收保费",
		"应收分保账款",
		"应收分保合同准备金",
		"应收利息",
		"应收股利",
		"其他应收款",
		"应收出口退税",
		"应收补贴款",
		"应收保证金",
		"内部应收款",
		"买入返售金融资产",
		"存货",
		"待摊费用",
		"待处理流动资产损益",
		"一年内到期的非流动资产",
		"其他流动资产",
		"流动资产合计",
		"非流动资产",
		"发放贷款及垫款",
		"可供出售金融资产",
		"持有至到期投资",
		"长期应收款",
		"长期股权投资",
		"其他长期投资",
		"投资性房地产",
		"固定资产原值",
		"累计折旧",
		"固定资产净值",
		"固定资产减值准备",
		"固定资产净额",
		"在建工程",
		"工程物资",
		"固定资产清理",
		"生产性生物资产",
		"公益性生物资产",
		"油气资产",
		"无形资产",
		"开发支出",
		"商誉",
		"长期待摊费用",
		"股权分置流通权",
		"递延所得税资产",
		"其他非流动资产",
		"非流动资产合计",
		"资产总计",
		"流动负债",
		"短期借款",
		"向中央银行借款",
		"吸收存款及同业存放",
		"拆入资金",
		"交易性金融负债",
		"衍生金融负债",
		"应付票据",
		"应付账款",
		"预收款项",
		"卖出回购金融资产款",
		"应付手续费及佣金",
		"应付职工薪酬",
		"应交税费",
		"应付利息",
		"应付股利",
		"其他应交款",
		"应付保证金",
		"内部应付款",
		"其他应付款",
		"预提费用",
		"预计流动负债",
		"应付分保账款",
		"保险合同准备金",
		"代理买卖证券款",
		"代理承销证券款",
		"国际票证结算",
		"国内票证结算",
		"递延收益",
		"应付短期债券",
		"一年内到期的非流动负债",
		"其他流动负债",
		"流动负债合计",
		"非流动负债",
		"长期借款",
		"应付债券",
		"长期应付款",
		"专项应付款",
		"预计非流动负债",
		"递延所得税负债",
		"其他非流动负债",
		"非流动负债合计",
		"负债合计",
		"所有者权益",
		"实收资本(或股本)",
		"资本公积",
		"减：库存股",
		"专项储备",
		"盈余公积",
		"一般风险准备",
		"未确定的投资损失",
		"未分配利润",
		"拟分配现金股利",
		"外币报表折算差额",
		"归属于母公司股东权益合计",
		"少数股东权益",
		"所有者权益(或股东权益)合计",
		"负债和所有者权益(或股东权益)总计"
	};

	for(int i=0; i< 113; i++)
	{
		m_row_name_2_index[header[i]] = i;
	}
}


CBalance::CBalance(void)
{
	Initialize();
}

CBalance::CBalance(const CBalance& rhs)
{
	if (this != &rhs)
	{
		Initialize();
		ReleaseVector();

		m_vec_report_date = rhs.m_vec_report_date;
		m_nRowNum = rhs.m_nRowNum;
		m_nCode = rhs.m_nCode;
		m_initial_report_date = rhs.m_initial_report_date;
		m_ConnectString = rhs.m_ConnectString;
		for(int row=1; row<= m_vec_index.size(); row++)
		{
			if(rhs.m_vec_index.count(row)>0)
			{
				vector<double> * pMap = rhs.m_vec_index.find(row)->second;
				vector<double>::iterator it = pMap->begin();
				vector<double>::iterator it_end = pMap->end();
				for(; it!=it_end; it++)
				{
					m_vec_index[row]->insert(m_vec_index[row]->end(),*it);
				}
			}

		}
	}

}


CBalance& CBalance::operator=(const CBalance& rhs)
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


CBalance::~CBalance(void)
{
	Initialize();
	ReleaseVector();

}



int CBalance::CheckReport(vector<string>&  arrayHeader, vector<vector<string>>&  arrayData)
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
				strRet += "|";
			strRet += indexName;
		}
	}

	if(!strRet.empty())
	{
		if(strRet!="划分为持有待售的资产|一年内的递延收益|长期应付职工薪酬|长期递延收益|其他综合收益")
		{
			stringstream ss;
			ss << "资产负债表 Format Error， code = " << m_nCode << ", " << strRet;
			LOG4CPLUS_DEBUG(g_logger, ss.str());

			return CSV_CHECK_FORMAT_ERROR;
		}
	}
	return CSV_CHECK_NO_ERROR;
}

string CBalance::GetDownloadUrl()
{
	string url = "";
	char cmdline[512]; 
	CString tmp;     

	string::size_type pos = GetCsvFileName().find(".csv");
	string xlsFileName = GetCsvFileName().substr(0, pos);
	xlsFileName += ".xls";

	/*tmp.Format("http://money.finance.sina.com.cn/corp/go.php/vDOWN_BalanceSheet/displaytype/4/stockid/%06d/ctrl/all.phtml", \
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

	tmp.Format("wget.exe http://money.finance.sina.com.cn/corp/go.php/vDOWN_BalanceSheet/displaytype/4/stockid/%06d/ctrl/all.phtml -O %s", \
		m_nCode, xlsFileName.c_str());    

	sprintf(cmdline,"%s",tmp);    
	url = string(cmdline);
	return url;
}


CStockDataModel * CBalance::NewCopy()
{
	CBalance * pTrading = new CBalance();
	pTrading->m_nRowNum = m_nRowNum;

	pTrading->m_vec_report_date = m_vec_report_date;

	return pTrading;
}