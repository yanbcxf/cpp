#include "stdafx.h"
#include "FinanceAnalysis.h"
#include "GridDlg.h"




//
//int CFinanceAnalysisObj::EarliestPaymentTime() {
//
//	return 0;
//}
//
//int CFinanceAnalysisObj::LatestPaymentTime() {
//	return 0;
//}




/***********************************************************************************/


/* 第六章 - 工程结算 */
string CFinanceAnalysis::m_ObjectCode = "01010401";
double CFinanceAnalysis::m_ObjectVersion = 1.0;


unsigned int CFinanceAnalysis::PopupMenuId(string menuCode) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

CFinanceAnalysis* CFinanceAnalysis::NewParent(CString scheme) {
	CFinanceAnalysis* p = NULL;
	if (scheme == "融资前")
		p = new CBeforeFinancing();
	if (scheme == "融资后")
		p = new CAfterFinancing();
	if (p) p->m_scheme = scheme;
	return p;
}


bool CFinanceAnalysis::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CFinanceAnalysis*>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	vector<string> vecHeader;
	vector<vector<string>> vecData;
	vecHeader.push_back("工程结算方案,120");
	vecHeader.push_back("工程描述,400");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");
	vecHeader.push_back(",120");
	vecHeader.push_back(",140");

	int i = 1;
	for (CFinanceAnalysis* e : cols) {
		vector<string> vec;
		vec.push_back(e->m_scheme.GetBuffer());
		vec.push_back(e->Description());
		vec.push_back("修改" + ActionType2String(ActionType::Update));
		vec.push_back("删除" + ActionType2String(ActionType::Delete));
		vec.push_back("增加" + ActionType2String(ActionType::Create));
		vec.push_back("计算" + ActionType2String(ActionType::Calculate));
		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

void CFinanceAnalysis::Serialize(CArchive& ar, double version, CFinanceAnalysis*  & p) {
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





void CFinanceAnalysis::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_discount;
		ar << m_net_residual_rate;
		ar << m_tax_surcharge_rate;
		ar << m_income_tax_rate;
		ar << m_tax_on_purchase;	

		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			ar << m_objs[i]->m_scheme;
			m_objs[i]->Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_discount;
		ar >> m_net_residual_rate;
		ar >> m_tax_surcharge_rate;
		ar >> m_income_tax_rate;
		ar >> m_tax_on_purchase;

		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CString scheme;
			ar >> scheme;
			CFinanceAnalysisObj* bs = NewChild(scheme);
			bs->Serialize(ar, version);
			m_objs.push_back(bs);
		}
		
		SortByMonth();
	}
}


bool CFinanceAnalysis::CreateOrUpdate() {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = m_scheme;	// 计算方案 不可以更改
	infd.m_vecFindItem[0][i][0].strItem = m_scheme;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("工程名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "直线法;双倍余额递减法";
	if (!m_discount.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_discount;
	else
		infd.m_vecFindItem[0][i][0].strItem = "直线法";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("折旧法"), 64);

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("期末净残值率（%）"), 64);
	if (m_net_residual_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_net_residual_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("税金附加费率（%）"), 64);
	if (m_tax_surcharge_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_tax_surcharge_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("所得税率（%）"), 64);
	if (m_income_tax_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_income_tax_rate * 100);
	infd.m_vecFindItem[0][i][0].dbMin = 0.01;
	infd.m_vecFindItem[0][i][0].dbMax = 100;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("建设期进项税额"), 64);
	if (m_tax_on_purchase > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_tax_on_purchase);
	infd.m_vecFindItem[0][i][0].dbMin = 0.00;
	infd.m_vecFindItem[0][i][0].dbMax = 100;


	infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		g = 0;
		infd.m_vecFindItem[g][i++][0].strItem;  // 计算方案 不可以更改
		m_name = infd.m_vecFindItem[g][i++][0].strItem;
		m_discount = infd.m_vecFindItem[g][i++][0].strItem;
		m_net_residual_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		m_tax_surcharge_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		m_income_tax_rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
		m_tax_on_purchase = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		return true;
	}
	return false;
}


string CFinanceAnalysis::Description() {
	stringstream ss;
	double price = 0;
	ss << "工程名称 : " << m_name.GetBuffer() << ",  ";

	return ss.str();
}

void CFinanceAnalysis::SortByMonth() {
	struct sort_deref
	{
		bool operator() (CFinanceAnalysisObj* lhs, CFinanceAnalysisObj* rhs)
		{
			int left = String2Double(lhs->m_month.GetBuffer());
			int right = String2Double(rhs->m_month.GetBuffer());
			if (left < right) return true;
			if (left == right) {
				string lscheme = lhs->m_scheme.GetBuffer();
				string rscheme = rhs->m_scheme.GetBuffer();
				bool lLoan = lscheme.find("借款") != string::npos;
				bool rLoan = rscheme.find("借款") != string::npos;
				bool lCost = lscheme.find("总成本费用") != string::npos;
				bool rCost = rscheme.find("总成本费用") != string::npos;
				bool lProfit = lscheme.find("利润及利润分配") != string::npos;
				bool rProfit = rscheme.find("利润及利润分配") != string::npos;
				bool lInput = lscheme.find("现金流入") != string::npos;
				bool rInput = rscheme.find("现金流入") != string::npos;
				bool lOutput = lscheme.find("现金流出") != string::npos;
				bool rOutput = rscheme.find("现金流出") != string::npos;
				if (lscheme != rscheme) {
					if (lLoan && rLoan) return lscheme < rscheme;
					else if (lLoan) return true;
					else if (rLoan) return false;
					else if (lCost) return true;
					else if (rCost) return false;
					else if (lInput) return true;
					else if (rInput) return false;
					else if (lOutput) return true;
					else if (rOutput) return false;
				}
				else {
					string lname = lhs->m_name.GetBuffer();
					string rname = rhs->m_name.GetBuffer();
					left = lname.find(".");
					right = rname.find(".");
					left = atoi(lname.substr(0, left).c_str());
					right = atoi(rname.substr(0, right).c_str());
					if (left < right) return true;
				}
				
			}
			return false;
		}
	};

	std::sort(m_objs.begin(), m_objs.end(), sort_deref());
}

void CFinanceAnalysis::Calculate() 
{
	bool bOK = true;
	map<int, double> mapMonth;
	map<int, double> mapMonthInput;
	map<int, double> mapMonthOutput;

	for (CFinanceAnalysisObj* e : m_objs) {
		int nMonth = String2Double(e->m_month.GetBuffer());
		if (e->m_scheme.Find("现金流入") >= 0) {
			if (mapMonthInput.count(nMonth) == 0) {
				mapMonthInput[nMonth] = e->AmountOfMoney();
			}
			else {
				mapMonthInput[nMonth] += e->AmountOfMoney();
			}
		}
		if (e->m_scheme.Find("现金流出") >= 0) {
			if (mapMonthOutput.count(nMonth) == 0) {
				mapMonthOutput[nMonth] = e->AmountOfMoney();
			}
			else {
				mapMonthOutput[nMonth] += e->AmountOfMoney();
			}
		}
		if (e->m_scheme.Find("现金") >= 0) {
			if (mapMonth.count(nMonth) == 0) {
				mapMonth[nMonth] = e->AmountOfMoney();
			}
			else {
				mapMonth[nMonth] += e->AmountOfMoney();
			}
		}
	}

	CGridDlg gridDlg;
	gridDlg.m_strTitle = "现金流量表";
	gridDlg.m_vecHeader.push_back("年度");
	gridDlg.m_vecHeader.push_back("现金流入(万元)");
	gridDlg.m_vecHeader.push_back("现金流出(万元)");
	gridDlg.m_vecHeader.push_back("净现金流量(万元)");
	gridDlg.m_vecHeader.push_back("折现净现金流量(万元)");
	gridDlg.m_vecHeader.push_back("累计款（静态回收期）(万元)");
	gridDlg.m_vecHeader.push_back("累计折现值(万元)");

	/* 逐月汇总计算 */
	double accStatic = 0;
	double accDynamic = 0;
	map<int, double>::iterator it = mapMonth.begin();
	for (; it != mapMonth.end(); it++) {
		int nMonth = it->first;
		double money = it->second;
		double input = mapMonthInput[nMonth];
		double output = mapMonthOutput[nMonth];
				
		vector<string> vec;
		vec.push_back(Int2String(nMonth));
		vec.push_back(Double2String(input, "%.3f"));
		vec.push_back(Double2String(output, "%.3f"));
		vec.push_back(Double2String(money, "%.3f"));


		double present = Future2Present(0.1, nMonth, 4) * money;
		vec.push_back(Double2String(present, "%.3f"));

		accStatic += money;
		vec.push_back(Double2String(accStatic, "%.3f"));

		accDynamic += present;
		vec.push_back(Double2String(accDynamic, "%.3f"));
		
		gridDlg.m_vecData.push_back(vec);
	}
	gridDlg.DoModal();
}


void CFinanceAnalysis::Adjust() {
	
}

bool CFinanceAnalysis::GetAmountOfMoney(CString month, CString scheme, CString name, double & amount) {
	for (CFinanceAnalysisObj * p : m_objs) {
		if (scheme.CompareNoCase(p->m_scheme) == 0) {
			if (String2Double(p->m_month.GetBuffer()) == String2Double(month.GetBuffer())
				&& p->m_name.Find(name) >= 0) {
				amount = p->m_amount_of_money;
				return true;
			}
		}
	}
	return false;
}



bool CFinanceAnalysis::AssistChild(string menuCode, int nRow) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		bool res = m_objs[nRow - 1]->Assist(this);
		SortByMonth();
		return res;
	}
	return false;
}

bool CFinanceAnalysis::CopyToChild(string menuCode, int nRow) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		bool res = m_objs[nRow - 1]->CopyTo(this);
		SortByMonth();
		return res;
	}
	return false;
}

bool CFinanceAnalysis::DrawChild(CGridCtrl* pGridCtrl)
{
	if (!pGridCtrl)
		return false;
	
	vector<string>			vecHeader;
	vector<vector<string>>	vecData;

	vecHeader.push_back("月份");
	vecHeader.push_back("款项类型");
	vecHeader.push_back("款项名称");
	vecHeader.push_back("描述");

	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");

	int ttt = m_objs.size();

	for (int i = 0; i < ttt; i++) {
		vector<string> vec;
		vec.push_back(m_objs[i]->m_month.GetBuffer());
		vec.push_back(m_objs[i]->m_scheme.GetBuffer());
		vec.push_back(m_objs[i]->m_name.GetBuffer());
		vec.push_back(m_objs[i]->Description());

		vec.push_back("修改" + ActionType2String(ActionType::Update));
		vec.push_back("删除" + ActionType2String(ActionType::Delete));
		vec.push_back("复制到" + ActionType2String(ActionType::CopyTo));
		if (m_objs[i]->HasAssist())	
			vec.push_back("辅助计算" + ActionType2String(ActionType::Assist));
		else
			vec.push_back("");

		vecData.push_back(vec);
	}

	return DrawGrid(pGridCtrl, vecHeader, vecData);
}



bool CFinanceAnalysis::AddChild(string menuCode) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "现金流出;现金流入;总成本费用;利润及利润分配;借款1;借款2;借款3;借款4;借款5";
	infd.m_vecFindItem[0][i][0].strItem = "现金流出";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	i++;
	string caption = "年度（生成所有选项）";
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, caption.c_str(), 64);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 100;

	infd.Init(_T("类型 方案选择"), _T("类型 方案选择"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		CString scheme = infd.m_vecFindItem[0][i++][0].strItem;
		int nMonth = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		CFinanceAnalysisObj* c = NewChild(scheme);
		if (nMonth <= 0) {
			if (c->CreateOrUpdate(menuCode, this)) {
				m_objs.push_back(c);

				SortByMonth();
				return true;
			}
			else {
				delete c;
			}
		}
		else {
			/* 添加该月份的所有选项 */
			c->m_month = Int2String(nMonth).c_str();
			vector<string> options = c->GetOptions();
			for (string e : options) {
				c->m_name = e.c_str();
				double amount;
				if (GetAmountOfMoney(c->m_month, c->m_scheme, c->m_name, amount) == false) {
					CFinanceAnalysisObj * p = c->Clone();
					m_objs.push_back(p);
				}
			}
			delete c;
			SortByMonth();
			return true;
		}
		
	}

	return false;
}

bool CFinanceAnalysis::UpdateChild(string menuCode, int nRow) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		bool res =  m_objs[nRow - 1]->CreateOrUpdate(menuCode, this);
		SortByMonth();
		return res;
	}
	return false;
}


bool CFinanceAnalysis::DeleteChild(string menuCode, int nRow) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= m_objs.size()) {
		int idx = 0;
		vector<CFinanceAnalysisObj*>::iterator it = m_objs.begin();
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
bool CFinanceAnalysis::Create(string strMenuCode, CFinanceAnalysis*  & p) {
	if (strMenuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "融资前;融资后";
	infd.m_vecFindItem[0][i][0].strItem = "融资前";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("计算方案"), 64);

	infd.Init(_T("财务分析 方案选择"), _T("财务分析 方案选择"));
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

bool CFinanceAnalysis::Update(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return  cols[nRow - 1]->CreateOrUpdate();
	return false;
}

bool CFinanceAnalysis::Delete(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CFinanceAnalysis*>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}


bool CFinanceAnalysis::Calculate(string menuCode, int nRow,  vector<CFinanceAnalysis*>& cols) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		cols[nRow - 1]->Calculate();
	}
	return true;
}

bool CFinanceAnalysis::Adjust(string menuCode, int nRow, vector<CFinanceAnalysis*>& cols) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		cols[nRow - 1]->Adjust();
	}
	return true;
}

/***********************************************************************************/

/* 截止到本月，累计应纳增值税 */
double CBeforeFinancing::AccumulativeTax(int nMonth) {
	double tax = 0;
	for (int i = 1; i <= nMonth; i++) {
		double amount;
		if (GetAmountOfMoney(Int2String(i).c_str(), "现金流入", "2.销项税额", amount)) {
			tax += amount;
		}
	}
	for (int i = 1; i <= nMonth; i++) {
		double amount;
		if (GetAmountOfMoney(Int2String(i).c_str(), "现金流出", "4.进项税额", amount)) {
			tax -= amount;
		}
	}
	tax -= m_tax_on_purchase;
	if (tax < 0) tax = 0;
	return tax;
}

/* 截止到本月，累计应纳增值税 */
double CAfterFinancing::AccumulativeTax(int nMonth) {
	double tax = 0;
	for (int i = 1; i <= nMonth; i++) {
		double amount;
		if (GetAmountOfMoney(Int2String(i).c_str(), "现金流入", "2.销项税额", amount)) {
			tax += amount;
		}
	}
	for (int i = 1; i <= nMonth; i++) {
		double amount;
		if (GetAmountOfMoney(Int2String(i).c_str(), "现金流出", "6.进项税额", amount)) {
			tax -= amount;
		}
	}
	tax -= m_tax_on_purchase;
	if (tax < 0) tax = 0;
	return tax;
}

/* 截止到本期初，贷款余额 */
double CAfterFinancing::LoanRemaining(int nMonth, int nLoan) {
	
	if (nLoan >5 || nLoan < 1 ) return 0;
	string loanScheme = "借款" + Int2String(nLoan);

	/* remain 保存截止前几期末的 贷款余额 */
	double remain = 0;
	for (int i = 1; i < nMonth; i++) {
		double amount;
		remain = remain * (1 + m_loan_rate[nLoan - 1]);
		if (GetAmountOfMoney(Int2String(i).c_str(), loanScheme.c_str(), "1.借入", amount)) {
			remain += amount;
			if (m_loan_borrow[nLoan - 1].CompareNoCase("期中")==0) {
				/* 新借入，本期利息减半 */
				remain += amount * 0.5 * m_loan_rate[nLoan - 1];
			}
			else  if (m_loan_borrow[nLoan - 1].CompareNoCase("期初") == 0) {
				remain += amount * m_loan_rate[nLoan - 1];
			} 
			
		}
		
		if (GetAmountOfMoney(Int2String(i).c_str(), loanScheme.c_str(), "2.还本", amount)) {
			remain -= amount;
		}

		if (GetAmountOfMoney(Int2String(i).c_str(), loanScheme.c_str(), "3.付息", amount)) {
			remain -= amount;
		}
	}
	
	return remain;
}

CFinanceAnalysisObj* CBeforeFinancing::NewChild(CString scheme) {
	CFinanceAnalysisObj* p = NULL;

	if (scheme == "现金流出")
		p = new CFinanceAnalysisObjA();
	if (scheme == "现金流入")
		p = new CFinanceAnalysisObjB();

	if (p) p->m_scheme = scheme;

	return p;
}

CFinanceAnalysisObj* CAfterFinancing::NewChild(CString scheme) {
	CFinanceAnalysisObj* p = NULL;

	if (scheme == "现金流出")
		p = new CFinanceAnalysisObjA1();
	else if (scheme == "现金流入")
		p = new CFinanceAnalysisObjB();
	else if (scheme.Find("借款") >= 0)
		p = new CFinanceAnalysisObjC();
	else if (scheme.Find("总成本费用") >= 0)
		p = new CFinanceAnalysisObjD();
	else if (scheme.Find("利润及利润分配") >= 0)
		p = new CFinanceAnalysisObjE();

	if (p) p->m_scheme = scheme;

	return p;
}

void CAfterFinancing::Serialize(CArchive& ar, double version) {
	CFinanceAnalysis::Serialize(ar, version);
	if (ar.IsStoring()) {
		for (int i = 0; i < 5; i++) {
			ar << m_loan_rate[i];
			ar << m_loan_borrow[i];
		}
	}
	else {
		for (int i = 0; i < 5; i++) {
			ar >> m_loan_rate[i];
			ar >> m_loan_borrow[i];
		}
	}
}

bool CAfterFinancing::CreateOrUpdate() {
	if (CFinanceAnalysis::CreateOrUpdate()) {
		CDyncItemGroupDlg infd;
		infd.CXCAPTION = 100;
		infd.GROUP_NUM_PER_LINE = 3;
		int g = 0;
		int i = 0;
		for (i = 0; i < 5; i++) {
			string caption = "借款" + Int2String(i+1) + " - 贷款利率（%）";
			infd.m_vecFindItem[0][2*i][0].nType = CDlgTemplateBuilder::EDIT;
			memcpy(infd.m_vecFindItem[0][2*i][0].caption, caption.c_str(), 64);
			if (m_loan_rate[i] > 0.00001)
				infd.m_vecFindItem[0][2*i][0].strItem.Format("%.3f", m_loan_rate[i] * 100);
			infd.m_vecFindItem[0][2*i][0].dbMin = 0;
			infd.m_vecFindItem[0][2*i][0].dbMax = 100;

			caption = "借款" + Int2String(i + 1) + " - 借入时点";
			infd.m_vecFindItem[0][2 * i + 1][0].nType = CDlgTemplateBuilder::COMBOBOX;
			memcpy(infd.m_vecFindItem[0][2 * i + 1][0].caption, caption.c_str(), 64);
			infd.m_vecFindItem[0][2 * i + 1][0].strData = "期初;期中;期末";
			if (m_loan_borrow[i].IsEmpty() == false)
				infd.m_vecFindItem[0][2 * i + 1][0].strItem = m_loan_borrow[i];
			else
				infd.m_vecFindItem[0][2 * i + 1][0].strItem = "期初";
		}
		
		infd.Init(_T("工程结算 参数设置"), _T("工程结算 参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			for (i = 0; i < 5; i++) {
				m_loan_rate[i] = String2Double(infd.m_vecFindItem[g][2*i][0].strItem.GetBuffer()) / 100;
				m_loan_borrow[i] = infd.m_vecFindItem[g][2 * i + 1][0].strItem;
			}
			return true;
		}
	}
	
	return false;
}

void CAfterFinancing::Calculate()
{
	{
		/* 确定资金缺口，以便确定是否需要 临时借款 */
		map<int, double> mapMonth;
		for (CFinanceAnalysisObj* e : m_objs) {
			int nMonth = String2Double(e->m_month.GetBuffer());
			if (e->m_scheme.Find("现金") >= 0) {
				if (e->m_name.Find("资本金") < 0) {
					if (mapMonth.count(nMonth) == 0) {
						mapMonth[nMonth] = e->AmountOfMoney();
					}
					else {
						mapMonth[nMonth] += e->AmountOfMoney();
					}
				}
			}
		}

		CGridDlg gridDlg;
		gridDlg.m_strTitle = "年度资金缺口表";
		gridDlg.m_vecHeader.push_back("年度");
		gridDlg.m_vecHeader.push_back("资金缺口(万元)");


		map<int, double>::iterator it = mapMonth.begin();
		for (; it != mapMonth.end(); it++) {
			int nMonth = it->first;
			double money = it->second;

			vector<string> vec;
			vec.push_back(Int2String(nMonth));
			vec.push_back(Double2String(money, "%.3f"));

			gridDlg.m_vecData.push_back(vec);
		}
		gridDlg.DoModal();
	}

	{
		/* 利润表 */
		map<int, double> mapMonth;
		for (CFinanceAnalysisObj* e : m_objs) {
			int nMonth = String2Double(e->m_month.GetBuffer());
			if (e->m_scheme.Find("现金") >= 0) {
				double amount;

				if (e->m_name.Find("资本金") < 0) {
					if (mapMonth.count(nMonth) == 0) {
						mapMonth[nMonth] = e->AmountOfMoney();
					}
					else {
						mapMonth[nMonth] += e->AmountOfMoney();
					}
				}
			}
		}
	}

	CFinanceAnalysis::Calculate();
}

/*****************************************************************************************/


void CFinanceAnalysisObj::SetDlgEditItem(CDyncItemGroupDlg & infd, CFinanceAnalysis * p, int i, string scheme, string caption, double dbMin, double dbMax, double def) {
	double amount;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, caption.c_str(), 64);
	if (scheme.empty() == false) {
		if (p->GetAmountOfMoney(m_month, scheme.c_str(), caption.c_str(), amount))
			infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", amount);
	}
	else {
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", def);
	}
	infd.m_vecFindItem[0][i][0].dbMin = dbMin;
	infd.m_vecFindItem[0][i][0].dbMax = dbMax;
}

void CFinanceAnalysisObj::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_month;
		ar << m_name;
		ar << m_amount_of_money;
	}
	else {
		ar >> m_month;
		ar >> m_name;
		ar >> m_amount_of_money;
	}
}

bool CFinanceAnalysisObj::CopyTo(CFinanceAnalysis* parent) {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;

	i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("开始年度"), 64);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("结束年度"), 64);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMin = 1000000;

	infd.Init(_T("参数设置"), _T("参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		g = 0;
		int v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		int v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		if (v2 < v1) v2 = v1;

		for (int i = v1; i <= v2; i++) {
			if (!parent->GetAmountOfMoney(Int2String(i).c_str(), m_scheme, m_name, amount)) {
				/* 不存在，则添加 */
				CFinanceAnalysisObj * p = Clone();
				p->m_month = Int2String(i).c_str();
				parent->m_objs.push_back(p);
			}
		}

		return true;
	}
	return false;
}

bool CFinanceAnalysisObj::CreateOrUpdate(string menuCode, CFinanceAnalysis* parent) {
	if (menuCode != CFinanceAnalysis::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.CYCOMBOX = 30;
	infd.GROUP_NUM_PER_LINE = 3;

	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("月份"), 64);
	if (!m_month.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_month;

	vector<string> options = GetOptions();
	string option; 
	for (int k = 0; k < options.size(); k++) {
		option += options[k];
		if (k < options.size() - 1)
			option += ";";
	}
	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = option.c_str();
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;
	else
		infd.m_vecFindItem[0][i][0].strItem = options.size() > 0 ? options[0].c_str() : "";
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("款项"), 64);

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("金额"), 64);
	if (m_amount_of_money != 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.3f", m_amount_of_money);
	infd.m_vecFindItem[0][i][0].dbMin = -100000;
	infd.m_vecFindItem[0][i][0].dbMax = 1000000;


	infd.Init(_T("参数设置"), _T("参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		CString month = infd.m_vecFindItem[0][i++][0].strItem;
		CString name = infd.m_vecFindItem[0][i++][0].strItem;
		double amount_of_money = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		bool bCheck = false;
		if (m_name.IsEmpty()) {
			/* 新增 */
			bCheck = true;
		}
		else {
			/* 修改 */
			if (String2Double(month.GetBuffer()) != String2Double(m_month.GetBuffer()) || name.CompareNoCase(m_name) != 0) {
				bCheck = true;
			}
		}
		if (bCheck) {
			double amount;
			if (parent->GetAmountOfMoney(month, m_scheme, name, amount)) {
				AfxMessageBox("已存在相同项");
				return false;
			}
		}
		m_month = month;
		m_name = name;
		m_amount_of_money = amount_of_money;
		return true;
	}
	return false;
}

/* ------------------------------------------------------------- */

 vector<string> CFinanceAnalysisObjA::GetOptions() {
	 vector<string> options;
	 options.push_back("1.建设投资");
	 options.push_back("2.流动资金投资");
	 options.push_back("3.经营成本(不含进项税)");
	 options.push_back("4.进项税额");
	 options.push_back("5.应纳增值税");
	 options.push_back("6.增值税附加");
	 options.push_back("7.维持运营投资");
	 options.push_back("8.调整所得税");
	 return options;
}

double CFinanceAnalysisObjA::AmountOfMoney() {
	return  -m_amount_of_money;
}

string CFinanceAnalysisObjA::Description() {
	stringstream ss;
	ss << "现金流出 ： " << Double2String(m_amount_of_money, "%.3f");

	return ss.str();
}

bool CFinanceAnalysisObjA::HasAssist() {
	if (m_name == "5.应纳增值税")
		return true;
	else if (m_name == "6.增值税附加")
		return true;
	else if (m_name == "8.调整所得税")
		return true;
	else
		return false;
}



bool CFinanceAnalysisObjA::Assist(CFinanceAnalysis* parent) {

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;

	if (m_name == "5.应纳增值税") {
		int nMonth = String2Double(m_month.GetBuffer());
		amount = parent->AccumulativeTax(nMonth);
		i = 0;
		SetDlgEditItem(infd, parent, i++, "", "累计截止本月应纳增值税", 0, 100000, amount);

		amount = parent->AccumulativeTax(nMonth - 1);
		SetDlgEditItem(infd, parent, i++, "", "累计截止上月应纳增值税", 0, 100000, amount);



		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double tax1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double tax2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			m_amount_of_money = tax1 - tax2;
			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name == "6.增值税附加") {
		i = 0;
		SetDlgEditItem(infd, parent, i++, "现金流出", "应纳增值税", -100000, 100000);

		SetDlgEditItem(infd, parent, i++, "", "税金附加费率(%)", 0, 100000, parent->m_tax_surcharge_rate * 100);


		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double tax1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double tax2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
			m_amount_of_money = tax1 * tax2;
			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name == "8.调整所得税") {
		i = 0;
		SetDlgEditItem(infd, parent, i++, "现金流入", "营业收入(不含销项税)", -100000, 100000);

		SetDlgEditItem(infd, parent, i++, "现金流入", "补贴收入", -100000, 100000);

		SetDlgEditItem(infd, parent, i++, "现金流出", "经营成本(不含进项税)", -100000, 100000);

		SetDlgEditItem(infd, parent, i++, "现金流出", "增值税附加", -100000, 100000);

		SetDlgEditItem(infd, parent, i++, "现金流出", "维持运营投资", -100000, 100000);

		SetDlgEditItem(infd, parent, i++, "总成本费用", "折旧费", 0, 100000);

		SetDlgEditItem(infd, parent, i++, "总成本费用", "摊销费", 0, 100000);

		SetDlgEditItem(infd, parent, i++, "", "所得税率(%)", 0, 100000, parent->m_income_tax_rate * 100);


		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double input1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double input2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			double output1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output4 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output5 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			double rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
			m_amount_of_money = input1 + input2 - output1 - output2 - output3 - output4 - output5;
			m_amount_of_money = m_amount_of_money * rate;

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------ */

vector<string> CFinanceAnalysisObjA1::GetOptions() {
	vector<string> options;
	options.push_back("1.项目资本金");
	options.push_back("2.借款本金偿还");
	options.push_back("3.借款利息偿还");
	options.push_back("4.流动资金投资");
	options.push_back("5.经营成本(不含进项税)");
	options.push_back("6.进项税额");
	options.push_back("7.应纳增值税");
	options.push_back("8.增值税附加");
	options.push_back("9.维持运营投资");
	options.push_back("10.所得税");
	return options;
}



double CFinanceAnalysisObjA1::AmountOfMoney() {
	return  -m_amount_of_money;
}

string CFinanceAnalysisObjA1::Description() {
	stringstream ss;
	ss << "现金流出 ： " << Double2String(m_amount_of_money, "%.3f");

	return ss.str();
}

bool CFinanceAnalysisObjA1::HasAssist() {
	if (m_name == "7.应纳增值税")
		return true;
	else if (m_name == "8.增值税附加")
		return true;
	else if (m_name == "10.所得税")
		return true;
	else
		return false;
}



bool CFinanceAnalysisObjA1::Assist(CFinanceAnalysis* p) {

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;
	CAfterFinancing * parent = (CAfterFinancing *)p;

	if (m_name == "7.应纳增值税") {
		int nMonth = String2Double(m_month.GetBuffer());
		amount = parent->AccumulativeTax(nMonth);
		i = 0;
		SetDlgEditItem(infd, p, i++, "", "累计截止本月应纳增值税", 0, 100000, amount);

		amount = parent->AccumulativeTax(nMonth - 1);
		SetDlgEditItem(infd, p, i++, "", "累计截止上月应纳增值税", 0, 100000, amount);


		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double tax1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double tax2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			m_amount_of_money = tax1 - tax2;
			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name == "8.增值税附加") {
		i = 0;
		SetDlgEditItem(infd, p, i++, "现金流出", "应纳增值税", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "", "税金附加费率(%)", 0, 100000, parent->m_tax_surcharge_rate * 100);
		

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double tax1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double tax2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
			m_amount_of_money = tax1 * tax2;
			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name == "10.所得税") {

		i = 0;
		SetDlgEditItem(infd, p, i++, "利润及利润分配", "利润总额", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "以前年度亏损", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "", "所得税率(%)", -100000, 100000, p->m_income_tax_rate);

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			m_amount_of_money = (v1 - v2) * v3;

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}

	return false;
}


/* --------------------------------------------- */

vector<string> CFinanceAnalysisObjB::GetOptions() {
	vector<string> options;
	options.push_back("1.营业收入(不含销项税)");
	options.push_back("2.销项税额");
	options.push_back("3.补贴收入");
	options.push_back("4.回收固定资产余值");
	options.push_back("5.回收流动资金");
	return options;
}


double CFinanceAnalysisObjB::AmountOfMoney() {
	return m_amount_of_money;
}

string CFinanceAnalysisObjB::Description() {
	return string("现金流入 ： ") + Double2String(m_amount_of_money, "%.3f");
}

bool CFinanceAnalysisObjB::HasAssist() {

	return false;
}

bool CFinanceAnalysisObjB::Assist(CFinanceAnalysis* parent) {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;

	if (m_name == "4.回收固定资产余值") {

	}

	return false;
}

/* --------------------------------------------- */

vector<string> CFinanceAnalysisObjC::GetOptions() {
	vector<string> options;
	options.push_back("1.借入");
	options.push_back("2.还本");
	options.push_back("3.付息");
	return options;
}


double CFinanceAnalysisObjC::AmountOfMoney() {
	return m_amount_of_money;
}

string CFinanceAnalysisObjC::Description() {
	return string("金额 ： ") + Double2String(m_amount_of_money, "%.3f");
}

bool CFinanceAnalysisObjC::HasAssist() {

	if (m_name == "3.付息")
		return true;
	else
		return false;
}

bool CFinanceAnalysisObjC::Assist(CFinanceAnalysis* parent) {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;
	CAfterFinancing*  p = (CAfterFinancing *)parent;
	int nMonth = atoi(m_month.GetBuffer());
	int nLoan = atoi(m_scheme.Mid(m_scheme.GetLength() - 1, 1).GetBuffer());

	if (m_name == "3.付息") {
		i = 0;
		amount = p->LoanRemaining(nMonth, nLoan);
		SetDlgEditItem(infd, p, i++, "", "截止本期初，贷款余额", -0.01, 100000, amount);

		SetDlgEditItem(infd, p, i++, m_scheme.GetBuffer(), "贷款利率(%)", 0, 100000, p->m_loan_rate[nLoan - 1] * 100);

		SetDlgEditItem(infd, p, i++, "", "借入", 0, 100000);
	

		infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
		memcpy(infd.m_vecFindItem[0][i][0].caption, "贷款借入时点", 64);
		infd.m_vecFindItem[0][i][0].strData = "期初;期中;期末";
		if (p->m_loan_borrow[nLoan - 1].IsEmpty() == false)
			infd.m_vecFindItem[0][i][0].strItem = p->m_loan_borrow[nLoan - 1];
		else
			infd.m_vecFindItem[0][i][0].strItem = "期初";


		infd.Init(_T("本期新产生利息"), _T("本期新产生利息"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double rate = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;
			if (infd.m_vecFindItem[g][i++][0].strItem.CompareNoCase("期中") == 0) {
				m_amount_of_money = v1 * rate + v2 * 0.5 * rate;
			}
			else if (infd.m_vecFindItem[g][i++][0].strItem.CompareNoCase("期初") == 0) {
				m_amount_of_money = v1 * rate + v2 * rate;
			}
			else {
				m_amount_of_money = v1 * rate;
			}
			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}

	return false;
}


/* --------------------------------------------- */

vector<string> CFinanceAnalysisObjD::GetOptions() {
	vector<string> options;
	options.push_back("1.折旧费");
	options.push_back("2.摊销费");
	
	return options;
}


double CFinanceAnalysisObjD::AmountOfMoney() {
	return m_amount_of_money;
}

string CFinanceAnalysisObjD::Description() {
	return string("成本费用 ： ") + Double2String(m_amount_of_money, "%.3f");
}

bool CFinanceAnalysisObjD::HasAssist() {

	return false;
}

bool CFinanceAnalysisObjD::Assist(CFinanceAnalysis* parent) {
	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;

	if (m_name == "4.回收固定资产余值") {

	}

	return false;
}



/* --------------------------------------------- */

vector<string> CFinanceAnalysisObjE::GetOptions() {
	vector<string> options;
	options.push_back("1.利润总额");
	options.push_back("2.以前年度亏损");
	options.push_back("3.净利润");
	options.push_back("4.可供分配利润");
	options.push_back("5.法定盈余公积金");
	options.push_back("6.应付投资者各方股利");
	options.push_back("7.未分配利润");
	options.push_back("7.用于还款未分配利润");
	options.push_back("8.剩余未分配利润");

	return options;
}


double CFinanceAnalysisObjE::AmountOfMoney() {
	return m_amount_of_money;
}

string CFinanceAnalysisObjE::Description() {
	return string("利润及利润分配 ： ") + Double2String(m_amount_of_money, "%.3f");
}

bool CFinanceAnalysisObjE::HasAssist() {

	if (m_name.Find("利润总额") >= 0)
		return true;
	return false;
}

bool CFinanceAnalysisObjE::Assist(CFinanceAnalysis* p) {

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 80;
	infd.CXCOMBOX = 100;
	infd.GROUP_NUM_PER_LINE = 3;
	int g = 0;
	int i = 0;
	double amount = 0;
	CAfterFinancing * parent = (CAfterFinancing *)p;

	if (m_name.Find("利润总额") >= 0) {
		i = 0;
		SetDlgEditItem(infd, p, i++, "现金流入", "营业收入(不含销项税)", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "现金流入", "补贴收入", -100000, 100000); 

		SetDlgEditItem(infd, p, i++, "现金流出", "借款利息偿还", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "现金流出", "经营成本(不含进项税)", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "现金流出", "增值税附加", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "现金流出", "维持运营投资", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "总成本费用", "折旧费", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "总成本费用", "摊销费", -100000, 100000);
				

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double input1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double input2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			double output1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output4 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output5 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double output6 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			m_amount_of_money = input1 + input2 - output1 - output2 - output3 - output4 - output5 - output6;

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name.Find("净利润") >= 0) {
		i = 0;
		SetDlgEditItem(infd, p, i++, "利润及利润分配", "利润总额", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "以前年度亏损", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "", "所得税率(%)", -100000, 100000, p->m_income_tax_rate);

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			
			m_amount_of_money = (v1 - v2) * v3;
			m_amount_of_money = v1 - m_amount_of_money;

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name.Find("可供分配利润") >= 0) {
		i = 0;
		SetDlgEditItem(infd, p, i++, "利润及利润分配", "净利润", -100000, 100000);
		
		SetDlgEditItem(infd, p, i++, "", "上年剩余未分配利润", -100000, 100000);

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			m_amount_of_money = (v1 + v2);

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name.Find("法定盈余公积金") >= 0) {
		i = 0;
		SetDlgEditItem(infd, p, i++, "利润及利润分配", "净利润", 0, 100000);
		
		SetDlgEditItem(infd, p, i++, "", "法定盈余率(%)", -100000, 100000, 10);

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			m_amount_of_money = (v1 * v2);

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name.Find("应付投资者各方股利") >= 0) {
		i = 0;
		SetDlgEditItem(infd, p, i++, "利润及利润分配", "可供分配利润", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "法定盈余公积金", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "", "约定的分配比例", -100000, 100000, 10);

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;

			m_amount_of_money = (v1 - v2) * v3;

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}

	}
	else if (m_name.Find("未分配利润") >= 0) {
	i = 0;
	SetDlgEditItem(infd, p, i++, "利润及利润分配", "可供分配利润", -100000, 100000);

	SetDlgEditItem(infd, p, i++, "利润及利润分配", "法定盈余公积金", -100000, 100000);

	SetDlgEditItem(infd, p, i++, "利润及利润分配", "应付投资者各方股利", -100000, 100000);

	infd.Init(_T("参数设置"), _T("参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		g = 0;
		double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
		double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer()) / 100;

		m_amount_of_money = v1 - v2 - v3;

		if (m_amount_of_money < 0) m_amount_of_money = 0;
		return true;
	}

	}
	else if (m_name.Find("用于还款未分配利润") >= 0) {
		double sum = 0;
		if(p->GetAmountOfMoney(m_month, "借款1", "还本", amount)) 
			sum += amount;
		if(p->GetAmountOfMoney(m_month, "借款2", "还本", amount))
			sum += amount;
		if(p->GetAmountOfMoney(m_month, "借款3", "还本", amount))
			sum += amount;
		if(p->GetAmountOfMoney(m_month, "借款4", "还本", amount))
			sum += amount;
		if(p->GetAmountOfMoney(m_month, "借款5", "还本", amount))
			sum += amount;

		i = 0;
		SetDlgEditItem(infd, p, i++, "", "应还本金", 0, 100000, sum);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "可供分配利润", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "总成本费用", "折旧费", 0, 100000);

		SetDlgEditItem(infd, p, i++, "总成本费用", "摊销费", 0, 100000);


		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v4 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			m_amount_of_money = 0;
			if (v1 > (v2 + v3 + v4)) {
				m_amount_of_money = v2;

				CString source;
				source.Format("资金缺口:  %.3f", v1 - (v2 + v3 + v4));
				AfxMessageBox(source);
				paste(source);
			}
			else {
				m_amount_of_money = v1 - (v3 + v4);
			}

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}
	else if (m_name.Find("剩余未分配利润") >= 0) {
		i = 0;
		SetDlgEditItem(infd, p, i++, "利润及利润分配", "净利润", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "法定盈余公积金", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "应付投资者各方股利", -100000, 100000);

		SetDlgEditItem(infd, p, i++, "利润及利润分配", "用于还款未分配利润", -100000, 100000);

		infd.Init(_T("参数设置"), _T("参数设置"));
		if (infd.DoModal() == IDOK) {
			i = 0;
			g = 0;
			double v1 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v2 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v3 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());
			double v4 = String2Double(infd.m_vecFindItem[g][i++][0].strItem.GetBuffer());

			m_amount_of_money = v1 - v2 - v3 - v4;

			if (m_amount_of_money < 0) m_amount_of_money = 0;
			return true;
		}
	}

	return false;
}
