#include "stdafx.h"
#include "ActivityOnArrow.h"
#include "GridDlg.h"


CAOANode::CAOANode()
{
	m_name = "";
	m_x = 0;
	m_y = 0;
}


CAOANode::~CAOANode()
{
}



void CAOANode::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_x;
		ar << m_y;
	}
	else {
		ar >> m_name;
		ar >> m_x;
		ar >> m_y;
	}
}



bool CAOANode::CreateOrUpdate(string menuCode) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("x"), 64);
	if (m_x > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_x);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("y"), 64);
	if (m_y > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_y);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	infd.Init(_T("图节点 参数设置"), _T("图节点 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_x = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_y = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		return true;
	}
	return false;
}

/**********************************************************************/

CAOAEdge::CAOAEdge()
{
	m_name = "";
	m_from_node = -1;
	m_to_node = -1;
	m_duration = -1;
}


CAOAEdge::~CAOAEdge()
{
}



void CAOAEdge::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_from_node;
		ar << m_to_node;
		ar << m_duration;
	}
	else {
		ar >> m_name;
		ar >> m_from_node;
		ar >> m_to_node;
		ar >> m_duration;
	}
}


bool CAOAEdge::CreateOrUpdate(string menuCode) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("起始节点"), 64);
	if (m_from_node >= 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_from_node);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结束节点"), 64);
	if (m_to_node >= 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_to_node);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("持续时间"), 64);
	if (m_duration >= 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_duration);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	
	infd.Init(_T("图边 参数设置"), _T("图边 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_from_node = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_to_node = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_duration = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		return true;
	}
	return false;
}

/**********************************************************************/

/* 双代号网络图 */
string CActivityOnArrow::m_ObjectCode = "01040701";
double CActivityOnArrow::m_ObjectVersion = 1.0;

CActivityOnArrow::CActivityOnArrow()
{
	m_name = "";
}


CActivityOnArrow::~CActivityOnArrow()
{
}

void CActivityOnArrow::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		
		ar << m_nodes.size();
		for (int i = 0; i < m_nodes.size(); i++) {
			m_nodes[i].Serialize(ar, version);
		}

		ar << m_edges.size();
		for (int i = 0; i < m_edges.size(); i++) {
			m_edges[i].Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CAOANode bs;
			bs.Serialize(ar, version);
			m_nodes.push_back(bs);
		}

		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CAOAEdge bs;
			bs.Serialize(ar, version);
			m_edges.push_back(bs);
		}

		InvalidateCaculate();
	}
}

bool CActivityOnArrow::CreateOrUpdate(string menuCode) {

	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("网络图名称"), 64);
	if(!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

		
	infd.Init(_T("双代号网络图 参数设置"), _T("双代号网络图 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		
		return true;
	}
	return false;
}



bool CActivityOnArrow::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CActivityOnArrow>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	vector<string> vecHeader;
	vector<vector<string>>	vecData;
	vecHeader.push_back("网络图名称");
	vecHeader.push_back("节点数");
	vecHeader.push_back("边数");
	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");

	for (CActivityOnArrow e : cols) {
		vector<string> vec;
		vec.push_back(e.m_name.GetBuffer());
		vec.push_back(Int2String(e.m_nodes.size()));
		vec.push_back(Int2String(e.m_edges.size()));
		vec.push_back("修改" + ActionType2String(ActionType::Update));
		vec.push_back("删除" + ActionType2String(ActionType::Delete));
		vec.push_back("复制" + ActionType2String(ActionType::Copy));
		if(!e.m_bTimeCoordinate)	vec.push_back("时标网络" + ActionType2String(ActionType::Calculate));
		else vec.push_back("非时标网络" + ActionType2String(ActionType::Calculate));

		vecData.push_back(vec);
	}
	return DrawGrid(pGridCtrl, vecHeader, vecData);
	
}

bool CActivityOnArrow::Update(string menuCode, int nRow, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	if(nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CActivityOnArrow::m_ObjectCode);
	return false;
}

bool CActivityOnArrow::Delete(string menuCode, int nRow, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CActivityOnArrow>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}

bool CActivityOnArrow::Copy(string menuCode, int nRow, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		CActivityOnArrow aoa = cols[nRow - 1];
		aoa.m_name += "-cp";
		cols.insert(cols.begin(), aoa);
		return true;
	}
	return false;
}


unsigned int CActivityOnArrow::PopupMenuId(string menuCode) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}


bool CActivityOnArrow::DrawGraph(CGraphCtrl* pCtrl) {
	if (!pCtrl)
		return false;

	
	int sizex = 0, sizey = 0;

	pCtrl->initGraph();
	for (int i = 0; i < m_nodes.size(); i++) {
		int x = m_nodes[i].m_x;
		int y = m_nodes[i].m_y;
		if (m_bTimeCoordinate) {
			if (m_nodes[i].m_earliest_event_time >= 0)
				x = 50 + m_nodes[i].m_earliest_event_time * 2 * 15;
		}

		/* 计算图纸的尺寸、时标线的间隔 */
		if (x > sizex)	sizex = x;
		if (y > sizey)	sizey = y;

		if (m_nodes[i].m_earliest_event_time < 0)
			pCtrl->AddNode(x, y, m_nodes[i].m_name.GetBuffer());
		else {
			string tips = "节点: " + string(m_nodes[i].m_name.GetBuffer()) + "\n";
			if (m_nodes[i].m_earliest_event_time >= 0) tips += "最早开始: " + Int2String(m_nodes[i].m_earliest_event_time) + "\n";
			if (m_nodes[i].m_latest_event_time >= 0) tips += "最晚完成: " + Int2String(m_nodes[i].m_latest_event_time) + "\n";

			pCtrl->AddNode(x, y, m_nodes[i].m_name.GetBuffer(), "", "" , tips);
		}
	}

	pCtrl->SetGraphSize(sizex + 100, sizey + 100, m_bTimeCoordinate ? 2 : 0);

	for (int i = 0; i < m_edges.size(); i++) {
		if (m_edges[i].m_earliest_start < 0) {
			pCtrl->AddEdge(m_edges[i].m_from_node, m_edges[i].m_to_node,
				m_edges[i].m_name.GetBuffer(),
				m_edges[i].m_duration >= 0 ? Int2String(m_edges[i].m_duration) : "", 
				"", false,
				m_edges[i].m_duration > 0 ? true : false);
		}
		else {
			string tips = "边: " + string(m_edges[i].m_name.GetBuffer()) + "\n";
			if (m_edges[i].m_earliest_start >= 0) tips += "Earliest Start: " + Int2String(m_edges[i].m_earliest_start) + "\n" ;
			if (m_edges[i].m_earliest_finish >= 0) tips += "Earliest Finish: " + Int2String(m_edges[i].m_earliest_finish) + "\n";
			if (m_edges[i].m_latest_start >= 0) tips += "Latest Start: " + Int2String(m_edges[i].m_latest_start) + "\n";
			if (m_edges[i].m_latest_finish >= 0) tips += "Latest Finish: " + Int2String(m_edges[i].m_latest_finish) + "\n";
			if (m_edges[i].m_total_float >= 0) tips += "Total Float: " + Int2String(m_edges[i].m_total_float) + "\n";
			if (m_edges[i].m_free_float >= 0) tips += "Free Float: " + Int2String(m_edges[i].m_free_float) + "\n";

			pCtrl->AddEdge(m_edges[i].m_from_node, m_edges[i].m_to_node,
				m_edges[i].m_name.GetBuffer(),
				m_edges[i].m_duration >= 0 ? Int2String(m_edges[i].m_duration) : "", tips,
				m_edges[i].m_total_float == 0? true : false,
				m_edges[i].m_duration > 0 ? true : false);
		}
		
	}
	pCtrl->Refresh();
	return true;
}

void CActivityOnArrow::InvalidateCaculate() {
	/* 初始化 数据结构 */
	for (int n = 0; n < m_nodes.size(); n++) {
		m_nodes[n].m_earliest_event_time = -1;
		m_nodes[n].m_latest_event_time = -1;
	}
	for (int e = 0; e < m_edges.size(); e++) {
		m_edges[e].m_earliest_start = -1;
		m_edges[e].m_earliest_finish = -1;
		m_edges[e].m_latest_start = -1;
		m_edges[e].m_latest_finish = -1;
		m_edges[e].m_total_float = -1;
		m_edges[e].m_free_float = -1;
	}
	m_bTimeCoordinate = false;
}

bool CActivityOnArrow::AddNode(string menuCode, int x, int y) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CAOANode node;
	node.m_x = x;
	node.m_y = y;
	node.m_name.Format("%d", m_nodes.size() + 1);
	if (node.CreateOrUpdate(menuCode)) {
		m_nodes.push_back(node);
	}
	return true;
}

string DecimalTo26System(int x) {
	if (x < 0) return "";
	int a = x / 26;
	int b = x % 26;
	return DecimalTo26System(a - 1) + (char)(b + 'A');
}

bool CActivityOnArrow::AddEdge(string menuCode, int from, int to) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CAOAEdge edge;
	edge.m_from_node = from;
	edge.m_to_node = to;
	edge.m_name = DecimalTo26System(m_edges.size()).c_str();
	if (edge.CreateOrUpdate(menuCode)) {
		m_edges.push_back(edge);
		InvalidateCaculate();
	}
	return true;
}

bool CActivityOnArrow::MoveNode(string menuCode, int nRow, int x, int y) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	m_nodes[nRow].m_x = x;
	m_nodes[nRow].m_y = y;
		
	return true;
}

bool CActivityOnArrow::UpdateNode(string menuCode, int nRow) {
	if (m_nodes[nRow].CreateOrUpdate(menuCode)) {
		InvalidateCaculate();
		return true;
	}
	return false;
}

bool CActivityOnArrow::UpdateEdge(string menuCode, int nRow) {
	if (m_edges[nRow].CreateOrUpdate(menuCode)) {
		InvalidateCaculate();
		return true;
	}
	return false;
}



bool CActivityOnArrow::DeleteNode(int nRow) {
	bool bDelete;
	do {
		bDelete = false;
		vector<CAOAEdge>::iterator it;
		for (it = m_edges.begin(); it != m_edges.end(); it++) {
			if (it->m_from_node == nRow || it->m_to_node == nRow) {
				m_edges.erase(it);
				bDelete = true;
				break;
			}
		}
	} while (bDelete);

	for (int i = 0; i < m_edges.size(); i++) {
		if (m_edges[i].m_from_node > nRow)
			m_edges[i].m_from_node = m_edges[i].m_from_node - 1;
		if (m_edges[i].m_to_node > nRow)
			m_edges[i].m_to_node = m_edges[i].m_to_node - 1;
	}

	vector<CAOANode>::iterator it;
	int idx = 0;
	for (it = m_nodes.begin(); it != m_nodes.end(); it++, idx++) {
		if (idx == nRow) {
			m_nodes.erase(it);
			bDelete = true;
			break;
		}
	}

	InvalidateCaculate();
	return true;
}

bool CActivityOnArrow::DeleteEdge(int nRow) {

	vector<CAOAEdge>::iterator it;
	int idx = 0;
	for (it = m_edges.begin(); it != m_edges.end(); it++, idx++) {
		if (idx == nRow) {
			m_edges.erase(it);
			break;
		}
	}
	InvalidateCaculate();
	return true;
}

void CActivityOnArrow::SteelQuantity(string menuCode, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return;

}

void CActivityOnArrow::Calculate() {
	/* 寻找每个图的 起始节点 和 结束节点 */
	vector<int> beginNode;
	vector<int> endNode;
	for (int n = 0; n < m_nodes.size(); n++) {
		bool bStart = false;
		bool bEnd = false;
		for (int e = 0; e < m_edges.size(); e++) {
			if (m_edges[e].m_from_node == n) bStart = true;
			if (m_edges[e].m_to_node == n) bEnd = true;
		}

		if (bStart && !bEnd) {
			beginNode.push_back(n);
		}
		if (!bStart && bEnd) {
			endNode.push_back(n);
		}
	}
	/* 有多个起始节点 和 结束节点，则图不合格 */
	if (beginNode.size() > 1 || endNode.size() > 1 || beginNode.size() == 0 || endNode.size() == 0)
		return;

	InvalidateCaculate();

	vector<int> begin;
	vector<int> end;
	/* 从前向后，计算各节点、各活动的 最早开始时间 */
	m_nodes[beginNode[0]].m_earliest_event_time = 0;
	begin.push_back(beginNode[0]);
	do {
		end.clear();
		for (auto n : begin) {
			for (int e = 0; e < m_edges.size(); e++) {
				if (m_edges[e].m_from_node == n) {
					m_edges[e].m_earliest_start = m_nodes[n].m_earliest_event_time;
					m_edges[e].m_earliest_finish = m_nodes[n].m_earliest_event_time + m_edges[e].m_duration;
					end.push_back(m_edges[e].m_to_node);
				}
			}
		}

		/*  */
		begin.clear();
		for (auto n : end) {
			int earliest_event_time = -1;
			for (int e = 0; e < m_edges.size(); e++) {
				if (m_edges[e].m_to_node == n) {
					if (m_edges[e].m_earliest_finish >= 0) {
						if (earliest_event_time < m_edges[e].m_earliest_finish)
							earliest_event_time = m_edges[e].m_earliest_finish;
					}
					else {
						/*  */
						earliest_event_time = -1;
						break;
					}
				}
			}

			if (earliest_event_time >= 0) {
				m_nodes[n].m_earliest_event_time = earliest_event_time;
				begin.push_back(n);
			}
		}

		/* 已经计算到最后节点，则停止   */
		if (m_nodes[endNode[0]].m_earliest_event_time >= 0) {
			end.push_back(endNode[0]);
			m_nodes[endNode[0]].m_latest_event_time = m_nodes[endNode[0]].m_earliest_event_time;
			break;
		}

	} while (1);

	/* 从后向前，计算各活动的 最晚开始时间 和 各个节点的最晚完成时间 */
	do {
		/* 计算各个边的最晚开始和最晚完成时间 */
		begin.clear();
		for (auto n : end) {
			for (int e = 0; e < m_edges.size(); e++) {
				if (m_edges[e].m_to_node == n) {
					m_edges[e].m_latest_finish = m_nodes[n].m_latest_event_time;
					m_edges[e].m_latest_start = m_nodes[n].m_latest_event_time - m_edges[e].m_duration;
					begin.push_back(m_edges[e].m_from_node);
				}
			}
		}


		/* 计算各个节点的 最晚完成时间  */
		end.clear();
		for (auto n : begin) {
			int latest_event_time = m_nodes[endNode[0]].m_latest_event_time + 100;
			for (int e = 0; e < m_edges.size(); e++) {
				if (m_edges[e].m_from_node == n) {
					if (m_edges[e].m_latest_start >= 0) {
						if (latest_event_time > m_edges[e].m_latest_start)
							latest_event_time = m_edges[e].m_latest_start;
					}
					else {
						/*  */
						latest_event_time = -1;
						break;
					}
				}
			}

			if (latest_event_time >= 0) {
				m_nodes[n].m_latest_event_time = latest_event_time;
				end.push_back(n);
			}
		}

		/* 已经计算到开始节点，则停止 */
		if (m_nodes[beginNode[0]].m_latest_event_time >= 0) {

			break;
		}
	} while (1);

	/* 计算总时差 和 自由时差  */
	for (int e = 0; e < m_edges.size(); e++) {
		m_edges[e].m_total_float = m_edges[e].m_latest_start - m_edges[e].m_earliest_start;
		int to = m_edges[e].m_to_node;
		m_edges[e].m_free_float = m_nodes[to].m_earliest_event_time - m_edges[e].m_earliest_finish;
	}

}

void CActivityOnArrow::Calculate(string menuCode, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return;

	for (int i = 0; i < cols.size(); i++) {
		cols[i].Calculate();
	}
}

bool CActivityOnArrow::TimeCoordinate(string menuCode, int nRow, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		cols[nRow - 1].m_bTimeCoordinate = !cols[nRow - 1].m_bTimeCoordinate;
	}
	return true;
}