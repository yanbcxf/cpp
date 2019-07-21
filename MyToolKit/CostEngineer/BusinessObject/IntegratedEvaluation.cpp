#include "stdafx.h"
#include "IntegratedEvaluation.h"
#include "GridDlg.h"


CIntegratedEvaluationObj::CIntegratedEvaluationObj()
{
	m_name = "";
	m_technique_score = 0;
	m_technique_building_plan = 0;
	m_technique_duration = 0;
	m_technique_quality = 0;
	m_technique_manage = 0;
	m_technique_credit = 0;
	m_technique_luban = 0;
	m_technique_shengyou = 0;

	m_business_score = 0;
	m_business_total_price = 0;
	m_business_unit_price = 0;
}


CIntegratedEvaluationObj::~CIntegratedEvaluationObj()
{
}



void CIntegratedEvaluationObj::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_technique_score;
		ar << m_technique_building_plan;
		ar << m_technique_duration;
		ar << m_technique_quality;
		ar << m_technique_manage;
		ar << m_technique_credit;
		ar << m_technique_luban;
		ar << m_technique_shengyou;

		ar << m_business_score;
		ar << m_business_total_price;
		ar << m_business_unit_price;
	}
	else {
		ar >> m_name;
		ar >> m_technique_score;
		ar >> m_technique_building_plan;
		ar >> m_technique_duration;
		ar >> m_technique_quality;
		ar >> m_technique_manage;
		ar >> m_technique_credit;
		ar >> m_technique_luban;
		ar >> m_technique_shengyou;

		ar >> m_business_score;
		ar >> m_business_total_price;
		ar >> m_business_unit_price;
	}
}


bool CIntegratedEvaluationObj::CreateOrUpdate(string menuCode, CIntegratedEvaluation * parent) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	
	return false;
}

int CIntegratedEvaluationObj::EarliestPaymentTime() {

	return 0;
}

int CIntegratedEvaluationObj::LatestPaymentTime() {
	return 0;
}


/***********************************************************************************/


/* 招投标 - 综合评估法 */
string CIntegratedEvaluation::m_ObjectCode = "01041001";
double CIntegratedEvaluation::m_ObjectVersion = 1.0;

CIntegratedEvaluation::CIntegratedEvaluation()
{
	m_name = "";
	m_evaluation_method = "";
}


CIntegratedEvaluation::~CIntegratedEvaluation()
{
	for (CIntegratedEvaluationObj* e : m_objs)
		delete e;
}

unsigned int CIntegratedEvaluation::PopupMenuId(string menuCode) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}


CIntegratedEvaluation* CIntegratedEvaluation::NewParent(CString method) {
	CIntegratedEvaluation* p = NULL;
	if (method == "案例10")
		p = new CIntegratedEvaluationEx10();
	else if (method == "案例11")
		p = new CIntegratedEvaluationEx11();
	else if (method == "案例12")
		p = new CIntegratedEvaluationEx12();

	return p;
}

void CIntegratedEvaluation::Serialize(CArchive& ar, double version, CIntegratedEvaluation* & p) {
	if (ar.IsStoring()) {
		if (p) {
			ar << p->m_name;
			ar << p->m_evaluation_method;
			ar << p->m_objs.size();
			for (int i = 0; i < p->m_objs.size(); i++) {
				p->m_objs[i]->Serialize(ar, version);
			}
		}
	}
	else {
		CString name, method;
		ar >> name;
		ar >> method;
		p = NewParent(method);
		p->m_name = name;
		p->m_evaluation_method = method;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CIntegratedEvaluationObj* bs = p->NewChild();
			bs->Serialize(ar, version);
			p->m_objs.push_back(bs);
		}
	}
}

bool CIntegratedEvaluation::CreateOrUpdate(string strMenuCode, CIntegratedEvaluation* & p) {

	if (strMenuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("名称"), 64);
	if (p && !p->m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = p->m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	if(!p)
		infd.m_vecFindItem[0][i][0].strData = "案例10;案例11;案例12";
	else 
		infd.m_vecFindItem[0][i][0].strData = p->m_evaluation_method;
	infd.m_vecFindItem[0][i][0].strItem = "案例10";
	if (p && !p->m_evaluation_method.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = p->m_evaluation_method;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("评标方法"), 64);

	
	infd.Init(_T("评标法 参数设置"), _T("评标法 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		CString name, method;
		name = infd.m_vecFindItem[0][i++][0].strItem;
		method = infd.m_vecFindItem[0][i++][0].strItem;

		if (!p) p = NewParent(method);
		p->m_name = name;
		p->m_evaluation_method = method;

		return true;
	}
	return false;
}


bool CIntegratedEvaluation::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CIntegratedEvaluation *>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	vector<string> vecHeader;
	vector<vector<string>> vecData;
	vecHeader.push_back("现金流量表");
	vecHeader.push_back("名称");
	vecHeader.push_back("评标方法");
	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");

	int i = 1;
	for (CIntegratedEvaluation* e : cols) {
		vector<string> vec;
		vec.push_back(Int2String(i++));
		vec.push_back(e->m_name.GetBuffer());
		vec.push_back(e->m_evaluation_method.GetBuffer());
		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");
		vec.push_back("增加（create）");
		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

bool CIntegratedEvaluation::Update(string menuCode, int nRow, vector<CIntegratedEvaluation*>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return CIntegratedEvaluation::CreateOrUpdate(CIntegratedEvaluation::m_ObjectCode, cols[nRow - 1]);
	return false;
}

bool CIntegratedEvaluation::Delete(string menuCode, int nRow, vector<CIntegratedEvaluation*>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CIntegratedEvaluation*>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}



void CIntegratedEvaluation::Calculate(string menuCode, vector<CIntegratedEvaluation *>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("单方造价");
	

	for (int i = 0; i < cols.size(); i++)
	{
		CIntegratedEvaluation * seb = cols[i];

		string str = seb->m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		
			
		/*vec.push_back(Double2String(total));
		gridDlg.m_vecData.push_back(vec);*/
	}
	gridDlg.DoModal();
}



bool CIntegratedEvaluation::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}


CIntegratedEvaluationObj* CIntegratedEvaluation::NewChild() {
	CIntegratedEvaluationObj* p = new CIntegratedEvaluationObj();
	return p;
}

bool CIntegratedEvaluation::AddChild(string menuCode) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	CIntegratedEvaluationObj* c = NewChild();
	if (c->CreateOrUpdate(menuCode, this)) {
		m_objs.push_back(c);
		return true;
	}

	return false;
}

bool CIntegratedEvaluation::UpdateChild(string menuCode, int nRow) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size())
		return m_objs[nRow - 1]->CreateOrUpdate(menuCode, this);
	return false;
}


bool CIntegratedEvaluation::DeleteChild(string menuCode, int nRow) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		int idx = 0;
		vector<CIntegratedEvaluationObj*>::iterator it = m_objs.begin();
		for (; it != m_objs.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		m_objs.erase(it);
		return true;
	}
	return false;
}

/**************************************************************************************/


CIntegratedEvaluationObj* CIntegratedEvaluationEx10::NewChild() {
	CIntegratedEvaluationObj* p = new CIntegratedEvaluationObjEx10();
	return p;
}

CIntegratedEvaluationObj* CIntegratedEvaluationEx11::NewChild() {
	CIntegratedEvaluationObj* p = new CIntegratedEvaluationObjEx11();
	return p;
}

CIntegratedEvaluationObj* CIntegratedEvaluationEx12::NewChild() {
	CIntegratedEvaluationObj* p = new CIntegratedEvaluationObjEx12();
	return p;
}



bool CIntegratedEvaluationEx10::DrawChild(CGridCtrl* pGridCtrl) {
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("投标人");
	vecHeader.push_back("技术标得分");
	vecHeader.push_back("施工方案");
	vecHeader.push_back("工期");
	vecHeader.push_back("质量");
	vecHeader.push_back("项目管理班");
	vecHeader.push_back("企业信誉");
	vecHeader.push_back("鲁班工程奖");
	vecHeader.push_back("省优工程奖");

	vecHeader.push_back("商务标得分(除总价外)");
	vecHeader.push_back("总价");
	vecHeader.push_back("单价");
	vecHeader.push_back("");
	vecHeader.push_back("");

	for (CIntegratedEvaluationObj* e : m_objs) {
		vector<string> vec;
		vec.push_back(e->m_name.GetBuffer());
		vec.push_back(Double2String(e->m_technique_score, "%.0f"));
		vec.push_back(Double2String(e->m_technique_building_plan, "%.0f"));
		vec.push_back(Double2String(e->m_technique_duration, "%.0f"));
		vec.push_back(Double2String(e->m_technique_quality, "%.0f"));
		vec.push_back(Double2String(e->m_technique_manage, "%.0f"));
		vec.push_back(Double2String(e->m_technique_credit, "%.0f"));
		vec.push_back(Double2String(e->m_technique_luban, "%.0f"));
		vec.push_back(Double2String(e->m_technique_shengyou, "%.0f"));

		vec.push_back(Double2String(e->m_business_score, "%.0f"));
		vec.push_back(Double2String(e->m_business_total_price, "%.2f"));
		vec.push_back(Double2String(e->m_business_unit_price, "%.2f"));

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}


bool CIntegratedEvaluationEx11::DrawChild(CGridCtrl* pGridCtrl) {
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("投标人");
	vecHeader.push_back("工期");
	vecHeader.push_back("质量");
	vecHeader.push_back("鲁班工程奖");
	vecHeader.push_back("省优工程奖");

	vecHeader.push_back("总价");

	vecHeader.push_back("技术标结果分");
	vecHeader.push_back("商务标结果分");
	vecHeader.push_back("结果总分");
	vecHeader.push_back("");
	vecHeader.push_back("");

	for (CIntegratedEvaluationObj* e : m_objs) {
		vector<string> vec;
		vec.push_back(e->m_name.GetBuffer());
		vec.push_back(Double2String(e->m_technique_duration, "%.0f"));
		vec.push_back(e->m_technique_quality == 1 ? "合格" : "省优");
		vec.push_back(Double2String(e->m_technique_luban, "%.0f"));
		vec.push_back(Double2String(e->m_technique_shengyou, "%.0f"));

		vec.push_back(Double2String(e->m_business_total_price, "%.2f"));

		/* 默认值为废标 */
		double technique = 0;
		double business = 0;
		if (e->m_technique_duration <= 36 && e->m_business_total_price <= 36500) {
			technique = 4 + (36 - e->m_technique_duration);
			if (e->m_technique_quality == 1 /* 合格 */) technique += 4;
			else if (e->m_technique_quality == 2 /* 省优 */) technique += 6;

			technique += e->m_technique_luban * 2;
			technique += e->m_technique_shengyou;
			technique += 20;		/* 施工方案均得分 */

			/* 计算评标基准价 */
			double basePrice = 0;
			int num = 0;
			for (CIntegratedEvaluationObj* o : m_objs) {
				if (o->m_business_total_price > 36500) continue;
				num++;
				basePrice += o->m_business_total_price;
			}
			if (num > 0) {
				basePrice = basePrice / num;
				double s = e->m_business_total_price / basePrice * 100;
				if (s < 98) {
					int ss = (98 - s) + 0.5;
					business = 60 - ss;
				}
				else {
					int ss = s - 98 + 0.5;
					business = 60 - ss * 2;
				}
			}
		}

		vec.push_back(Double2String(technique, "%.0f"));
		vec.push_back(Double2String(business, "%.0f"));
		vec.push_back(Double2String(technique + business, "%.0f"));

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}


bool CIntegratedEvaluationEx12::DrawChild(CGridCtrl* pGridCtrl) {
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("投标人");
	vecHeader.push_back("施工方案");
	vecHeader.push_back("工期");
	vecHeader.push_back("质量");
	vecHeader.push_back("项目管理班");
	vecHeader.push_back("企业信誉");

	vecHeader.push_back("总价");

	vecHeader.push_back("技术标结果分");
	vecHeader.push_back("商务标结果分");
	vecHeader.push_back("结果总分");
	vecHeader.push_back("");
	vecHeader.push_back("");

	for (CIntegratedEvaluationObj* e : m_objs) {
		vector<string> vec;
		vec.push_back(e->m_name.GetBuffer());
		vec.push_back(Double2String(e->m_technique_building_plan, "%.1f"));
		vec.push_back(Double2String(e->m_technique_duration, "%.1f"));
		vec.push_back(Double2String(e->m_technique_quality, "%.1f"));
		vec.push_back(Double2String(e->m_technique_manage, "%.1f"));
		vec.push_back(Double2String(e->m_technique_credit, "%.1f"));

		vec.push_back(Double2String(e->m_business_total_price, "%.2f"));

		/* 默认值为废标 */
		double business = 0;
		double technique = 0;
		technique += e->m_technique_building_plan;
		technique += e->m_technique_duration;
		technique += e->m_technique_quality;
		technique += e->m_technique_manage;
		technique += e->m_technique_credit;

		if (technique >= 28) {
			/* 确定技术标 废标者 */
			vector<string>  invalid;
			for (CIntegratedEvaluationObj* o : m_objs) {
				double t = 0;
				t += o->m_technique_building_plan;
				t += o->m_technique_duration;
				t += o->m_technique_quality;
				t += o->m_technique_manage;
				t += o->m_technique_credit;
				if (t < 28)
					invalid.push_back(o->m_name.GetBuffer());
			}

			/* 确定最高价 */
			double first_highest = 0;
			double second_highest = 0;
			string high_name = "";
			for (CIntegratedEvaluationObj* o : m_objs) {
				bool bNotFound = true;
				for (string inv : invalid) {
					if (inv == string(o->m_name.GetBuffer())) {
						bNotFound = false;
						break;
					}
				}
				if (bNotFound == false) continue;

				if (o->m_business_total_price > first_highest) {
					second_highest = first_highest;
					first_highest = o->m_business_total_price;
					high_name = string(o->m_name.GetBuffer());
				}
				else if (o->m_business_total_price > second_highest) {
					second_highest = o->m_business_total_price;
				}
			}
			if ((first_highest - second_highest) / second_highest <= 0.15)
				high_name = "";

			/* 确定最低价 */
			double first_lowest = 100000000;
			double second_lowest = 100000000;
			string low_name = "";
			for (CIntegratedEvaluationObj* o : m_objs) {
				bool bNotFound = true;
				for (string inv : invalid) {
					if (inv == string(o->m_name.GetBuffer())) {
						bNotFound = false;
						break;
					}
				}
				if (bNotFound == false) continue;

				if (o->m_business_total_price < first_lowest) {
					second_lowest = first_lowest;
					first_lowest = o->m_business_total_price;
					low_name = string(o->m_name.GetBuffer());
				}
				else if (o->m_business_total_price < second_lowest) {
					second_lowest = o->m_business_total_price;
				}
			}
			if ((second_lowest - first_lowest) / second_lowest <= 0.15)
				low_name = "";

			if (low_name == string(e->m_name.GetBuffer()) || high_name == string(e->m_name.GetBuffer())) {
				business = 15;
			}
			else {

				if (!low_name.empty()) invalid.push_back(low_name);
				if (!high_name.empty()) invalid.push_back(high_name);

				/* 计算基准价 */
				int num = 0;
				double basePrice = 0;
				for (CIntegratedEvaluationObj* o : m_objs) {
					bool bNotFound = true;
					for (string inv : invalid) {
						if (inv == string(o->m_name.GetBuffer())) {
							bNotFound = false;
							break;
						}
					}
					if (bNotFound) {
						num++;
						basePrice += o->m_business_total_price;
					}
				}
				if (num > 0) {
					basePrice = basePrice / num;
					// basePrice = 13790.0 / 2 + basePrice / 2;

					double s = e->m_business_total_price / basePrice * 100;
					if (s < 100) {
						s = ((int)((100 - s) * 100 + 0.5)) / 100.0;
						business = 60 - s;
					}
					else {
						s = ((int)((s - 100) * 100 + 0.5)) / 100.0;
						business = 60 - s * 2;
					}
				}

			}
		}
		else {
			technique = 0;
		}


		vec.push_back(Double2String(technique, "%.2f"));
		vec.push_back(Double2String(business, "%.2f"));
		vec.push_back(Double2String(technique + business, "%.2f"));

		vec.push_back("修改（update）");
		vec.push_back("删除（delete）");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

bool CIntegratedEvaluationObjEx10::CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.GROUP_NUM_PER_LINE = 3;
	string method = string(parent->m_evaluation_method.GetBuffer());

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("投标人"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 得分"), 64);
	if (m_technique_score > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.0f", m_technique_score);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 10000;


	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 得分(除总价外)"), 64);
	if (m_business_score >= 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_score);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 10000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 总价"), 64);
	if (m_business_total_price > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_total_price);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 10000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 单价"), 64);
	if (m_business_unit_price > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_unit_price);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 10000;

	infd.Init(_T("工程支付 参数设置"), _T("工程支付 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;

		m_technique_score = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_business_score = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_business_total_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_business_unit_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}

bool CIntegratedEvaluationObjEx11::CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.GROUP_NUM_PER_LINE = 3;
	string method = string(parent->m_evaluation_method.GetBuffer());

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("投标人"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;




	if (method == "案例11" || method == "案例12") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 工期"), 64);

		if (method == "案例11") {
			if (m_technique_duration > 0)
				infd.m_vecFindItem[0][i][0].strItem.Format("%.0f", m_technique_duration);
			infd.m_vecFindItem[0][i][0].nMin = 1;
			infd.m_vecFindItem[0][i][0].nMax = 10000;
		}
		if (method == "案例12") {
			if (m_technique_duration > 0)
				infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_duration);
			infd.m_vecFindItem[0][i][0].dbMin = 0;
			infd.m_vecFindItem[0][i][0].dbMax = 8;
		}
	}


	if (method == "案例11" || method == "案例12") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 质量"), 64);

		if (method == "案例11") {
			if (m_technique_quality > 0)
				infd.m_vecFindItem[0][i][0].strItem.Format("%.0f", m_technique_quality);
			infd.m_vecFindItem[0][i][0].nMin = 1;
			infd.m_vecFindItem[0][i][0].nMax = 2;
		}
		if (method == "案例12") {
			if (m_technique_quality > 0)
				infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_quality);
			infd.m_vecFindItem[0][i][0].dbMin = 0;
			infd.m_vecFindItem[0][i][0].dbMax = 6;
		}
	}


	if (method == "案例11") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 鲁班工程奖"), 64);
		if (m_technique_luban >= 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.0f", m_technique_luban);
		infd.m_vecFindItem[0][i][0].nMin = 0;
		infd.m_vecFindItem[0][i][0].nMax = 10000;
	}


	if (method == "案例11") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 省优工程奖"), 64);
		if (m_technique_shengyou >= 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.0f", m_technique_shengyou);
		infd.m_vecFindItem[0][i][0].nMin = 0;
		infd.m_vecFindItem[0][i][0].nMax = 10000;
	}


	if (method == "案例10" || method == "案例11" || method == "案例12") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 总价"), 64);
		if (m_business_total_price > 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_total_price);
		infd.m_vecFindItem[0][i][0].dbMin = 1;
		infd.m_vecFindItem[0][i][0].dbMax = 10000000;
	}



	infd.Init(_T("工程支付 参数设置"), _T("工程支付 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;

		m_technique_duration = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_technique_quality = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		m_technique_luban = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_technique_shengyou = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		m_business_total_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		
		return true;
	}
	return false;
}

bool CIntegratedEvaluationObjEx12::CreateOrUpdate(string menuCode, CIntegratedEvaluation* parent) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.GROUP_NUM_PER_LINE = 3;
	string method = string(parent->m_evaluation_method.GetBuffer());

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("投标人"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 施工方案"), 64);
	if (m_technique_building_plan > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_building_plan);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 15;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 工期"), 64);
	if (m_technique_duration > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_duration);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 8;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 质量"), 64);
	if (m_technique_quality > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_quality);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 6;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 项目管理班"), 64);
	if (m_technique_manage >= 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_manage);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 6;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 企业信誉"), 64);
	if (m_technique_credit >= 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_credit);
	infd.m_vecFindItem[0][i][0].dbMin = 0;
	infd.m_vecFindItem[0][i][0].dbMax = 5;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 总价"), 64);
	if (m_business_total_price > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_total_price);
	infd.m_vecFindItem[0][i][0].dbMin = 1;
	infd.m_vecFindItem[0][i][0].dbMax = 10000000;


	infd.Init(_T("工程支付 参数设置"), _T("工程支付 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;

		
		m_technique_building_plan = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_technique_duration = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_technique_quality = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_technique_manage = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_technique_credit = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());	
		m_business_total_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}