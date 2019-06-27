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
}


CAOAEdge::~CAOAEdge()
{
}



void CAOAEdge::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_from_node;
		ar << m_to_node;
	}
	else {
		ar >> m_name;
		ar >> m_from_node;
		ar >> m_to_node;
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

	
	infd.Init(_T("图边 参数设置"), _T("图边 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_from_node = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_to_node = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
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

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(3 + 3);		//	额外增加三列 ： 序号/修改/删除
		pGridCtrl->SetFixedRowCount(1);
		pGridCtrl->SetFixedColumnCount(1);
		pGridCtrl->SetHeaderSort(TRUE);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	for (int row = 0; row < pGridCtrl->GetRowCount(); row++)
	{
		for (int col = 0; col < pGridCtrl->GetColumnCount(); col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			Item.nMargin = 10;
			string val;

			if (row < 1) {
				Item.nFormat = DT_LEFT | DT_WORDBREAK;

				if (col == 0)		val = "";
				else if (col == 1)	val = "网络图名称";
				else if (col == 2)	val = "节点数";
				else if (col == 3)	val = "边数";
								
				else if (col == 4)	val = "";
				else if (col == 5)	val = "";
				
				Item.strText.Format(_T("%s"), val.c_str());
			}
			else
			{
				if (col <= 2)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col >= 2 && col <= 3 || col == 0 )
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 4 ) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 5) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				
				if (col == 0)	val = "";
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Int2String(cols[row - 1].m_nodes.size());
				else if (col == 3)  val = Int2String(cols[row - 1].m_edges.size());
								
				else if (col == 4)	val = "修改（update）";
				else if (col == 5)	val = "删除（delete）";
		
				Item.strText.Format(_T("%s"), val.c_str());
			}
			pGridCtrl->SetItem(&Item);
		}
	}
	//pGridCtrl->AutoFill();
	pGridCtrl->Refresh();
	pGridCtrl->ExpandColumnsToFit();
	return true;
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


unsigned int CActivityOnArrow::PopupMenuId(string menuCode) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

void CActivityOnArrow::SteelQuantity(string menuCode, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return ;

}

void CActivityOnArrow::Calculate(string menuCode, vector<CActivityOnArrow>& cols) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return ;
	
	
}


bool CActivityOnArrow::DrawGraph(CGraphCtrl* pCtrl) {
	if (!pCtrl)
		return false;

	pCtrl->initGraph();
	for (int i = 0; i < m_nodes.size(); i++) {
		pCtrl->AddNode(m_nodes[i].m_x, m_nodes[i].m_y);
	}

	for (int i = 0; i < m_edges.size(); i++) {
		pCtrl->AddEdge(m_edges[i].m_from_node, m_edges[i].m_to_node);
	}
	pCtrl->Refresh();
	return true;
}

bool CActivityOnArrow::AddNode(string menuCode, int x, int y) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CAOANode node;
	node.m_x = x;
	node.m_y = y;
	if (node.CreateOrUpdate(menuCode)) {
		m_nodes.push_back(node);
	}
	return true;
}

bool CActivityOnArrow::AddEdge(string menuCode, int from, int to) {
	if (menuCode != CActivityOnArrow::m_ObjectCode)
		return false;

	CAOAEdge edge;
	edge.m_from_node = from;
	edge.m_to_node = to;
	if (edge.CreateOrUpdate(menuCode)) {
		m_edges.push_back(edge);
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
	if(m_nodes[nRow].CreateOrUpdate(menuCode))
		return true;
	return false;
}

bool CActivityOnArrow::UpdateEdge(int nRow) {

	return true;
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

	return true;
}

bool CActivityOnArrow::DeleteEdge(int nRow) {

	return true;
}