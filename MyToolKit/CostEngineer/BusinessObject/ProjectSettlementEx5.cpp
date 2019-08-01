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
		ar << m_material_change;
		ar << m_people_change;
	}
	else {
		ar >> m_month;
		ar >> m_name;
		ar >> m_actual_workload;
		ar >> m_material_change;
		ar >> m_people_change;
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

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("材料费变动(不含规费和税金)（元）"), 64);
	if (m_material_change != 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_material_change);
	infd.m_vecFindItem[0][i][0].dbMin = -100000;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("人工费变动(不含规费和税金)（元）"), 64);
	if (m_people_change != 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_people_change);
	infd.m_vecFindItem[0][i][0].dbMin = -100000;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;

	infd.Init(_T("按工程量结算 参数设置"), _T("按工程量结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_month = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_name = infd.m_vecFindItem[0][i++][0].strItem.GetBuffer();
		m_actual_workload = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_material_change = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_people_change = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}

double CProjectSettlementEx5ObjA::ProjectPrice() {
	return m_actual_workload ;
}

string CProjectSettlementEx5ObjA::Description() {
	stringstream ss;
	ss << "工程量 ： " << Double2String(m_actual_workload, "%.2f");
	if (m_material_change != 0)
		ss << ", " << "材料费变动 : " << Double2String(m_material_change, "%.2f");
	if (m_people_change != 0)
		ss << ", " << "人工费变动 : " << Double2String(m_people_change, "%.2f");
	return ss.str();
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("金额(不含规费和税金)"), 64);
	if (m_fund > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_fund);
	infd.m_vecFindItem[0][i][0].dbMin = -100000;
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

string CProjectSettlementEx5ObjB::Description() {
	return string("进度款 ： ") + Double2String(m_fund, "%.2f");
}

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
	vecHeader.push_back(",80");
	vecHeader.push_back(",80");
	vecHeader.push_back(",80");
	vecHeader.push_back(",80");
	vecHeader.push_back(",80");

	int i = 1;
	for (CProjectSettlementEx5* e : cols) {
		vector<string> vec;
		vec.push_back(e->m_scheme.GetBuffer());
		vec.push_back(e->Description());
		vec.push_back("修改" + ActionType2String(ActionType::Update));
		vec.push_back("删除" + ActionType2String(ActionType::Delete));
		vec.push_back("增加" + ActionType2String(ActionType::Create));
		vec.push_back("计算" + ActionType2String(ActionType::Calculate));
		vec.push_back("调整款计算" + ActionType2String(ActionType::Assist));
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
		ar << m_manage_rate;
		ar << m_net_rate;
		ar << m_regulation_rate;
		ar << m_tax_rate;

		ar << m_unit_measure;
		ar << m_total_measure;

		ar << m_provisional_sum;
		ar << m_daywork_labor;
		ar << m_estimate_engineering;
		ar << m_estimate_material;
		ar << m_general_constracting_service_fee;

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
		ar >> m_manage_rate;
		ar >> m_net_rate;
		ar >> m_regulation_rate;
		ar >> m_tax_rate;

		ar >> m_unit_measure;
		ar >> m_total_measure;

		ar >> m_provisional_sum;
		ar >> m_daywork_labor;
		ar >> m_estimate_engineering;
		ar >> m_estimate_material;
		ar >> m_general_constracting_service_fee;

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
		SortByMonth();
	}
}


bool CProjectSettlementEx5::CreateOrUpdate() {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("管理费率（%）"), 64);
	if (m_manage_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_manage_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("利润率（%）"), 64);
	if (m_net_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_net_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

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

	/* 措施项目费 */
	i=0;
	infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("单价措施项目费（万元）"), 64);
	if (m_unit_measure > 0)
		infd.m_vecFindItem[1][i][0].strItem.Format("%.2f", m_unit_measure);
	infd.m_vecFindItem[1][i][0].dbMin = 0.00;
	infd.m_vecFindItem[1][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[1][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[1][i][0].caption, _T("总价措施项目费（万元）"), 64);
	if (m_total_measure > 0)
		infd.m_vecFindItem[1][i][0].strItem.Format("%.2f", m_total_measure);
	infd.m_vecFindItem[1][i][0].dbMin = 0.00;
	infd.m_vecFindItem[1][i][0].dbMax = 100000;

	/* 其他项目费 */
	i = 0;
	infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("暂列金额（万元）"), 64);
	if (m_provisional_sum > 0)
		infd.m_vecFindItem[2][i][0].strItem.Format("%.2f", m_provisional_sum);
	infd.m_vecFindItem[2][i][0].dbMin = 0.00;
	infd.m_vecFindItem[2][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("材料暂估单价（万元）"), 64);
	if (m_estimate_material > 0)
		infd.m_vecFindItem[2][i][0].strItem.Format("%.2f", m_estimate_material);
	infd.m_vecFindItem[2][i][0].dbMin = 0.00;
	infd.m_vecFindItem[2][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("专业工程暂估价（万元）"), 64);
	if (m_estimate_engineering > 0)
		infd.m_vecFindItem[2][i][0].strItem.Format("%.2f", m_estimate_engineering);
	infd.m_vecFindItem[2][i][0].dbMin = 0.00;
	infd.m_vecFindItem[2][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("计日工（万元）"), 64);
	if (m_daywork_labor > 0)
		infd.m_vecFindItem[2][i][0].strItem.Format("%.2f", m_daywork_labor);
	infd.m_vecFindItem[2][i][0].dbMin = 0.00;
	infd.m_vecFindItem[2][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[2][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[2][i][0].caption, _T("总承包服务费（万元）"), 64);
	if (m_general_constracting_service_fee > 0)
		infd.m_vecFindItem[2][i][0].strItem.Format("%.2f", m_general_constracting_service_fee);
	infd.m_vecFindItem[2][i][0].dbMin = 0.00;
	infd.m_vecFindItem[2][i][0].dbMax = 100000;


	/* 获取所有的分项工程 */
	g = 3;
	i = 0;
	map<string, double>::iterator it1 = m_mapProjectUnitPrice.begin();
	for (; it1 != m_mapProjectUnitPrice.end(); it1++) {
		infd.m_vecFindItem[g][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[g][i][0].caption, (it1->first + "单价（元/立方米）").c_str() , 64);
		if (it1->second > 0)
			infd.m_vecFindItem[g][i][0].strItem.Format("%.2f", it1->second);
		infd.m_vecFindItem[g][i][0].dbMin = 0.01;
		infd.m_vecFindItem[g][i][0].dbMax = 10000000;
		i++;
	}

	g = 4;
	i = 0;
	map<string, double>::iterator it2 = m_mapProjectWorkload.begin();
	for (; it2 != m_mapProjectWorkload.end(); it2++) {
		infd.m_vecFindItem[g][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[g][i][0].caption, (it2->first + "工程量（立方米）").c_str(), 64);
		if (it2->second > 0)
			infd.m_vecFindItem[g][i][0].strItem.Format("%.2f", it2->second);
		infd.m_vecFindItem[g][i][0].dbMin = 0.01;
		infd.m_vecFindItem[g][i][0].dbMax = 10000000;
		i++;
	}
	

	infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		g = 0;
		m_scheme = infd.m_vecFindItem[g][i++][0].strItem;
		m_name = infd.m_vecFindItem[g][i++][0].strItem;
		m_manage_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		m_net_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		m_regulation_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		m_tax_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;

		i = 0;
		g = 1;
		m_unit_measure = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		m_total_measure = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

		i = 0;
		g = 2;
		m_provisional_sum = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		m_estimate_material = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		m_estimate_engineering = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		m_daywork_labor = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		m_general_constracting_service_fee = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

		i = 0;
		g = 3;
		map<string, double>::iterator it1 = m_mapProjectUnitPrice.begin();
		for (; it1 != m_mapProjectUnitPrice.end(); it1++) {
			it1->second = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		}

		i = 0;
		g = 4;
		map<string, double>::iterator it2 = m_mapProjectWorkload.begin();
		for (; it2 != m_mapProjectWorkload.end(); it2++) {
			it2->second = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		}

		return true;
	}
	return false;
}


string CProjectSettlementEx5::Description() {
	stringstream ss;
	double price = 0;
	ss << "工程名称 : " << m_name.GetBuffer() << ",  ";

	for (map<string, double>::iterator it = m_mapProjectWorkload.begin(); it != m_mapProjectWorkload.end(); it++) {
		double p = it->second * m_mapProjectUnitPrice[it->first] / 10000;
		price += p;
	}
	price += m_daywork_labor;
	price += m_estimate_engineering;
	price += m_general_constracting_service_fee;
	price += m_provisional_sum;
	price += m_total_measure;
	price += m_unit_measure;
	price = price * (1 + m_regulation_rate);
	ss << "工程总造价（不含税）（万元）: " << Double2String(price) << ",  ";
	price = price * (1 + m_tax_rate);
	ss << "工程总造价（含税）（万元）: " << Double2String(price) << ",  ";

	/* 统计实际工程量 */
	map<string, double> mapProjectWorkload = m_mapProjectWorkload;
	for (map<string, double>::iterator it = mapProjectWorkload.begin(); it != mapProjectWorkload.end(); it++) {
		it->second = 0;
	}
	for (int i = 0; i < m_objs.size(); i++) {
		if (m_objs[i]->m_scheme == "按工程量结算款") {
			string proj = m_objs[i]->m_name.GetBuffer();
			mapProjectWorkload[proj] += m_objs[i]->ProjectPrice();
		}
	}
	for (map<string, double>::iterator it = mapProjectWorkload.begin(); it != mapProjectWorkload.end(); it++) {
		ss << it->first <<  "实际工程量 : " << Double2String(it->second) << ", " ;
	}

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

	/* 获取所有的分项工程 */
	map<string, double> mapProjectUnitPrice;
	map<string, double> mapProjectWorkload;
	for (int i = 0; i < m_objs.size(); i++) {
		if (m_objs[i]->m_scheme == "按工程量结算款") {
			string proj = m_objs[i]->m_name.GetBuffer();
			if (mapProjectUnitPrice.count(proj) == 0) {
				mapProjectUnitPrice[proj] = 0;
				mapProjectWorkload[proj] = 0;
			}
		}
	}
	for (map<string, double>::iterator it = mapProjectUnitPrice.begin(); it != mapProjectUnitPrice.end(); it++) {
		if (m_mapProjectUnitPrice.count(it->first) > 0)
			it->second = m_mapProjectUnitPrice[it->first];
	}
	for (map<string, double>::iterator it = mapProjectWorkload.begin(); it != mapProjectWorkload.end(); it++) {
		if (m_mapProjectWorkload.count(it->first) > 0)
			it->second = m_mapProjectWorkload[it->first];
	}
	m_mapProjectUnitPrice = mapProjectUnitPrice;
	m_mapProjectWorkload = mapProjectWorkload;
}

void CProjectSettlementEx5::Calculate() 
{
	bool bOK = true;
	for (map<string, double>::iterator it = m_mapProjectUnitPrice.begin(); it != m_mapProjectUnitPrice.end(); it++) {
		if (it->second == 0 ) bOK = false;
	}
	if (!bOK) {
		AfxMessageBox("请为分项工程设置‘单价’");
		return;
	}

	for (map<string, double>::iterator it = m_mapProjectWorkload.begin(); it != m_mapProjectWorkload.end(); it++) {
		if (it->second == 0) bOK = false;
	}
	if (!bOK) {
		AfxMessageBox("请为分项工程设置‘工程量’");
		return;
	}

	/* 记录施工月份的进度款 */
	map<string, double> mapMonth;
	for (int i = 0; i < m_objs.size(); i++) {
		string month = m_objs[i]->m_month.GetBuffer();
		if (mapMonth.count(month) == 0) {
			mapMonth[month] = 0;
		}
	}

	/* 记录当前月的所有分项工程的累计工程量 */
	map<string, double> mapWorkload = m_mapProjectWorkload;
	for (map<string, double>::iterator it = mapWorkload.begin(); it != mapWorkload.end(); it++) {
		it->second = 0;
	}

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("月份");
	gridDlg.m_vecHeader.push_back("进度款(万元)");

	/* 逐月汇总计算 */
	map<string, double>::iterator it = mapMonth.begin();
	for (; it != mapMonth.end(); it++) {
		string month = it->first;
		string desc;
		for (int i = 0; i < m_objs.size(); i++) {
			if (m_objs[i]->m_month.GetBuffer() == month) {
				double price;
				if (m_objs[i]->m_scheme == "按工程量结算款") {
					string proj = m_objs[i]->m_name.GetBuffer();
					CProjectSettlementEx5ObjA * pa = (CProjectSettlementEx5ObjA *)m_objs[i];
					/* 按工程量计算进度款 */
					if (mapWorkload[proj] > m_mapProjectWorkload[proj] * 1.15) {
						price = m_mapProjectUnitPrice[proj] * 0.9 * pa->m_actual_workload;
					}
					else if (mapWorkload[proj] + pa->m_actual_workload > m_mapProjectWorkload[proj] * 1.15) {
						price = (m_mapProjectWorkload[proj] * 1.15 - mapWorkload[proj]) * m_mapProjectUnitPrice[proj];
						price += (mapWorkload[proj] + pa->m_actual_workload - m_mapProjectWorkload[proj] * 1.15)  * m_mapProjectUnitPrice[proj] * 0.9;
					}
					else {
						price = m_mapProjectUnitPrice[proj] * pa->m_actual_workload;
					}
					mapWorkload[proj] += pa->m_actual_workload;
					
					/* 材料费变动款 */
					price += pa->m_material_change;
					/* 人工费变动款 */
					price += pa->m_people_change;

					price = price / 10000;
					
				}
				else {
					price = m_objs[i]->ProjectPrice();
				}
				if (!desc.empty())	desc += " , ";
				desc += m_objs[i]->m_name.GetBuffer() + string(" : ") + Double2String(price, "%.3f");
				/* 汇总 每月的工程款 （不含 规费和税金） */
				mapMonth[month] += price;
			}
		}
		if (!desc.empty())	desc += " , ";
		desc += "已完工程款 ： " + Double2String(mapMonth[month] * (1+m_regulation_rate) * (1+ m_tax_rate), "%.3f");

		vector<string> vec;
		vec.push_back(month);
		vec.push_back(desc);
		gridDlg.m_vecData.push_back(vec);
	}
	gridDlg.DoModal();
}

/* 计算人工费 、材料费等的调整款 */
void CProjectSettlementEx5::Adjust() {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;

	int g = 0;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("材料量/劳动量"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("单价调整额(元)"), 64);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("管理费率（%）"), 64);
	if (m_manage_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_manage_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("利润率（%）"), 64);
	if (m_net_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_net_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

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

	infd.Init(_T("调整款计算 参数设置"), _T("调整款计算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		g = 0;
		double amount = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		double unit_price = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

		double manage_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		double net_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		double regulation_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		double tax_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;

		double price = amount * unit_price * (1 + manage_rate) * (1 + net_rate) ;
		double price1 = price * (1 + regulation_rate) * (1 + tax_rate);
		string msg = "调整款(不含规费和税金)(元） : " + Double2String(price, "%.2f");
		msg += ",   调整款(元） : " + Double2String(price1, "%.2f");
		AfxMessageBox(msg.c_str());
	}
}

bool CProjectSettlementEx5::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;
	
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

			SortByMonth();
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

	if (nRow > 0 && nRow <= m_objs.size()) {
		bool res =  m_objs[nRow - 1]->CreateOrUpdate(menuCode, this);
		SortByMonth();
		return res;
	}
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
		SortByMonth();
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


bool CProjectSettlementEx5::Calculate(string menuCode, int nRow,  vector<CProjectSettlementEx5*>& cols) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		cols[nRow - 1]->Calculate();
	}
	return true;
}

bool CProjectSettlementEx5::Adjust(string menuCode, int nRow, vector<CProjectSettlementEx5*>& cols) {
	if (menuCode != CProjectSettlementEx5::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		cols[nRow - 1]->Adjust();
	}
	return true;
}

/***********************************************************************************/

