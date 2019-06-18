#pragma once


/* 第三章，案例九，消耗量定额 */

class CConsumptionQuotaObj
{
public:
	CConsumptionQuotaObj();

	~CConsumptionQuotaObj();

	void Serialize(CArchive& ar, double version);
	bool CreateOrUpdate(string menuCode);
	
	static bool Draw(CGridCtrl* pGridCtrl, vector<CConsumptionQuotaObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CConsumptionQuotaObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CConsumptionQuotaObj>& cols);

	
	CString m_name;				//  人工 、耗材名称
	double	m_quantity;			//	消耗数量
	double	m_unit_price;		//	单位价格
	CString m_unit;				//	计量单位
};


class CConsumptionQuota
{
public:
	CConsumptionQuota();
	~CConsumptionQuota();

	void Serialize(CArchive& ar, double version);
	bool CreateOrUpdate(string strMenuCode);
	void GetQuotaFee(double& people, double& material, double& machine);
	
	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CConsumptionQuota>& cols);
	static bool Update(string menuCode, int nRow, vector<CConsumptionQuota>& cols);
	static bool Delete(string menuCode, int nRow, vector<CConsumptionQuota>& cols);
	static unsigned int PopupMenuId(string menuCode);
	static void Calculate(string menuCode, vector<CConsumptionQuota>& cols);
	static CConsumptionQuota * FindQuota(CString quotaId, vector<CConsumptionQuota>& quotas);

	static string m_ObjectCode;
	static double m_ObjectVersion;


	CString m_identifier;	//	定额 编号
	CString m_name;			//	定额 名称
	int		m_unit;			//	定额 单位
	
	vector<CConsumptionQuotaObj>	m_materials;	//	各种耗材,人工等消耗量定额
};

