#pragma once


/* 第三章，案例三，概算指标法 */

class CBudgetIndexObj
{
public:
	CBudgetIndexObj();

	~CBudgetIndexObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CBudgetIndexObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CBudgetIndexObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBudgetIndexObj>& cols);

	
	CString m_name;				//  人工 、耗材名称
	double	m_quantity;			//	消耗数量
	double	m_unit_price;		//	单位价格
	
};


class CBudgetIndex
{
public:
	CBudgetIndex();
	~CBudgetIndex();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CBudgetIndex>& cols);
	static bool Update(string menuCode, int nRow, vector<CBudgetIndex>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBudgetIndex>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CBudgetIndex>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;			//	
	double  m_original_unilateral_cost;		//	原工程单方造价
	double	m_quantity;		// 现工程工程量		

	vector<CBudgetIndexObj>	m_percents;	//	类似工程各费用的比例 及现工程与类似工程的差异系数
};

