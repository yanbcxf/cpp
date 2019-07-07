#pragma once


/* 第四章，案例十，综合评估法表 */

class CIntegratedEvaluation;

class CIntegratedEvaluationObj
{
public:
	CIntegratedEvaluationObj();

	~CIntegratedEvaluationObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode, CIntegratedEvaluation& parent);

	/* 最早收到支付款的月份 */
	int  EarliestPaymentTime();
	/* 最后结清款的月份 */
	int	 LatestPaymentTime();
	/* 终值都换算到结清月的月末或月初 */
	double FutureValueOfPartitionedProject(double i);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CIntegratedEvaluationObj>& cols, CIntegratedEvaluation& parent);
	static bool Update(string menuCode, int nRow, vector<CIntegratedEvaluationObj>& cols, CIntegratedEvaluation& parent);
	static bool Delete(string menuCode, int nRow, vector<CIntegratedEvaluationObj>& cols);

	
	CString m_name;						//  工程名称
	double  m_technique_score;			//  技术标 - 得分
	double	m_technique_building_plan;	//	技术标 - 施工方案	
	double	m_technique_duration;		//	技术标 - 工期
	double	m_technique_quality;			//	技术标 - 质量
	double	m_technique_manage;			//	技术标 - 项目管理班
	double	m_technique_credit;			//  技术标 - 企业信誉
	double	m_technique_luban;			//  技术标 - 鲁班工程奖
	double	m_technique_shengyou;		//  技术标 - 省优工程奖

	double	m_business_score;			//	商务标 - 得分
	double	m_business_total_price;		//	商务标 - 总价
	double	m_business_unit_price;		//	商务标 - 单价

};


class CIntegratedEvaluation
{
public:
	CIntegratedEvaluation();
	~CIntegratedEvaluation();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	/* 整个工程结清的月份 */
	int	 LatestPaymentTime();
	/* 整个工程的终值，换算到整个工程的结清月的月末 */
	double FutureValueOfWholeProject();
	/* 整个工程的现值 */
	double PresentValueOfWholeProject();
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CIntegratedEvaluation>& cols);
	static bool Update(string menuCode, int nRow, vector<CIntegratedEvaluation>& cols);
	static bool Delete(string menuCode, int nRow, vector<CIntegratedEvaluation>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CIntegratedEvaluation>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;				//	
	CString m_evaluation_method;	//	评标法
	
	vector<CIntegratedEvaluationObj>	m_objs;	
};

