#include "stdafx.h"
#include "ColumnObj.h"

/* 矩形柱 */
string CColumnObj::m_ObjectCode = "01030103";

CColumnObj::CColumnObj()
{
	m_height = 0;
	m_length = 0;
	m_breadth = 0;
	m_quantity = 0;
	m_name = "";
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
	}
	else {
		ar >> m_breadth;
		ar >> m_height;
		ar >> m_length;
		ar >> m_name;
		ar >> m_quantity;
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
			string val;

			if (row < 1) {
				Item.nFormat = DT_LEFT | DT_WORDBREAK;

				if (col == 0)		val = "矩形柱";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "高";
				else if (col == 3)	val = "宽";
				else if (col == 4)	val = "长";
				else if (col == 5)	val = "数量";
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

				if (col >= 2 && col <= 5 || col == 0 )
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

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row-1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_height);
				else if (col == 3)  val = Double2String(cols[row - 1].m_breadth);
				else if (col == 4)  val = Double2String(cols[row - 1].m_length);
				else if (col == 5)  val = Double2String(cols[row - 1].m_quantity, "%.0f");
				else if (col == 6)	val = "修改（update）";
				else if (col == 7)	val = "删除（delete）";
		
				Item.strText.Format(_T("%s"), val.c_str());
			}
			pGridCtrl->SetItem(&Item);
		}
	}
	pGridCtrl->AutoFill();
	return true;
}

bool CColumnObj::Update(string menuCode, int nRow, vector<CColumnObj>& cols) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return false;

	if(nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CColumnObj::m_ObjectCode);
	return false;
}

unsigned int CColumnObj::PopupMenuId(string menuCode) {
	if (menuCode != CColumnObj::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}