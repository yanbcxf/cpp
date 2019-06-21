#pragma once
class CDecisionTree
{
public:
	CDecisionTree();

	~CDecisionTree();

	void Serialize(CArchive& ar, double version);

	bool CreateOrUpdate(string strMenuCode);

	static bool Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CDecisionTree>& cols);
	static bool Update(string menuCode, int nRow, vector<CDecisionTree>& cols);
	static bool Delete(string menuCode, int nRow, vector<CDecisionTree>& cols);
	static unsigned int PopupMenuId(string menuCode);
	
	static void Calculate(string menuCode, vector<CDecisionTree>& cols);
	static void SteelQuantity(string menuCode, vector<CDecisionTree>& cols);

	static string m_ObjectCode;
	static double m_ObjectVersion;

	int		m_code;					//  树节点编号
	CString m_branch_node;			//	分支节点名称	
	double	m_probability;			//	分支发生的概率
	double	m_score;				//  得分 （一般是期望利润）
	int		m_parent_code;			//	父节点编号
};

