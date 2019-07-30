#pragma once


/* 第六章，案例一，工程结算 */

class CProjectSettlementEx5;

class CProjectSettlementEx5Obj
{
public:
	CProjectSettlementEx5Obj() {};
	virtual ~CProjectSettlementEx5Obj() {};

	virtual void Serialize(CArchive& ar, double version) = 0;
	virtual bool CreateOrUpdate(string menuCode, CProjectSettlementEx5* parent) = 0;
	virtual string Description() = 0;
	virtual double ProjectPrice() = 0;

public:
	CString	m_scheme;

	CString	m_month;	
	CString	m_name;
};

/* 案例5 工程结算 */
class CProjectSettlementEx5
{
public:
	CProjectSettlementEx5() {
		m_scheme = "";

		m_name = "";
		m_manage_rate = 0;
		m_net_rate = 0;
		m_regulation_rate = 0;
		m_tax_rate = 0;

		m_unit_measure = 0;
		m_total_measure = 0;

		m_provisional_sum = 0;
		m_daywork_labor = 0;
		m_estimate_engineering = 0;
		m_estimate_material = 0;
		m_general_constracting_service_fee = 0;
	};

	~CProjectSettlementEx5()
	{
		for (CProjectSettlementEx5Obj* e : m_objs)
			delete e;
	};

	// virtual void Serialize(CArchive& ar, double version) = 0;
	// virtual bool CreateOrUpdate() = 0;
	// virtual string Description() = 0;
	
	// virtual CProjectSettlementEx5Obj* NewChild(CString scheme);
	// virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual bool AddChild(string menuCode);
	virtual bool UpdateChild(string menuCode, int nRow);
	virtual bool DeleteChild(string menuCode, int nRow);

	static CProjectSettlementEx5* NewParent(CString name);
	static void Serialize(CArchive& ar, double version, CProjectSettlementEx5*  & p);
	static bool Create(string strMenuCode, CProjectSettlementEx5*  & p);
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CProjectSettlementEx5*>& cols);
	static bool Update(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols);
	static bool Delete(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols);
	static unsigned int PopupMenuId(string menuCode);
	// static void Calculate(string menuCode, vector<CProjectSettlementEx5*>& cols);
	static bool Calculate(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols);
	static bool Adjust(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;
		
public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate();
	virtual string Description();
	virtual CProjectSettlementEx5Obj* NewChild(CString scheme);
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual void SortByMonth();
	virtual void Calculate();
	virtual void Adjust();

public:
	CString	m_scheme;			//	方案类型 -> 确定后续程序的执行 
	vector<CProjectSettlementEx5Obj *>	m_objs;	

public:
	CString m_name;				//	工程名称
	double  m_manage_rate;		//	管理费率
	double	m_net_rate;			//	利润率
	double	m_regulation_rate;	//	规费率
	double	m_tax_rate;			//	税率

	double	m_unit_measure;		//	单价措施费
	double	m_total_measure;	//	总价措施费

	double	m_estimate_material;	//	材料暂估单价
	double	m_estimate_engineering;	//	专业工程暂估价
	double	m_daywork_labor;		//	计日工
	double	m_provisional_sum;		//	暂列金额
	double	m_general_constracting_service_fee;		//	总承包服务费

	map<string, double> m_mapProjectUnitPrice;
	map<string, double> m_mapProjectWorkload;
};

/*************************************************************************************/

/* 单价分项分部工程款 */
class CProjectSettlementEx5ObjA : public CProjectSettlementEx5Obj {
public:
	CProjectSettlementEx5ObjA() {
		m_actual_workload = 0;
		m_people_change = 0;
		m_material_change = 0;
	};

public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate(string menuCode, CProjectSettlementEx5* parent);
	virtual string Description();
	virtual double ProjectPrice();

public:
	double	m_actual_workload;		//	工作量
	double	m_material_change;		//	材料费变动
	double	m_people_change;		//	人工费变动
};

/* 单价措施项目费 */
class CProjectSettlementEx5ObjB : public CProjectSettlementEx5Obj {
public:
	CProjectSettlementEx5ObjB() {
		m_fund = 0;
	};

public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate(string menuCode, CProjectSettlementEx5* parent);
	virtual string Description() ;
	virtual double ProjectPrice();
public:
	double	m_fund;		//	措施费
};


