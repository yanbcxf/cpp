#include "stdafx.h"
#include "BudgetIndex.h"
#include "GridDlg.h"


CBudgetIndexObj::CBudgetIndexObj()
{
	m_unit_price = 0;
	m_quantity = 0;
	m_name = "";
}


CBudgetIndexObj::~CBudgetIndexObj()
{
}



void CBudgetIndexObj::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_unit_price;
		ar << m_quantity;
		ar << m_name;
	}
	else {
		ar >> m_unit_price;
		ar >> m_quantity;
		ar >> m_name;
	}
}




bool CBudgetIndexObj::CreateOrUpdate(string menuCode) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("材料名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("消耗量"), 64);
	if (m_quantity > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_quantity);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("材料单价"), 64);
	if (m_unit_price > 0 )
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_unit_price);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 10000;


	infd.Init(_T("材料消耗 参数设置"), _T("材料消耗 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_quantity = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_unit_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		return true;
	}
	return false;
}

bool CBudgetIndexObj::Draw(CGridCtrl* pGridCtrl, vector<CBudgetIndexObj>& cols) {
	if (!pGridCtrl)
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

				if (col == 0)		val = "费用比例";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "消耗量";
				else if (col == 3)	val = "材料单价";
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

				if (col >= 2 && col <= 3 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 4) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 5) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_quantity);
				else if (col == 3)  val = Double2String(cols[row - 1].m_unit_price);
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

bool CBudgetIndexObj::Update(string menuCode, int nRow, vector<CBudgetIndexObj>& cols) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(menuCode);
	return false;
}


bool CBudgetIndexObj::Delete(string menuCode, int nRow, vector<CBudgetIndexObj>& cols) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CBudgetIndexObj>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


/***********************************************************************************/


/* 概算指标法 */
string CBudgetIndex::m_ObjectCode = "01030302";
double CBudgetIndex::m_ObjectVersion = 1.0;

CBudgetIndex::CBudgetIndex()
{
	m_name = "";
	m_other_material_cost_percent = 0;
	m_machine_tool_cost_percent = 0;
}


CBudgetIndex::~CBudgetIndex()
{
}




void CBudgetIndex::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_other_material_cost_percent;
		ar << m_machine_tool_cost_percent;
		ar << m_name;
		ar << m_materials.size();
		for (int i = 0; i < m_materials.size(); i++) {
			m_materials[i].Serialize(ar, version);
		}
	}
	else {
		ar >> m_other_material_cost_percent;
		ar >> m_machine_tool_cost_percent;
		ar >> m_name;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CBudgetIndexObj bs;
			bs.Serialize(ar, version);
			m_materials.push_back(bs);
		}
	}
}

bool CBudgetIndex::CreateOrUpdate(string menuCode) {

	if (menuCode != CBudgetIndex::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("其它材料费占主材料费的比例(%)"), 64);
	if (m_other_material_cost_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_other_material_cost_percent);
	infd.m_vecFindItem[0][i][0].dbMin = 0.0001;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("机具使用费占人材机之和的比例(%)"), 64);
	if (m_machine_tool_cost_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_machine_tool_cost_percent);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("概算指标法 参数设置"), _T("概算指标法 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_other_material_cost_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_machine_tool_cost_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}




bool CBudgetIndex::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CBudgetIndex>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CBudgetIndex::m_ObjectCode)
		return false;

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(3 + 4);		//	额外增加4列 ： 序号/修改/删除/增加
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

				if (col == 0)		val = "类似工程预算法";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "其它材料费占主材料费的比例(%)";
				else if (col == 3)	val = "机具使用费占人材机之和的比例(%)";
				else if (col == 4)	val = "";
				else if (col == 5)	val = "";
				else if (col == 6)	val = "";

				Item.strText.Format(_T("%s"), val.c_str());
			}
			else
			{
				if (col <= 2)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col >= 2 && col <= 3 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 4 || col == 6) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 5) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_other_material_cost_percent, "%.2f");
				else if (col == 3)  val = Double2String(cols[row - 1].m_machine_tool_cost_percent, "%.2f");
				
				else if (col == 4)	val = "修改（update）";
				else if (col == 5)	val = "删除（delete）";
				else if (col == 6)	val = "增加（create）";

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

bool CBudgetIndex::Update(string menuCode, int nRow, vector<CBudgetIndex>& cols) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CBudgetIndex::m_ObjectCode);
	return false;
}

bool CBudgetIndex::Delete(string menuCode, int nRow, vector<CBudgetIndex>& cols) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CBudgetIndex>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CBudgetIndex::PopupMenuId(string menuCode) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}



void CBudgetIndex::Calculate(string menuCode, vector<CBudgetIndex>& cols) {
	if (menuCode != CBudgetIndex::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("单方造价");
	

	for (int i = 0; i < cols.size(); i++)
	{
		CBudgetIndex seb = cols[i];

		string str = seb.m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		
		// 主材费
		double material = 0;
		double people = 0;
		for (auto e : seb.m_materials)
		{
			if (e.m_name.Find("人工")>=0) {
				people += e.m_quantity * e.m_unit_price;
			} else 
				material += e.m_quantity * e.m_unit_price ;
		}
		// 加 其它材料费
		material = material * seb.m_other_material_cost_percent / 100 + material;
		// 加 施工机具费
		material = (material + people) / (1 - seb.m_machine_tool_cost_percent / 100);
		// 加 综合取费
		material = material * (1 + 0.2);
		vec.push_back(Double2String(material));
		gridDlg.m_vecData.push_back(vec);
	}
	gridDlg.DoModal();
}