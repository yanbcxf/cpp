#pragma once


/* 第三章，案例四，施工图预算-  实物量法 */

class CBuildingBudgetObj
{
public:
	CBuildingBudgetObj();

	~CBuildingBudgetObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CBuildingBudgetObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CBuildingBudgetObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBuildingBudgetObj>& cols);

	
	CString m_name;				//  人工 、耗材名称
	double	m_quantity;			//	消耗数量
	double	m_unit_price;		//	单位价格
	CString m_unit;				//	计量单位
};


class CBuildingBudget
{
public:
	CBuildingBudget();
	~CBuildingBudget();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CBuildingBudget>& cols);
	static bool Update(string menuCode, int nRow, vector<CBuildingBudget>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBuildingBudget>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CBuildingBudget>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;			//	
	double  m_other_material_cost_percent;		//	其它材料费占主材料费的比例
	double	m_machine_tool_cost_percent;		//	施工机具使用费占人材机之和的比例		

	vector<CBuildingBudgetObj>	m_materials;	//	各种耗材,人工等
};

