#pragma once


/* 第四章，案例十，综合评估法表 */

class CIntegratedEvaluation;

class CIntegratedEvaluationObj
{
public:
	CIntegratedEvaluationObj();

	~CIntegratedEvaluationObj();

	virtual void Serialize(CArchive& ar, double version);

	virtual bool CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent);

	/* 最早收到支付款的月份 */
	int  EarliestPaymentTime();
	/* 最后结清款的月份 */
	int	 LatestPaymentTime();
	/* 终值都换算到结清月的月末或月初 */
	double FutureValueOfPartitionedProject(double i);

	

	
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
	virtual ~CIntegratedEvaluation();


	virtual CIntegratedEvaluationObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual bool AddChild(string menuCode);
	virtual bool UpdateChild(string menuCode, int nRow);
	virtual bool DeleteChild(string menuCode, int nRow);
	
	static CIntegratedEvaluation* NewParent(CString name);
	static void Serialize(CArchive& ar, double version, CIntegratedEvaluation*  & p);
	static bool CreateOrUpdate(string strMenuCode, CIntegratedEvaluation* & p);
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CIntegratedEvaluation *>& cols);
	static bool Update(string menuCode, int nRow, vector<CIntegratedEvaluation *>& cols);
	static bool Delete(string menuCode, int nRow, vector<CIntegratedEvaluation *>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CIntegratedEvaluation *>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;				//	
	CString m_evaluation_method;	//	评标法
	
	vector<CIntegratedEvaluationObj*>	m_objs;	
};

class CIntegratedEvaluationEx10 : public CIntegratedEvaluation {
	virtual CIntegratedEvaluationObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
};

class CIntegratedEvaluationEx11 : public CIntegratedEvaluation {
	virtual CIntegratedEvaluationObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
};

class CIntegratedEvaluationEx12 : public CIntegratedEvaluation {
	virtual CIntegratedEvaluationObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
};

class CIntegratedEvaluationObjEx10 : public CIntegratedEvaluationObj {
	virtual bool CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent);
};

class CIntegratedEvaluationObjEx11 : public CIntegratedEvaluationObj {
	virtual bool CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent);
};

class CIntegratedEvaluationObjEx12 : public CIntegratedEvaluationObj {
	virtual bool CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent);
};