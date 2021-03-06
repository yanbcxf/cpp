
// LotteryDoc.cpp: CLotteryDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Lottery.h"
#endif

#include "LotteryDoc.h"
#include "CSVFile.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLotteryDoc

IMPLEMENT_DYNCREATE(CLotteryDoc, CDocument)

BEGIN_MESSAGE_MAP(CLotteryDoc, CDocument)
END_MESSAGE_MAP()


// CLotteryDoc 构造/析构

CLotteryDoc::CLotteryDoc()
{
	// TODO: 在此添加一次性构造代码
	
	//SetPathName("Lottery_test");

	m_fred_select = -1;
	m_bContainSpecial = false;
	m_strDataSource = "";
	m_strFilterSource = "";

	m_filter_start = m_filter_end = 0;
	m_nOperType = 0;		//	0 后台运行 ， 1 漏奖查询

	m_pai_analysis_periods = 10;
	m_pai_start = m_pai_end = 0;
	m_pai_mul_adjunstment = 10000;
	m_pai_value = 3.14159;

	m_ln_pai_analysis_periods = 10;
	m_ln_pai_start = m_ln_pai_end = 0;
	m_ln_pai_mul_adjunstment = 10000;
	m_ln_pai_value = 3.14159;

	m_swing_analysis_periods = 10;
	m_swing_start = m_swing_end = 0;
	m_swing_mul_adjunstment = 10000;

	m_ln_swing_analysis_periods = 10;
	m_ln_swing_start = m_ln_swing_end = 0;
	m_ln_swing_mul_adjunstment = 10000;

	m_bernoulli_analysis_periods = 10;
	m_bernoulli_start = m_bernoulli_end = 0;
	m_bernoulli_mul_adjunstment = 10000;

	m_potential_analysis_periods = 10;
	m_potential_start = m_potential_end = 0;
	m_potential_mul_adjunstment = 10000;
	m_potential_period_absolute = 0;
	
}

CLotteryDoc::~CLotteryDoc()
{
}

BOOL CLotteryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CLotteryDoc 序列化

void CLotteryDoc::Serialize(CArchive& ar)
{
	CString  str;
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
		str.Format("%s", m_lottery_info.code.c_str());
		ar << str;
		str.Format("%s", m_lottery_info.name.c_str());
		ar << str;
		ar << m_lottery_info.regularnum;
		ar << m_lottery_info.specialnum;
		ar << m_lottery_info.totalnum;
		ar << m_bContainSpecial;

		ar << m_strDataSource;
		ar << m_strFilterSource;

		// 统计特征
		ar << m_var_start.size();
		for (int i = 0; i < m_var_start.size(); i++)
		{
			ar << m_var_start[i];
			ar << m_var_end[i];
		}

		ar << m_devsq_start.size();
		for (int i = 0; i < m_devsq_start.size(); i++)
		{
			ar << m_devsq_start[i];
			ar << m_devsq_end[i];
		}

		ar << m_stdev_start.size();
		for (int i = 0; i < m_stdev_start.size(); i++)
		{
			ar << m_stdev_start[i];
			ar << m_stdev_end[i];
		}

		ar << m_kurt_start.size();
		for (int i = 0; i < m_kurt_start.size(); i++)
		{
			ar << m_kurt_start[i];
			ar << m_kurt_end[i];
		}

		ar << m_skew_start.size();
		for (int i = 0; i < m_skew_start.size(); i++)
		{
			ar << m_skew_start[i];
			ar << m_skew_end[i];
		}

		//	高级特征
		ar << m_fred_select;
		ar << m_fred_theta.size();
		for (int i = 0; i < m_fred_theta.size(); i++)
		{
			ar << m_fred_alpha[i];
			ar << m_fred_beta[i];
			ar << m_fred_upsilon[i];
			ar << m_fred_theta[i];
			ar << m_fred_eta[i];
		}

		ar << m_fred_start.size();
		for (map<int, double>::iterator it = m_fred_start.begin();   it!= m_fred_start.end(); it++)
		{
			ar << it->first;	ar << it->second;
		}
		ar << m_fred_end.size();
		for (map<int, double>::iterator it = m_fred_end.begin(); it != m_fred_end.end(); it++)
		{
			ar << it->first;	ar << it->second;
		}

		ar << m_ac_start.size();
		for (int i = 0; i < m_ac_start.size(); i++)
		{
			ar << m_ac_start[i];
			ar << m_ac_end[i];
		}

		ar << m_grp_qty_start.size();
		for (int i = 0; i < m_grp_qty_start.size(); i++)
		{
			ar << m_grp_qty_start[i];
			ar << m_grp_qty_end[i];
		}

		ar << m_fn_qty_start.size();
		for (int i = 0; i < m_fn_qty_start.size(); i++)
		{
			ar << m_fn_qty_start[i];
			ar << m_fn_qty_end[i];
		}

		// 基本特征
		ar << m_sum_start.size();
		for (int i = 0; i < m_sum_start.size(); i++)
		{
			ar << m_sum_start[i];
			ar << m_sum_end[i];
		}

		ar << m_odd.size();
		for (int i = 0; i < m_odd.size(); i++)
		{
			ar << m_odd[i];
		}

		// 相同尾数
		ar << m_tail_1.size();
		for (map<int, int>::iterator  it = m_tail_1.begin(); it != m_tail_1.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_tail_2.size();
		for (map<int, int>::iterator it = m_tail_2.begin(); it != m_tail_2.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_tail_3.size();
		for (map<int, int>::iterator it = m_tail_3.begin(); it != m_tail_3.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_tail_4.size();
		for (map<int, int>::iterator it = m_tail_4.begin(); it != m_tail_4.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_tail_5.size();
		for (map<int, int>::iterator it = m_tail_5.begin(); it != m_tail_5.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_tail_6.size();
		for (map<int, int>::iterator it = m_tail_6.begin(); it != m_tail_6.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}

		//	连号设置
		ar << m_consecutive_1.size();
		for (map<int, int>::iterator it = m_consecutive_1.begin(); it != m_consecutive_1.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_consecutive_2.size();
		for (map<int, int>::iterator it = m_consecutive_2.begin(); it != m_consecutive_2.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_consecutive_3.size();
		for (map<int, int>::iterator it = m_consecutive_3.begin(); it != m_consecutive_3.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_consecutive_4.size();
		for (map<int, int>::iterator it = m_consecutive_4.begin(); it != m_consecutive_4.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_consecutive_5.size();
		for (map<int, int>::iterator it = m_consecutive_5.begin(); it != m_consecutive_5.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}
		ar << m_consecutive_6.size();
		for (map<int, int>::iterator it = m_consecutive_6.begin(); it != m_consecutive_6.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}

		//	区间设置
		ar << m_region_1_start.size();
		for (int i = 0; i < m_region_1_start.size(); i++)
		{
			ar << m_region_1_start[i];	ar << m_region_1_end[i];
		}
		ar << m_region_2_start.size();
		for (int i = 0; i < m_region_2_start.size(); i++)
		{
			ar << m_region_2_start[i];	ar << m_region_2_end[i];
		}
		ar << m_region_3_start.size();
		for (int i = 0; i < m_region_3_start.size(); i++)
		{
			ar << m_region_3_start[i];	ar << m_region_3_end[i];
		}
		ar << m_region_4_start.size();
		for (int i = 0; i < m_region_4_start.size(); i++)
		{
			ar << m_region_4_start[i];	ar << m_region_4_end[i];
		}
		ar << m_region_5_start.size();
		for (int i = 0; i < m_region_5_start.size(); i++)
		{
			ar << m_region_5_start[i];	ar << m_region_5_end[i];
		}
		ar << m_region_6_start.size();
		for (int i = 0; i < m_region_6_start.size(); i++)
		{
			ar << m_region_6_start[i];	ar << m_region_6_end[i];
		}

		//	相邻号码设置
		ar << m_adjacent_totality_1.size();
		for (int i = 0; i < m_adjacent_totality_1.size(); i++)
		{
			str.Format("%s", m_adjacent_totality_1[i].c_str());
			ar << str;
			ar << m_adjacent_totality_2[i];
			str.Format("%s", m_adjacent_totality_3[i].c_str());
			ar << str;
			ar << m_adjacent_totality_4[i];
		}

		ar << m_adjacent_zone_1.size();
		for (int i = 0; i < m_adjacent_zone_1.size(); i++)
		{
			str.Format("%s", m_adjacent_zone_1[i].c_str());
			ar << str;
			ar << m_adjacent_zone_2[i];
			str.Format("%s", m_adjacent_zone_3[i].c_str());
			ar << str;
			ar << m_adjacent_zone_4[i];
		}

		ar << m_adjacent_sum_1.size();
		for (int i = 0; i < m_adjacent_sum_1.size(); i++)
		{
			str.Format("%s", m_adjacent_sum_1[i].c_str());
			ar << str;
			ar << m_adjacent_sum_2[i];
		}

		// 系统过滤
		ar << m_filter_start;
		ar << m_filter_end;

		ar << m_filter_arithmetic_correct.size();
		for (map<int, int>::iterator it = m_filter_arithmetic_correct.begin(); 
			it != m_filter_arithmetic_correct.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}

		ar << m_filter_equal_number.size();
		for (map<int, int>::iterator it = m_filter_equal_number.begin(); 
			it != m_filter_equal_number.end(); it++)
		{
			ar << it->first;    ar << it->second;
		}

		// π赋值
		ar << m_pai_value;
		ar << m_pai_analysis_periods;
		ar << m_pai_start;
		ar << m_pai_end;
		ar << m_pai_period_difference;
		ar << m_pai_mul_adjunstment;

		ar << m_pai_add_start.size();
		for (int i = 0; i < m_pai_add_start.size(); i++)
		{
			ar << m_pai_add_start[i];
			ar << m_pai_add_end[i];
		}

		ar << m_pai_mul_start.size();
		for (int i = 0; i < m_pai_mul_start.size(); i++)
		{
			ar << m_pai_mul_start[i];
			ar << m_pai_mul_end[i];
		}

		// lnπ赋值
		ar << m_ln_pai_value;
		ar << m_ln_pai_analysis_periods;
		ar << m_ln_pai_start;
		ar << m_ln_pai_end;
		ar << m_ln_pai_period_difference;
		ar << m_ln_pai_mul_adjunstment;

		ar << m_ln_pai_add_start.size();
		for (int i = 0; i < m_ln_pai_add_start.size(); i++)
		{
			ar << m_ln_pai_add_start[i];
			ar << m_ln_pai_add_end[i];
		}

		ar << m_ln_pai_mul_start.size();
		for (int i = 0; i < m_ln_pai_mul_start.size(); i++)
		{
			ar << m_ln_pai_mul_start[i];
			ar << m_ln_pai_mul_end[i];
		}

		// 涨跌动力(swing power)赋值
		ar << m_swing_analysis_periods;
		ar << m_swing_start;
		ar << m_swing_end;
		ar << m_swing_mul_adjunstment;

		ar << m_swing_add_start.size();
		for (int i = 0; i < m_swing_add_start.size(); i++)
		{
			ar << m_swing_add_start[i];
			ar << m_swing_add_end[i];
		}

		ar << m_swing_mul_start.size();
		for (int i = 0; i < m_swing_mul_start.size(); i++)
		{
			ar << m_swing_mul_start[i];
			ar << m_swing_mul_end[i];
		}

		// ln 涨跌动力(swing power)赋值
		ar << m_ln_swing_analysis_periods;
		ar << m_ln_swing_start;
		ar << m_ln_swing_end;
		ar << m_ln_swing_mul_adjunstment;

		ar << m_ln_swing_add_start.size();
		for (int i = 0; i < m_ln_swing_add_start.size(); i++)
		{
			ar << m_ln_swing_add_start[i];
			ar << m_ln_swing_add_end[i];
		}

		ar << m_ln_swing_mul_start.size();
		for (int i = 0; i < m_ln_swing_mul_start.size(); i++)
		{
			ar << m_ln_swing_mul_start[i];
			ar << m_ln_swing_mul_end[i];
		}

		// bernoulli 赋值
		ar << m_bernoulli_analysis_periods;
		ar << m_bernoulli_start;
		ar << m_bernoulli_end;
		ar << m_bernoulli_period_difference;
		ar << m_bernoulli_mul_adjunstment;

		ar << m_bernoulli_add_start.size();
		for (int i = 0; i < m_bernoulli_add_start.size(); i++)
		{
			ar << m_bernoulli_add_start[i];
			ar << m_bernoulli_add_end[i];
		}

		ar << m_bernoulli_mul_start.size();
		for (int i = 0; i < m_bernoulli_mul_start.size(); i++)
		{
			ar << m_bernoulli_mul_start[i];
			ar << m_bernoulli_mul_end[i];
		}

		// potential 赋值
		ar << m_potential_analysis_periods;
		ar << m_potential_start;
		ar << m_potential_end;
		ar << m_potential_period_difference;
		ar << m_potential_mul_adjunstment;
		ar << m_potential_period_absolute;

		ar << m_potential_add_start.size();
		for (int i = 0; i < m_potential_add_start.size(); i++)
		{
			ar << m_potential_add_start[i];
			ar << m_potential_add_end[i];
		}

		ar << m_potential_mul_start.size();
		for (int i = 0; i < m_potential_mul_start.size(); i++)
		{
			ar << m_potential_mul_start[i];
			ar << m_potential_mul_end[i];
		}

	}
	else
	{
		// TODO: 在此添加加载代码
		ar >> str;
		m_lottery_info.code = str.GetBuffer();
		ar >> str;
		m_lottery_info.name = str.GetBuffer();
		ar >> m_lottery_info.regularnum;
		ar >> m_lottery_info.specialnum;
		ar >> m_lottery_info.totalnum;
		ar >> m_bContainSpecial;

		ar >> m_strDataSource;
		ar >> m_strFilterSource;

		int num;
		double db;
		int		ndb, ndb1;
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_var_start.push_back(db);
			ar >> db;	m_var_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_devsq_start.push_back(db);
			ar >> db;	m_devsq_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_stdev_start.push_back(db);
			ar >> db;	m_stdev_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;		m_kurt_start.push_back(db);
			ar >> db;		m_kurt_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;		m_skew_start.push_back(db);
			ar >> db;		m_skew_end.push_back(db);
		}

		//	高级特征
		ar >> m_fred_select;
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_fred_alpha.push_back(db);
			ar >> db;	m_fred_beta.push_back(db);
			ar >> db;	m_fred_upsilon.push_back(db);
			ar >> db;	m_fred_theta.push_back(db);
			ar >> db;	m_fred_eta.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;  ar >> db;	m_fred_start[ndb] = db;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;  ar >> db;	m_fred_end[ndb] = db;
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	m_ac_start.push_back(ndb);
			ar >> ndb;	m_ac_end.push_back(ndb);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	m_grp_qty_start.push_back(ndb);
			ar >> ndb;	m_grp_qty_end.push_back(ndb);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	m_fn_qty_start.push_back(ndb);
			ar >> ndb;	m_fn_qty_end.push_back(ndb);
		}

		// 基本特征
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	m_sum_start.push_back(ndb);
			ar >> ndb;	m_sum_end.push_back(ndb);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	m_odd.push_back(ndb);
		}

		// 相同尾数
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_tail_1[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_tail_2[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_tail_3[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_tail_4[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_tail_5[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_tail_6[ndb] = ndb1;
		}

		// 连号设置
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_consecutive_1[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_consecutive_2[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_consecutive_3[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_consecutive_4[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_consecutive_5[ndb] = ndb1;
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_consecutive_6[ndb] = ndb1;
		}
		
		// 区间设置
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_region_1_start.push_back(ndb);
			m_region_1_end.push_back( ndb1);
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_region_2_start.push_back(ndb);
			m_region_2_end.push_back(ndb1);
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_region_3_start.push_back(ndb);
			m_region_3_end.push_back(ndb1);
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_region_4_start.push_back(ndb);
			m_region_4_end.push_back(ndb1);
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_region_5_start.push_back(ndb);
			m_region_5_end.push_back(ndb1);
		}
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_region_6_start.push_back(ndb);
			m_region_6_end.push_back(ndb1);
		}

		//	相邻号码设置
		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> str;	 ar >> ndb;
			m_adjacent_totality_1.push_back(str.GetBuffer());
			m_adjacent_totality_2.push_back(ndb);
			ar >> str;	 ar >> ndb;
			m_adjacent_totality_3.push_back(str.GetBuffer());
			m_adjacent_totality_4.push_back(ndb);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> str;	 ar >> ndb;
			m_adjacent_zone_1.push_back(str.GetBuffer());
			m_adjacent_zone_2.push_back(ndb);
			ar >> str;	 ar >> ndb;
			m_adjacent_zone_3.push_back(str.GetBuffer());
			m_adjacent_zone_4.push_back(ndb);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> str;	 ar >> ndb;
			m_adjacent_sum_1.push_back(str.GetBuffer());
			m_adjacent_sum_2.push_back(ndb);
		}

		// 系统过滤
		ar >> m_filter_start;
		ar >> m_filter_end;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_filter_arithmetic_correct[ndb] = ndb1;
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> ndb;	 ar >> ndb1;
			m_filter_equal_number[ndb] = ndb1;
		}

		// π赋值
		ar >> m_pai_value;
		ar >> m_pai_analysis_periods;
		ar >> m_pai_start;
		ar >> m_pai_end;
		ar >> m_pai_period_difference;
		ar >> m_pai_mul_adjunstment;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_pai_add_start.push_back(db);
			ar >> db;	m_pai_add_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_pai_mul_start.push_back(db);
			ar >> db;	m_pai_mul_end.push_back(db);
		}

		// lnπ赋值
		ar >> m_ln_pai_value;
		ar >> m_ln_pai_analysis_periods;
		ar >> m_ln_pai_start;
		ar >> m_ln_pai_end;
		ar >> m_ln_pai_period_difference;
		ar >> m_ln_pai_mul_adjunstment;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_ln_pai_add_start.push_back(db);
			ar >> db;	m_ln_pai_add_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_ln_pai_mul_start.push_back(db);
			ar >> db;	m_ln_pai_mul_end.push_back(db);
		}

		// 涨跌动力(swing power)赋值
		ar >> m_swing_analysis_periods;
		ar >> m_swing_start;
		ar >> m_swing_end;
		ar >> m_swing_mul_adjunstment;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_swing_add_start.push_back(db);
			ar >> db;	m_swing_add_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_swing_mul_start.push_back(db);
			ar >> db;	m_swing_mul_end.push_back(db);
		}

		// ln 涨跌动力(swing power)赋值
		ar >> m_ln_swing_analysis_periods;
		ar >> m_ln_swing_start;
		ar >> m_ln_swing_end;
		ar >> m_ln_swing_mul_adjunstment;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_ln_swing_add_start.push_back(db);
			ar >> db;	m_ln_swing_add_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_ln_swing_mul_start.push_back(db);
			ar >> db;	m_ln_swing_mul_end.push_back(db);
		}

		// bernoulli 赋值
		ar >> m_bernoulli_analysis_periods;
		ar >> m_bernoulli_start;
		ar >> m_bernoulli_end;
		ar >> m_bernoulli_period_difference;
		ar >> m_bernoulli_mul_adjunstment;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_bernoulli_add_start.push_back(db);
			ar >> db;	m_bernoulli_add_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_bernoulli_mul_start.push_back(db);
			ar >> db;	m_bernoulli_mul_end.push_back(db);
		}

		// potential 赋值
		ar >> m_potential_analysis_periods;
		ar >> m_potential_start;
		ar >> m_potential_end;
		ar >> m_potential_period_difference;
		ar >> m_potential_mul_adjunstment;
		ar >> m_potential_period_absolute;

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_potential_add_start.push_back(db);
			ar >> db;	m_potential_add_end.push_back(db);
		}

		ar >> num;
		for (int i = 0; i < num; i++)
		{
			ar >> db;	m_potential_mul_start.push_back(db);
			ar >> db;	m_potential_mul_end.push_back(db);
		}
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CLotteryDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CLotteryDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CLotteryDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CLotteryDoc 诊断

#ifdef _DEBUG
void CLotteryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLotteryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLotteryDoc 命令


BOOL CLotteryDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码

	return TRUE;
}


BOOL CLotteryDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDocument::OnSaveDocument(lpszPathName);
}

string CLotteryDoc::SqlScript()
{
	stringstream ss;
	ss << "create table if not exists lottery_param (code varchar(32), name varchar(32), serial int, val varchar(32), "
		<< " PRIMARY KEY ( code, name, serial ))";
	return ss.str();
}


BOOL CLotteryDoc::SaveDocumentIntoDB()
{
	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	LotteryInfo  info = m_lottery_info;

	vector<string>  vecSqlStr;
	stringstream ss;
	try {

		if (1 /*m_fred_alpha.size() > 0*/)
		{
			ss.str("");
			ss << "delete from lottery_param where code = \'" << info.code << "\' and name like \'fred%\' ";
			vecSqlStr.push_back(ss.str());

			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'fred_select\' "
				<< ", 0" 
				<< ", " << Int2String(m_fred_select)
				<< " from dual ";
			vecSqlStr.push_back(ss.str());
		}
		for (int i = 0; i < m_fred_alpha.size(); i++)
		{
			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'fred_alpha\' "
				<< ", " << i
				<< ", " << Double2String(m_fred_alpha[i], "%.4f")
				<< " from dual ";
			vecSqlStr.push_back(ss.str());

			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'fred_beta\' "
				<< ", " << i
				<< ", " << Double2String(m_fred_beta[i], "%.4f")
				<< " from dual ";
			vecSqlStr.push_back(ss.str());

			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'fred_upsilon\' "
				<< ", " << i
				<< ", " << Double2String(m_fred_upsilon[i], "%.4f")
				<< " from dual ";
			vecSqlStr.push_back(ss.str());

			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'fred_theta\' "
				<< ", " << i
				<< ", " << Double2String(m_fred_theta[i], "%.4f")
				<< " from dual ";
			vecSqlStr.push_back(ss.str());

			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'fred_eta\' "
				<< ", " << i
				<< ", " << Double2String(m_fred_eta[i], "%.4f")
				<< " from dual ";
			vecSqlStr.push_back(ss.str());

		}

		if (1 /*m_region_start.size() > 0*/)
		{
			ss.str("");
			ss << "delete from lottery_param where code = \'" << info.code << "\' and name like \'region%\' ";
			vecSqlStr.push_back(ss.str());
		}
		/*for (int i = 0; i < m_region_start.size(); i++)
		{
			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'region_start\' "
				<< ", " << i
				<< ", " << Int2String(m_region_start[i])
				<< " from dual ";
			vecSqlStr.push_back(ss.str());

			ss.str("");
			ss << "insert into lottery_param (code, name ,serial, val) "
				<< " select \'" << info.code << "\'"
				<< ", \'region_end\' "
				<< ", " << i
				<< ", " << Int2String(m_region_end[i])
				<< " from dual ";
			vecSqlStr.push_back(ss.str());
		}*/

		if (vecSqlStr.size()>0)
		{
			session sql(g_MysqlPool);

			ss.str("");
			ss << SqlScript();
			sql << ss.str();

			for (int i = 0; i< vecSqlStr.size(); i++)
				sql << vecSqlStr[i];

			ss.str("");
			ss << info.name ;
			ss << ", 参数保存成功. \r\n";
			sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		}
		return TRUE;
	}
	catch (...)
	{
		ss.str("");
		ss << info.name;
		ss << ", 参数保存失败. \r\n";
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);

		return FALSE;
	}
}

BOOL CLotteryDoc::LoadDocumentFromDB()
{
	vector<LotteryInfo>	vecInfo = ((CLotteryApp *)AfxGetApp())->m_vec_lottery_info;
	LotteryInfo  info = m_lottery_info;

	stringstream ss;
	try {
		session sql(g_MysqlPool);
		sql << SqlScript();

		ss.str("");
		ss << "select code, name ,serial, val "
			"from  lottery_param where code = \'"
			<< info.code << "\' order by name, serial ";

		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test, into(r));
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			string strName = r.get<string>("name", "");
			string strVal = r.get<string>("val", "");

			if (strName.find("fred_select") != string::npos)
			{
				if (!strVal.empty())
					m_fred_select = (atoi(strVal.c_str()));
				else
					m_fred_select = -1;
			}

			if (strName.find("fred_alpha") != string::npos)
			{
				m_fred_alpha.push_back(String2Double(strVal.c_str()));
			}

			if (strName.find("fred_beta") != string::npos)
			{
				m_fred_beta.push_back(String2Double(strVal.c_str()));
			}

			if (strName.find("fred_upsilon") != string::npos)
			{
				m_fred_upsilon.push_back(String2Double(strVal.c_str()));
			}

			if (strName.find("fred_theta") != string::npos)
			{
				m_fred_theta.push_back(String2Double(strVal.c_str()));
			}

			if (strName.find("fred_eta") != string::npos)
			{
				m_fred_eta.push_back(String2Double(strVal.c_str()));
			}

			/*if (strName.find("region_start") != string::npos)
			{
				m_region_start.push_back(atoi(strVal.c_str()));
			}

			if (strName.find("region_end") != string::npos)
			{
				m_region_end.push_back(atoi(strVal.c_str()));
			}*/
		}

		ss.str("");
		ss << info.name;
		ss << ", 参数载入成功. \r\n";
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		return TRUE;
	}
	catch (...)
	{
		ss.str("");
		ss << info.name;
		ss << ", 参数载入失败. \r\n";
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		return FALSE;
	}
}

string CLotteryDoc::SelectClause(string strDataSource)
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (bSpecial)
	{
		ss << "select name,"
			<< "lottery_" << m_lottery_info.code << ".number_1,"
			<< "lottery_" << m_lottery_info.code << ".number_2,"
			<< "lottery_" << m_lottery_info.code << ".number_3,"
			<< "lottery_" << m_lottery_info.code << ".number_4,"
			<< "lottery_" << m_lottery_info.code << ".number_5,"
			<< "lottery_" << m_lottery_info.code << ".number_6,"
			<< "lottery_" << m_lottery_info.code << ".number_7,"
			<< "lottery_" << m_lottery_info.code << ".special_number, "
			<< " sum_number_s as sum_number, odd_s as odd, even_s as even, "
			"consecutive0_s as  consecutive0 , consecutive1_s  as  consecutive1, consecutive2_s  as  consecutive2 ,  equal_tail_s  as  equal_tail,  "
			"var_s as  var,  stdev_s  as stdev ,  devsq_s  as  devsq,  kurt_s  as kurt,  skew_s  as skew,"
			"ac_s  as  ac, group_qty_s  as group_qty , fringe_no_qty_s  as  fringe_no_qty "
			"from  lottery_" << m_lottery_info.code;
		if (strDataSource.empty() == false)
			ss << ", " << strDataSource;
		else
		{
			/* 对于特别号参与生成号码时， 仅仅取特别号参与从大到小排序的组合   */
			/*if (m_lottery_info.regularnum == 5)
				ss << " where lottery_" << m_lottery_info.code << ".number_5 < lottery_";
			else if (m_lottery_info.regularnum == 6)
				ss << " where lottery_" << m_lottery_info.code << ".number_6 < lottery_";
			else 
				ss << " where lottery_" << m_lottery_info.code << ".number_7 < lottery_";
			ss << m_lottery_info.code << ".special_number ";*/

			ss << " where 1 = 1 ";
		}
	}
	else
	{
		ss << "select name, "
			<< "lottery_" << m_lottery_info.code << ".number_1,"
			<< "lottery_" << m_lottery_info.code << ".number_2,"
			<< "lottery_" << m_lottery_info.code << ".number_3,"
			<< "lottery_" << m_lottery_info.code << ".number_4,"
			<< "lottery_" << m_lottery_info.code << ".number_5,"
			<< "lottery_" << m_lottery_info.code << ".number_6,"
			<< "lottery_" << m_lottery_info.code << ".number_7,"
			<< "0 as special_number, "
			<< " sum_number, odd, even, "
			"consecutive0, consecutive1, consecutive2,  equal_tail, var, stdev, devsq, kurt, skew ,"
			"ac,  group_qty,  fringe_no_qty "
			"from  lottery_" << m_lottery_info.code;
		if (strDataSource.empty() == false)
			ss << ", " << strDataSource;
		else if(m_lottery_info.specialnum > 0)
		{
			/* 对于特别号参与生成号码时， 仅仅取特别号参与从大到小排序的组合   */
			/*if (m_lottery_info.regularnum == 5)
				ss << " where lottery_" << m_lottery_info.code << ".number_5 < lottery_";
			else if (m_lottery_info.regularnum == 6)
				ss << " where lottery_" << m_lottery_info.code << ".number_6 < lottery_";
			else
				ss << " where lottery_" << m_lottery_info.code << ".number_7 < lottery_";
			ss << m_lottery_info.code << ".special_number ";*/
			ss << " where lottery_" << m_lottery_info.code << ".special_first = 1 ";
		}
		else
			ss << " where 1 = 1 ";
	}

	return ss.str();
}


string CLotteryDoc::WhereSum()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_sum_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_sum_start.size(); i++)
		{
			if (bSpecial)
				ss << " sum_number_s >=" << Int2String(m_sum_start[i]) << " and " << " sum_number_s <=" << Int2String(m_sum_end[i]);
			else
				ss << " sum_number >=" << Int2String(m_sum_start[i]) << " and " << " sum_number <=" << Int2String(m_sum_end[i]);
			if (i < m_sum_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}

	return ss.str();
}

string CLotteryDoc::WhereOdd()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_odd.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_odd.size(); i++)
		{
			if (bSpecial)
				ss << " odd_s =" << Int2String(m_odd[i]);
			else
				ss << " odd =" << Int2String(m_odd[i]);
			if (i < m_odd.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereAc()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_ac_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_ac_start.size(); i++)
		{
			if (bSpecial)
				ss << " ac_s >=" << Int2String(m_ac_start[i]) << " and " << " ac_s <=" << Int2String(m_ac_end[i]);
			else
				ss << " ac >=" << Int2String(m_ac_start[i]) << " and " << " ac <=" << Int2String(m_ac_end[i]);
			if (i < m_ac_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereGrpQty()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_grp_qty_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_grp_qty_start.size(); i++)
		{
			if (bSpecial)
				ss << " group_qty_s >=" << Int2String(m_grp_qty_start[i]) << " and " << " group_qty_s <=" << Int2String(m_grp_qty_end[i]);
			else
				ss << " group_qty >=" << Int2String(m_grp_qty_start[i]) << " and " << " group_qty <=" << Int2String(m_grp_qty_end[i]);
			if (i < m_grp_qty_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}


string CLotteryDoc::WhereFnQty()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_fn_qty_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_fn_qty_start.size(); i++)
		{
			if (bSpecial)
				ss << " fringe_no_qty_s >=" << Int2String(m_fn_qty_start[i]) << " and " << " fringe_no_qty_s <=" << Int2String(m_fn_qty_end[i]);
			else
				ss << " fringe_no_qty >=" << Int2String(m_fn_qty_start[i]) << " and " << " fringe_no_qty <=" << Int2String(m_fn_qty_end[i]);
			if (i < m_fn_qty_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}


string CLotteryDoc::WhereVar()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_var_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_var_start.size(); i++)
		{
			if (bSpecial)
				ss << " var_s >=" << Double2String(m_var_start[i]) << " and " << " var_s <=" << Double2String(m_var_end[i]);
			else
				ss << " var >=" << Double2String(m_var_start[i]) << " and " << " var <=" << Double2String(m_var_end[i]);
			if (i < m_var_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereDevsq()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_devsq_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_devsq_start.size(); i++)
		{
			if (bSpecial)
				ss << " devsq_s >=" << Double2String(m_devsq_start[i]) << " and " << " devsq_s <=" << Double2String(m_devsq_end[i]);
			else
				ss << " devsq >=" << Double2String(m_devsq_start[i]) << " and " << " devsq <=" << Double2String(m_devsq_end[i]);
			if (i < m_devsq_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereStdev()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_stdev_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_stdev_start.size(); i++)
		{
			if (bSpecial)
				ss << " stdev_s >=" << Double2String(m_stdev_start[i]) << " and " << " stdev_s <=" << Double2String(m_stdev_end[i]);
			else
				ss << " stdev >=" << Double2String(m_stdev_start[i]) << " and " << " stdev <=" << Double2String(m_stdev_end[i]);
			if (i < m_stdev_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereKurt()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_kurt_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_kurt_start.size(); i++)
		{
			if (bSpecial)
				ss << " kurt_s >=" << Double2String(m_kurt_start[i]) << " and " << " kurt_s <=" << Double2String(m_kurt_end[i]);
			else
				ss << " kurt >=" << Double2String(m_kurt_start[i]) << " and " << " kurt <=" << Double2String(m_kurt_end[i]);
			if (i < m_kurt_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereSkew()
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_skew_start.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < m_skew_start.size(); i++)
		{
			if (bSpecial)
				ss << " skew_s >=" << Double2String(m_skew_start[i]) << " and " << " skew_s <=" << Double2String(m_skew_end[i]);
			else
				ss << " skew >=" << Double2String(m_skew_start[i]) << " and " << " skew <=" << Double2String(m_skew_end[i]);
			if (i < m_skew_start.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereAdjacentSum()
{
	//	相邻号码间隔和 
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	vector<string>	vecAdjacentSum;
	for (int i = 0; i < m_adjacent_sum_1.size(); i++)
	{
		if (m_adjacent_sum_1[i] != _T("无"))
		{
			stringstream ss1;
			if (bSpecial)
			{
				if (m_lottery_info.regularnum == 5)
					ss1 << " if( special_number > number_5 , special_number , number_5) - number_1 ";
				if (m_lottery_info.regularnum == 6)
					ss1 << " if( special_number > number_6 , special_number , number_6) - number_1 ";
				if (m_lottery_info.regularnum == 7)
					ss1 << " if( special_number > number_7 , special_number , number_7) - number_1 ";
			}
			else
			{
				if (m_lottery_info.regularnum == 5)
					ss1 << "  number_5 - number_1 ";
				if (m_lottery_info.regularnum == 6)
					ss1 << "  number_6 - number_1 ";
				if (m_lottery_info.regularnum == 7)
					ss1 << "  number_7 - number_1 ";
			}

			ss1 << m_adjacent_sum_1[i] << m_adjacent_sum_2[i] << "  ";
			vecAdjacentSum.push_back(ss1.str());
		}
	}
	if (vecAdjacentSum.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < vecAdjacentSum.size(); i++)
		{
			ss << vecAdjacentSum[i];
			if (i < vecAdjacentSum.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereAdjacentTotality()
{
	// 相邻号码间隔总体设置
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	vector<string>	vecAdjacentTotality;
	for (int i = 0; i < m_adjacent_totality_1.size(); i++)
	{
		if (m_adjacent_totality_1[i] != _T("无"))
		{
			stringstream ss1;
			if (bSpecial)
			{
				ss1 << " if( interval_12_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_23_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_34_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_45_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_56_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";

				if (m_lottery_info.regularnum >= 6)
					ss1 << "+if( interval_67_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				if (m_lottery_info.regularnum >= 7)
					ss1 << "+if( interval_78_s" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";

				ss1 << m_adjacent_totality_1[i] << m_adjacent_totality_2[i];
				vecAdjacentTotality.push_back(ss1.str());
			}
			else
			{
				ss1 << " if( interval_12" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_23" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_34" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				ss1 << "+if( interval_45" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";

				if (m_lottery_info.regularnum >= 6)
					ss1 << "+if( interval_56" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";
				if (m_lottery_info.regularnum >= 7)
					ss1 << "+if( interval_67" << m_adjacent_totality_3[i] << m_adjacent_totality_4[i] << ",1,0)";

				ss1 << m_adjacent_totality_1[i] << m_adjacent_totality_2[i];
				vecAdjacentTotality.push_back(ss1.str());
			}
		}
	}
	if (vecAdjacentTotality.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < vecAdjacentTotality.size(); i++)
		{
			ss << vecAdjacentTotality[i];
			if (i < vecAdjacentTotality.size() - 1)
				ss << " or ";
		}
		ss << " ) ";
	}
	return ss.str();
}


string CLotteryDoc::WhereAdjacentInterval()
{
	// 相邻号码间隔分区设置
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	vector<string>	vecAdjacentInterval;
	for (int i = 0; i < m_adjacent_zone_1.size(); i++)
	{
		if (i + 1 >= (m_lottery_info.regularnum + (bSpecial ? 1 : 0)))
			break;

		string strField = "";
		if (bSpecial)
		{
			if (i == 0) strField = "interval_12_s";
			if (i == 1) strField = "interval_23_s";
			if (i == 2) strField = "interval_34_s";
			if (i == 3) strField = "interval_45_s";
			if (i == 4) strField = "interval_56_s";
			if (i == 5) strField = "interval_67_s";
			if (i == 6) strField = "interval_78_s";
		}
		else
		{
			if (i == 0) strField = "interval_12";
			if (i == 1) strField = "interval_23";
			if (i == 2) strField = "interval_34";
			if (i == 3) strField = "interval_45";
			if (i == 4) strField = "interval_56";
			if (i == 5) strField = "interval_67";
		}

		stringstream ss1;
		if (m_adjacent_zone_1[i] != _T("无"))
		{
			ss1 << strField << m_adjacent_zone_1[i] << m_adjacent_zone_2[i];
		}
		if (m_adjacent_zone_3[i] != _T("无"))
		{
			if (ss1.str().empty() == false)	ss1 << " and ";
			ss1 << strField << m_adjacent_zone_3[i] << m_adjacent_zone_4[i];
		}

		if (ss1.str().empty() == false) vecAdjacentInterval.push_back(ss1.str());
	}
	if (vecAdjacentInterval.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < vecAdjacentInterval.size(); i++)
		{
			ss << vecAdjacentInterval[i];
			if (i < vecAdjacentInterval.size() - 1)
				ss << " and ";
		}
		ss << " ) ";
	}
	return ss.str();
}

string CLotteryDoc::WhereRegion()
{
	//	区间设置
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	vector<string>	vecRegion;
	for (int i = 0; i < 6; i++)
	{
		vector<int>	vecStart, vecEnd;
		if (i == 0) { vecStart = m_region_1_start; vecEnd = m_region_1_end; }
		if (i == 1) { vecStart = m_region_2_start; vecEnd = m_region_2_end; }
		if (i == 2) { vecStart = m_region_3_start; vecEnd = m_region_3_end; }
		if (i == 3) { vecStart = m_region_4_start; vecEnd = m_region_4_end; }
		if (i == 4) { vecStart = m_region_5_start; vecEnd = m_region_5_end; }
		if (i == 5) { vecStart = m_region_6_start; vecEnd = m_region_6_end; }

		if (vecStart.size() >= 2)
		{
			stringstream ss1;
			ss1 << " if( number_1 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			ss1 << "+if( number_2 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			ss1 << "+if( number_3 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			ss1 << "+if( number_4 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			ss1 << "+if( number_5 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			if (m_lottery_info.regularnum >= 6)
				ss1 << "+if( number_6 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			if (m_lottery_info.regularnum >= 7)
				ss1 << "+if( number_7 between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";
			if (bSpecial)
				ss1 << "+if( special_number between " << vecStart[0] << " and " << vecEnd[0] << ",1,0)";

			string strField = "( " + ss1.str() + " )";
			ss1.str("");
			for (int k = 1; k < vecStart.size(); k++)
			{
				ss1 << strField << " between " << vecStart[k] << " and " << vecEnd[k];
				if (k < vecStart.size() - 1)	ss1 << " or ";
			}
			vecRegion.push_back(ss1.str());
		}
	}
	if (vecRegion.size() > 0)
	{
		ss << " and ( ";
		for (int i = 0; i < vecRegion.size(); i++)
		{
			ss << vecRegion[i];
			if (i < vecRegion.size() - 1)
				ss << " and ";
		}
		ss << " ) ";
	}
	return ss.str();
}

// 现在 no use
string CLotteryDoc::WhereArithmeticCorrect()
{
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	stringstream ss;
	if (m_filter_arithmetic_correct.size() > 0)
	{
		ss << " and  not exists ( select 1 from lottery_mxn where code=\'" 
			<< m_lottery_info.code << "\' and ( ";
		for (map<int, int>::iterator it = m_filter_arithmetic_correct.begin(); 
			it != m_filter_arithmetic_correct.end(); )
		{
			int n = it->first - 1;
			ss << " ( ";
			for (int k = 1; k <= m_lottery_info.regularnum; k++)
			{
				ss << " lottery_mxn.number_" << k
					<< "= lottery_" << m_lottery_info.code << ".number_" << k << "+" << n;
				if (k < m_lottery_info.regularnum) ss << " and  ";
			}
			if(bSpecial)
				ss << " and lottery_mxn.special_number = lottery_" << m_lottery_info.code << ".special_number +" << n;;
			ss << ") or ( ";
			for (int k = 1; k <= m_lottery_info.regularnum; k++)
			{
				ss << " lottery_mxn.number_" << k
					<< "= lottery_" << m_lottery_info.code << ".number_" << k << "-" << n;;
				if (k < m_lottery_info.regularnum) ss << " and  ";
			}
			if (bSpecial)
				ss << " and lottery_mxn.special_number = lottery_" << m_lottery_info.code << ".special_number -" << n;;
			ss << ") ";

			it++;
			if (it != m_filter_arithmetic_correct.end())
				ss << " or ";
		}
		ss << " ) ) ";
	}

	return ss.str();
}

void CLotteryDoc::FilterStatistic(string filterName, int nRet)
{
	if (nRet != 0) 
	{
		if (m_filter_statistic_input.count(filterName) == 0) 
		{
			m_filter_statistic_input[filterName] = 1;
			m_filter_statistic_output[filterName] = 0;
		}
		else m_filter_statistic_input[filterName] = m_filter_statistic_input[filterName] + 1;
	}
	if (nRet > 0)
	{
		m_filter_statistic_output[filterName] = m_filter_statistic_output[filterName] + 1;
	}
}

void CLotteryDoc::FilterStatisticPrint()
{
	stringstream ss;
	map<string, int>::iterator mit;
	// 利用 tmp 进行按照 value 值排序
	for (mit = m_filter_statistic_input.begin(); mit != m_filter_statistic_input.end(); mit++) {
		ss.str("");
		ss << "【运行汇总】" << mit->first << ", 输入 " << mit->second << ", 输出 " << m_filter_statistic_output[mit->first] << "\r\n";
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
}

int CLotteryDoc::ConsecutiveFilter(string consecutive0, string consecutive1, string consecutive2)
{
	if (m_consecutive_1.size() > 0 || m_consecutive_2.size() > 0 || m_consecutive_3.size() > 0 ||
		m_consecutive_4.size() > 0 || m_consecutive_5.size() > 0 || m_consecutive_6.size() > 0)
	{
		map<int, int> mapConsecutive;
		mapConsecutive[1] = 0;		//		间 0 连 2
		mapConsecutive[2] = 0;		//		间 0 连 3
		mapConsecutive[3] = 0;		//		间 0 连 4
		mapConsecutive[4] = 0;		//		间 0 连 5
		mapConsecutive[5] = 0;		//		间 1 连 2
		mapConsecutive[6] = 0;		//		间 1 连 3
		mapConsecutive[7] = 0;		//		间 2 连 2
		mapConsecutive[8] = 0;		//		间 2 连 3

									// 连号统计及
		vector<string>vecResult;
		string  strConsecutive = consecutive0;
		splitString(strConsecutive, ",", vecResult);
		for (int i = 0; i < vecResult.size(); i++)
		{
			vector<string>		vecResult1;
			splitString(vecResult[i], "/", vecResult1);
			if (vecResult1.size() == 2)	mapConsecutive[1]++;
			if (vecResult1.size() == 3)	mapConsecutive[2]++;
			if (vecResult1.size() == 4)	mapConsecutive[3]++;
			if (vecResult1.size() >= 5)	mapConsecutive[4]++;
		}

		strConsecutive = consecutive1;
		splitString(strConsecutive, ",", vecResult);
		for (int i = 0; i < vecResult.size(); i++)
		{
			vector<string>		vecResult1;
			splitString(vecResult[i], "/", vecResult1);
			if (vecResult1.size() == 2)	mapConsecutive[5]++;
			if (vecResult1.size() == 3)	mapConsecutive[6]++;
		}

		strConsecutive = consecutive2;
		splitString(strConsecutive, ",", vecResult);
		for (int i = 0; i < vecResult.size(); i++)
		{
			vector<string>		vecResult1;
			splitString(vecResult[i], "/", vecResult1);
			if (vecResult1.size() == 2)	mapConsecutive[7]++;
			if (vecResult1.size() == 3)	mapConsecutive[8]++;
		}

		// 过滤
		int i = 0;
		for (i = 0; i < 6; i++)
		{
			map<int, int> vec;
			if (i == 0)	vec = m_consecutive_1;
			if (i == 1)	vec = m_consecutive_2;
			if (i == 2)	vec = m_consecutive_3;
			if (i == 3)	vec = m_consecutive_4;
			if (i == 4)	vec = m_consecutive_5;
			if (i == 5)	vec = m_consecutive_6;

			if (vec.size() == 0)		continue;

			bool  bAllOK = true;
			for (map<int, int>::iterator it = vec.begin(); it != vec.end(); it++)
			{
				if (mapConsecutive[it->first] != it->second)
				{
					bAllOK = false;
					break;
				}
			}
			if (bAllOK)		break;
		}

		if (i == 6)
		{
			//	6 条规则中 ， 没有一条通过，则本号码抛弃
			return -1;
		}
		return 1;
	}
	else
		return 0;	
}

int CLotteryDoc::TailFilter(string equal_tail)
{
	stringstream ss;
	if (m_tail_1.size() > 0 || m_tail_2.size() > 0 || m_tail_3.size() > 0 ||
		m_tail_4.size() > 0 || m_tail_5.size() > 0 || m_tail_6.size() > 0)
	{
		map<int, int>	mapTail;
		mapTail[0] = 0;
		mapTail[1] = 0;
		mapTail[2] = 0;

		// 相同尾号统计及
		vector<string>vecResult;
		string  strTail = equal_tail;
		splitString(strTail, ",", vecResult);
		for (int i = 0; i < vecResult.size(); i++)
		{
			vector<string>		vecResult1;
			splitString(vecResult[i], "/", vecResult1);
			mapTail[i] = vecResult1.size();

		}

		// 过滤
		int i = 0;
		for (i = 0; i < 6; i++)
		{
			map<int, int> vec;
			if (i == 0)	vec = m_tail_1;
			if (i == 1)	vec = m_tail_2;
			if (i == 2)	vec = m_tail_3;
			if (i == 3)	vec = m_tail_4;
			if (i == 4)	vec = m_tail_5;
			if (i == 5)	vec = m_tail_6;

			if (vec.size() == 0)		continue;

			bool  bAllOK = true;
			for (map<int, int>::iterator it = vec.begin(); it != vec.end(); it++)
			{
				if (mapTail[it->first - 1] != it->second)
				{
					bAllOK = false;
					break;
				}
			}
			if (bAllOK)		break;
		}

		if (i == 6)
		{
			//	6 条规则中 ， 没有一条通过，则本号码抛弃

			ss.str("");
			ss << "【运行】相同尾数过滤";
			
			//sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
			return -1;
		}
		return 1;
	}
	else
		return 0;
}

int CLotteryDoc::FredFilter(int sum, int ac, int fringe_no_qty, int group_qty)
{
	if (m_fred_start.size() > 0)
	{
		double fred0 = m_fred_alpha[0] + m_fred_beta[0] * ac + m_fred_upsilon[0] * log((double)sum)
			+ m_fred_theta[0] * fringe_no_qty + m_fred_eta[0] * group_qty;

		double fred1 = m_fred_alpha[1] + m_fred_beta[1] * ac + m_fred_upsilon[1] * log((double)sum)
			+ m_fred_theta[1] * fringe_no_qty + m_fred_eta[1] * group_qty;

		double fred2 = m_fred_alpha[2] + m_fred_beta[2] * ac + m_fred_upsilon[2] * log((double)sum)
			+ m_fred_theta[2] * fringe_no_qty + m_fred_eta[2] * group_qty;

		double fred3 = m_fred_alpha[3] + m_fred_beta[3] * ac + m_fred_upsilon[3] * log((double)sum)
			+ m_fred_theta[3] * fringe_no_qty + m_fred_eta[3] * group_qty;
		
		int nPass = 0;
		if (m_fred_start.count(1) > 0)
		{
			if (fred0 >= m_fred_start[1] && fred0 <= m_fred_end[1])		nPass++;
		}
		if (m_fred_start.count(2) > 0)
		{
			if (fred1 >= m_fred_start[2] && fred1 <= m_fred_end[2])		nPass++;
		}
		if (m_fred_start.count(3) > 0)
		{
			if (fred2 >= m_fred_start[3] && fred2 <= m_fred_end[3])		nPass++;
		}
		if (m_fred_start.count(4) > 0)
		{
			if (fred3 >= m_fred_start[4] && fred3 <= m_fred_end[4])		nPass++;
		}

		if (nPass == 0)
			return -1;
		return 1;
	}
	else
		return 0;
}

// 根据已经开奖的记录
int CLotteryDoc::ArithmeticCorrectFilter(vector<LotteryNumber> & vec, int number[])
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_filter_arithmetic_correct.size() > 0)
	{
		for (int i = 0; i < vec.size(); i++)
		{
			LotteryNumber ln = vec[i];
			if (m_filter_start > 0 && ln.periods < m_filter_start)
				continue;
			if (m_filter_end > 0 && ln.periods > m_filter_end)
				continue;

			for (map<int, int>::iterator it = m_filter_arithmetic_correct.begin(); it != m_filter_arithmetic_correct.end(); it++)
			{
				int nCount = it->first - 1;
				LotteryNumber ln0 = ln;
				if(ln0.number[m_lottery_info.regularnum - 1] + nCount <= m_lottery_info.totalnum)
					for (int k = 0; k < m_lottery_info.regularnum; k++)
					{
						ln0.number[k] = ln0.number[k] + nCount;
					}

				LotteryNumber ln1 = ln;
				if (ln1.number[0] - nCount >= 1)
					for (int k = 0; k < m_lottery_info.regularnum; k++)
					{
						ln1.number[k] = ln1.number[k] - nCount;
					}

				int k = 0;
				for (k = 0; k < m_lottery_info.regularnum; k++)
				{
					if (number[k] != ln0.number[k])
					{
						break;
					}
				}
				if (k == m_lottery_info.regularnum)
					return -1;

				k = 0;
				for (k = 0; k < m_lottery_info.regularnum; k++)
				{
					if (number[k] != ln1.number[k])
					{
						break;
					}
				}
				if (k == m_lottery_info.regularnum)
					return -1;
			}

		}
		return 1;
	}
	else 
	{
		return 0;
	}
}

// 根据已经开奖的记录，对相同号码的个数进行过滤
int CLotteryDoc::EqualNumberFilter(vector<LotteryNumber> & vec, int number[])
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if (m_filter_equal_number.size() > 0)
	{
		for (int i = 0; i < vec.size(); i++)
		{
			LotteryNumber ln = vec[i];
			if (m_filter_start > 0 && ln.periods < m_filter_start)
				continue;
			if (m_filter_end > 0 && ln.periods > m_filter_end)
				continue;

			int nCount = 0;
			for (int k = 0; k < m_lottery_info.regularnum; k++)
			{
				for (int m = 0; m < m_lottery_info.regularnum; m++)
				{
					if (number[k] == ln.number[m])
					{
						nCount++;
						break;
					}
				}
			}

			bool bOk = false;
			for (map<int, int>::iterator it = m_filter_equal_number.begin(); it != m_filter_equal_number.end(); it++)
			{
				if (it->first + 2 == nCount)
				{
					bOk = true;
					break;
				}
			}

			// 抛弃
			if (bOk)
			{
				if (m_nOperType == 1)
				{
					ss.str("");
					ss << _T("【漏奖查询】已开奖号码 相同号码个数过滤");
					for (int k = 0; k < m_lottery_info.regularnum; k++)
					{
						ss << "," << ln.number[k];
					}
					ss << ",相同个数 " << nCount;
					sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
				}
				
				return -1;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

int CLotteryDoc::RegularExpressFilter(LotteryNumber ln)
{
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	if(m_strFilterSource.IsEmpty())
		return 0;

	if (m_vecFilterArrayData.size() == 0)
	{
		vector<string> arrayHeader;
		CCSVFile csvFile(m_strFilterSource.GetBuffer());
		csvFile.ReadAllData(arrayHeader, m_vecFilterArrayData);
		csvFile.Close();
		if (arrayHeader.size() > 0)
			m_vecFilterArrayData.push_back(arrayHeader);
	}
	
	if (m_vecFilterArrayData.size() == 0)
		return 0;

	int nExpected = m_lottery_info.regularnum + (bSpecial ? 1 : 0);
	ss.str("");
	for (int i = 0; i < nExpected; i++)
	{
		int m;
		if (i < m_lottery_info.regularnum)
			m = ln.number[i];
		else
			m = ln.special_number;

		ss << Int2String(m, "%02d") << " ";
	}
	string sLine = ss.str();

	bool bPassed = false;
	for (int i = 0; i < m_vecFilterArrayData.size(); i++)
	{
		// "[\\s\\t]+[\\-]?[\\d]+"
		if (m_vecFilterArrayData[i].size() > 0) {
			ss.str("");
			for (int k = 0; k < m_vecFilterArrayData[i].size(); k++)
			{
				int w = atoi(m_vecFilterArrayData[i][k].c_str());
				ss << Int2String(w, "%02d") << "[\\s]+[\\s\\d]*";
			}

			string strPattern = ss.str();
			vector<string> vecMatch;
			if (Pcre2Grep(strPattern.c_str(), sLine.c_str(), vecMatch) > 0)
			{
				bPassed = true;
				break;
			}
		}
		
	}
	
	if (bPassed)
		return 1;
	else
		return -1;
}

string CLotteryDoc::LotteryLevel(int nLevel)
{
	if (m_lottery_info.specialnum > 0)
	{
		if (nLevel % 2 == 0)
		{
			int nSum = m_lottery_info.regularnum - nLevel / 2;
			string str = Int2String(nSum) + _T("个正选号 + 特别号");
			return str;
		}
		else
		{
			int nSum = m_lottery_info.regularnum - (nLevel - 1) / 2;
			string str = Int2String(nSum) + _T("个正选号");
			return str;
		}
	}
	else
	{
		string str = Int2String(m_lottery_info.regularnum - nLevel + 1) + _T("个号");
		return str;
	}
}

// 0 未获奖
int CLotteryDoc::WinLotteryQuery(int * query, int * std, string & mark)
{
	// 查询在这组号码中有几个 正选号
	int nSum = 0;
	for (int i = 0; i < m_lottery_info.regularnum + (m_lottery_info.specialnum > 0 ? 1 : 0); i++)
	{
		for (int k = 0; k < m_lottery_info.regularnum; k++)
		{
			if (query[i] == std[k])
			{
				nSum++;
				mark = mark + Int2String(query[i]) + " ";
				break;
			}
		}
	}

	// 查询在这组号码中是否有特别号
	int nSpecial = 0;
	if (m_lottery_info.specialnum > 0) {
		for (int i = 0; i < m_lottery_info.regularnum + 1; i++)
		{
			if (query[i] == std[m_lottery_info.regularnum])
			{
				nSpecial++;
				mark = mark + " (" + Int2String(query[i])+ ")";
				break;
			}
		}
	}

	if (m_lottery_info.specialnum > 0)
	{
		if (nSum > 0 || nSpecial >0)
		{
			int nLevel = (m_lottery_info.regularnum - nSum)*2 +1;
			if (nSpecial)
			{
				if (nLevel > 1) nLevel--;
			}
			return nLevel;
		}
		else
		{
			// 未获奖
			return 0;
		}
	}
	else
	{
		if (m_lottery_info.regularnum - nSum < m_lottery_info.regularnum)
		{
			stringstream ss;
			return m_lottery_info.regularnum - nSum + 1;
		}
		else
		{
			// 未获奖
			return 0;
		}
	}
}

vector<LotteryNumber> CLotteryDoc::FetchAwardRecord()
{
	stringstream ss;
	vector<LotteryNumber> vec;
	ss << "select name, periods, number_1, number_2, number_3, number_4,"
		<< "number_5, number_6, number_7, special_number "
		<< "from  lottery_mxn where code =\'" << m_lottery_info.code << "\' ";
	/*if (m_filter_start > 0)
		ss << " and periods >= " << m_filter_start;
	if (m_filter_end > 0)
		ss << " and periods <= " << m_filter_end;*/

	string test = ss.str();
		
	try {
		
		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << ss.str(), into(r));
		st.execute();
		while (st.fetch())
		{
			LotteryNumber ln;
			ln.name = r.get<string>("name", "");
			ln.periods = r.get<int>("periods", 0);
			ln.number[0] = r.get<int>("number_1", 0);
			ln.number[1] = r.get<int>("number_2", 0);
			ln.number[2] = r.get<int>("number_3", 0);
			ln.number[3] = r.get<int>("number_4", 0);
			ln.number[4] = r.get<int>("number_5", 0);
			ln.number[5] = r.get<int>("number_6", 0);
			ln.number[6] = r.get<int>("number_7", 0);
			ln.special_number = r.get<int>("special_number", 0);
			vec.push_back(ln);
		}
	}
	catch (...) {

	}

	if (m_nOperType == 1)
	{
		ss.str("");
		ss << _T("【漏奖查询】获取已开奖号码数据源，记录数 ") << vec.size();

		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
	

	return vec;
}


// 根据已经开奖的记录，预先计算 期差，累计球数等
map<int,Assignment> CLotteryDoc::AssignmentFilterPre(vector<LotteryNumber> & vec1, int periods_start ,int periods_end, int goal_count_control)
{
	map<int, Assignment> assignMap;
	stringstream ss;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;

	//	初始化各个球的 统计结构
	for (int i = 1; i <= m_lottery_info.totalnum; i++)
	{
		Assignment ass;
		ass.total_periods = 0;
		ass.periods_different = 0;
		ass.goal_count = 0;
		ass.periods_max_internal = 1;
		ass.periods_recent_number = -1;
		
		ass.dResult = 0.0;
		assignMap[i] = ass;
	}

	if (periods_start > 0 && periods_end > periods_start)
	{
		// 过滤起始期数
		vector<LotteryNumber>  vec;
		for (int i = 0; i < vec1.size(); i++)
		{
			LotteryNumber ln = vec1[i];
			if (periods_start > 0 && ln.periods < periods_start)
				continue;
			if (periods_end > 0 && ln.periods > periods_end)
				continue;

			vec.push_back(ln);
		}


		for (int i = 1; i <= m_lottery_info.totalnum; i++) {
			assignMap[i].periods_different = goal_count_control;
			assignMap[i].total_periods = vec.size();
		}
			

		for (int i = 0; i < vec.size(); i++)
		{
			for (int m = 0; m < m_lottery_info.regularnum; m++)
			{
				int k = vec[i].number[m];
				int interval = i - assignMap[k].periods_recent_number;
				// 记录最近出现该球的期序号
				assignMap[k].periods_recent_number = i;

				// 计算最大间隔数
				if (interval > assignMap[k].periods_max_internal)
					assignMap[k].periods_max_internal = interval;

				assignMap[k].goal_count++;
				//	计算首次 的期差
				if (assignMap[k].goal_count == 1) {
					// 超过 累计球控制期数，不予计算
					if (i < goal_count_control) {
						assignMap[k].periods_different = i;
					}
				}
				
			}
			if (bSpecial)
			{
				int k = vec[i].special_number;
				int interval = i - assignMap[k].periods_recent_number;
				// 记录最近出现该球的期序号
				assignMap[k].periods_recent_number = i;

				// 计算最大间隔数
				if (interval > assignMap[k].periods_max_internal)
					assignMap[k].periods_max_internal = interval;

				assignMap[k].goal_count++;
				//	计算首次 的期差
				if (assignMap[k].goal_count == 1) {
					// 超过 累计球控制期数，不予计算
					if (i < goal_count_control) {
						assignMap[k].periods_different = i;
					}
				}
			}
				
		}
	}

	return assignMap;
}

void CLotteryDoc::AssignmentFilterLog(map<int, Assignment> mapPai, string type, LotteryNumber ln)
{
	//	起止期数 选择不对
	if (mapPai[1].total_periods == 0)
		return;

	stringstream ss;
	for (int i = 0; i < m_lottery_info.regularnum; i++)
	{
		int m = ln.number[i];
		ss.str("");
		ss << _T("【漏奖查询】");
		ss << type << " >> " << Int2String(m, "%#2d");
		ss << _T(", 累计球数 ") << Int2String(mapPai[m].goal_count, "%#2d");
		ss << _T(", 期差 ") << mapPai[m].periods_different;
		ss << _T(", 最近出现的期数序号 ") << mapPai[m].periods_recent_number;
		ss << _T(", 最大的间隔期数 ") << Int2String(mapPai[m].periods_max_internal, "%#2d");
		ss << _T(", 参与分析的总期数 ") << mapPai[m].total_periods;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}
}


// 根据已经开奖的记录，π赋值过滤
int CLotteryDoc::PaiFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog)
{
	if (m_pai_value <= 0 || m_pai_value >= 10)
		return 0;

	//	起止期数 选择不对
	if (map1[1].total_periods == 0)
		return 0;

	double add = 0;
	double mul = 1;

	int k = m_lottery_info.regularnum;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;
	if (bSpecial)
		k++;

	if(bLog)
		AssignmentFilterLog(map1, _T("π赋值"), ln);

	stringstream ss;
	ss << _T("【漏奖查询】π赋值 ");
	for (int i = 0; i < k; i++)
	{
		int m = 0;
		if (i < m_lottery_info.regularnum)	m = ln.number[i];
		else	m = ln.special_number;
		int goal_count = map1[m].goal_count + 1;
		int total_periods = map1[m].total_periods + 1;
		int periods_different = map1[m].periods_different;
		//if (periods_different == 0) periods_different = total_periods - 1;
		if (m_pai_period_difference == 0) periods_different = 0;

		double ass = -1.0 * goal_count/(total_periods- periods_different);
		ass = 1 - pow (m_pai_value, ass);

		add += ass;
		mul *= ass;

		map1[m].dResult = ass;

		if (bLog)
		{
			ss << " [" << m  << "]=" << Double2String(ass, "%.6f");
		}
	}
	if (bLog)
	{
		ss << " [加]=" << Double2String(add, "%.6f");
		ss << " [乘]=" << Double2String(mul, "%.6f");
		ss << " [乘的修正]=" << Double2String(mul * m_pai_mul_adjunstment, "%.6f");
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}


	if (m_pai_mul_adjunstment > 0)  mul *= m_pai_mul_adjunstment;

	if (m_pai_add_start.size() == 0 && m_pai_mul_start.size() == 0)
		return 0;
	
	//	判断 和数 是否符合
	int result = 0;
	for (int i = 0; i < m_pai_add_start.size(); i++)
	{
		if (add >= m_pai_add_start[i] && add <= m_pai_add_end[i])
		{
			result = 1;
			break;
		}
	}

	if (result == 0 && m_pai_add_start.size() > 0)
		return -1;

	//	判断 乘数 是否符合
	int result1 = 0;
	for (int i = 0; i < m_pai_mul_start.size(); i++)
	{
		if (mul >= m_pai_mul_start[i] && mul <= m_pai_mul_end[i])
		{
			result1 = 1;
			break;
		}
	}

	if (result1 == 0 && m_pai_mul_start.size() > 0)
		return -1;
	
	return 1;
}


// 根据已经开奖的记录，lnπ赋值过滤
int CLotteryDoc::LnPaiFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog)
{
	if (m_ln_pai_value <= 0 || m_ln_pai_value >= 10)
		return 0;

	//	起止期数 选择不对
	if (map1[1].total_periods == 0)
		return 0;

	double add = 0;
	double mul = 1;

	int k = m_lottery_info.regularnum;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;
	if (bSpecial)
		k++;

	if (bLog)
		AssignmentFilterLog(map1, _T("Lnπ赋值"), ln);

	stringstream ss;
	ss << _T("【漏奖查询】Lnπ赋值 ");
	for (int i = 0; i < k; i++)
	{
		int m = 0;
		if (i < m_lottery_info.regularnum)	m = ln.number[i];
		else	m = ln.special_number;
		int goal_count = map1[m].goal_count + 1;
		int total_periods = map1[m].total_periods + 1;
		int periods_different = map1[m].periods_different;
		//if (periods_different == 0) periods_different = total_periods -1;
		if (m_pai_period_difference == 0) periods_different = 0;

		double ass = -1.0 * log(double(goal_count))/ log(double(total_periods - periods_different));
		ass = 1 - pow(m_ln_pai_value, ass);

		add += ass;
		mul *= ass;

		map1[m].dResult = ass;

		if (bLog)
		{
			ss << " [" << m << "]=" << Double2String(ass, "%.6f");
		}
	}
	if (bLog)
	{
		ss << " [加]=" << Double2String(add, "%.6f");
		ss << " [乘]=" << Double2String(mul, "%.6f");
		ss << " [乘的修正]=" << Double2String(mul * m_ln_pai_mul_adjunstment, "%.6f");
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}


	if (m_ln_pai_mul_adjunstment > 0)  mul *= m_ln_pai_mul_adjunstment;

	if (m_ln_pai_add_start.size() == 0 && m_ln_pai_mul_start.size() == 0)
		return 0;

	//	判断 和数 是否符合
	int result = 0;
	for (int i = 0; i < m_ln_pai_add_start.size(); i++)
	{
		if (add >= m_ln_pai_add_start[i] && add <= m_ln_pai_add_end[i])
		{
			result = 1;
			break;
		}
	}

	if (result == 0 && m_ln_pai_add_start.size() > 0)
		return -1;

	//	判断 乘数 是否符合
	int result1 = 0;
	for (int i = 0; i < m_ln_pai_mul_start.size(); i++)
	{
		if (mul >= m_ln_pai_mul_start[i] && mul <= m_ln_pai_mul_end[i])
		{
			result1 = 1;
			break;
		}
	}

	if (result1 == 0 && m_ln_pai_mul_start.size() > 0)
		return -1;

	return 1;
}

// 根据已经开奖的记录，涨落动力赋值过滤
int CLotteryDoc::SwingPowerFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog)
{
	//	起止期数 选择不对
	if (map1[1].total_periods == 0)
		return 0;

	double add = 0;
	double mul = 1;

	int k = m_lottery_info.regularnum;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;
	if (bSpecial)
		k++;

	if (bLog)
		AssignmentFilterLog(map1, _T("涨跌动力赋值"), ln);

	stringstream ss;
	ss << _T("【漏奖查询】涨落动力赋值 ");
	for (int i = 0; i < k; i++)
	{
		int m = 0;
		if (i < m_lottery_info.regularnum)	m = ln.number[i];
		else	m = ln.special_number;
		int goal_count = map1[m].goal_count + 1;
		int total_periods = map1[m].total_periods + 1;

		int hi = total_periods - map1[m].periods_recent_number - 1;
		double hp = (total_periods - hi) * 1.0 / goal_count;
		
		double ass = hi / (hp * (1 - hp / map1[m].periods_max_internal));

		add += ass;
		mul *= ass;

		map1[m].dResult = ass;

		if (bLog)
		{
			ss << " [" << m << "]=" << Double2String(ass, "%.6f");
		}
	}
	if (bLog)
	{
		ss << " [加]=" << Double2String(add, "%.6f");
		ss << " [乘]=" << Double2String(mul, "%.6f");
		ss << " [乘的修正]=" << Double2String(mul * m_swing_mul_adjunstment, "%.6f");
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}


	if (m_swing_mul_adjunstment > 0)  mul *= m_swing_mul_adjunstment;

	if (m_swing_add_start.size() == 0 && m_swing_mul_start.size() == 0)
		return 0;

	//	判断 和数 是否符合
	int result = 0;
	for (int i = 0; i < m_swing_add_start.size(); i++)
	{
		if (add >= m_swing_add_start[i] && add <= m_swing_add_end[i])
		{
			result = 1;
			break;
		}
	}

	if (result == 0 && m_swing_add_start.size() > 0)
		return -1;

	//	判断 乘数 是否符合
	int result1 = 0;
	for (int i = 0; i < m_swing_mul_start.size(); i++)
	{
		if (mul >= m_swing_mul_start[i] && mul <= m_swing_mul_end[i])
		{
			result1 = 1;
			break;
		}
	}

	if (result1 == 0 && m_swing_mul_start.size() > 0)
		return -1;

	return 1;
}

// 根据已经开奖的记录，ln 涨落动力赋值过滤
int CLotteryDoc::LnSwingPowerFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog)
{
	//	起止期数 选择不对
	if (map1[1].total_periods == 0)
		return 0;

	double add = 0;
	double mul = 1;

	int k = m_lottery_info.regularnum;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;
	if (bSpecial)
		k++;

	if (bLog)
		AssignmentFilterLog(map1, _T("Ln 涨落动力赋值"), ln);

	stringstream ss;
	ss << _T("【漏奖查询】Ln 涨落动力赋值 ");
	for (int i = 0; i < k; i++)
	{
		int m = 0;
		if (i < m_lottery_info.regularnum)	m = ln.number[i];
		else	m = ln.special_number;
		int goal_count = map1[m].goal_count + 1;
		int total_periods = map1[m].total_periods + 1;

		int hi = total_periods - map1[m].periods_recent_number -1;
		double hp = (total_periods - hi) * 1.0 / goal_count;

		double ass = log((double)hi) / (log(hp) * (1 - log(hp / map1[m].periods_max_internal)));

		add += ass;
		mul *= ass;

		map1[m].dResult = ass;

		if (bLog)
		{
			ss << " [" << m << "]=" << Double2String(ass, "%.6f");
		}
	}
	if (bLog)
	{
		ss << " [加]=" << Double2String(add, "%.6f");
		ss << " [乘]=" << Double2String(mul, "%.6f");
		ss << " [乘的修正]=" << Double2String(mul * m_ln_swing_mul_adjunstment, "%.6f");
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}


	if (m_ln_swing_mul_adjunstment > 0)  mul *= m_ln_swing_mul_adjunstment;

	if (m_ln_swing_add_start.size() == 0 && m_ln_swing_mul_start.size() == 0)
		return 0;

	//	判断 和数 是否符合
	int result = 0;
	for (int i = 0; i < m_ln_swing_add_start.size(); i++)
	{
		if (add >= m_ln_swing_add_start[i] && add <= m_ln_swing_add_end[i])
		{
			result = 1;
			break;
		}
	}

	if (result == 0 && m_ln_swing_add_start.size() > 0)
		return -1;

	//	判断 乘数 是否符合
	int result1 = 0;
	for (int i = 0; i < m_ln_swing_mul_start.size(); i++)
	{
		if (mul >= m_ln_swing_mul_start[i] && mul <= m_ln_swing_mul_end[i])
		{
			result1 = 1;
			break;
		}
	}

	if (result1 == 0 && m_ln_swing_mul_start.size() > 0)
		return -1;

	return 1;
}

// 根据已经开奖的记录，伯努利赋值过滤
int CLotteryDoc::BernoulliFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog)
{
	//	起止期数 选择不对
	if (map1[1].total_periods == 0)
		return 0;

	double add = 0;
	double mul = 1;

	int k = m_lottery_info.regularnum;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;
	if (bSpecial)
		k++;

	if (bLog)
		AssignmentFilterLog(map1, _T("Bernoulli 赋值"), ln);

	stringstream ss;
	ss << _T("【漏奖查询】Bernoulli 赋值 ");
	for (int i = 0; i < k; i++)
	{
		int m = 0;
		if (i < m_lottery_info.regularnum)	m = ln.number[i];
		else	m = ln.special_number;
		int goal_count = map1[m].goal_count + 1;
		int total_periods = map1[m].total_periods + 1;
		int periods_different = map1[m].periods_different;
		//if (periods_different == 0) periods_different = total_periods - 1;
		if (m_bernoulli_period_difference == 0) periods_different = 0;

		double p = m_lottery_info.regularnum * 1.0 / m_lottery_info.totalnum;
		double q = (m_lottery_info.totalnum - m_lottery_info.regularnum) * 1.0 / m_lottery_info.totalnum;
		double t = (goal_count - (total_periods - periods_different) * p) / sqrt((total_periods - periods_different)* p *q);

		double ass = exp(-1 * t * t / 2) / sqrt(2 * 3.14159 * (total_periods - periods_different) * q * p);

		add += ass;
		mul *= ass;

		map1[m].dResult = ass;

		if (bLog)
		{
			ss << " [" << m << "]=" << Double2String(ass, "%.6f");
		}
	}
	if (bLog)
	{
		ss << " [加]=" << Double2String(add, "%.6f");
		ss << " [乘]=" << Double2String(mul, "%.6f");
		ss << " [乘的修正]=" << Double2String(mul * m_bernoulli_mul_adjunstment, "%.6f");
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}


	if (m_bernoulli_mul_adjunstment > 0)  mul *= m_bernoulli_mul_adjunstment;

	if (m_bernoulli_add_start.size() == 0 && m_bernoulli_mul_start.size() == 0)
		return 0;

	//	判断 和数 是否符合
	int result = 0;
	for (int i = 0; i < m_bernoulli_add_start.size(); i++)
	{
		if (add >= m_bernoulli_add_start[i] && add <= m_bernoulli_add_end[i])
		{
			result = 1;
			break;
		}
	}

	if (result == 0 && m_bernoulli_add_start.size() > 0)
		return -1;

	//	判断 乘数 是否符合
	int result1 = 0;
	for (int i = 0; i < m_bernoulli_mul_start.size(); i++)
	{
		if (mul >= m_bernoulli_mul_start[i] && mul <= m_bernoulli_mul_end[i])
		{
			result1 = 1;
			break;
		}
	}

	if (result1 == 0 && m_bernoulli_mul_start.size() > 0)
		return -1;

	return 1;
}

// 根据已经开奖的记录，势能赋值过滤
int CLotteryDoc::PotentialFilter(map<int, Assignment> & map1, LotteryNumber ln, bool bLog)
{
	//	起止期数 选择不对
	if (map1[1].total_periods == 0)
		return 0;

	double add = 0;
	double mul = 1;

	int k = m_lottery_info.regularnum;
	bool bSpecial = false;
	if (m_bContainSpecial && m_lottery_info.specialnum > 0)
		bSpecial = true;
	if (bSpecial)
		k++;

	// 计算 h1 + h2 + h3 + ...... + hi + .....
	double hsum = 0;
	for (int m = 1; m <= m_lottery_info.totalnum; m++)
	{
		int tt = (map1[m].total_periods  - map1[m].periods_recent_number);
		hsum += (map1[m].total_periods  - map1[m].periods_recent_number);
	}
	//hsum += k;

	if (bLog)
		AssignmentFilterLog(map1, _T("Potential 赋值"), ln);

	stringstream ss;
	ss << _T("【漏奖查询】Potential 赋值 ");
	for (int i = 0; i < k; i++)
	{
		int m = 0;
		if (i < m_lottery_info.regularnum)	m = ln.number[i];
		else	m = ln.special_number;
		int goal_count = map1[m].goal_count + 1;
		int total_periods = map1[m].total_periods + 1;
		int periods_different = map1[m].periods_different;
		//if (periods_different == 0) periods_different = total_periods - 1;
		if (m_potential_period_difference == 0) periods_different = 0;

		double p = m_lottery_info.regularnum * 1.0/ m_lottery_info.totalnum;
		double q = (m_lottery_info.totalnum - m_lottery_info.regularnum) * 1.0 / m_lottery_info.totalnum;
		double t = (goal_count - (total_periods - periods_different) * p) / sqrt((total_periods - periods_different)* p *q);

		double bernoulli = exp(-1 * t * t / 2) / sqrt(2 * 3.14159 * (total_periods - periods_different) * q * p);
		double pi = (total_periods - map1[m].periods_recent_number - 1) / hsum;

		double K = 1.0;
		double ass = -1 * K * (goal_count * 1.0 / (total_periods - periods_different)) * log(pi / bernoulli);

		if (m_potential_period_absolute)
			if (ass < 0) ass = -1.0 * ass;

		add += ass;
		mul *= ass;

		map1[m].dResult = ass;

		if (bLog)
		{
			ss << " [" << m << "]=" << Double2String(ass, "%.6f");
		}
	}

	if (bLog)
	{
		ss << " [加]=" << Double2String(add, "%.6f");
		ss << " [乘]=" << Double2String(mul, "%.6f");
		ss << " [乘的修正]=" << Double2String(mul * m_potential_mul_adjunstment, "%.6f");
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
	}

	if (m_potential_mul_adjunstment > 0)  mul *= m_potential_mul_adjunstment;

	if (m_potential_add_start.size() == 0 && m_potential_mul_start.size() == 0)
		return 0;
	
	//	判断 和数 是否符合
	int result = 0;
	for (int i = 0; i < m_potential_add_start.size(); i++)
	{
		if (add >= m_potential_add_start[i] && add <= m_potential_add_end[i])
		{
			result = 1;
			break;
		}
	}

	if (result == 0 && m_potential_add_start.size() > 0)
		return -1;

	//	判断 乘数 是否符合
	int result1 = 0;
	for (int i = 0; i < m_potential_mul_start.size(); i++)
	{
		if (mul >= m_potential_mul_start[i] && mul <= m_potential_mul_end[i])
		{
			result1 = 1;
			break;
		}
	}

	if (result1 == 0 && m_potential_mul_start.size() > 0)
		return -1;

	return 1;
}