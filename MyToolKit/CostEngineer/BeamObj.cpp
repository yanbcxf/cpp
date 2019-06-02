#include "stdafx.h"
#include "BeamObj.h"


/* 矩形梁 */
string CBeamObj::m_ObjectCode = "01030104";

CBeamObj::CBeamObj()
{
	m_breadth = 0;
	m_height = 0;
	m_length = 0;
	m_name = "";
	m_quantity = 0;
}


CBeamObj::~CBeamObj()
{
}




void CBeamObj::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << m_breadth;
		ar << m_height;
		ar << m_length;
		ar << m_name;
		ar << m_quantity;
		ar << m_steel_vertical;
		ar << m_spans.size();
		for (int i = 0; i < m_spans.size(); i++) {
			m_spans[i].Serialize(ar);
		}
	}
	else {
		ar >> m_breadth;
		ar >> m_height;
		ar >> m_length;
		ar >> m_name;
		ar >> m_quantity;
		ar >> m_steel_vertical;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CBeamSpan bs;
			bs.Serialize(ar);
			m_spans.push_back(bs);
		}
	}
}

bool CBeamObj::CreateOrUpdate(string menuCode) {

	if (menuCode != CBeamObj::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("高"), 64);
	if (m_height > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_height);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("长"), 64);
	if (m_length > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_length);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("宽"), 64);
	if (m_breadth > 0)
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
	if (m_quantity > 0)
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

bool CBeamObj::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CBeamObj>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CBeamObj::m_ObjectCode)
		return false;

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(7 + 3);		//	额外增加三列 ： 序号/修改/删除
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

				if (col == 0)		val = "矩形梁";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "高";
				else if (col == 3)	val = "宽";
				else if (col == 4)	val = "长";
				else if (col == 5)	val = "纵筋";
				else if (col == 6)	val = "数量";
				else if (col == 7)	val = "";
				else if (col == 8)	val = "";
				else if (col == 9)	val = "";

				Item.strText.Format(_T("%s"), val.c_str());
			}
			else
			{
				if (col <= 2)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col >= 2 && col <= 5 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 7 || col == 9) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 8) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_height);
				else if (col == 3)  val = Double2String(cols[row - 1].m_breadth);
				else if (col == 4)  val = Double2String(cols[row - 1].m_length);
				else if (col == 5) 	val = cols[row - 1].m_steel_vertical.GetBuffer();
				else if (col == 6)  val = Double2String(cols[row - 1].m_quantity, "%.0f");
				else if (col == 7)	val = "修改（update）";
				else if (col == 8)	val = "删除（delete）";
				else if (col == 9)	val = "增加（create）";

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

bool CBeamObj::Update(string menuCode, int nRow, vector<CBeamObj>& cols) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CBeamObj::m_ObjectCode);
	return false;
}

bool CBeamObj::Delete(string menuCode, int nRow, vector<CBeamObj>& cols) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CBeamObj>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


unsigned int CBeamObj::PopupMenuId(string menuCode) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

/*****************************************************************************************************/

CBeamSpan::CBeamSpan()
{
	m_breadth = 0;
	m_height = 0;
	m_length = 0;
	m_name = "";
	m_steel_right = "";
	m_steel_left = "";
	m_steel_middle = "";
}


CBeamSpan::~CBeamSpan()
{
}



void CBeamSpan::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << m_breadth;
		ar << m_height;
		ar << m_length;
		ar << m_name;
		ar << m_steel_right;
		ar << m_steel_left;
		ar << m_steel_middle;
	}
	else {
		ar >> m_breadth;
		ar >> m_height;
		ar >> m_length;
		ar >> m_name;
		ar >> m_steel_right;
		ar >> m_steel_left;
		ar >> m_steel_middle;
	}
}




bool CBeamSpan::CreateOrUpdate(string menuCode) {
	if (menuCode != CBeamObj::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("高"), 64);
	if (m_height > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_height);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("长"), 64);
	if (m_length > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_length);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("宽"), 64);
	if (m_breadth > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_breadth);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("左支座钢筋"), 64);
	if (!m_steel_left.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_left;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("右支座钢筋"), 64);
	if (!m_steel_right.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_right;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("底部中间钢筋"), 64);
	if (!m_steel_middle.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_middle;

	infd.Init(_T("梁跨 参数设置"), _T("梁跨 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_height = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_length = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_breadth = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_steel_left = infd.m_vecFindItem[0][i++][0].strItem;
		m_steel_right = infd.m_vecFindItem[0][i++][0].strItem;
		m_steel_middle = infd.m_vecFindItem[0][i++][0].strItem;
		return true;
	}
	return false;
}

bool CBeamSpan::Draw(CGridCtrl* pGridCtrl, vector<CBeamSpan>& cols) {
	if (!pGridCtrl)
		return false;

	
	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(7 + 3);		//	额外增加三列 ： 序号/修改/删除
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

				if (col == 0)		val = "梁跨";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "高";
				else if (col == 3)	val = "宽";
				else if (col == 4)	val = "长";
				else if (col == 5)	val = "左支座钢筋";
				else if (col == 6)	val = "右支座钢筋";
				else if (col == 7)	val = "底部中间钢筋";
				else if (col == 8)	val = "";
				else if (col == 9)	val = "";
			

				Item.strText.Format(_T("%s"), val.c_str());
			}
			else
			{
				if (col <= 2)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col >= 2 && col <= 5 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 8) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 9) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_height);
				else if (col == 3)  val = Double2String(cols[row - 1].m_breadth);
				else if (col == 4)  val = Double2String(cols[row - 1].m_length);
				else if (col == 5) 	val = cols[row - 1].m_steel_left.GetBuffer();
				else if (col == 6) 	val = cols[row - 1].m_steel_right.GetBuffer();
				else if (col == 7) 	val = cols[row - 1].m_steel_middle.GetBuffer();
				else if (col == 8)	val = "修改（update）";
				else if (col == 9)	val = "删除（delete）";

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

bool CBeamSpan::Update(string menuCode, int nRow, vector<CBeamSpan>& cols) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(menuCode);
	return false;
}


bool CBeamSpan::Delete(string menuCode, int nRow, vector<CBeamSpan>& cols) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CBeamSpan>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}