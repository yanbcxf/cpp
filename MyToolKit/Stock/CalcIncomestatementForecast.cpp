#include "StdAfx.h"
#include "CalcIncomestatementForecast.h"

#include "nr.h"

CCalcIncomestatementForecast::CCalcIncomestatementForecast(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcIncomestatementForecast::~CCalcIncomestatementForecast(void)
{
}

void CCalcIncomestatementForecast::Execute1(void* firstArg)
{
	CCalcIncomestatementForecast * sink = (CCalcIncomestatementForecast *)firstArg;
	sink->Execute();
}



DP splint(Vec_I_DP &xa, Vec_I_DP &ya, const DP x)
{
	//	计算二阶导数

	DP	yp1 = (ya[1] - ya[0]) / (xa[1] - xa[0]);
	DP	ypn = (ya[xa.size()-1] - ya[xa.size()-2]) / (xa[xa.size()-1] - xa[xa.size()-2]);
	Vec_DP y2(xa.size());
	NR::spline(xa, ya, yp1, ypn ,y2);

	//	计算三次样条插值法

	DP y;
	NR::splint(xa, ya, y2, x, y);
	return y;
}


void NeedQuarter(int nYear, int nMonth, int nDay, COleDateTime dt, vector<string> & vec)
{
	COleDateTime dt1;
	char tmp[32] = { 0 };
	snprintf(tmp, 32, "%04d-%02d-%02d", nYear, nMonth, nDay);
	string str = string(tmp);
	dt1.ParseDateTime(str.c_str());
	if(dt1 > dt && dt1.m_dt < dt.m_dt + 400)
	{
		vec.insert(vec.end(), str);
	}
}

void CCalcIncomestatementForecast::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_request_code.size()<=0)
		return;

	stringstream ss;
	

	try{
		
		session sql(g_MysqlPool);
		//////////////////////////////////////////////////////////////////////////

		m_IncomeStatement.m_nCode = atoi(m_vec_request_code[0].c_str());
		m_IncomeStatement.ExportFromDatabase();

				
		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		
		for(int i=0; i< m_IncomeStatement.m_vec_report_date.size(); i++)
		{
			AddPoint("NO.01", "营业收入", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_sales_from_operations[i]);

			AddPoint("NO.02", "营业成本", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_cost_of_operations[i]);

			AddPoint("NO.03", "营业税金及附加", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_sales_tax_and_additions[i]);

			AddPoint("NO.04", "销售费用", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_selling_and_distribution_expenses[i]);

			AddPoint("NO.05", "管理费用", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_general_and_administrative_expenses[i]);

			AddPoint("NO.06", "财务费用", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_financial_expenses[i]);

			AddPoint("NO.07", "资产减值损失", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_impairment_loss_on_assets[i]);

			AddPoint("NO.08", "投资收益", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_investment_income[i]);

			//////////////////////////////////////////////////////////////////////////
			AddPoint("NO.09", "营业利润", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_profit_from_operations[i]);

			AddPoint("NO.10", "营业外收入", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_non_operating_profit[i]);

			AddPoint("NO.11", "营业外支出", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_non_operating_expenses[i]);

			AddPoint("NO.12", "利润总额", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_profit_before_tax[i]);

			AddPoint("NO.13", "所得税费用", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_income_tax_expenses[i]);

			AddPoint("NO.14", "净利润", 
				m_IncomeStatement.m_vec_report_date[i], "元", 
				m_IncomeStatement.m_vec_net_profit[i]);
		}

		//////////////////////////////////////////////////////////////////////////
		//	今年、明年预测

		vector<double>  q1;
		vector<double>  q2;
		vector<double>  q3;
		vector<double>  q4;

		for(int i=0; i< m_IncomeStatement.m_vec_report_date.size(); i++)
		{
			COleDateTime dt;
			dt.ParseDateTime(m_IncomeStatement.m_vec_report_date[i].c_str());
			if(m_IncomeStatement.m_vec_report_date[i].find("3-31")!=string::npos)
			{
				q1.insert(q1.end(), dt);
			}
			if(m_IncomeStatement.m_vec_report_date[i].find("6-30")!=string::npos)
			{
				q2.insert(q2.end(), dt);
			}
			if(m_IncomeStatement.m_vec_report_date[i].find("9-30")!=string::npos)
			{
				q3.insert(q3.end(), dt);
			}
			if(m_IncomeStatement.m_vec_report_date[i].find("-12-31")!=string::npos)
			{
				q4.insert(q4.end(), dt);
			}
		}

		Vec_DP  quarter1(q1.size());
		Vec_DP  quarter2(q2.size());
		Vec_DP  quarter3(q3.size());
		Vec_DP  quarter4(q4.size());

		for(int i=0 ; i<q1.size(); i++) quarter1[i] = q1[i];
		for(int i=0 ; i<q2.size(); i++) quarter2[i] = q2[i];
		for(int i=0 ; i<q3.size(); i++) quarter3[i] = q3[i];
		for(int i=0 ; i<q4.size(); i++) quarter4[i] = q4[i];

		
		map<int, Vec_DP>  quarter_sales_from_operations;	// 营业收入
		map<int, Vec_DP>  quarter_cost_from_operations;	// 营业成本
		map<int, Vec_DP>  quarter_sales_tax_and_additions;	// 营业税金及附加
		map<int, Vec_DP>  quarter_selling_and_distribution_expenses;	// 销售费用
		map<int, Vec_DP>  quarter_general_and_administrative_expenses;	//	管理费用
		map<int, Vec_DP>  quarter_financial_expenses;		//	财务费用

		map<int, Vec_DP>  quarter_impairment_loss_on_assets;	//	资产减值损失
		map<int, Vec_DP>  quarter_investment_income;			//	投资收益
		map<int, Vec_DP>  quarter_profit_from_operations;	//	营业利润
		map<int, Vec_DP>  quarter_non_operating_profit;	//	营业外收入
		map<int, Vec_DP>  quarter_non_operating_expenses;	//	营业外支出
		map<int, Vec_DP>  quarter_profit_before_tax;	//	利润总额
		map<int, Vec_DP>  quarter_income_tax_expenses;	//	所得税费用
		map<int, Vec_DP>  quarter_net_profit;	//	净利润


		
		quarter_sales_from_operations[1]= quarter1;
		quarter_sales_from_operations[2]= quarter2;
		quarter_sales_from_operations[3]= quarter3;
		quarter_sales_from_operations[4]= quarter4;

		quarter_cost_from_operations[1]= quarter1;
		quarter_cost_from_operations[2]= quarter2;
		quarter_cost_from_operations[3]= quarter3;
		quarter_cost_from_operations[4]= quarter4;

		quarter_sales_tax_and_additions[1]= quarter1;
		quarter_sales_tax_and_additions[2]= quarter2;
		quarter_sales_tax_and_additions[3]= quarter3;
		quarter_sales_tax_and_additions[4]= quarter4;

		quarter_selling_and_distribution_expenses[1]= quarter1;
		quarter_selling_and_distribution_expenses[2]= quarter2;
		quarter_selling_and_distribution_expenses[3]= quarter3;
		quarter_selling_and_distribution_expenses[4]= quarter4;

		quarter_general_and_administrative_expenses[1]= quarter1;
		quarter_general_and_administrative_expenses[2]= quarter2;
		quarter_general_and_administrative_expenses[3]= quarter3;
		quarter_general_and_administrative_expenses[4]= quarter4;

		quarter_financial_expenses[1]= quarter1;
		quarter_financial_expenses[2]= quarter2;
		quarter_financial_expenses[3]= quarter3;
		quarter_financial_expenses[4]= quarter4;

		quarter_impairment_loss_on_assets[1]= quarter1;
		quarter_impairment_loss_on_assets[2]= quarter2;
		quarter_impairment_loss_on_assets[3]= quarter3;
		quarter_impairment_loss_on_assets[4]= quarter4;

		quarter_investment_income[1]= quarter1;
		quarter_investment_income[2]= quarter2;
		quarter_investment_income[3]= quarter3;
		quarter_investment_income[4]= quarter4;

		quarter_profit_from_operations[1]= quarter1;
		quarter_profit_from_operations[2]= quarter2;
		quarter_profit_from_operations[3]= quarter3;
		quarter_profit_from_operations[4]= quarter4;

		quarter_non_operating_profit[1]= quarter1;
		quarter_non_operating_profit[2]= quarter2;
		quarter_non_operating_profit[3]= quarter3;
		quarter_non_operating_profit[4]= quarter4;

		quarter_non_operating_expenses[1]= quarter1;
		quarter_non_operating_expenses[2]= quarter2;
		quarter_non_operating_expenses[3]= quarter3;
		quarter_non_operating_expenses[4]= quarter4;

		quarter_profit_before_tax[1]= quarter1;
		quarter_profit_before_tax[2]= quarter2;
		quarter_profit_before_tax[3]= quarter3;
		quarter_profit_before_tax[4]= quarter4;

		quarter_income_tax_expenses[1]= quarter1;
		quarter_income_tax_expenses[2]= quarter2;
		quarter_income_tax_expenses[3]= quarter3;
		quarter_income_tax_expenses[4]= quarter4;

		quarter_net_profit[1]= quarter1;
		quarter_net_profit[2]= quarter2;
		quarter_net_profit[3]= quarter3;
		quarter_net_profit[4]= quarter4;

		int q = 1;
		int q1_cnt = 0;
		int q2_cnt = 0;
		int q3_cnt = 0;
		int q4_cnt = 0;
		for(int i=0; i< m_IncomeStatement.m_vec_report_date.size(); i++)
		{
			
			if(m_IncomeStatement.m_vec_report_date[i].find("3-31")!=string::npos)
			{
				q = 1;
				q1_cnt ++;
			}

			if(m_IncomeStatement.m_vec_report_date[i].find("6-30")!=string::npos)
			{
				q =2;
				q2_cnt ++;
			}
			if(m_IncomeStatement.m_vec_report_date[i].find("9-30")!=string::npos)
			{
				q =3;
				q3_cnt ++;
			}
			if(m_IncomeStatement.m_vec_report_date[i].find("-12-31")!=string::npos)
			{
				q = 4;
				q4_cnt ++;
			}

			int idx;
			switch(q)
			{
			case 1: idx = q1_cnt; break;
			case 2: idx = q2_cnt; break;
			case 3: idx = q3_cnt; break;
			case 4: idx = q4_cnt; break;
			}
			idx--;

			quarter_sales_from_operations[q][idx] = m_IncomeStatement.m_vec_sales_from_operations[i];
			quarter_cost_from_operations[q][idx] = 	m_IncomeStatement.m_vec_cost_of_operations[i];
			quarter_sales_tax_and_additions[q][idx] = m_IncomeStatement.m_vec_sales_tax_and_additions[i];
			quarter_selling_and_distribution_expenses[q][idx] = m_IncomeStatement.m_vec_selling_and_distribution_expenses[i];
			quarter_general_and_administrative_expenses[q][idx] = m_IncomeStatement.m_vec_general_and_administrative_expenses[i];
			quarter_financial_expenses[q][idx] = m_IncomeStatement.m_vec_financial_expenses[i];

			quarter_impairment_loss_on_assets[q][idx] = m_IncomeStatement.m_vec_impairment_loss_on_assets[i];
			quarter_investment_income[q][idx] = m_IncomeStatement.m_vec_investment_income[i];
			quarter_profit_from_operations[q][idx] = m_IncomeStatement.m_vec_profit_from_operations[i];
			quarter_non_operating_profit[q][idx] = m_IncomeStatement.m_vec_non_operating_profit[i];
			quarter_non_operating_expenses[q][idx] = m_IncomeStatement.m_vec_non_operating_expenses[i];
			quarter_profit_before_tax[q][idx] = m_IncomeStatement.m_vec_profit_before_tax[i];
			quarter_income_tax_expenses[q][idx] = m_IncomeStatement.m_vec_income_tax_expenses[i];
			quarter_net_profit[q][idx] = m_IncomeStatement.m_vec_net_profit[i];
		}
		
		//	计算需预测的季度
		COleDateTime dt;
		dt.ParseDateTime(m_IncomeStatement.m_vec_report_date[m_IncomeStatement.m_vec_report_date.size()-1].c_str());
		dt.m_dt += 1;		//	以最后一个的季度为起点，开始计算随后的两年
		int firstYear = dt.GetYear();
		vector<string>	needQuarter;
		
		NeedQuarter(firstYear , 3, 31, dt, needQuarter);
		NeedQuarter(firstYear , 6, 30, dt, needQuarter);
		NeedQuarter(firstYear , 9, 30, dt, needQuarter);
		NeedQuarter(firstYear , 12, 31, dt, needQuarter);
		NeedQuarter(firstYear + 1, 3, 31, dt, needQuarter);
		NeedQuarter(firstYear + 1, 6, 30, dt, needQuarter);
		NeedQuarter(firstYear + 1, 9, 30, dt, needQuarter);
		NeedQuarter(firstYear + 1, 12, 31, dt, needQuarter);


		for(int k = 0; k<needQuarter.size(); k++)
		{
			COleDateTime odt;
			odt.ParseDateTime(needQuarter[k].c_str());
			int q = -1;
			Vec_DP  quarterTmp; 
			if(needQuarter[k].find("03-31")!=string::npos && quarter1.size()>3)
			{
				q = 1;
				quarterTmp = quarter1;
			}
			if(needQuarter[k].find("06-30")!=string::npos && quarter2.size()>3)
			{
				q = 2;
				quarterTmp = quarter2;
			}
			if(needQuarter[k].find("09-30")!=string::npos && quarter3.size()>3)
			{
				q = 3;
				quarterTmp = quarter3;
			}
			if(needQuarter[k].find("12-31")!=string::npos && quarter4.size()>3)
			{
				q = 4;
				quarterTmp = quarter4;
			}

			if(q>0)
			{
				AddPoint("NO.01", "营业收入", needQuarter[k], "预", splint(quarterTmp, quarter_sales_from_operations[q],odt.m_dt));
				AddPoint("NO.02", "营业成本", needQuarter[k], "预", splint(quarterTmp, quarter_cost_from_operations[q],odt.m_dt));
				AddPoint("NO.03", "营业税金及附加", needQuarter[k], "预", splint(quarterTmp, quarter_sales_tax_and_additions[q],odt.m_dt));
				AddPoint("NO.04", "销售费用", needQuarter[k], "预", splint(quarterTmp, quarter_selling_and_distribution_expenses[q],odt.m_dt));
				AddPoint("NO.05", "管理费用", needQuarter[k], "预", splint(quarterTmp, quarter_general_and_administrative_expenses[q],odt.m_dt));
				AddPoint("NO.06", "财务费用", needQuarter[k], "预", splint(quarterTmp, quarter_financial_expenses[q],odt.m_dt));

				AddPoint("NO.07", "资产减值损失", needQuarter[k], "预", splint(quarterTmp, quarter_impairment_loss_on_assets[q],odt.m_dt));
				AddPoint("NO.08", "投资收益", needQuarter[k], "预", splint(quarterTmp, quarter_investment_income[q],odt.m_dt));
				AddPoint("NO.09", "营业利润", needQuarter[k], "预", splint(quarterTmp, quarter_profit_from_operations[q],odt.m_dt));
				AddPoint("NO.10", "营业外收入", needQuarter[k], "预", splint(quarterTmp, quarter_non_operating_profit[q],odt.m_dt));
				AddPoint("NO.11", "营业外支出", needQuarter[k], "预", splint(quarterTmp, quarter_non_operating_expenses[q],odt.m_dt));
				AddPoint("NO.12", "利润总额", needQuarter[k], "预", splint(quarterTmp, quarter_profit_before_tax[q],odt.m_dt));
				AddPoint("NO.13", "所得税费用", needQuarter[k], "预", splint(quarterTmp, quarter_income_tax_expenses[q],odt.m_dt));
				AddPoint("NO.14", "净利润", needQuarter[k], "预", splint(quarterTmp, quarter_net_profit[q],odt.m_dt));
			}
		
		}
		
		

		//////////////////////////////////////////////////////////////////////////

		
		ss.str("");
		ss << "CCalcIncomestatementForecast 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}

