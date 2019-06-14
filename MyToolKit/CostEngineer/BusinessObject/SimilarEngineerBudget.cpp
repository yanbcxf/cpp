#include "stdafx.h"
#include "SimilarEngineerBudget.h"
#include "GridDlg.h"


CSimilarEngineerBudgetObj::CSimilarEngineerBudgetObj()
{
	m_percent = 0;
	m_different = 0;
	m_name = "";
}


CSimilarEngineerBudgetObj::~CSimilarEngineerBudgetObj()
{
}



void CSimilarEngineerBudgetObj::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_percent;
		ar << m_different;
		ar << m_name;
	}
	else {
		ar >> m_percent;
		ar >> m_different;
		ar >> m_name;
	}
}




bool CSimilarEngineerBudgetObj::CreateOrUpdate(string menuCode) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("费用名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("类似工程费用比例(%)"), 64);
	if (m_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_percent);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("现工程差异系数(倍数)"), 64);
	if (m_different > 0 || m_different < 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_different);
	infd.m_vecFindItem[0][i][0].dbMin = -100;
	infd.m_vecFindItem[0][i][0].dbMax = 100;


	infd.Init(_T("费用比例 参数设置"), _T("费用比例 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_different = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		return true;
	}
	return false;
}

bool CSimilarEngineerBudgetObj::Draw(CGridCtrl* pGridCtrl, vector<CSimilarEngineerBudgetObj>& cols) {
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
				else if (col == 2)	val = "占比(%)";
				else if (col == 3)	val = "差异系数(倍数)";
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
				else if (col == 2)  val = Double2String(cols[row - 1].m_percent);
				else if (col == 3)  val = Double2String(cols[row - 1].m_different);
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

bool CSimilarEngineerBudgetObj::Update(string menuCode, int nRow, vector<CSimilarEngineerBudgetObj>& cols) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(menuCode);
	return false;
}


bool CSimilarEngineerBudgetObj::Delete(string menuCode, int nRow, vector<CSimilarEngineerBudgetObj>& cols) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CSimilarEngineerBudgetObj>::iterator it = cols.begin();
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


/* 类似工程预算法 */
string CSimilarEngineerBudget::m_ObjectCode = "01030301";
double CSimilarEngineerBudget::m_ObjectVersion = 1.0;

CSimilarEngineerBudget::CSimilarEngineerBudget()
{
	m_name = "";
	m_original_unilateral_cost = 0;
	m_quantity = 0;
}


CSimilarEngineerBudget::~CSimilarEngineerBudget()
{
}




void CSimilarEngineerBudget::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_original_unilateral_cost;
		ar << m_quantity;
		ar << m_name;
		ar << m_percents.size();
		for (int i = 0; i < m_percents.size(); i++) {
			m_percents[i].Serialize(ar, version);
		}
	}
	else {
		ar >> m_original_unilateral_cost;
		ar >> m_quantity;
		ar >> m_name;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CSimilarEngineerBudgetObj bs;
			bs.Serialize(ar, version);
			m_percents.push_back(bs);
		}
	}
}

bool CSimilarEngineerBudget::CreateOrUpdate(string menuCode) {

	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("类似工程单方造价"), 64);
	if (m_original_unilateral_cost > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_original_unilateral_cost);
	infd.m_vecFindItem[0][i][0].dbMin = 0.0001;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("现工程量"), 64);
	if (m_quantity > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_quantity);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("类似工程预算法 参数设置"), _T("类似工程预算法 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_original_unilateral_cost = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_quantity = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}




bool CSimilarEngineerBudget::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CSimilarEngineerBudget>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
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
				else if (col == 2)	val = "类似工程单方造价";
				else if (col == 3)	val = "现工程量";
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
				else if (col == 2)  val = Double2String(cols[row - 1].m_original_unilateral_cost, "%.2f");
				else if (col == 3)  val = Double2String(cols[row - 1].m_quantity, "%.2f");
				
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

bool CSimilarEngineerBudget::Update(string menuCode, int nRow, vector<CSimilarEngineerBudget>& cols) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CSimilarEngineerBudget::m_ObjectCode);
	return false;
}

bool CSimilarEngineerBudget::Delete(string menuCode, int nRow, vector<CSimilarEngineerBudget>& cols) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CSimilarEngineerBudget>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CSimilarEngineerBudget::PopupMenuId(string menuCode) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}



void CSimilarEngineerBudget::Calculate(string menuCode, vector<CSimilarEngineerBudget>& cols) {
	if (menuCode != CSimilarEngineerBudget::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("单方造价");
	

	for (int i = 0; i < cols.size(); i++)
	{
		CSimilarEngineerBudget seb = cols[i];

		string str = seb.m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		
		double percent = 0;
		for (auto e : seb.m_percents) {
			percent += e.m_different * e.m_percent/100.0 ;
		}
		percent = seb.m_original_unilateral_cost * percent;
		vec.push_back(Double2String(percent));
		gridDlg.m_vecData.push_back(vec);
	}
	gridDlg.DoModal();
}