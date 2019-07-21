#pragma once


/* 第六章，案例一，工程结算 */

class CProjectSettlement;

class CProjectSettlementObj
{
public:
	CProjectSettlementObj();

	virtual ~CProjectSettlementObj();

	virtual void Serialize(CArchive& ar, double version);

	virtual bool CreateOrUpdate(string menuCode, CProjectSettlement* parent);

	
	CString	m_month;				//  月份
	double	m_actual_workload;		//	实际完成工作量
	double	m_schedule_workload;	//	计划完成工作量
	double	m_party_a_material;		//	甲供材料
};


class CProjectSettlement
{
public:
	CProjectSettlement();
	~CProjectSettlement();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	

	virtual CProjectSettlementObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual bool AddChild(string menuCode);
	virtual bool UpdateChild(string menuCode, int nRow);
	virtual bool DeleteChild(string menuCode, int nRow);

	static CProjectSettlement* NewParent(CString name);
	static void Serialize(CArchive& ar, double version, CProjectSettlement*  & p);
	static bool CreateOrUpdate(string strMenuCode, CProjectSettlement* & p);
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CProjectSettlement*>& cols);
	static bool Update(string menuCode, int nRow, vector<CProjectSettlement*>& cols);
	static bool Delete(string menuCode, int nRow, vector<CProjectSettlement*>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CProjectSettlement*>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;				//	工程名称
	CString m_method;			//	案例N

	double  m_total_price;		//	工程总造价
	double	m_material_percent;	//	建筑材料及设备费占比
	double	m_advance_payment_percent;	//	预付款占比
	double	m_quality_bond_percent;		//	质量保证金占比
		
	vector<CProjectSettlementObj *>	m_objs;	
};

/*************************************************************************************/

class CProjectSettlementEx2 : public CProjectSettlement {
	virtual CProjectSettlementObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
};