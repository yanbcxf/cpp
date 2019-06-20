#pragma once


/* 第三章，案例九，综合单价分析表 */

class CItemOfUnitPriceObj
{
public:
	CItemOfUnitPriceObj();

	~CItemOfUnitPriceObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode, vector<CConsumptionQuota>& quotas);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CItemOfUnitPriceObj>& cols, vector<CConsumptionQuota>& quotas);
	static bool Update(string menuCode, int nRow, vector<CItemOfUnitPriceObj>& cols, vector<CConsumptionQuota>& quotas);
	static bool Delete(string menuCode, int nRow, vector<CItemOfUnitPriceObj>& cols);

	
	CString m_consumption_quota_id;				//  消耗定额编号
	double	m_consumption_quantity;		//	消耗量定额工程量
};


class CItemOfUnitPrice
{
public:
	CItemOfUnitPrice();
	~CItemOfUnitPrice();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CItemOfUnitPrice>& cols);
	static bool Update(string menuCode, int nRow, vector<CItemOfUnitPrice>& cols);
	static bool Delete(string menuCode, int nRow, vector<CItemOfUnitPrice>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CItemOfUnitPrice>& cols, vector<CConsumptionQuota>& quotas);
	static void Calculate1(string menuCode, vector<CItemOfUnitPrice>& cols, vector<CConsumptionQuota>& quotas);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;			//	
	double  m_formula_quantity;		//	计量规范工程量
			

	vector<CItemOfUnitPriceObj>	m_materials;	//	各种耗材,人工等消耗量定额
};

