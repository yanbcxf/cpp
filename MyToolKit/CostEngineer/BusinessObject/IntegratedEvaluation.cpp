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


bool CIntegratedEvaluationObj::CreateOrUpdate(string menuCode, CIntegratedEvaluation& parent) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.GROUP_NUM_PER_LINE = 3;
	string method = string(parent.m_evaluation_method.GetBuffer());

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("投标人"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	if (method == "案例10") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 得分"), 64);
		if (m_technique_score > 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.0f", m_technique_score);
		infd.m_vecFindItem[0][i][0].nMin = 1;
		infd.m_vecFindItem[0][i][0].nMax = 10000;
	}

	if (method == "案例12") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 施工方案"), 64);
		if (m_technique_building_plan > 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_building_plan);
		infd.m_vecFindItem[0][i][0].dbMin = 1;
		infd.m_vecFindItem[0][i][0].dbMax = 15;
	}
	

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
	

	if (method == "案例12") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 项目管理班"), 64);
		if (m_technique_manage >= 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_manage);
		infd.m_vecFindItem[0][i][0].dbMin = 0;
		infd.m_vecFindItem[0][i][0].dbMax = 6;
	}
	

	if (method == "案例12") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("技术标 - 企业信誉"), 64);
		if (m_technique_credit >= 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.1f", m_technique_credit);
		infd.m_vecFindItem[0][i][0].dbMin = 0;
		infd.m_vecFindItem[0][i][0].dbMax = 5;
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
	
	if (method == "案例10") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 得分(除总价外)"), 64);
		if (m_business_score >= 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_score);
		infd.m_vecFindItem[0][i][0].dbMin = 0;
		infd.m_vecFindItem[0][i][0].dbMax = 10000;
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
	

	if (method == "案例10") {
		i++;
		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
		memcpy(infd.m_vecFindItem[0][i][0].caption, _T("商务标 - 单价"), 64);
		if (m_business_unit_price > 0)
			infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_business_unit_price);
		infd.m_vecFindItem[0][i][0].dbMin = 1;
		infd.m_vecFindItem[0][i][0].dbMax = 10000;
	}
	

	
	infd.Init(_T("工程支付 参数设置"), _T("工程支付 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;

		if (method == "案例10")
			m_technique_score = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例12")
			m_technique_building_plan = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例11" || method == "案例12")
			m_technique_duration = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例11" || method == "案例12")
			m_technique_quality = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例12") 
			m_technique_manage = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例12") 
			m_technique_credit = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例11") 
			m_technique_luban = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例11") 
			m_technique_shengyou = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		if (method == "案例10")
			m_business_score = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例10" || method == "案例11" || method == "案例12")
			m_business_total_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (method == "案例10") 
			m_business_unit_price = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
				
		return true;
	}
	return false;
}


bool CIntegratedEvaluationObj::Draw(CGridCtrl* pGridCtrl, vector<CIntegratedEvaluationObj>& cols, CIntegratedEvaluation& parent)
{
	if (!pGridCtrl)
		return false;

	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	if (parent.m_evaluation_method.GetBuffer() == string("案例11")) {
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

		for (CIntegratedEvaluationObj e : cols) {
			vector<string> vec;
			vec.push_back(e.m_name.GetBuffer());
			vec.push_back(Double2String(e.m_technique_duration, "%.0f"));
			vec.push_back(e.m_technique_quality==1?"合格":"省优");
			vec.push_back(Double2String(e.m_technique_luban, "%.0f"));
			vec.push_back(Double2String(e.m_technique_shengyou, "%.0f"));

			vec.push_back(Double2String(e.m_business_total_price, "%.2f"));

			/* 默认值为废标 */
			double technique = 0;
			double business = 0;
			if (e.m_technique_duration <= 36 && e.m_business_total_price <= 36500) {
				technique = 4 + (36 - e.m_technique_duration);
				if (e.m_technique_quality == 1 /* 合格 */) technique += 4;
				else if (e.m_technique_quality == 2 /* 省优 */) technique += 6;

				technique += e.m_technique_luban * 2;
				technique += e.m_technique_shengyou;
				technique += 20;		/* 施工方案均得分 */

				/* 计算评标基准价 */
				double basePrice = 0;
				int num = 0;
				for (CIntegratedEvaluationObj o : cols) {
					if (o.m_business_total_price > 36500) continue;
					num++;
					basePrice += o.m_business_total_price;
				}
				if (num > 0) {
					basePrice = basePrice / num ;
					double s = e.m_business_total_price / basePrice * 100;
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
	} 
	else if (parent.m_evaluation_method.GetBuffer() == string("案例12")) {
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

		for (CIntegratedEvaluationObj e : cols) {
			vector<string> vec;
			vec.push_back(e.m_name.GetBuffer());
			vec.push_back(Double2String(e.m_technique_building_plan, "%.1f"));
			vec.push_back(Double2String(e.m_technique_duration, "%.1f"));
			vec.push_back(Double2String(e.m_technique_quality, "%.1f"));
			vec.push_back(Double2String(e.m_technique_manage, "%.1f"));
			vec.push_back(Double2String(e.m_technique_credit, "%.1f"));

			vec.push_back(Double2String(e.m_business_total_price, "%.2f"));

			/* 默认值为废标 */
			double business = 0;
			double technique = 0;
			technique += e.m_technique_building_plan;
			technique += e.m_technique_duration;
			technique += e.m_technique_quality;
			technique += e.m_technique_manage;
			technique += e.m_technique_credit;

			if (technique >= 28 ) {
				/* 确定技术标 废标者 */
				vector<string>  invalid;
				for (CIntegratedEvaluationObj o : cols) {
					double t = 0;
					t += o.m_technique_building_plan;
					t += o.m_technique_duration;
					t += o.m_technique_quality;
					t += o.m_technique_manage;
					t += o.m_technique_credit;
					if (t < 28)
						invalid.push_back(o.m_name.GetBuffer());
				}

				/* 确定最高价 */
				double first_highest = 0;
				double second_highest = 0;
				string high_name = "";
				for (CIntegratedEvaluationObj o : cols) {
					bool bNotFound = true;
					for (string inv : invalid) {
						if (inv == string(o.m_name.GetBuffer())) {
							bNotFound = false;
							break;
						}
					}
					if (bNotFound == false) continue;

					if (o.m_business_total_price > first_highest) {
						second_highest = first_highest;
						first_highest = o.m_business_total_price;
						high_name = string(o.m_name.GetBuffer());
					}
					else if (o.m_business_total_price > second_highest) {
						second_highest = o.m_business_total_price;
					}
				}
				if ((first_highest - second_highest) / second_highest <= 0.15)
					high_name = "";

				/* 确定最低价 */
				double first_lowest = 100000000;
				double second_lowest = 100000000;
				string low_name = "";
				for (CIntegratedEvaluationObj o : cols) {
					bool bNotFound = true;
					for (string inv : invalid) {
						if (inv == string(o.m_name.GetBuffer())) {
							bNotFound = false;
							break;
						}
					}
					if (bNotFound == false) continue;

					if (o.m_business_total_price < first_lowest) {
						second_lowest = first_lowest;
						first_lowest = o.m_business_total_price;
						low_name = string(o.m_name.GetBuffer());
					}
					else if (o.m_business_total_price < second_lowest) {
						second_lowest = o.m_business_total_price;
					}
				}
				if ((second_lowest - first_lowest) / second_lowest <= 0.15)
					low_name = "";

				if (low_name == string(e.m_name.GetBuffer()) || high_name == string(e.m_name.GetBuffer())) {
					business = 15;
				}
				else {
					
					if (!low_name.empty()) invalid.push_back(low_name);
					if (!high_name.empty()) invalid.push_back(high_name);

					/* 计算基准价 */
					int num = 0;
					double basePrice = 0;
					for (CIntegratedEvaluationObj o : cols) {
						bool bNotFound = true;
						for (string inv : invalid) {
							if (inv == string(o.m_name.GetBuffer())) {
								bNotFound = false;
								break;
							}
						}
						if (bNotFound) {
							num++;
							basePrice += o.m_business_total_price;
						}
					}
					if (num > 0) {
						basePrice = basePrice / num;
						// basePrice = 13790.0 / 2 + basePrice / 2;

						double s = e.m_business_total_price / basePrice * 100;
						if (s < 100) {
							s = ((int)((100 - s) * 100 + 0.5))/100.0;
							business = 60 - s;
						}
						else {
							s = ((int)((s - 100) *100 + 0.5))/100.0;
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
	}
	else {
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

		for (CIntegratedEvaluationObj e : cols) {
			vector<string> vec;
			vec.push_back(e.m_name.GetBuffer());
			vec.push_back(Double2String(e.m_technique_score, "%.0f"));
			vec.push_back(Double2String(e.m_technique_building_plan, "%.0f"));
			vec.push_back(Double2String(e.m_technique_duration, "%.0f"));
			vec.push_back(Double2String(e.m_technique_quality, "%.0f"));
			vec.push_back(Double2String(e.m_technique_manage, "%.0f"));
			vec.push_back(Double2String(e.m_technique_credit, "%.0f"));
			vec.push_back(Double2String(e.m_technique_luban, "%.0f"));
			vec.push_back(Double2String(e.m_technique_shengyou, "%.0f"));

			vec.push_back(Double2String(e.m_business_score, "%.0f"));
			vec.push_back(Double2String(e.m_business_total_price, "%.2f"));
			vec.push_back(Double2String(e.m_business_unit_price, "%.2f"));

			vec.push_back("修改（update）");
			vec.push_back("删除（delete）");

			vecData.push_back(vec);
		}
	}
		
	try {
		pGridCtrl->SetRowCount(vecData.size() + 1);
		pGridCtrl->SetColumnCount(vecHeader.size());	//	额外增加三列 ： 修改/删除
		pGridCtrl->SetFixedRowCount(1);
		pGridCtrl->SetFixedColumnCount(1);
		pGridCtrl->SetHeaderSort(FALSE);
		pGridCtrl->SetEditable(FALSE);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return TRUE;
	}

	for (int row = 0; row < pGridCtrl->GetRowCount(); row++)
	{
		for (int col = 0; col < pGridCtrl->GetColumnCount(); col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < 1) {
				Item.nFormat = DT_LEFT | DT_WORDBREAK;

				Item.strText.Format(_T("%s"), vecHeader[col].c_str());
			}
			else
			{
				if (col == 0)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col < vecHeader.size() - 2)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return TRUE;
				}

				if (col == vecHeader.size() - 2 ) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == vecHeader.size() - 1) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				Item.strText.Format(_T("%s"), vecData[row - 1][col].c_str());
			}

			pGridCtrl->SetItem(&Item);
		}

	}

	pGridCtrl->ExpandColumnsToFit();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



bool CIntegratedEvaluationObj::Update(string menuCode, int nRow, vector<CIntegratedEvaluationObj>& cols, CIntegratedEvaluation& parent) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(menuCode, parent);
	return false;
}


bool CIntegratedEvaluationObj::Delete(string menuCode, int nRow, vector<CIntegratedEvaluationObj>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CIntegratedEvaluationObj>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
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
}

unsigned int CIntegratedEvaluation::PopupMenuId(string menuCode) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}


void CIntegratedEvaluation::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_evaluation_method;
		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			m_objs[i].Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_evaluation_method;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CIntegratedEvaluationObj bs;
			bs.Serialize(ar, version);
			m_objs.push_back(bs);
		}
	}
}

bool CIntegratedEvaluation::CreateOrUpdate(string menuCode) {

	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
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
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "案例10;案例11;案例12";
	infd.m_vecFindItem[0][i][0].strItem = "案例10";
	if (!m_evaluation_method.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_evaluation_method;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("评标方法"), 64);

	
	infd.Init(_T("评标法 参数设置"), _T("评标法 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_evaluation_method = infd.m_vecFindItem[0][i++][0].strItem;

		return true;
	}
	return false;
}


bool CIntegratedEvaluation::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CIntegratedEvaluation>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(2 + 4);		//	额外增加4列 ： 序号/修改/删除/增加
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
				if (col == 0)	val = "现金流量表";
				else if (col == 1)	val = "名称";
				else if (col == 2)	val = "评标方法";
				
				else if (col == 3)	val = "";
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

				if (col >= 2 && col <= 2 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 3 || col == 5) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 4) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = cols[row - 1].m_evaluation_method.GetBuffer();
				else if (col == 3)	val = "修改（update）";
				else if (col == 4)	val = "删除（delete）";
				else if (col == 5)	val = "增加（create）";

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

bool CIntegratedEvaluation::Update(string menuCode, int nRow, vector<CIntegratedEvaluation>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CIntegratedEvaluation::m_ObjectCode);
	return false;
}

bool CIntegratedEvaluation::Delete(string menuCode, int nRow, vector<CIntegratedEvaluation>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CIntegratedEvaluation>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}



void CIntegratedEvaluation::Calculate(string menuCode, vector<CIntegratedEvaluation>& cols) {
	if (menuCode != CIntegratedEvaluation::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("单方造价");
	

	for (int i = 0; i < cols.size(); i++)
	{
		CIntegratedEvaluation seb = cols[i];

		string str = seb.m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		
			
		/*vec.push_back(Double2String(total));
		gridDlg.m_vecData.push_back(vec);*/
	}
	gridDlg.DoModal();
}

