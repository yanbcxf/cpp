#pragma once

class CCalcFinanceReport : public CCalculateTask
{
public:
	CCalcFinanceReport(HWND hwnd, int logType);
	~CCalcFinanceReport(void);

	string ClassName(){ return "CCalcFinanceReport"; }

	void Execute();
	static void Execute1(void* firstArg);


public :
	string				m_str_db_code;			//	指定请求的 财务报表
	vector<string>		m_vec_zb_code;			//	指定请求的 指标 （营业收入/ 营业成本/ 利润等）

	vector<string>		m_vec_csrc_gate_code;	//	证监会行业代码: 门类
	vector<string>		m_vec_csrc_big_code;	//	证监会行业代码: 大类
	vector<string>		m_vec_reg_code;			//	身份区域代码

};
