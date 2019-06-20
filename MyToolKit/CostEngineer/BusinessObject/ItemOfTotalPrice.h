#pragma once
class CItemOfTotalPrice
{
public:
	CItemOfTotalPrice();

	~CItemOfTotalPrice();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CItemOfTotalPrice>& cols);
	static bool Update(string menuCode, int nRow, vector<CItemOfTotalPrice>& cols);
	static bool Delete(string menuCode, int nRow, vector<CItemOfTotalPrice>& cols);
	static unsigned int PopupMenuId(string menuCode);
	
	static void Calculate(string menuCode, vector<CItemOfTotalPrice>& cols);
	static void SteelQuantity(string menuCode, vector<CItemOfTotalPrice>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;
	double	m_calculate_base;		//	计算基础
	double	m_rate;					//	费率
};

