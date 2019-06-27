#pragma once

class CAOANode
{
public:
	CAOANode();

	~CAOANode();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	CString m_name;			//  节点名称
	int	m_x;			//	x 坐标
	int	m_y;			//	y 坐标

};

class CAOAEdge
{
public:
	CAOAEdge();

	~CAOAEdge();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	CString m_name;				//  边（活动）名称
	int		m_from_node;		//	开始节点
	int		m_to_node;			//	

};

class CActivityOnArrow
{
public:
	CActivityOnArrow();

	~CActivityOnArrow();

	void Serialize(CArchive& ar, double version);
	bool CreateOrUpdate(string strMenuCode);
	bool DrawGraph(CGraphCtrl* pCtrl);
	bool AddNode(string menuCode, int x, int y);
	bool AddEdge(string menuCode, int from, int to);
	bool MoveNode(string menuCode, int nRow, int x, int y);
	bool UpdateNode(string menuCode, int nRow);
	bool UpdateEdge(int nRow);
	bool DeleteNode(int nRow);
	bool DeleteEdge(int nRow);

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CActivityOnArrow>& cols);
	static bool Update(string menuCode, int nRow, vector<CActivityOnArrow>& cols);
	static bool Delete(string menuCode, int nRow, vector<CActivityOnArrow>& cols);
	
	static unsigned int PopupMenuId(string menuCode);
	
	static void Calculate(string menuCode, vector<CActivityOnArrow>& cols);
	static void SteelQuantity(string menuCode, vector<CActivityOnArrow>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;					//  网络图名称
	vector<CAOANode> m_nodes;
	vector<CAOAEdge> m_edges;		//	分支节点名称	
	
};

