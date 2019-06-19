#include "stdafx.h"
#include "ConsumptionQuota.h"
#include "GridDlg.h"


CConsumptionQuotaObj::CConsumptionQuotaObj()
{
	m_unit_price = 0;
	m_quantity = 0;
	m_name = "";
	m_unit = "";
}


CConsumptionQuotaObj::~CConsumptionQuotaObj()
{
}



void CConsumptionQuotaObj::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_unit_price;
		ar << m_quantity;
		ar << m_name;
		ar << m_unit;
	}
	else {
		ar >> m_unit_price;
		ar >> m_quantity;
		ar >> m_name;
		ar >> m_unit;
	}
}




bool CConsumptionQuotaObj::CreateOrUpdate(string menuCode) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.CXEDIT = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("材料名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("单位"), 64);
	if (!m_unit.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_unit;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("消耗量"), 64);
	if (m_quantity > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_quantity);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("单价"), 64);
	if (m_unit_price > 0 )
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_unit_price);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 10000;

	
	infd.Init(_T("材料消耗 参数设置"), _T("材料消耗 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_unit = infd.m_vecFindItem[0][i++][0].strItem;
		m_quantity = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_unit_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		return true;
	}
	return false;
}

bool CConsumptionQuotaObj::Draw(CGridCtrl* pGridCtrl, vector<CConsumptionQuotaObj>& cols) {
	if (!pGridCtrl)
		return false;


	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(4 + 3);		//	额外增加三列 ： 序号/修改/删除
		pGridCtrl->SetFixedRowCount(1);
		pGridCtrl->SetFixedColumnCount(1);
		pGridCtrl->SetHeaderSort(TRUE);
		pGridCtrl->SetEditable(FALSE);
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

				if (col == 0)		val = "消耗量定额(10立方米)";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "单位";
				else if (col == 3)	val = "消耗量";
				else if (col == 4)	val = "单价";
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

				if (col >= 3 && col <= 4 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 5) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 6) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2) 	val = cols[row - 1].m_unit.GetBuffer();
				else if (col == 3)  val = Double2String(cols[row - 1].m_quantity);
				else if (col == 4)  val = Double2String(cols[row - 1].m_unit_price);
				else if (col == 5)	val = "修改（update）";
				else if (col == 6)	val = "删除（delete）";

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

bool CConsumptionQuotaObj::Update(string menuCode, int nRow, vector<CConsumptionQuotaObj>& cols) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(menuCode);
	return false;
}


bool CConsumptionQuotaObj::Delete(string menuCode, int nRow, vector<CConsumptionQuotaObj>& cols) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CConsumptionQuotaObj>::iterator it = cols.begin();
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


/* 消耗量定额 */
string CConsumptionQuota::m_ObjectCode = "01030901";
double CConsumptionQuota::m_ObjectVersion = 1.0;

CConsumptionQuota::CConsumptionQuota()
{
	m_name = "";
	m_identifier = "";
	m_unit = 1;

}


CConsumptionQuota::~CConsumptionQuota()
{
}




void CConsumptionQuota::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_unit;
		ar << m_identifier;
		ar << m_name;
		ar << m_materials.size();
		for (int i = 0; i < m_materials.size(); i++) {
			m_materials[i].Serialize(ar, version);
		}
	}
	else {
		ar >> m_unit;
		ar >> m_identifier;
		ar >> m_name;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CConsumptionQuotaObj bs;
			bs.Serialize(ar, version);
			m_materials.push_back(bs);
		}
	}
}

bool CConsumptionQuota::CreateOrUpdate(string menuCode) {

	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.CXEDIT = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("定额编号"), 64);
	if (!m_identifier.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_identifier;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("定额名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("定额单位"), 64);
	if (m_unit > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_unit);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 1000;

	infd.Init(_T("消耗量定额 参数设置"), _T("消耗量定额 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_identifier = infd.m_vecFindItem[0][i++][0].strItem;
		m_name = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_unit = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}




bool CConsumptionQuota::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CConsumptionQuota>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CConsumptionQuota::m_ObjectCode)
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

				if (col == 0)		val = "消耗量定额";
				else if (col == 1)	val = "定额编号";
				else if (col == 2)	val = "定额名称";
				else if (col == 3)	val = "定额单位";
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
				else if (col == 1) 	val = cols[row - 1].m_identifier.GetBuffer();
				else if (col == 2)  val = cols[row - 1].m_name.GetBuffer();
				else if (col == 3)  val = Double2String(cols[row - 1].m_unit, "%.0f");
				
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

bool CConsumptionQuota::Update(string menuCode, int nRow, vector<CConsumptionQuota>& cols) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CConsumptionQuota::m_ObjectCode);
	return false;
}

bool CConsumptionQuota::Delete(string menuCode, int nRow, vector<CConsumptionQuota>& cols) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CConsumptionQuota>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CConsumptionQuota::PopupMenuId(string menuCode) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}


void CConsumptionQuota::GetQuotaFee(double& people, double& material, double& machine) {
	people = 0;
	material = 0;
	machine = 0;
	for (auto e : m_materials)
	{
		if (e.m_unit.Find("工日") >= 0) {
			people += e.m_quantity * e.m_unit_price;
		}
		else if (e.m_unit.Find("台班") >= 0) {
			machine += e.m_quantity * e.m_unit_price;
		}
		else
			material += e.m_quantity * e.m_unit_price;
	}
}

CConsumptionQuota * CConsumptionQuota::FindQuota(CString quotaId, vector<CConsumptionQuota>& quotas) {
	for (int i = 0; i < quotas.size(); i++) {
		if (quotas[i].m_identifier == quotaId) {
			return &quotas[i];
		}
	}
	return NULL;
}

void CConsumptionQuota::Calculate(string menuCode, vector<CConsumptionQuota>& cols) {
	if (menuCode != CConsumptionQuota::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("定额名称");
	gridDlg.m_vecHeader.push_back("人工费");
	gridDlg.m_vecHeader.push_back("材料费");
	gridDlg.m_vecHeader.push_back("机具使用费");
	gridDlg.m_vecHeader.push_back("管理费和利润");
	gridDlg.m_vecHeader.push_back("定额综合单价");

	for (int i = 0; i < cols.size(); i++)
	{
		CConsumptionQuota seb = cols[i];

		string str = seb.m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		
		// 主材费
		double material = 0;
		double people = 0;
		double machine = 0;

		seb.GetQuotaFee(people, material, machine);
		
		// 管理费
		double manage = (people ) * 0.3;
		// 利润
		double net = (people ) * 0.2;
		
		if (people > 0) vec.push_back(Double2String(people));
		else vec.push_back("");

		if (material > 0) vec.push_back(Double2String(material));
		else vec.push_back("");

		if (machine > 0) vec.push_back(Double2String(machine));
		else vec.push_back("");

		if (manage + net > 0) vec.push_back(Double2String(manage + net));
		else vec.push_back("");

		vec.push_back(Double2String(people + material + machine + manage + net));
		
		gridDlg.m_vecData.push_back(vec);
	}
	gridDlg.DoModal();
}