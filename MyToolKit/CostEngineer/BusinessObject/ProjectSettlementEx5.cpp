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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("�·�"), 64);
	if (!m_month.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_month;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("��������"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("�������������ף�"), 64);
	if (m_actual_workload > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_actual_workload);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("������������ ��������"), _T("������������ ��������"));
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
	return m_actual_workload ;
}

string CProjectSettlementEx5ObjA::Description() {
	return string("������ �� ") + Double2String(m_actual_workload, "%.2f");
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("�·�"), 64);
	if (!m_month.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_month;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("��������"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("���"), 64);
	if (m_fund > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_fund);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;



	infd.Init(_T("������ ��������"), _T("������ ��������"));
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

string CProjectSettlementEx5ObjB::Description() {
	return string("���ȿ� �� ") + Double2String(m_fund, "%.2f");
}

/***********************************************************************************/


/* ������ - ���̽��� */
string CProjectSettlementEx5::m_ObjectCode = "01060502";
double CProjectSettlementEx5::m_ObjectVersion = 1.0;




unsigned int CProjectSettlementEx5::PopupMenuId(string menuCode) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

CProjectSettlementEx5* CProjectSettlementEx5::NewParent(CString scheme) {
	CProjectSettlementEx5* p = NULL;
	if (scheme == "����5")
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
	vecHeader.push_back("���̽��㷽��,120");
	vecHeader.push_back("��������,550");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");

	int i = 1;
	for (CProjectSettlementEx5* e : cols) {
		vector<string> vec;
		vec.push_back(e->m_scheme.GetBuffer());
		vec.push_back(e->Description());
		vec.push_back("�޸ģ�update��");
		vec.push_back("ɾ����delete��");
		vec.push_back("���ӣ�create��");
		vec.push_back("���㣨operate1��");
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




CProjectSettlementEx5Obj* CProjectSettlementEx5::NewChild(CString scheme) {
	CProjectSettlementEx5Obj* p = NULL;

	if (scheme == "�������������")
		p = new CProjectSettlementEx5ObjA();
	if (scheme == "����������")
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

		ar << m_mapProjectUnitPrice.size();
		for (map<string, double>::iterator it = m_mapProjectUnitPrice.begin(); it != m_mapProjectUnitPrice.end(); it++) {
			CString proj = it->first.c_str();
			ar << proj;
			ar << it->second;
		}

		ar << m_mapProjectWorkload.size();
		for (map<string, double>::iterator it = m_mapProjectWorkload.begin(); it != m_mapProjectWorkload.end(); it++) {
			CString proj = it->first.c_str();
			ar << proj;
			ar << it->second;
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
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CString proj;	double db;
			ar >> proj;		ar >> db;
			m_mapProjectUnitPrice[proj.GetBuffer()] = db;
		}
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CString proj;	double db;
			ar >> proj;		ar >> db;
			m_mapProjectWorkload[proj.GetBuffer()] = db;
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("���㷽��"), 64);

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("��������"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("����ʣ�%��"), 64);
	if (m_regulation_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_regulation_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("��ֵ˰�ʣ�%��"), 64);
	if (m_tax_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_tax_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	/* ��ȡ���еķ���� */

	map<string, double>::iterator it1 = m_mapProjectUnitPrice.begin();
	for (; it1 != m_mapProjectUnitPrice.end(); it1++) {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, (it1->first + "���ۣ�Ԫ/�����ף�").c_str() , 64);
		if (it1->second > 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", it1->second);
		infd.m_vecFindItem[0][i][0].dbMin = 0.01;
		infd.m_vecFindItem[0][i][0].dbMax = 10000000;
	}

	map<string, double>::iterator it2 = m_mapProjectWorkload.begin();
	for (; it2 != m_mapProjectWorkload.end(); it2++) {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, (it2->first + "�������������ף�").c_str(), 64);
		if (it2->second > 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", it2->second);
		infd.m_vecFindItem[0][i][0].dbMin = 0.01;
		infd.m_vecFindItem[0][i][0].dbMax = 100000000;
	}
	


	infd.Init(_T("���̽��� ��������"), _T("���̽��� ��������"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_scheme = infd.m_vecFindItem[0][i++][0].strItem;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_regulation_rate = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		m_tax_rate = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;

		map<string, double>::iterator it1 = m_mapProjectUnitPrice.begin();
		for (; it1 != m_mapProjectUnitPrice.end(); it1++) {
			it1->second = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		}

		map<string, double>::iterator it2 = m_mapProjectWorkload.begin();
		for (; it2 != m_mapProjectWorkload.end(); it2++) {
			it2->second = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		}

		return true;
	}
	return false;
}


string CProjectSettlementEx5::Description() {
	stringstream ss;
	double price = 0;
	ss << "�������� : " << m_name.GetBuffer() << ",  ";
	for (int i = 0; i < m_objs.size(); i++) {
		price += m_objs[i]->ProjectPrice();
	}
	price = price * (1 + m_regulation_rate);
	ss << "��������ۣ�����˰������Ԫ��: " << Double2String(price) << ",  ";

	price = price * (1 + m_tax_rate);

	ss << "��������ۣ���˰������Ԫ��: " << Double2String(price) << ",  ";

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

void CProjectSettlementEx5::Calculate() 
{
	bool bOK = true;
	for (map<string, double>::iterator it = m_mapProjectUnitPrice.begin(); it != m_mapProjectUnitPrice.end(); it++) {
		if (it->second == 0 ) bOK = false;
	}
	if (!bOK) {
		AfxMessageBox("��Ϊ��������á����ۡ�");
		return;
	}

	for (map<string, double>::iterator it = m_mapProjectWorkload.begin(); it != m_mapProjectWorkload.end(); it++) {
		if (it->second == 0) bOK = false;
	}
	if (!bOK) {
		AfxMessageBox("��Ϊ��������á���������");
		return;
	}

	/* ��¼ʩ���·ݵĽ��ȿ� */
	map<string, double> mapMonth;
	for (int i = 0; i < m_objs.size(); i++) {
		string month = m_objs[i]->m_month.GetBuffer();
		if (mapMonth.count(month) == 0) {
			mapMonth[month] = 0;
		}
	}

	/* ��¼��ǰ�µ����з���̵��ۼƹ����� */
	map<string, double> mapProj = m_mapProjectUnitPrice;
	for (map<string, double>::iterator it = mapProj.begin(); it != mapProj.end(); it++) {
		it->second = 0;
	}

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("�·�");
	gridDlg.m_vecHeader.push_back("���ȿ�");

	/* ���»��ܼ��� */
	map<string, double>::iterator it = mapMonth.begin();
	for (; it != mapMonth.end(); it++) {
		string month = it->first;
		string desc;
		for (int i = 0; i < m_objs.size(); i++) {
			if (m_objs[i]->m_month.GetBuffer() == month) {
				double price;
				if (m_objs[i]->m_scheme == "�������������") {
					string proj = m_objs[i]->m_name.GetBuffer();
					if (mapProj[proj] > m_mapProjectWorkload[proj]) {
						price = m_mapProjectUnitPrice[proj] * 0.9 * m_objs[i]->ProjectPrice();
					}
					else if (mapProj[proj] + m_objs[i]->ProjectPrice() > m_mapProjectWorkload[proj]) {
						price = (m_mapProjectWorkload[proj] - mapProj[proj]) * m_mapProjectUnitPrice[proj];
						price += (mapProj[proj] + m_objs[i]->ProjectPrice() - m_mapProjectWorkload[proj])  * m_mapProjectUnitPrice[proj] * 0.9;
					}
					else {
						price = m_mapProjectUnitPrice[proj] * m_objs[i]->ProjectPrice();
					}
				}
				else {
					price = m_objs[i]->ProjectPrice();
				}
				if (!desc.empty())	desc += " , ";
				desc += m_objs[i]->m_name.GetBuffer() + string(" : ") + Double2String(price, "%.2f");
				mapMonth[month] += price;
			}
		}
		if (!desc.empty())	desc += " , ";
		desc += "���ȿ� �� " + Double2String(mapMonth[month], "%.2f");

		vector<string> vec;
		vec.push_back(month);
		vec.push_back(desc);
		gridDlg.m_vecData.push_back(vec);
	}
	gridDlg.DoModal();
}


bool CProjectSettlementEx5::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	SortByMonth();

	/* ��ȡ���еķ���� */
	for (int i = 0; i < m_objs.size(); i++) {
		if (m_objs[i]->m_scheme == "�������������") {
			string proj = m_objs[i]->m_name.GetBuffer();
			if (m_mapProjectUnitPrice.count(proj) == 0) {
				m_mapProjectUnitPrice[proj] = 0;
				m_mapProjectWorkload[proj] = 0;
			}
		}
	}

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("�·�");
	vecHeader.push_back("��������");
	vecHeader.push_back("��������");
	vecHeader.push_back("����");

	vecHeader.push_back("");
	vecHeader.push_back("");

	int ttt = m_objs.size();

	for (int i = 0; i < ttt; i++) {
		vector<string> vec;
		vec.push_back(m_objs[i]->m_month.GetBuffer());
		vec.push_back(m_objs[i]->m_name.GetBuffer());
		vec.push_back(m_objs[i]->m_scheme.GetBuffer());
		vec.push_back(m_objs[i]->Description());

		vec.push_back("�޸ģ�update��");
		vec.push_back("ɾ����delete��");

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
	infd.m_vecFindItem[0][i][0].strData = "�������������;����������";
	infd.m_vecFindItem[0][i][0].strItem = "�������������";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("���㷽��"), 64);

	infd.Init(_T("���̽��� ����ѡ��"), _T("���̽��� ����ѡ��"));
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


/* "����" ���ģʽ */
bool CProjectSettlementEx5::Create(string strMenuCode, CProjectSettlementEx5*  & p) {
	if (strMenuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "����5";
	infd.m_vecFindItem[0][i][0].strItem = "����5";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("���㷽��"), 64);

	infd.Init(_T("��ͬ�ۿ� ����ѡ��"), _T("��ͬ�ۿ� ����ѡ��"));
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


bool CProjectSettlementEx5::Calculate(string menuCode, int nRow,  vector<CProjectSettlementEx5*>& cols) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		cols[nRow - 1]->Calculate();
	}
	return true;
}

/***********************************************************************************/
