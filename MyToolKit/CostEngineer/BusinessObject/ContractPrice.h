#pragma once


/* 第六章，案例一，工程结算 */

class CContractPrice;

class CContractPriceObj
{
public:
	CContractPriceObj() {};
	virtual ~CContractPriceObj() {};

	virtual void Serialize(CArchive& ar, double version) = 0;
	virtual bool CreateOrUpdate(string menuCode, CContractPrice* parent) = 0;
	virtual string Description() = 0;

public:
	CString	m_name;
	CString	m_scheme;
};


class CContractPrice
{
public:
	CContractPrice();
	~CContractPrice();

	virtual void Serialize(CArchive& ar, double version) = 0;
	virtual bool CreateOrUpdate() = 0;
	virtual string Description() = 0;
	
	virtual CContractPriceObj* NewChild(CString scheme);
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual bool AddChild(string menuCode);
	virtual bool UpdateChild(string menuCode, int nRow);
	virtual bool DeleteChild(string menuCode, int nRow);

	static CContractPrice* NewParent(CString name);
	static void Serialize(CArchive& ar, double version, CContractPrice*  & p);
	static bool Create(string strMenuCode, CContractPrice*  & p);
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CContractPrice*>& cols);
	static bool Update(string menuCode, int nRow, vector<CContractPrice*>& cols);
	static bool Delete(string menuCode, int nRow, vector<CContractPrice*>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CContractPrice*>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

public:
	CString	m_scheme;			//	方案类型 -> 确定后续程序的执行 
		
	vector<CContractPriceObj *>	m_objs;	
};

/*************************************************************************************/

/* 单价分项分部工程款 */
class CContractPriceEx2ObjA : public CContractPriceObj {
public:
	CContractPriceEx2ObjA() {
		m_actual_workload = 0;
		m_unit_price = 0;
	};

public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate(string menuCode, CContractPrice* parent);
	virtual string Description() { return " Object A "; };

public:
	double	m_actual_workload;		//	工作量
	double	m_unit_price;			//	单价
};

/* 单价措施项目费 */
class CContractPriceEx2ObjB : public CContractPriceObj {
public:
	CContractPriceEx2ObjB() {
		m_fund = 0;
	};

public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate(string menuCode, CContractPrice* parent);
	virtual string Description() { return " Object B "; };
public:
	double	m_fund;		//	措施费
};


/* 合同价款 */
class CContractPriceEx2 : public CContractPrice {
public:

	CContractPriceEx2() {
		m_name = "";
		m_total_price = 0;
		m_advance_payment_percent = 0;
		m_material_percent = 0;
		m_quality_bond_percent = 0;
	};

	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate();
	virtual string Description();
	virtual CContractPriceObj* NewChild(CString scheme);
	virtual bool DrawChild(CGridCtrl* pGridCtrl);

public:
	CString m_name;				//	工程名称
	double  m_total_price;		//	工程总造价
	double	m_material_percent;	//	建筑材料及设备费占比
	double	m_advance_payment_percent;	//	预付款占比
	double	m_quality_bond_percent;		//	质量保证金占比
};
