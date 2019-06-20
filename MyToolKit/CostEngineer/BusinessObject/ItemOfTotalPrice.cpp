#include "stdafx.h"
#include "ItemOfTotalPrice.h"
#include "GridDlg.h"

/* 总价措施费 */
string CItemOfTotalPrice::m_ObjectCode = "01030903";
double CItemOfTotalPrice::m_ObjectVersion = 1.0;

CItemOfTotalPrice::CItemOfTotalPrice()
{
	m_calculate_base = 0;
	m_rate = 0;
	m_name = "";
}


CItemOfTotalPrice::~CItemOfTotalPrice()
{
}

void CItemOfTotalPrice::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_calculate_base;
		ar << m_rate;
		ar << m_name;
	}
	else {
		ar >> m_calculate_base;
		ar >> m_rate;
		ar >> m_name;
	}
}

bool CItemOfTotalPrice::CreateOrUpdate(string menuCode) {

	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("名称"), 64);
	if(!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算基础"), 64);
	if(m_calculate_base > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_calculate_base);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 10000000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("费率(%)"), 64);
	if(m_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_rate);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	
	infd.Init(_T("总价措施费 参数设置"), _T("总价措施费 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_calculate_base = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_rate = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		return true;
	}
	return false;
}

bool CItemOfTotalPrice::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CItemOfTotalPrice>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
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

				if (col == 0)		val = "总价措施费";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "计算基础";
				else if (col == 3)	val = "费率(%)";
				
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

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row-1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_calculate_base);
				else if (col == 3)  val = Double2String(cols[row - 1].m_rate);
				
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

bool CItemOfTotalPrice::Update(string menuCode, int nRow, vector<CItemOfTotalPrice>& cols) {
	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
		return false;

	if(nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CItemOfTotalPrice::m_ObjectCode);
	return false;
}

bool CItemOfTotalPrice::Delete(string menuCode, int nRow, vector<CItemOfTotalPrice>& cols) {
	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CItemOfTotalPrice>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CItemOfTotalPrice::PopupMenuId(string menuCode) {
	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

void CItemOfTotalPrice::SteelQuantity(string menuCode, vector<CItemOfTotalPrice>& cols) {
	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
		return ;

}

void CItemOfTotalPrice::Calculate(string menuCode, vector<CItemOfTotalPrice>& cols) {
	if (menuCode != CItemOfTotalPrice::m_ObjectCode)
		return ;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("计算基础");
	gridDlg.m_vecHeader.push_back("费率(%)");
	gridDlg.m_vecHeader.push_back("金额");
	
	for (int i = 0; i < cols.size(); i++)
	{
		vector<string> vec;
		vec.push_back(cols[i].m_name.GetBuffer());
		vec.push_back(Double2String(cols[i].m_calculate_base));
		vec.push_back(Double2String(cols[i].m_rate));
		double fee = cols[i].m_calculate_base * cols[i].m_rate / 100;
		double manage_net = fee * 0.2 * 0.5;
		vec.push_back(Double2String(fee + manage_net));
		
		gridDlg.m_vecData.push_back(vec);

	}
	gridDlg.DoModal();
}