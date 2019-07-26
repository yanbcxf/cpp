#include "stdafx.h"
#include "ProjectSettlementEx5.h"
#include "GridDlg.h"




//
//int CProjectSettlementEx5Obj::EarliestPaymentTime() {
//
//	return 0;
//}
//
//int CProjectSettlementEx5Obj::LatestPaymentTime() {
//	return 0;
//}

/***********************************************************************************/


/* 第六章 - 工程结算 */
string CProjectSettlementEx5::m_ObjectCode = "01060502";
double CProjectSettlementEx5::m_ObjectVersion = 1.0;




unsigned int CProjectSettlementEx5::PopupMenuId(string menuCode) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

CProjectSettlementEx5* CProjectSettlementEx5::NewParent(CString scheme) {
	CProjectSettlementEx5* p = NULL;
	if (scheme == "案例5")
		p = new CProjectSettlementEx5();
	if (p) p->m_scheme = scheme;
	return p;
}


bool CProjectSettlementEx5::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CProjectSettlementEx5*>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	vector<string> vecHeader;
	vector<vector<string>> vecData;
	vecHeader.push_back("工程结算方案,120");
	vecHeader.push_back("工程描述,550");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");

	int i = 1;
	for (CProjectSettlementEx5* e : cols) {
		vector<string> vec;
		vec.push_back(e->m_scheme.GetBuffer());
		vec.push_back(e->Description());
		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");
		vec.push_back("增加（create）");
		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

void CProjectSettlementEx5::Serialize(CArchive& ar, double version, CProjectSettlementEx5*  & p) {
	if (ar.IsStoring()) {
		ar << p->m_scheme;
		p->Serialize(ar, version);
	}
	else {
		CString scheme;
		ar >> scheme;
		p = NewParent(scheme);
		p->Serialize(ar, version);
	}
}


void CProjectSettlementEx5::Calculate(string menuCode, vector<CProjectSettlementEx5*>& cols) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("单方造价");


	for (int i = 0; i < cols.size(); i++)
	{


		/*vec.push_back(Double2String(total));
		gridDlg.m_vecData.push_back(vec);*/
	}
	gridDlg.DoModal();
}

CProjectSettlementEx5Obj* CProjectSettlementEx5::NewChild(CString scheme) {
	CProjectSettlementEx5Obj* p = NULL;

	if (scheme == "按工程量结算款")
		p = new CProjectSettlementEx5ObjA();
	if (scheme == "其它结算款费")
		p = new CProjectSettlementEx5ObjB();
	
	if (p) p->m_scheme = scheme;

	return p;
}


void CProjectSettlementEx5::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_regulation_rate;
		ar << m_tax_rate;
		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			ar << m_objs[i]->m_scheme;
			m_objs[i]->Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_regulation_rate;
		ar >> m_tax_rate;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CString scheme;
			ar >> scheme;
			CProjectSettlementEx5Obj* bs = NewChild(scheme);
			bs->Serialize(ar, version);
			m_objs.push_back(bs);
		}
	}
}


bool CProjectSettlementEx5::CreateOrUpdate() {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = m_scheme;
	infd.m_vecFindItem[0][i][0].strItem = m_scheme;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("工程名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("规费率（%）"), 64);
	if (m_regulation_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_regulation_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("增值税率（%）"), 64);
	if (m_tax_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_tax_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;


	infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_scheme = infd.m_vecFindItem[0][i++][0].strItem;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_regulation_rate = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		m_tax_rate = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		return true;
	}
	return false;
}


string CProjectSettlementEx5::Description() {
	stringstream ss;
	double price = 0;
	ss << "工程名称 : " << m_name.GetBuffer() << ",  ";
	for (int i = 0; i < m_objs.size(); i++) {
		price += m_objs[i]->ProjectPrice();
	}
	price = price * (1 + m_regulation_rate);
	ss << "工程总造价（不含税）（万元）: " << Double2String(price) << ",  ";

	price = price * (1 + m_tax_rate);

	ss << "工程总造价（含税）（万元）: " << Double2String(price) << ",  ";

	return ss.str();
}

void CProjectSettlementEx5::SortByMonth() {
	struct sort_deref
	{
		bool operator() (CProjectSettlementEx5Obj* lhs, CProjectSettlementEx5Obj* rhs)
		{
			return String2Double(lhs->m_month.GetBuffer()) < String2Double(rhs->m_month.GetBuffer());
		}
	};

	std::sort(m_objs.begin(), m_objs.end(), sort_deref());
}


bool CProjectSettlementEx5::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	SortByMonth();

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("月份");
	vecHeader.push_back("款项名称");
	vecHeader.push_back("款项类型");
	vecHeader.push_back("描述");

	vecHeader.push_back("");
	vecHeader.push_back("");

	int ttt = m_objs.size();

	for (int i = 0; i < ttt; i++) {
		vector<string> vec;
		vec.push_back(m_objs[i]->m_month.GetBuffer());
		vec.push_back(m_objs[i]->m_name.GetBuffer());
		vec.push_back(m_objs[i]->m_scheme.GetBuffer());
		vec.push_back(m_objs[i]->Description());

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}



bool CProjectSettlementEx5::AddChild(string menuCode) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "按工程量结算款;其它结算款费";
	infd.m_vecFindItem[0][i][0].strItem = "按工程量结算款";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	infd.Init(_T("工程结算 方案选择"), _T("工程结算 方案选择"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		CString scheme = infd.m_vecFindItem[0][i++][0].strItem;
		CProjectSettlementEx5Obj* c = NewChild(scheme);
		if (c->CreateOrUpdate(menuCode, this)) {
			m_objs.push_back(c);
			return true;
		}
		else {
			delete c;
		}
	}

	return false;
}

bool CProjectSettlementEx5::UpdateChild(string menuCode, int nRow) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size())
		return m_objs[nRow - 1]->CreateOrUpdate(menuCode, this);
	return false;
}


bool CProjectSettlementEx5::DeleteChild(string menuCode, int nRow) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		int idx = 0;
		vector<CProjectSettlementEx5Obj*>::iterator it = m_objs.begin();
		for (; it != m_objs.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		m_objs.erase(it);
		return true;
	}
	return false;
}


/* "工厂" 设计模式 */
bool CProjectSettlementEx5::Create(string strMenuCode, CProjectSettlementEx5*  & p) {
	if (strMenuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "案例5";
	infd.m_vecFindItem[0][i][0].strItem = "案例5";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	infd.Init(_T("合同价款 方案选择"), _T("合同价款 方案选择"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		CString scheme = infd.m_vecFindItem[0][i++][0].strItem;
		p = NewParent(scheme);
		bool res = p->CreateOrUpdate();
		if (res == false) {
			delete p;
			p = NULL;
		}
		return res;
	}

	return false;
}

bool CProjectSettlementEx5::Update(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return  cols[nRow - 1]->CreateOrUpdate();
	return false;
}

bool CProjectSettlementEx5::Delete(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CProjectSettlementEx5*>::iterator it = cols.begin();
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

void CProjectSettlementEx5ObjA::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_month;
		ar << m_name;
		ar << m_actual_workload;
	}
	else {
		ar >> m_month;
		ar >> m_name;
		ar >> m_actual_workload;
	}
}


bool CProjectSettlementEx5ObjA::CreateOrUpdate(string menuCode, CProjectSettlementEx5* parent) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.GROUP_NUM_PER_LINE = 3;

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("月份"), 64);
	if (!m_month.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_month;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("款项名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("工程量（立方米）"), 64);
	if (m_actual_workload > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_actual_workload);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("按工程量结算 参数设置"), _T("按工程量结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_month = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_name = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_actual_workload = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}

double CProjectSettlementEx5ObjA::ProjectPrice() {
	return m_unit_price * m_actual_workload / 10000;
}

void CProjectSettlementEx5ObjB::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_month;
		ar << m_name;
		ar << m_fund;
	}
	else {
		ar >> m_month;
		ar >> m_name;
		ar >> m_fund;
	}
}


bool CProjectSettlementEx5ObjB::CreateOrUpdate(string menuCode, CProjectSettlementEx5* parent) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.GROUP_NUM_PER_LINE = 3;

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("月份"), 64);
	if (!m_month.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_month;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("款项名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("金额"), 64);
	if (m_fund > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_fund);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	

	infd.Init(_T("金额结算 参数设置"), _T("金额结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_month = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_name = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_fund = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}

double CProjectSettlementEx5ObjB::ProjectPrice() {
	return m_fund;
}
