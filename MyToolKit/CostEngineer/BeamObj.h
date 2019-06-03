#pragma once

/* 梁跨 */
class CBeamSpan
{
public:
	CBeamSpan();

	~CBeamSpan();

	void Serialize(CArchive& ar);

	bool CreateOrUpdate(string menuCode);

	static bool Draw(CGridCtrl* pGridCtrl, vector<CBeamSpan>& cols);
	static bool Update(string menuCode, int nRow, vector<CBeamSpan>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBeamSpan>& cols);
	
	// static void ConcreteQuantity(string menuCode, vector<CColumnObj>& cols);
	// static void SteelQuantity(string menuCode, vector<CColumnObj>& cols);


	CString m_name;
	double	m_length;
	double	m_height;			//	原位标注时 有效
	double	m_breadth;			//	原位标注时 有效
	CString m_steel_bottom;		//	原位标注时 有效
	CString m_steel_left;		//	原位标注时 有效
	CString m_steel_right;		//	原位标注时 有效

};


/* 梁 */
class CBeamObj
{
public:
	CBeamObj();

	~CBeamObj();

	void Serialize(CArchive& ar);

	bool CreateOrUpdate(string strMenuCode);
	void SteelQuantityOfBeamSpan(int spanId, vector<vector<string>>& vecData);

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CBeamObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CBeamObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBeamObj>& cols);
	static unsigned int PopupMenuId(string menuCode);

	static void ConcreteQuantity(string menuCode, vector<CBeamObj>& cols);
	static void SteelQuantity(string menuCode, vector<CBeamObj>& cols);

	static string m_ObjectCode;

	CString m_name;
	double	m_height;
	double	m_breadth;
	CString m_steel_top;
	CString m_steel_bottom;
	CString m_steel_hooping;	
	int		m_quantity;

	vector<CBeamSpan>	m_spans;
};

