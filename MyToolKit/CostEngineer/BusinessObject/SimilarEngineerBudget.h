#pragma once


/* 第三章，案例三，类似工程预算法 */

class CSimilarEngineerBudgetObj
{
public:
	CSimilarEngineerBudgetObj();

	~CSimilarEngineerBudgetObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CSimilarEngineerBudgetObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CSimilarEngineerBudgetObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CSimilarEngineerBudgetObj>& cols);

	
	CString m_name;				//  人工费，材料费等
	double	m_percent;			//	占类似工程的比例
	double	m_different;		//	现工程与原工程的差异系数
	
};


class CSimilarEngineerBudget
{
public:
	CSimilarEngineerBudget();
	~CSimilarEngineerBudget();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CSimilarEngineerBudget>& cols);
	static bool Update(string menuCode, int nRow, vector<CSimilarEngineerBudget>& cols);
	static bool Delete(string menuCode, int nRow, vector<CSimilarEngineerBudget>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CSimilarEngineerBudget>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;			//	
	double  m_original_unilateral_cost;		//	原工程单方造价
	double	m_quantity;		// 现工程工程量		

	vector<CSimilarEngineerBudgetObj>	m_percents;	//	类似工程各费用的比例 及现工程与类似工程的差异系数
};

