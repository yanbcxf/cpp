#pragma once


/* 第四章，案例九，绘制现金流量表 */

class CCashFlow;

class CCashFlowObj
{
public:
	CCashFlowObj();

	~CCashFlowObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	/* 最早收到支付款的月份 */
	int  EarliestPaymentTime();
	/* 最后结清款的月份 */
	int	 LatestPaymentTime();
	/* 终值都换算到结清月的月末或月初 */
	double FutureValueOfPartitionedProject(double i);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CCashFlowObj>& cols, CCashFlow& parent);
	static bool Update(string menuCode, int nRow, vector<CCashFlowObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CCashFlowObj>& cols);

	
	CString m_name;				//  分部工程名称
	int		m_building_start;	//	开始时间（月，周）
	int		m_building_duration;//	工期
	double	m_building_cost;	//	分部工程造价
	int		m_payment_interval;	//	支付周期 - 即每 N 个月结算一次
	int		m_payment_lag;		//  支付延迟时间
	int		m_payment_time;		//	0 期初支付， 1 期末支付
};


class CCashFlow
{
public:
	CCashFlow();
	~CCashFlow();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	/* 整个工程结清的月份 */
	int	 LatestPaymentTime();
	/* 整个工程的终值，换算到整个工程的结清月的月末 */
	double FutureValueOfWholeProject();
	/* 整个工程的现值 */
	double PresentValueOfWholeProject();
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CCashFlow>& cols);
	static bool Update(string menuCode, int nRow, vector<CCashFlow>& cols);
	static bool Delete(string menuCode, int nRow, vector<CCashFlow>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CCashFlow>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;				//	
	double  m_interest_rate;	//	利率
	
	vector<CCashFlowObj>	m_objs;	
};

