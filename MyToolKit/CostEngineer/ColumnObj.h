#pragma once
class CColumnObj
{
public:
	CColumnObj();

	~CColumnObj();

	void Serialize(CArchive& ar);

	bool CreateOrUpdate(string strMenuCode);

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CColumnObj>& cols);
	static bool Update(string menuCode, int nRow, vector<CColumnObj>& cols);
	static unsigned int PopupMenuId(string menuCode);

	static string m_ObjectCode;

	CString m_name;
	double	m_height;
	double	m_length;
	double	m_breadth;
	int		m_quantity;
};

