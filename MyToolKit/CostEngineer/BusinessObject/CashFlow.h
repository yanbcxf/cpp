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
	/* 最后一笔支付款的月份 */
	int	 LatestPaymentTime();

	static bool Draw(CGridCtrl* pGridCtrl, vector<CCashFlowObj>& cols, CCashFlow& parent);
	static bool Update(string menuCode, int nRow, vector<CCashFlowObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CCashFlowObj>& cols);

	
	CString m_name;				//  分部工程名称
	int		m_building_start;	//	开始时间（月，周）
	int		m_building_duration;//	工期
	double	m_building_cost;	//	分部工程造价
	int		m_payment_interval;	//	支付间隔时间
	int		m_payment_lag;		//  支付延迟时间
	
};


class CCashFlow
{
public:
	CCashFlow();
	~CCashFlow();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	/* 最后一笔支付款的月份 */
	int	 LatestPaymentTime();
	
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

