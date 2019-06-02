#pragma once

/* Áº¿ç */
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
	
	//static void ConcreteQuantity(string menuCode, vector<CColumnObj>& cols);
	//static void SteelQuantity(string menuCode, vector<CColumnObj>& cols);


	CString m_name;
	double	m_height;
	double	m_length;
	double	m_breadth;
	CString m_steel_middle;		//	
	CString m_steel_left;		//
	CString m_steel_right;		//	

};


/* Áº */
class CBeamObj
{
public:
	CBeamObj();

	~CBeamObj();

	void Serialize(CArchive& ar);

	bool CreateOrUpdate(string strMenuCode);

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CBeamObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CBeamObj>& cols);
	static bool Delete(string menuCode, int nRow, vector<CBeamObj>& cols);
	static unsigned int PopupMenuId(string menuCode);

	//static void ConcreteQuantity(string menuCode, vector<CColumnObj>& cols);
	//static void SteelQuantity(string menuCode, vector<CColumnObj>& cols);

	static string m_ObjectCode;

	CString m_name;
	double	m_height;
	double	m_length;
	double	m_breadth;
	CString m_steel_vertical;
	int		m_quantity;

	vector<CBeamSpan>	m_spans;
};

