#include "stdafx.h"
#include "DecisionTree.h"
#include "GridDlg.h"

/* 决策树法 */
string CDecisionTree::m_ObjectCode = "01040601";
double CDecisionTree::m_ObjectVersion = 1.0;

CDecisionTree::CDecisionTree()
{
	m_code = 0;
	m_parent_code = -1;
	m_probability = 0;
	m_score = 0;
	m_branch_node = "";
}


CDecisionTree::~CDecisionTree()
{
}

void CDecisionTree::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_code;
		ar << m_parent_code;
		ar << m_branch_node;
		ar << m_probability;
		ar << m_score;
	}
	else {
		ar >> m_code;
		ar >> m_parent_code;
		ar >> m_branch_node;
		ar >> m_probability;
		ar >> m_score;
	}
}

bool CDecisionTree::CreateOrUpdate(string menuCode) {

	if (menuCode != CDecisionTree::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("分支名称"), 64);
	if(!m_branch_node.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_branch_node;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("节点编号"), 64);
	if(m_code > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_code);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("父节点编号"), 64);
	if (m_parent_code > -2)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_parent_code);
	infd.m_vecFindItem[0][i][0].nMin = -1;
	infd.m_vecFindItem[0][i][0].nMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("发生概率"), 64);
	if(m_probability > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_probability);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 1;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("得分(期望利润)"), 64);
	if (m_score > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_score);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	
	infd.Init(_T("决策树 参数设置"), _T("决策树 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_branch_node = infd.m_vecFindItem[0][i++][0].strItem;
		m_code = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_parent_code = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		m_probability = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_score = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		return true;
	}
	return false;
}

bool getBranchPath(CDecisionTree col, vector<CDecisionTree>& cols, vector<CDecisionTree>& path) {
	
	do {
		path.push_back(col);
		if (col.m_parent_code < 0) break;
		// 是否形成环路
		bool bLoop = false;	
		for (auto e : path) {
			if (e.m_code == col.m_parent_code) {
				col = e;
				bLoop = true;
				break;
			}
		}
		if (bLoop) break;
		// 是否存在父节点
		bool bExist = false;
		for (auto e : cols) {
			if (e.m_code == col.m_parent_code) {
				col = e;
				bExist = true;
				break;
			}
		}
		if (!bExist) break;
	}while(true);

	return true;
}

bool CDecisionTree::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CDecisionTree>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CDecisionTree::m_ObjectCode)
		return false;

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(5 + 3);		//	额外增加三列 ： 序号/修改/删除
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
				else if (col == 1)	val = "分支名称";
				else if (col == 2)	val = "节点编号";
				else if (col == 3)	val = "父节点编号";
				else if (col == 4)	val = "发生概率";
				else if (col == 5)	val = "得分(期望利润)";
				
				else if (col == 6)	val = "";
				else if (col == 7)	val = "";
				
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
				if (col == 6 ) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 7) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				vector<CDecisionTree> path;
				bool bPath = getBranchPath(cols[row - 1], cols, path);
				string brachName = "Error";
				if (bPath) {
					brachName = "";
					for (int k = path.size() - 1; k >= 0; k--) {
						brachName += path[k].m_branch_node.GetBuffer();
						if (k > 0) brachName += "-";
					}
				}

				if (col == 0)	val = "";
				else if (col == 1) 	val = brachName;
				else if (col == 2)  val = Int2String(cols[row - 1].m_code);
				else if (col == 3)  val = Int2String(cols[row - 1].m_parent_code);
				else if (col == 4)  val = Double2String(cols[row - 1].m_probability);
				else if (col == 5)  val = Double2String(cols[row - 1].m_score);
				
				else if (col == 6)	val = "修改（update）";
				else if (col == 7)	val = "删除（delete）";
		
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

bool CDecisionTree::Update(string menuCode, int nRow, vector<CDecisionTree>& cols) {
	if (menuCode != CDecisionTree::m_ObjectCode)
		return false;

	if(nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CDecisionTree::m_ObjectCode);
	return false;
}

bool CDecisionTree::Delete(string menuCode, int nRow, vector<CDecisionTree>& cols) {
	if (menuCode != CDecisionTree::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CDecisionTree>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CDecisionTree::PopupMenuId(string menuCode) {
	if (menuCode != CDecisionTree::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

void CDecisionTree::SteelQuantity(string menuCode, vector<CDecisionTree>& cols) {
	if (menuCode != CDecisionTree::m_ObjectCode)
		return ;

}

void CDecisionTree::Calculate(string menuCode, vector<CDecisionTree>& cols) {
	if (menuCode != CDecisionTree::m_ObjectCode)
		return ;
	

	CTreeGridDlg treeDlg;
	treeDlg.m_vecCode.push_back(1);
	treeDlg.m_vecContent.push_back("1111111");
	treeDlg.m_vecParentCode.push_back(-1);

	treeDlg.m_vecCode.push_back(2);
	treeDlg.m_vecContent.push_back("222222");
	treeDlg.m_vecParentCode.push_back(1);
	

	treeDlg.m_vecCode.push_back(3);
	treeDlg.m_vecContent.push_back("222222-1");
	treeDlg.m_vecParentCode.push_back(1);
	

	treeDlg.m_vecCode.push_back(4);
	treeDlg.m_vecContent.push_back("333333");
	treeDlg.m_vecParentCode.push_back(2);
	treeDlg.DoModal();
}