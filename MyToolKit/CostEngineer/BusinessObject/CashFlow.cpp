#include "stdafx.h"
#include "CashFlow.h"
#include "GridDlg.h"


CCashFlowObj::CCashFlowObj()
{
	m_name = "";
	m_building_start = 0;
	m_building_duration = 0;
	m_building_cost = 0;
	m_payment_interval = 1;		//  默认以 一个月为一个支付期
	m_payment_lag = 0;			//	默认为 当期支付
	m_payment_time = 1;			//	默认为 期末发生支付 或 扣款
}


CCashFlowObj::~CCashFlowObj()
{
}



void CCashFlowObj::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_building_start;
		ar << m_building_duration;
		ar << m_building_cost;
		ar << m_payment_interval;
		ar << m_payment_lag;
		ar << m_payment_time;
	}
	else {
		ar >> m_name;
		ar >> m_building_start;
		ar >> m_building_duration;
		ar >> m_building_cost;
		ar >> m_payment_interval;
		ar >> m_payment_lag;
		ar >> m_payment_time;
	}
}


bool CCashFlowObj::CreateOrUpdate(string menuCode) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	CDyncItemGroupDlg infd;
	infd.CXCAPTION = 50;
	infd.GROUP_NUM_PER_LINE = 3;
	int i = 0;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("分部工程名称"), 64);
	if (!m_name.IsEmpty())
		infd.m_vecFindItem[0][i][0].strItem = m_name;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("开始时间(月)"), 64);
	if (m_building_start > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_building_start);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 10000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("工期(月)"), 64);
	if (m_building_duration > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_building_duration);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 10000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("造价"), 64);
	if (m_building_cost != 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_building_cost);
	infd.m_vecFindItem[0][i][0].dbMin = -1000000;
	infd.m_vecFindItem[0][i][0].dbMax =  1000000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("支付周期(月)"), 64);
	if (m_payment_interval > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_payment_interval);
	infd.m_vecFindItem[0][i][0].nMin = 1;
	infd.m_vecFindItem[0][i][0].nMax = 10000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::EDIT;
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("支付延迟(月)"), 64);
	if (m_payment_lag > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%d", m_payment_lag);
	infd.m_vecFindItem[0][i][0].nMin = 0;
	infd.m_vecFindItem[0][i][0].nMax = 10000;

	i++;
	infd.m_vecFindItem[0][i][0].nType = CDlgTemplateBuilder::COMBOBOX;
	infd.m_vecFindItem[0][i][0].strData = "期初;期末";
	infd.m_vecFindItem[0][i][0].strItem = "期末";
	if (m_payment_time == 0)
		infd.m_vecFindItem[0][i][0].strItem = "期初";
	memcpy(infd.m_vecFindItem[0][i][0].caption,  _T("支付时间") , 64);

	
	infd.Init(_T("工程支付 参数设置"), _T("工程支付 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_building_start = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_building_duration = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_building_cost = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_payment_interval = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		m_payment_lag = atoi(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());
		if (string(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer()) == "期末")
			m_payment_time = 1;
		else
			m_payment_time = 0;
		
		return true;
	}
	return false;
}



bool CCashFlowObj::Draw(CGridCtrl* pGridCtrl, vector<CCashFlowObj>& cols, CCashFlow& parent) {
	if (!pGridCtrl)
		return false;

	vector<string>	vecHeader;
	vector<vector<string>>	vecData;
	vecHeader.push_back("分部工程名称");
	vecHeader.push_back("开始时间(月)");
	vecHeader.push_back("工期(月)");
	vecHeader.push_back("造价");
	vecHeader.push_back("支付周期(月)");
	vecHeader.push_back("支付延迟(月)");
	vecHeader.push_back("支付时间");
	vecHeader.push_back("终值 (以分部工程结清为基准)");
	vecHeader.push_back("");
	vecHeader.push_back("");

	for (CCashFlowObj e : cols) {
		vector<string> vec;
		vec.push_back(e.m_name.GetBuffer());
		vec.push_back(Int2String(e.m_building_start));
		vec.push_back(Int2String(e.m_building_duration));
		vec.push_back(Double2String(e.m_building_cost));
		vec.push_back(Int2String(e.m_payment_interval));
		vec.push_back(Int2String(e.m_payment_lag));
		vec.push_back(e.m_payment_time == 0 ? "期初" : "期末");
		vec.push_back(Double2String(e.FutureValueOfPartitionedProject(parent.m_interest_rate)));
		vec.push_back("修改" + ActionType2String(ActionType::Update));
		vec.push_back("删除" + ActionType2String(ActionType::Delete));

		vecData.push_back(vec);
	}
	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

bool CCashFlowObj::Update(string menuCode, int nRow, vector<CCashFlowObj>& cols) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(menuCode);
	return false;
}


bool CCashFlowObj::Delete(string menuCode, int nRow, vector<CCashFlowObj>& cols) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CCashFlowObj>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}

int CCashFlowObj::EarliestPaymentTime() {
	int num = m_building_duration / m_payment_interval;
	int remain = m_building_duration % m_payment_interval;
	if (m_payment_time == 0) {
		/* 期初支付 */
		return m_building_start + m_payment_lag;
	}
	else {
		/* 期末支付*/
		if(num > 0)
			return m_building_start + m_payment_interval -1 + m_payment_lag;
		else 
			return m_building_start + remain - 1 + m_payment_lag;
	}
	
}

int CCashFlowObj::LatestPaymentTime() {
	int num = m_building_duration / m_payment_interval;
	int remain = m_building_duration % m_payment_interval;
	if (m_payment_time == 0) {
		/* 期初支付 */
		if (remain > 0)
			return m_building_start + m_payment_interval * num + m_payment_lag;
		else
			return m_building_start + m_payment_interval * (num-1) + m_payment_lag;
	}
	else {
		/* 期末支付 */
		return m_building_start + m_building_duration - 1 + m_payment_lag;
	}
}


/* 终值都换算到结清月的月末或月初 */
double CCashFlowObj::FutureValueOfPartitionedProject(double i) {
	double annuity = m_building_cost / m_building_duration;
	int num = m_building_duration / m_payment_interval;		/* 相同年金的笔数 */
	int remain = m_building_duration % m_payment_interval;	/* 最后一笔款包含的月份数 */
	double future;
	if (m_payment_time == 0) {
		/* 年金发生在期初，则终值换算到结清月的月初 */
		future = Annuity2Future(i * m_payment_interval, num ,3) * annuity  * m_payment_interval;
		if (remain > 0) {
			future = future * Present2Future(i, m_payment_interval);
			future += remain * annuity;
		}
	}
	else {
		/* 年金发生在期末，则终值换算到结清月的月末 */
		future = Annuity2Future(i * m_payment_interval, num, 3) * annuity  * m_payment_interval;
		if (remain > 0) {
			future = future * Present2Future(i, remain);
			future += remain * annuity;
		}
	}
	return future;
}

/* 现值都换算到开始支付月的月初 */
double CCashFlowObj::PresentValueOfPartitionedProject(double i) {
	double annuity = m_building_cost / m_building_duration;
	int num = m_building_duration / m_payment_interval;		/* 相同年金的笔数 */
	int remain = m_building_duration % m_payment_interval;	/* 最后一笔款包含的月份数 */
	double future;
	if (m_payment_time == 0) {
		/* 年金发生在期初，则现值换算到开始支付月的月初 */
		future = Annuity2Present(i * m_payment_interval, num - 1, 4) * annuity  * m_payment_interval;
		future += annuity * m_payment_interval;		/* 加上月初的第一笔 */
		if (remain > 0) {
			future += remain * annuity * Future2Present(i * m_payment_interval, num, 4);
		}
	}
	else {
		/* 年金发生在期末，则现值换算到开始支付月的月初 */
		future = Annuity2Present(i * m_payment_interval, num, 4);
		future = future *annuity  * m_payment_interval;
		if (remain > 0) {
			
			future += remain * annuity * Future2Present(i, m_building_duration, 4);
		}
	}
	return future;
}


/***********************************************************************************/


/* 现金流量表 */
string CCashFlow::m_ObjectCode = "01040901";
double CCashFlow::m_ObjectVersion = 1.0;

CCashFlow::CCashFlow()
{
	m_name = "";
	m_interest_rate = 0;
}


CCashFlow::~CCashFlow()
{
}


void CCashFlow::Serialize(CArchive& ar, double version) {
	if (ar.IsStoring()) {
		ar << m_name;
		ar << m_interest_rate;
		ar << m_objs.size();
		for (int i = 0; i < m_objs.size(); i++) {
			m_objs[i].Serialize(ar, version);
		}
	}
	else {
		ar >> m_name;
		ar >> m_interest_rate;
		int nNum;
		ar >> nNum;
		for (int i = 0; i < nNum; i++) {
			CCashFlowObj bs;
			bs.Serialize(ar, version);
			m_objs.push_back(bs);
		}
	}
}

bool CCashFlow::CreateOrUpdate(string menuCode) {

	if (menuCode != CCashFlow::m_ObjectCode)
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
	memcpy(infd.m_vecFindItem[0][i][0].caption, _T("利率(月)"), 64);
	if (m_interest_rate > 0)
		infd.m_vecFindItem[0][i][0].strItem.Format("%.2f", m_interest_rate);
	infd.m_vecFindItem[0][i][0].dbMin = 0.0001;
	infd.m_vecFindItem[0][i][0].dbMax = 100000;

	
	infd.Init(_T("现金流量表 参数设置"), _T("现金流量表 参数设置"));
	if (infd.DoModal() == IDOK) {
		i = 0;
		m_name = infd.m_vecFindItem[0][i++][0].strItem;
		m_interest_rate = String2Double(infd.m_vecFindItem[0][i++][0].strItem.GetBuffer());

		return true;
	}
	return false;
}


bool CCashFlow::Draw(string menuCode, CGridCtrl* pGridCtrl, vector<CCashFlow>& cols) {
	if (!pGridCtrl)
		return false;

	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	vector<string>	vecHeader;
	vector<vector<string>>	vecData;
	vecHeader.push_back("现金流量表名");
	vecHeader.push_back("利率(%)(月)");
	vecHeader.push_back("最后一笔支付款收到月份");
	vecHeader.push_back("终值");
	vecHeader.push_back("现值");
	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");
	vecHeader.push_back("");

	for (CCashFlow e : cols) {
		vector<string> vec;
		vec.push_back(e.m_name.GetBuffer());
		vec.push_back(Double2String(e.m_interest_rate * 100, "%.2f"));
		vec.push_back(Int2String(e.LatestPaymentTime()));
		vec.push_back(Double2String(e.FutureValueOfWholeProject(), "%.2f"));
		vec.push_back(Double2String(e.PresentValueOfWholeProject(), "%.2f"));
		vec.push_back("修改" + ActionType2String(ActionType::Update));
		vec.push_back("删除" + ActionType2String(ActionType::Delete));
		vec.push_back("增加" + ActionType2String(ActionType::Create));
		vec.push_back("复制" + ActionType2String(ActionType::Copy));

		vecData.push_back(vec);
	}
	return DrawGrid(pGridCtrl, vecHeader, vecData);
}

bool CCashFlow::Update(string menuCode, int nRow, vector<CCashFlow>& cols) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size())
		return cols[nRow - 1].CreateOrUpdate(CCashFlow::m_ObjectCode);
	return false;
}

bool CCashFlow::Delete(string menuCode, int nRow, vector<CCashFlow>& cols) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		int idx = 0;
		vector<CCashFlow>::iterator it = cols.begin();
		for (; it != cols.end(); it++, idx++) {
			if (idx == nRow - 1)
				break;
		}
		cols.erase(it);
		return true;
	}
	return false;
}

bool CCashFlow::Copy(string menuCode, int nRow, vector<CCashFlow>& cols) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return false;

	if (nRow > 0 && nRow <= cols.size()) {
		CCashFlow aoa = cols[nRow - 1];
		aoa.m_name += "-cp";
		cols.insert(cols.begin(), aoa);
		return true;
	}
	return false;
}


unsigned int CCashFlow::PopupMenuId(string menuCode) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return 0;
	return IDR_POPUP_COMPONENT;
}

int CCashFlow::LatestPaymentTime() {
	int latest = 0;
	for (auto e : m_objs) {
		if (e.LatestPaymentTime() > latest) {
			latest = e.LatestPaymentTime();
		}
	}
	return latest;
}



double CCashFlow::FutureValueOfWholeProject() {
	int latest = LatestPaymentTime();
	double sum = 0;
	for (auto e : m_objs) {
		double future = e.FutureValueOfPartitionedProject(m_interest_rate);
		/* 换算到整个工程的结清月的月末 */
		int months = latest - e.LatestPaymentTime();
		if (e.m_payment_time == 0)
			/* 如果为期初支付，则增加一个月 */
			months++;

		int num = months / e.m_payment_interval;
		int remain = months % e.m_payment_interval;
				
		future = future * Present2Future(m_interest_rate * e.m_payment_interval, num, 3);
		future = future * Present2Future(m_interest_rate, remain, 3);
		sum += future;
	}
	return sum;
}

double CCashFlow::PresentValueOfWholeProject() {
	int latest = LatestPaymentTime();
	double sum = 0;
	for (auto e : m_objs) {
		double present = e.PresentValueOfPartitionedProject(m_interest_rate);
		/* 换算到整个工程的月初 */
		int months = e.EarliestPaymentTime() - 1;
		

		int num = months / e.m_payment_interval;
		int remain = months % e.m_payment_interval;

		present = present * Future2Present(m_interest_rate * e.m_payment_interval, num, 4);
		present = present * Future2Present(m_interest_rate, remain, 4);
		sum += present;
	}
	return sum;
}

void CCashFlow::Calculate(string menuCode, vector<CCashFlow>& cols) {
	if (menuCode != CCashFlow::m_ObjectCode)
		return;

	CGridDlg gridDlg;
	gridDlg.m_vecHeader.push_back("名称");
	gridDlg.m_vecHeader.push_back("单方造价");
	

	for (int i = 0; i < cols.size(); i++)
	{
		CCashFlow seb = cols[i];

		string str = seb.m_name.GetBuffer();
		vector<string> vec;
		vec.push_back(str);
		
			
		/*vec.push_back(Double2String(total));
		gridDlg.m_vecData.push_back(vec);*/
	}
	gridDlg.DoModal();
}

