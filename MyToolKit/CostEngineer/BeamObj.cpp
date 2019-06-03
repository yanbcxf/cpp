#include "stdafx.h"
#include "BeamObj.h"
#include "GridDlg.h"




CBeamSpan::CBeamSpan()
{
	m_breadth = 0;
	m_height = 0;
	m_length = 0;
	m_name = "";
	m_steel_right = "";
	m_steel_left = "";
	m_steel_bottom = "";
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
		ar << m_steel_bottom;
	}
	else {
		ar >> m_breadth;
		ar >> m_height;
		ar >> m_length;
		ar >> m_name;
		ar >> m_steel_right;
		ar >> m_steel_left;
		ar >> m_steel_bottom;
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("长(mm)"), 64);
	if (m_length > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_length);
	infd.m_vecFindItem[0][i][0].dbMin = 100;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("宽(mm)"), 64);
	if (m_breadth > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_breadth);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("高(mm)"), 64);
	if (m_height > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_height);
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
	if (!m_steel_bottom.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_bottom;

	infd.Init(_T("梁跨 参数设置"), _T("梁跨 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_length = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_breadth = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_height = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		CString _steel_left = infd.m_vecFindItem[0][i++][0].strItem;
		if (!_steel_left.IsEmpty() && !parseSteelMarking(_steel_left.GetBuffer())) {
			AfxMessageBox("左支座钢筋不合法");
			return false;
		}
		else
			m_steel_left = _steel_left;

		CString _steel_right = infd.m_vecFindItem[0][i++][0].strItem;
		if (!_steel_right.IsEmpty() && !parseSteelMarking(_steel_right.GetBuffer())) {
			AfxMessageBox("右支座钢筋不合法");
			return false;
		}
		else
			m_steel_right = _steel_right;


		CString _steel_bottom = infd.m_vecFindItem[0][i++][0].strItem;
		if (!_steel_bottom.IsEmpty() && !parseSteelMarking(_steel_bottom.GetBuffer())) {
			AfxMessageBox("底部中间钢筋不合法");
			return false;
		}
		else
			m_steel_bottom = _steel_bottom;
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
				else if (col == 2)	val = "长";
				else if (col == 3)	val = "宽";
				else if (col == 4)	val = "高";
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
				else if (col == 2)  val = Double2String(cols[row - 1].m_length);
				else if (col == 3)  val = Double2String(cols[row - 1].m_breadth);
				else if (col == 4)  val = Double2String(cols[row - 1].m_height);
				else if (col == 5) 	val = cols[row - 1].m_steel_left.GetBuffer();
				else if (col == 6) 	val = cols[row - 1].m_steel_right.GetBuffer();
				else if (col == 7) 	val = cols[row - 1].m_steel_bottom.GetBuffer();
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




/*****************************************************************************************************/

/* 矩形梁 */
string CBeamObj::m_ObjectCode = "01030104";

CBeamObj::CBeamObj()
{
	m_breadth = 0;
	m_height = 0;
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
		ar << m_name;
		ar << m_quantity;
		ar << m_steel_top;
		ar << m_steel_bottom;
		ar << m_steel_hooping;
		ar << m_spans.size();
		for (int i = 0; i < m_spans.size(); i++) {
			m_spans[i].Serialize(ar);
		}
	}
	else {
		ar >> m_breadth;
		ar >> m_height;
		ar >> m_name;
		ar >> m_quantity;
		ar >> m_steel_top;
		ar >> m_steel_bottom;
		ar >> m_steel_hooping;
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("高(mm)"), 64);
	if (m_height > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_height);
	infd.m_vecFindItem[0][i][0].dbMin = 100;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;
		
	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("宽(mm)"), 64);
	if (m_breadth > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_breadth);
	infd.m_vecFindItem[0][i][0].dbMin = 100;
	infd.m_vecFindItem[0][i][0].dbMax = 100000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("上部钢筋"), 64);
	if (!m_steel_top.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_top;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("下部钢筋"), 64);
	if (!m_steel_bottom.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_bottom;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("箍筋"), 64);
	if (!m_steel_hooping.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_steel_hooping;

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
		m_breadth = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		CString _steel_top = infd.m_vecFindItem[0][i++][0].strItem;
		if (!_steel_top.IsEmpty() && !parseSteelMarking(_steel_top.GetBuffer())) {
			AfxMessageBox("上部钢筋不合法");
			return false;
		}
		else
			m_steel_top = _steel_top;

		CString _steel_bottom = infd.m_vecFindItem[0][i++][0].strItem;
		if (!_steel_bottom.IsEmpty() && !parseSteelMarking(_steel_bottom.GetBuffer())) {
			AfxMessageBox("下部钢筋不合法");
			return false;
		}
		else
			m_steel_bottom = _steel_bottom;

		CString _steel_hooping = infd.m_vecFindItem[0][i++][0].strItem;
		if (!_steel_hooping.IsEmpty() && !parseSteelMarking(_steel_hooping.GetBuffer())) {
			AfxMessageBox("箍筋不合法");
			return false;
		}
		else
			m_steel_hooping = _steel_hooping;

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
		pGridCtrl->SetColumnCount(8 + 3);		//	额外增加三列 ： 序号/修改/删除
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
				else if (col == 2)	val = "宽";
				else if (col == 3)	val = "高";
				else if (col == 4)	val = "上部钢筋";
				else if (col == 5)	val = "下部钢筋";
				else if (col == 6)	val = "箍筋";
				else if (col == 7)	val = "数量";
				else if (col == 8)	val = "";
				else if (col == 9)	val = "";
				else if (col == 10)	val = "";

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
				if (col == 8 || col == 10) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 9) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_breadth, "%.0f");
				else if (col == 3)  val = Double2String(cols[row - 1].m_height, "%.0f");
				else if (col == 4)  val = cols[row - 1].m_steel_top.GetBuffer();
				else if (col == 5) 	val = cols[row - 1].m_steel_bottom.GetBuffer();
				else if (col == 6) 	val = cols[row - 1].m_steel_hooping.GetBuffer();
				else if (col == 7)  val = Double2String(cols[row - 1].m_quantity, "%.0f");
				else if (col == 8)	val = "修改（update）";
				else if (col == 9)	val = "删除（delete）";
				else if (col == 10)	val = "增加（create）";

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



void CBeamObj::ConcreteQuantity(string menuCode, vector<CBeamObj>& cols) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return;

}

void CBeamObj::SteelQuantityOfBeamSpan(int spanId, vector<vector<string>>& vecData) {
	
	if (spanId >= m_spans.size()) return;

	int quantity, first, second, diameter;
	double weight;

	if (!m_spans[spanId].m_steel_left.IsEmpty()) {
		if (parseSteelMarking(m_spans[spanId].m_steel_left.GetBuffer(), &quantity, &diameter, &weight, &first, &second)) {
			vector<string> vec;
			vec.push_back("左支座钢筋(" + string(m_spans[spanId].m_name.GetBuffer()) + ")");
			vec.push_back(Int2String(diameter));
			vec.push_back("");
			vec.push_back(Int2String(quantity));
			vec.push_back("");
			vec.push_back("");
			vec.push_back("");
			vecData.push_back(vec);
		}
	}

	if (!m_spans[spanId].m_steel_right.IsEmpty()) {
		if (parseSteelMarking(m_spans[spanId].m_steel_right.GetBuffer(), &quantity, &diameter, &weight, &first, &second)) {
			vector<string> vec;
			vec.push_back("右支座钢筋(" + string(m_spans[spanId].m_name.GetBuffer()) + ")");
			vec.push_back(Int2String(diameter));
			vec.push_back("");
			vec.push_back(Int2String(quantity));
			vec.push_back("");
			vec.push_back("");
			vec.push_back("");
			vecData.push_back(vec);
		}
	}

	if (!m_spans[spanId].m_steel_bottom.IsEmpty()) {
		if (parseSteelMarking(m_spans[spanId].m_steel_bottom.GetBuffer(), &quantity, &diameter, &weight, &first, &second)) {
			vector<string> vec;
			vec.push_back("下部中间钢筋(" + string(m_spans[spanId].m_name.GetBuffer()) + ")");
			vec.push_back(Int2String(diameter));
			vec.push_back("");
			vec.push_back(Int2String(quantity));
			vec.push_back("");
			vec.push_back("");
			vec.push_back("");
			vecData.push_back(vec);
		}
	}
}

void CBeamObj::SteelQuantity(string menuCode, vector<CBeamObj>& cols) {
	if (menuCode != CBeamObj::m_ObjectCode)
		return;

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
		parseSteelMarking(cols[i].m_steel_top.GetBuffer(), &quantity, &diameter, &weight, &first, &second);
		
		vector<string> vec;
		vec.push_back(string(cols[i].m_name.GetBuffer()));
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		vec.push_back("");
		gridDlg.m_vecData.push_back(vec);

		vec.clear();
		vec.push_back(string(cols[i].m_name.GetBuffer()) + " 上部通长钢筋");
		vec.push_back(Int2String(diameter));
		vec.push_back("12d + height + 15d");
		vec.push_back(Int2String(quantity));
		double len = 10 + 12 * diameter / 1000.0 + 15 * diameter / 1000.0;
		vec.push_back(Double2String(len));
		vec.push_back(Double2String(len * quantity * cols[i].m_quantity));
		vec.push_back(Double2String(len * quantity * cols[i].m_quantity * weight));
		gridDlg.m_vecData.push_back(vec);

		for (int k = 0; k < cols[i].m_spans.size(); k++) {
			cols[i].SteelQuantityOfBeamSpan(k, gridDlg.m_vecData);
		}
	}
	gridDlg.DoModal();
}

