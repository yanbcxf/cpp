#pragma once


/* 第六章，案例一，工程结算 */

class CFinanceAnalysis;

class CFinanceAnalysisObj
{
public:
	CFinanceAnalysisObj() {
		m_scheme = "";
		m_month = "";
		m_name = "";

		m_amount_of_money = 0;
	};
	virtual ~CFinanceAnalysisObj() {};

	virtual void Serialize(CArchive& ar, double version);
	virtual bool CopyTo(CFinanceAnalysis* parent);
	virtual bool CreateOrUpdate(string menuCode, CFinanceAnalysis* parent);
	
	virtual string Description() = 0;
	virtual double AmountOfMoney() = 0;
	virtual bool Assist(CFinanceAnalysis* parent) = 0;
	virtual bool HasAssist() = 0;
	virtual CFinanceAnalysisObj * Clone() = 0;
	virtual vector<string> GetOptions() = 0;

	void SetDlgEditItem(CDyncItemGroupDlg & infd, CFinanceAnalysis * p, int nRow, string scheme, string caption,  double dbMin, double dbMax, double def = 0);

public:
	CString	m_scheme;

	CString	m_month;	
	CString	m_name;

	double	m_amount_of_money;		//	金额
};

/* 案例5 工程结算 */
class CFinanceAnalysis
{
public:
	CFinanceAnalysis() {
		m_scheme = "";

		m_name = "";
		m_discount = "";
		m_net_residual_rate = 0;
		m_tax_surcharge_rate = 0;
		m_income_tax_rate = 0;

		m_tax_on_purchase = 0;
	};

	~CFinanceAnalysis()
	{
		for (CFinanceAnalysisObj* e : m_objs)
			delete e;
	};

	// virtual void Serialize(CArchive& ar, double version) = 0;
	// virtual bool CreateOrUpdate() = 0;
	// virtual string Description() = 0;
	
	// virtual CFinanceAnalysisObj* NewChild(CString scheme);
	// virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual bool AddChild(string menuCode);
	virtual bool UpdateChild(string menuCode, int nRow);
	virtual bool DeleteChild(string menuCode, int nRow);
	virtual bool AssistChild(string menuCode, int nRow);
	virtual bool CopyToChild(string menuCode, int nRow);

	static CFinanceAnalysis* NewParent(CString name);
	static void Serialize(CArchive& ar, double version, CFinanceAnalysis*  & p);
	static bool Create(string strMenuCode, CFinanceAnalysis*  & p);
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CFinanceAnalysis*>& cols);
	static bool Update(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols);
	static bool Delete(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols);
	static unsigned int PopupMenuId(string menuCode);
	// static void Calculate(string menuCode, vector<CFinanceAnalysis*>& cols);
	static bool Calculate(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols);
	static bool Adjust(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;
		
public:
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate();
	virtual string Description();
	virtual CFinanceAnalysisObj* NewChild(CString scheme) = 0;
	virtual bool DrawChild(CGridCtrl* pGridCtrl);
	virtual void SortByMonth();
	virtual void Calculate();
	virtual void Adjust();
	virtual bool GetAmountOfMoney(CString month, CString scheme, CString name, double & amount);
	virtual double AccumulativeTax(int nMonth) = 0;


public:
	CString	m_scheme;			//	方案类型 -> 确定后续程序的执行 
	vector<CFinanceAnalysisObj *>	m_objs;	

public:
	CString m_name;				//	工程名称
	CString m_discount;			//	固定资产折旧方法
	double	m_net_residual_rate;	//	折旧期末净残值率
	double	m_tax_surcharge_rate;	//	税金附加率
	double	m_income_tax_rate;		//	所得税率

	double	m_tax_on_purchase;		//	建设期进项税额

};

class CBeforeFinancing : public CFinanceAnalysis {
public:
	virtual double AccumulativeTax(int nMonth);
	virtual CFinanceAnalysisObj* NewChild(CString scheme);
};

class CAfterFinancing : public CFinanceAnalysis {
public:
	CAfterFinancing() {
		for (int i = 0; i < 5; i++) {
			m_loan_borrow[i] = "";
			m_loan_rate[i] = 0;
		}
	};
	virtual double AccumulativeTax(int nMonth);
	virtual CFinanceAnalysisObj* NewChild(CString scheme);
	virtual void Serialize(CArchive& ar, double version);
	virtual bool CreateOrUpdate();
	virtual double LoanRemaining(int nMonth, int nLoan);
	virtual void Calculate();
public:
	double	m_loan_rate[5];		//	贷款利率
	CString	m_loan_borrow[5];	//	贷款借入时点	
};

/*************************************************************************************/

/* 现金流出 */
class CFinanceAnalysisObjA : public CFinanceAnalysisObj {
public:
	CFinanceAnalysisObjA() {
		
	};

public:
	virtual string Description();
	virtual double AmountOfMoney();
	virtual bool Assist(CFinanceAnalysis* parent);
	virtual bool HasAssist();
	virtual vector<string> GetOptions(); 

	virtual CFinanceAnalysisObj * Clone() {
		CFinanceAnalysisObjA * p = new CFinanceAnalysisObjA();
		p->m_scheme = m_scheme;
		p->m_month = m_month;
		p->m_name = m_name;
		p->m_amount_of_money = m_amount_of_money;
		return p;
	};

public:
	
};

class CFinanceAnalysisObjA1 : public CFinanceAnalysisObj {
public:
	CFinanceAnalysisObjA1() {

	};

public:
	virtual string Description();
	virtual double AmountOfMoney();
	virtual bool Assist(CFinanceAnalysis* parent);
	virtual bool HasAssist();
	virtual vector<string> GetOptions();

	virtual CFinanceAnalysisObj * Clone() {
		CFinanceAnalysisObjA1 * p = new CFinanceAnalysisObjA1();
		p->m_scheme = m_scheme;
		p->m_month = m_month;
		p->m_name = m_name;
		p->m_amount_of_money = m_amount_of_money;
		return p;
	};

public:

};

/* 现金流入 */
class CFinanceAnalysisObjB : public CFinanceAnalysisObj {
public:
	CFinanceAnalysisObjB() {
		
	};

public:
	virtual string Description() ;
	virtual double AmountOfMoney();
	virtual bool Assist(CFinanceAnalysis* parent);
	virtual bool HasAssist();
	virtual vector<string> GetOptions();

	virtual CFinanceAnalysisObj * Clone() {
		CFinanceAnalysisObjB * p = new CFinanceAnalysisObjB();
		p->m_scheme = m_scheme;
		p->m_month = m_month;
		p->m_name = m_name;
		p->m_amount_of_money = m_amount_of_money;
		return p;
	};
public:
	
};

/* 借款n */
class CFinanceAnalysisObjC : public CFinanceAnalysisObj {
public:
	CFinanceAnalysisObjC() {

	};

public:
	virtual string Description();
	virtual double AmountOfMoney();
	virtual bool Assist(CFinanceAnalysis* parent);
	virtual bool HasAssist();
	virtual vector<string> GetOptions();

	virtual CFinanceAnalysisObj * Clone() {
		CFinanceAnalysisObjC * p = new CFinanceAnalysisObjC();
		p->m_scheme = m_scheme;
		p->m_month = m_month;
		p->m_name = m_name;
		p->m_amount_of_money = m_amount_of_money;
		return p;
	};
public:

};

/* 总成本费用 */
class CFinanceAnalysisObjD : public CFinanceAnalysisObj {
public:
	CFinanceAnalysisObjD() {

	};

public:
	virtual string Description();
	virtual double AmountOfMoney();
	virtual bool Assist(CFinanceAnalysis* parent);
	virtual bool HasAssist();
	virtual vector<string> GetOptions();

	virtual CFinanceAnalysisObj * Clone() {
		CFinanceAnalysisObjD * p = new CFinanceAnalysisObjD();
		p->m_scheme = m_scheme;
		p->m_month = m_month;
		p->m_name = m_name;
		p->m_amount_of_money = m_amount_of_money;
		return p;
	};
public:

};

/* 利润及利润分配 */
class CFinanceAnalysisObjE : public CFinanceAnalysisObj {
public:
	CFinanceAnalysisObjE() {

	};

public:
	virtual string Description();
	virtual double AmountOfMoney();
	virtual bool Assist(CFinanceAnalysis* parent);
	virtual bool HasAssist();
	virtual vector<string> GetOptions();

	virtual CFinanceAnalysisObj * Clone() {
		CFinanceAnalysisObjE * p = new CFinanceAnalysisObjE();
		p->m_scheme = m_scheme;
		p->m_month = m_month;
		p->m_name = m_name;
		p->m_amount_of_money = m_amount_of_money;
		return p;
	};
public:

};



