#pragma once


/* 第六章，案例一，工程结算 */

class CProjectSettlement;

class CProjectSettlementObj
{
public:
	CProjectSettlementObj() {};
	virtual ~CProjectSettlementObj() {};

	virtual void Serialize(CArchive& ar, double version) = 0;
	virtual bool CreateOrUpdate(string menuCode, CProjectSettlement* parent) = 0;
};


class CProjectSettlement
{
public:
	CProjectSettlement();
	~CProjectSettlement();

	virtual void Serialize(CArchive& ar, double version) = 0;
	virtual bool CreateOrUpdate() = 0;
	virtual string Description() = 0;
	
	virtual CProjectSettlementObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual bool AddChild(string menuCode);
	virtual bool UpdateChild(string menuCode, int nRow);
	virtual bool DeleteChild(string menuCode, int nRow);

	static CProjectSettlement* NewParent(CString name);
	static void Serialize(CArchive& ar, double version, CProjectSettlement*  & p);
	static bool Create(string strMenuCode, CProjectSettlement*  & p);
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CProjectSettlement*>& cols);
	static bool Update(string menuCode, int nRow, vector<CProjectSettlement*>& cols);
	static bool Delete(string menuCode, int nRow, vector<CProjectSettlement*>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CProjectSettlement*>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

public:
	CString	m_scheme;			//	方案类型 -> 确定后续程序的执行 
		
	vector<CProjectSettlementObj *>	m_objs;	
};

/*************************************************************************************/

class CProjectSettlementObjEx2 : public CProjectSettlementObj {
public:
	CProjectSettlementObjEx2() {
		m_month = "";
		m_actual_workload = 0;
		m_schedule_workload = 0;
		m_party_a_material = 0;
	};

public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate(string menuCode, CProjectSettlement* parent);

public:
	CString	m_month;				//  月份
	double	m_actual_workload;		//	实际完成工作量
	double	m_schedule_workload;	//	计划完成工作量
	double	m_party_a_material;		//	甲供材料
};

class CProjectSettlementEx2 : public CProjectSettlement {
public:

	CProjectSettlementEx2() {
		m_name = "";
		m_total_price = 0;
		m_advance_payment_percent = 0;
		m_material_percent = 0;
		m_quality_bond_percent = 0;
	};

	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate();
	virtual string Description();
	virtual CProjectSettlementObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);

public:
	CString m_name;				//	工程名称
	double  m_total_price;		//	工程总造价
	double	m_material_percent;	//	建筑材料及设备费占比
	double	m_advance_payment_percent;	//	预付款占比
	double	m_quality_bond_percent;		//	质量保证金占比
};

/*************************************************************************************/

class CProjectSettlementObjEx3 : public CProjectSettlementObj {
public:
	CProjectSettlementObjEx3() {
		m_month = "";
		m_actual_workload = 0;
	};

public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate(string menuCode, CProjectSettlement* parent);

public:
	CString	m_month;				//  月份
	double	m_actual_workload;		//	实际完成工作量
};


class CProjectSettlementEx3 : public CProjectSettlement {
public:
	CProjectSettlementEx3() {
		m_name = "";
		m_total_workload = 0;
		m_unit_price = 0;
		m_advance_payment_percent = 0;
		m_quality_bond_percent = 0;
	};

	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate();
	virtual string Description();
	virtual CProjectSettlementObj* NewChild();
	virtual bool DrawChild(CGridCtrl* pGridCtrl);

public:
	CString m_name;				//	工程名称
	double	m_total_workload;	//	工程计划总量
	double  m_unit_price;		//	单价
	double	m_advance_payment_percent;	//	预付款占比
	double	m_quality_bond_percent;		//	质量保证金占比
};