#pragma once

class CAOANode
{
public:
	CAOANode();

	~CAOANode();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string menuCode);

	CString m_name;		//  节点名称
	int	m_x;			//	x 坐标
	int	m_y;			//	y 坐标

	/* 以下不保存到 DOC 文档 */
	int	m_earliest_event_time;	//	以该节点为开始节点的各项工作的最早开始时间
	int m_latest_event_time;	//  以该节点为结束节点的各项工作的最迟完成时间
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
	int		m_duration;			//  活动持续时间

	/* 以下不保存到 DOC 文档 */
	int		m_earliest_start;		// 最早开始时间
	int		m_earliest_finish;		// 最早完成时间
	int		m_latest_start;			// 最晚开始时间
	int		m_latest_finish;		// 最晚完成时间
	int		m_total_float;			// 总时差
	int		m_free_float;			// 自由时差

};

class CActivityOnArrow
{
public:
	CActivityOnArrow();

	~CActivityOnArrow();

	void Serialize(CArchive& ar, double version);
	bool CreateOrUpdate(string strMenuCode);
	bool DrawGraph(CGraphCtrl* pCtrl);
	void InvalidateCaculate();
	bool AddNode(string menuCode, int x, int y);
	bool AddEdge(string menuCode, int from, int to);
	bool MoveNode(string menuCode, int nRow, int x, int y);
	bool UpdateNode(string menuCode, int nRow);
	bool UpdateEdge(string menuCode, int nRow);
	bool DeleteNode(int nRow);
	bool DeleteEdge(int nRow);
	void Calculate();

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CActivityOnArrow>& cols);
	static bool Update(string menuCode, int nRow, vector<CActivityOnArrow>& cols);
	static bool Delete(string menuCode, int nRow, vector<CActivityOnArrow>& cols);
	static bool Copy(string menuCode, int nRow, vector<CActivityOnArrow>& cols);
	static bool TimeCoordinate(string menuCode, int nRow, vector<CActivityOnArrow>& cols);

	static unsigned int PopupMenuId(string menuCode);
	
	static void Calculate(string menuCode, vector<CActivityOnArrow>& cols);
	static void SteelQuantity(string menuCode, vector<CActivityOnArrow>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	CString m_name;					//  网络图名称
	vector<CAOANode> m_nodes;
	vector<CAOAEdge> m_edges;		//	分支节点名称	
	
};

