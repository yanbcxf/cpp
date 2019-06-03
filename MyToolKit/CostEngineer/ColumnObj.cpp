#include "stdafx.h"
#include "ColumnObj.h"
#include "GridDlg.h"

/* 矩形柱 */
string CColumnObj::m_ObjectCode = "01030103";

CColumnObj::CColumnObj()
{
	m_height = 0;
	m_length = 0;
	m_breadth = 0;
	m_quantity = 0;
	m_name = "";
	m_steel_vertical = "";
}


CColumnObj::~CColumnObj()
{
}

void CColumnObj::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << m_breadth;
		ar << m_height;
		ar << m_length;
		ar << m_name;
		ar << m_quantity;
		ar << m_steel_vertical;
	}
	else {
		ar >> m_breadth;
		ar >> m_height;
		ar >> m_length;
		ar >> m_name;
		ar >> m_quantity;
		ar >> m_steel_vertical;
	}
}

bool CColumnObj::CreateOrUpdate(string menuCode) {

	if (menuCode != CColumnObj::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("高"), 64);
	if(m_height > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_height);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("长"), 64);
	if(m_length > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_length);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("宽"), 64);
	if(m_breadth > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_breadth);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("纵筋"), 64);
	if (!m_steel_vertical.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_vertical;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("数量"), 64);
	if(m_quantity > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_quantity);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 100000000;

	infd.Init(_T("矩形柱 参数设置"), _T("矩形柱 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_height = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_length = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_breadth = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_steel_vertical = infd.m_vecFindItem[0][i++][0].strItem;
		m_quantity = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		return true;
	}
	return false;
}

bool CColumnObj::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CColumnObj>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CColumnObj::m_ObjectCode)
		return false;

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(6 + 3);		//	额外增加三列 ： 序号/修改/删除
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

				if (col == 0)		val = "矩形柱";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "高";
				else if (col == 3)	val = "宽";
				else if (col == 4)	val = "长";
				else if (col == 5)	val = "纵筋";
				else if (col == 6)	val = "数量";
				else if (col == 7)	val = "";
				else if (col == 8)	val = "";
				
				Item.strText.Format(_T("%s"), val.c_str());
			}
			else
			{
				if (col <= 2)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col >= 2 && col <= 5 || col == 0 )
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 7 ) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 8) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row-1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_height);
				else if (col == 3)  val = Double2String(cols[row - 1].m_breadth);
				else if (col == 4)  val = Double2String(cols[row - 1].m_length);
				else if (col == 5) 	val = cols[row - 1].m_steel_vertical.GetBuffer();
				else if (col == 6)  val = Double2String(cols[row - 1].m_quantity, "%.0f");
				else if (col == 7)	val = "修改（update）";
				else if (col == 8)	val = "删除（delete）";
		
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

bool CColumnObj::Update(string menuCode, int nRow, vector<CColumnObj>& cols) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return false;

	if(nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CColumnObj::m_ObjectCode);
	return false;
}

bool CColumnObj::Delete(string menuCode, int nRow, vector<CColumnObj>& cols) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CColumnObj>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CColumnObj::PopupMenuId(string menuCode) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

void CColumnObj::ConcreteQuantity(string menuCode, vector<CColumnObj>& cols) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return ;

}

void CColumnObj::SteelQuantity(string menuCode, vector<CColumnObj>& cols) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return ;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("位置");
	gridDlg.m_vecHeader.push_back("筋号及直径");
	gridDlg.m_vecHeader.push_back("计算公式");
	gridDlg.m_vecHeader.push_back("根数");
	gridDlg.m_vecHeader.push_back("单长(m)");
	gridDlg.m_vecHeader.push_back("总长(m)");
	gridDlg.m_vecHeader.push_back("总重(kg)");

	for (int i = 0; i < cols.size(); i++)
	{
		int quantity, diameter, first, second;
		double weight;
		parseSteelMarking(cols[i].m_steel_vertical.GetBuffer(), &quantity, &diameter, &weight, &first, &second);
		string str = cols[i].m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		gridDlg.m_vecData.push_back(vec);

		vec.clear();
		vec.push_back(string(cols[i].m_name.GetBuffer()) + " 竖向纵筋");
		vec.push_back(Int2String(diameter));
		vec.push_back("12d + height + 15d");
		vec.push_back(Int2String(quantity));
		double len = cols[i].m_length + 12 * diameter/ 1000.0 + 15 * diameter/1000.0;
		vec.push_back(Double2String(len));
		vec.push_back(Double2String(len * quantity * cols[i].m_quantity));
		vec.push_back(Double2String(len * quantity * cols[i].m_quantity * weight));
		gridDlg.m_vecData.push_back(vec);

	}
	gridDlg.DoModal();
}