#include "stdafx.h"
#include "ProjectSettlement.h"
#include "GridDlg.h"




//
//int CProjectSettlementObj::EarliestPaymentTime() {
//
//	return 0;
//}
//
//int CProjectSettlementObj::LatestPaymentTime() {
//	return 0;
//}

/***********************************************************************************/


/* 第六章 - 工程结算 */
string CProjectSettlement::m_ObjectCode = "01060101";
double CProjectSettlement::m_ObjectVersion = 1.0;

CProjectSettlement::CProjectSettlement()
{
	m_scheme = "";
}


CProjectSettlement::~CProjectSettlement()
{
	for (CProjectSettlementObj* e : m_objs)
		delete e;
}

unsigned int CProjectSettlement::PopupMenuId(string menuCode) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

CProjectSettlement* CProjectSettlement::NewParent(CString scheme) {
	CProjectSettlement* p = NULL;
	if (scheme == "案例2")
		p = new CProjectSettlementEx2();
	if (scheme == "案例3")
		p = new CProjectSettlementEx3();
	if (scheme == "案例4")
		p = new CProjectSettlementEx4();

	if (p) p->m_scheme = scheme;
	return p;
}


bool CProjectSettlement::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CProjectSettlement*>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CProjectSettlement::m_ObjectCode)
		return false;

	vector<string> vecHeader;
	vector<vector<string>> vecData;
	vecHeader.push_back("工程结算方案,120");
	vecHeader.push_back("工程描述,550");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");

	int i = 1;
	for (CProjectSettlement* e : cols) {
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

void CProjectSettlement::Serialize(CArchive& ar, double version, CProjectSettlement*  & p) {
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


void CProjectSettlement::Calculate(string menuCode, vector<CProjectSettlement*>& cols) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
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

bool CProjectSettlement::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}


CProjectSettlementObj* CProjectSettlement::NewChild() {
	CProjectSettlementObj* p = new CProjectSettlementObjEx2();
	return p;
}

bool CProjectSettlement::AddChild(string menuCode) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
		return false;

	CProjectSettlementObj* c = NewChild();
	if (c->CreateOrUpdate(menuCode, this)) {
		m_objs.push_back(c);
		return true;
	}

	return false;
}

bool CProjectSettlement::UpdateChild(string menuCode, int nRow) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size())
		return m_objs[nRow - 1]->CreateOrUpdate(menuCode, this);
	return false;
}


bool CProjectSettlement::DeleteChild(string menuCode, int nRow) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		int idx = 0;
		vector<CProjectSettlementObj*>::iterator it = m_objs.begin();
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
bool CProjectSettlement::Create(string strMenuCode, CProjectSettlement*  & p) {
	if (strMenuCode != CProjectSettlement::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "案例2;案例3;案例4";
	infd.m_vecFindItem[0][i][0].strItem = "案例2";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	infd.Init(_T("工程结算 方案选择"), _T("工程结算 方案选择"));
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

bool CProjectSettlement::Update(string menuCode, int nRow, vector<CProjectSettlement*>& cols) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return  cols[nRow - 1]->CreateOrUpdate();
	return false;
}

bool CProjectSettlement::Delete(string menuCode, int nRow, vector<CProjectSettlement*>& cols) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CProjectSettlement*>::iterator it = cols.begin();
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

void CProjectSettlementObjEx2::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_month;
		ar << m_actual_workload;
		ar << m_schedule_workload;
		ar << m_party_a_material;
	}
	else {
		ar >> m_month;
		ar >> m_actual_workload;
		ar >> m_schedule_workload;
		ar >> m_party_a_material;
	}
}


bool CProjectSettlementObjEx2::CreateOrUpdate(string menuCode, CProjectSettlement* parent) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("实际完成产值(万元)"), 64);
	if (m_actual_workload > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_actual_workload);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计划完成产值(万元)"), 64);
	if (m_schedule_workload > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_schedule_workload);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("甲供材料设备价值(万元)"), 64);
	if (m_party_a_material > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_party_a_material);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;


	infd.Init(_T("各月实际完成产值 参数设置"), _T("各月实际完成产值 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_month = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_actual_workload = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_schedule_workload = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_party_a_material = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}



CProjectSettlementObj* CProjectSettlementEx2::NewChild() {
	CProjectSettlementObj* p = new CProjectSettlementObjEx2();
	return p;
}


void CProjectSettlementEx2::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_total_price;
		ar << m_advance_payment_percent;
		ar << m_material_percent;
		ar << m_quality_bond_percent;
		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			m_objs[i]->Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_total_price;
		ar >> m_advance_payment_percent;
		ar >> m_material_percent;
		ar >> m_quality_bond_percent;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CProjectSettlementObj* bs = NewChild();
			bs->Serialize(ar, version);
			m_objs.push_back(bs);
		}
	}
}


bool CProjectSettlementEx2::CreateOrUpdate() {
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("工程造价(万元)"), 64);
	if (m_total_price > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_total_price);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("建筑材料及设备费占比（%）"), 64);
	if (m_material_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_material_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("预付款占比（%）"), 64);
	if (m_advance_payment_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_advance_payment_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("质量保证金占比（%）"), 64);
	if (m_quality_bond_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_quality_bond_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;


	infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_scheme = infd.m_vecFindItem[0][i++][0].strItem;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_total_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_material_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		m_advance_payment_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		m_quality_bond_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		return true;
	}
	return false;
}


string CProjectSettlementEx2::Description() {
	stringstream ss;
	ss << "工程名称 : " << m_name.GetBuffer() << ",  ";
	ss << "工程造价（万元）: " << Double2String(m_total_price) << ",  ";
	ss << "建筑材料及设备费占比（%）: " << Double2String(m_material_percent * 100, "%.2f") << ",  ";
	ss << "预付款占比（%）: " << Double2String(m_advance_payment_percent * 100, "%.2f") << ",  ";
	ss << "质量保证金占比（%）: " << Double2String(m_quality_bond_percent * 100, "%.2f") << ",  ";
	return ss.str();
}


bool CProjectSettlementEx2::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("月份");
	vecHeader.push_back("完成产值");
	vecHeader.push_back("计划产值");
	vecHeader.push_back("甲供材料价值");

	vecHeader.push_back("累计工程款");
	vecHeader.push_back("预付款扣回");
	vecHeader.push_back("质量保证金扣留");
	vecHeader.push_back("工程款扣留");
	vecHeader.push_back("进度款");

	vecHeader.push_back("");
	vecHeader.push_back("");

	/* 预付款 */
	double advance = m_total_price * m_advance_payment_percent;
	/* 工程款起扣点 */
	double deductValue = advance / m_material_percent;
	deductValue = m_total_price - deductValue;

	/* 累计工程款 */
	double value = 0;
	/* 累计质量保证金 */
	double quality = 0;
	/* 累计扣留工程款 */
	double detain = 0;

	int ttt = m_objs.size();

	for (int i = 0; i < ttt; i++) {
		CProjectSettlementObjEx2 e = *(CProjectSettlementObjEx2 *)m_objs[i];
		vector<string> vec;
		vec.push_back(e.m_month.GetBuffer());
		vec.push_back(Double2String(e.m_actual_workload, "%.2f"));
		vec.push_back(Double2String(e.m_schedule_workload, "%.2f"));
		vec.push_back(Double2String(e.m_party_a_material, "%.2f"));

		/* 累计工程款 */
		value += e.m_actual_workload;
		vec.push_back(Double2String(value, "%.2f"));

		/* 工程进度款 */
		double progress = e.m_actual_workload;

		double deduct = 0;	/* 应扣回预付款 */
		if (value > deductValue) {
			if (value - e.m_actual_workload > deductValue) {
				/* 上个月 工程款就已超过 起扣点 */
				deduct = e.m_actual_workload * m_material_percent;
			}
			else {
				/* 本月开始起扣 */
				deduct = (value - deductValue) * m_material_percent;
			}
			advance = advance - deduct;
		}

		/* 进度款中扣除 预付款的扣回 */
		progress -= deduct;
		vec.push_back(Double2String(deduct, "%.2f"));

		/* 进度款中扣除 质量保证金 */
		progress -= e.m_actual_workload * m_quality_bond_percent;
		quality += e.m_actual_workload * m_quality_bond_percent;
		vec.push_back(Double2String(e.m_actual_workload * m_quality_bond_percent, "%.2f"));

		/* 进度款中扣除 工程款扣留 */
		if (e.m_actual_workload < e.m_schedule_workload) {
			if ((e.m_schedule_workload - e.m_actual_workload) / e.m_schedule_workload >= 0.1) {
				progress -= e.m_actual_workload * 0.05;
				detain += e.m_actual_workload * 0.05;
				vec.push_back(Double2String(e.m_actual_workload * 0.05, "%.2f"));
			}
			else
				vec.push_back(Double2String(0, "%.2f"));
		}
		else
			vec.push_back(Double2String(0, "%.2f"));

		/* 进度款中扣除 甲供材料价值 */
		progress -= e.m_party_a_material;
		/* 进度款 */
		vec.push_back(Double2String(progress, "%.2f"));

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}


/***********************************************************************/

void CProjectSettlementObjEx3::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_month;
		ar << m_actual_workload;
	}
	else {
		ar >> m_month;
		ar >> m_actual_workload;
	}
}


bool CProjectSettlementObjEx3::CreateOrUpdate(string menuCode, CProjectSettlement* parent) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("实际完成产值(万元)"), 64);
	if (m_actual_workload > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_actual_workload);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;


	infd.Init(_T("各月实际完成产值 参数设置"), _T("各月实际完成产值 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_month = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_actual_workload = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}



CProjectSettlementObj* CProjectSettlementEx3::NewChild() {
	CProjectSettlementObj* p = new CProjectSettlementObjEx3();
	return p;
}



void CProjectSettlementEx3::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_total_workload;
		ar << m_unit_price;
		ar << m_advance_payment_percent;
		ar << m_quality_bond_percent;
		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			m_objs[i]->Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_total_workload;
		ar >> m_unit_price;
		ar >> m_advance_payment_percent;
		ar >> m_quality_bond_percent;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CProjectSettlementObj* bs = NewChild();
			bs->Serialize(ar, version);
			m_objs.push_back(bs);
		}
	}
}

bool CProjectSettlementEx3::CreateOrUpdate() {
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计划总量(立方米)"), 64);
	if (m_total_workload > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_total_workload);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("单价(元/立方米)"), 64);
	if (m_unit_price > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_unit_price);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("预付款占比（%）"), 64);
	if (m_advance_payment_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_advance_payment_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("质量保证金占比（%）"), 64);
	if (m_quality_bond_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_quality_bond_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;


	infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_scheme = infd.m_vecFindItem[0][i++][0].strItem;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_total_workload = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_unit_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_advance_payment_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		m_quality_bond_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		return true;
	}
	return false;
}


string CProjectSettlementEx3::Description() {
	stringstream ss;
	ss << "工程名称 : " << m_name.GetBuffer() << ",  ";
	ss << "计划总量(立方米): " << Double2String(m_total_workload) << ",  ";
	ss << "单价(元/立方米): " << Double2String(m_unit_price) << ",  ";
	ss << "预付款占比（%）: " << Double2String(m_advance_payment_percent * 100, "%.2f") << ",  ";
	ss << "质量保证金占比（%）: " << Double2String(m_quality_bond_percent * 100, "%.2f") << ",  ";
	/* 预付款起扣点 */
	double deductValue = m_total_workload * m_unit_price  * 0.3;
	ss << "预付款起扣点 : " << Double2String(deductValue, "%.2f");
	return ss.str();
}



bool CProjectSettlementEx3::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("月份");
	vecHeader.push_back("完成工程量（立方米）");

	vecHeader.push_back("理论进度款");
	vecHeader.push_back("累计工程款");
	vecHeader.push_back("累计工程量（立方米）");
	
	vecHeader.push_back("质量保证金扣留");
	vecHeader.push_back("预付款扣回");

	vecHeader.push_back("进度款");

	vecHeader.push_back("");
	vecHeader.push_back("");

	/* 预付款 */
	double advance = m_total_workload * m_unit_price * m_advance_payment_percent;
	/* 预付款起扣点 */
	double deductValue = m_total_workload * m_unit_price  * 0.3;
	/* 工程量变化，单价调整点 */
	double adjustWorkload = m_total_workload * 1.15;

	/* 累计工程款 */
	double accValue = 0;
	/* 累计工程量 */
	double accWorkload = 0;
	/* 累计质量保证金 */
	double accQuality = 0;
	/* 累计扣留工程款 */
	double accDetain = 0;
	
	/* 应扣回预付款 */
	double deduct = 0;	
	int deductStart, deductEnd;
	int ttt = m_objs.size();
	deductStart = deductEnd = ttt;

	for (int i = 0; i < ttt; i++) {
		CProjectSettlementObjEx3 e = *(CProjectSettlementObjEx3 *)m_objs[i];
		vector<string> vec;
		vec.push_back(e.m_month.GetBuffer());
		vec.push_back(Double2String(e.m_actual_workload, "%.2f"));

		
		/* 工程进度款 */
		double progress;
		if ((accWorkload + e.m_actual_workload) < adjustWorkload) {
			progress = e.m_actual_workload * m_unit_price;
		}
		else if (accWorkload < adjustWorkload && (accWorkload + e.m_actual_workload) > adjustWorkload) {
			progress = (adjustWorkload - accWorkload) * m_unit_price;
			progress += (accWorkload + e.m_actual_workload - adjustWorkload) * m_unit_price * 0.9;
		}
		else  {
			progress = e.m_actual_workload * m_unit_price * 0.9;
		}
		vec.push_back(Double2String(progress, "%.2f"));

		
		/* 累计工程款 */
		accValue += progress;
		vec.push_back(Double2String(accValue, "%.2f"));

		/* 累计工程量 */
		accWorkload += e.m_actual_workload;
		vec.push_back(Double2String(accWorkload, "%.2f"));
		
		/* 进度款中扣除 质量保证金 */
		accQuality += progress * m_quality_bond_percent;
		vec.push_back(Double2String(progress * m_quality_bond_percent, "%.2f"));
		progress -= progress * m_quality_bond_percent;

		/* 计算随后几个月的 应扣预付款 */
		if (deduct == 0) {
			if (accValue > deductValue) {
				int n = ttt - 2 - i;
				if (n > 0) deduct = advance / n;
				deductStart = i + 1;
				deductEnd = ttt - 2;
			}
		}

		/* 进度款中扣除 预付款的扣回 */
		if ( i >= deductStart &&  i <= deductEnd) {
			progress -= deduct;
			vec.push_back(Double2String(deduct, "%.2f"));
		}
		else {
			vec.push_back(Double2String(0, "%.2f"));
		}
		

		if (accDetain + progress < 150000) {
			accDetain += progress;
			progress = 0;
		}
		else {
			progress = accDetain + progress;
			accDetain = 0;
		}

		/* 进度款 */
		vec.push_back(Double2String(progress, "%.2f"));

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

/***********************************************************************/

void CProjectSettlementObjEx4::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_month;
		ar << m_actual_fund;
		ar << m_adjust_fund;
		ar << m_ft1;
		ar << m_ft2;
		ar << m_ft3;
		ar << m_ft4;
		ar << m_ft5;
	}
	else {
		ar >> m_month;
		ar >> m_actual_fund;
		ar >> m_adjust_fund;
		ar >> m_ft1;
		ar >> m_ft2;
		ar >> m_ft3;
		ar >> m_ft4;
		ar >> m_ft5;
	}
}


bool CProjectSettlementObjEx4::CreateOrUpdate(string menuCode, CProjectSettlement* parent) {
	if (menuCode != CProjectSettlement::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("完成工程款(万元)"), 64);
	if (m_actual_fund > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_actual_fund);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("变动调整款(万元)"), 64);
	if (m_adjust_fund != 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_adjust_fund);
	infd.m_vecFindItem[0][i][0].dbMin = -1000000;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结算期价格指数1"), 64);
	if (m_ft1 > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_ft1);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结算期价格指数2"), 64);
	if (m_ft2 > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_ft2);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结算期价格指数3"), 64);
	if (m_ft3 > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_ft3);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结算期价格指数4"), 64);
	if (m_ft4 > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_ft4);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结算期价格指数5"), 64);
	if (m_ft5 > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_ft5);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("各月实际完成产值 参数设置"), _T("各月实际完成产值 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_month = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_actual_fund = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_adjust_fund = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_ft1 = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_ft2 = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_ft3 = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_ft4 = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_ft5 = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}



CProjectSettlementObj* CProjectSettlementEx4::NewChild() {
	CProjectSettlementObj* p = new CProjectSettlementObjEx4();
	return p;
}



void CProjectSettlementEx4::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_total_fund;
		ar << m_advance_payment_percent;
		ar << m_quality_bond_percent;
		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			m_objs[i]->Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_total_fund;
		ar >> m_advance_payment_percent;
		ar >> m_quality_bond_percent;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CProjectSettlementObj* bs = NewChild();
			bs->Serialize(ar, version);
			m_objs.push_back(bs);
		}
	}
}

bool CProjectSettlementEx4::CreateOrUpdate() {
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("合同总价(万元)"), 64);
	if (m_total_fund > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_total_fund);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("预付款占比（%）"), 64);
	if (m_advance_payment_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_advance_payment_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("质量保证金占比（%）"), 64);
	if (m_quality_bond_percent > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_quality_bond_percent * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;


	infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_scheme = infd.m_vecFindItem[0][i++][0].strItem;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_total_fund = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_advance_payment_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		m_quality_bond_percent = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) / 100;
		return true;
	}
	return false;
}


string CProjectSettlementEx4::Description() {
	stringstream ss;
	ss << "工程名称 : " << m_name.GetBuffer() << ",  ";
	ss << "合同总价(万元): " << Double2String(m_total_fund) << ",  ";
	ss << "预付款占比（%）: " << Double2String(m_advance_payment_percent * 100, "%.2f") << ",  ";
	ss << "质量保证金占比（%）: " << Double2String(m_quality_bond_percent * 100, "%.2f") << ",  ";
	/* 预付款起扣点 */
	double deductValue = 0.3;
	ss << "预付款起扣点 : " << Double2String(deductValue, "%.2f");
	return ss.str();
}



bool CProjectSettlementEx4::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("月份");
	
	vecHeader.push_back("合同进度款");
	vecHeader.push_back("调值后进度款");

	vecHeader.push_back("质量保证金扣留");
	vecHeader.push_back("预付款扣回");

	vecHeader.push_back("进度款");

	vecHeader.push_back("");
	vecHeader.push_back("");

	/* 预付款 */
	double advance = m_total_fund * m_advance_payment_percent;
	

	/* 累计工程款 */
	double accValue = 0;
	
	/* 累计质量保证金 */
	double accQuality = 0;
	/* 累计扣留工程款 */
	double accDetain = 0;

	/* 应扣回预付款 */
	double deduct = advance / 2;

	int ttt = m_objs.size();

	for (int i = 0; i < ttt; i++) {
		CProjectSettlementObjEx4 e = *(CProjectSettlementObjEx4 *)m_objs[i];
		vector<string> vec;
		vec.push_back(e.m_month.GetBuffer());
		vec.push_back(Double2String(e.m_actual_fund, "%.2f"));


		/* 工程进度款， 先调值后 再增减款 */
		double progress;
		progress = m_b0 + (m_b1 * e.m_ft1 / m_f01 + m_b2 * e.m_ft2 / m_f02 + m_b3 * e.m_ft3 / m_f03 + m_b4 * e.m_ft4 / m_f04 + m_b5 * e.m_ft5 / m_f05);
		progress = progress * e.m_actual_fund  ;
		vec.push_back(Double2String(progress, "%.2f"));

		
		/* 进度款中扣除 质量保证金 */
		accQuality += progress * m_quality_bond_percent;
		vec.push_back(Double2String(progress * m_quality_bond_percent, "%.2f"));
		progress -= progress * m_quality_bond_percent;

		/* 进度款中扣除 预付款的扣回 */
		if (i >= ttt - 2 && i <= ttt -1) {
			progress -= deduct;
			vec.push_back(Double2String(deduct, "%.2f"));
		}
		else {
			vec.push_back(Double2String(0, "%.2f"));
		}

		progress += e.m_adjust_fund;
		/* 进度款 */
		vec.push_back(Double2String(progress, "%.2f"));

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}