#pragma once


/* 第三章，案例九，综合单价分析表 */

class CCompositeUnitPriceObj
{
public:
	CCompositeUnitPriceObj();

	~CCompositeUnitPriceObj();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CCompositeUnitPriceObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CCompositeUnitPriceObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CCompositeUnitPriceObj>& cols);

	
	CString m_name;				//  人工 、耗材名称
	double	m_quantity;			//	消耗数量
	double	m_unit_price;		//	单位价格
	CString m_unit;				//	计量单位
};


class CCompositeUnitPrice
{
public:
	CCompositeUnitPrice();
	~CCompositeUnitPrice();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CCompositeUnitPrice>& cols);
	static bool Update(string menuCode, int nRow, vector<CCompositeUnitPrice>& cols);
	static bool Delete(string menuCode, int nRow, vector<CCompositeUnitPrice>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CCompositeUnitPrice>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;			//	
	double  m_formula_quantity;		//	计量规范工程量
	double	m_consumption_quantity;		//	消耗量定额工程量		

	vector<CCompositeUnitPriceObj>	m_materials;	//	各种耗材,人工等消耗量定额
};

