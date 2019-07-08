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


	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(8 + 3);		//	额外增加三列 ： 序号/修改/删除
		pGridCtrl->SetFixedRowCount(1);
		pGridCtrl->SetFixedColumnCount(1);
		pGridCtrl->SetHeaderSort(TRUE);
		pGridCtrl->SetEditable(FALSE);
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

				if (col == 0)	val = "";
				else if (col == 1)	val = "分部工程名称";
				else if (col == 2)	val = "开始时间(月)";
				else if (col == 3)	val = "工期(月)";
				else if (col == 4)	val = "造价";
				else if (col == 5)	val = "支付周期(月)";
				else if (col == 6)	val = "支付延迟(月)";
				else if (col == 7)	val = "支付时间";
				else if (col == 8)	val = "终值 (以分部工程结清为基准)";
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

				if (col >= 3 && col <= 4 || col == 0)
				{
					if (!pGridCtrl->SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return false;
				}
				if (col == 9) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 10) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2) 	val = Int2String(cols[row - 1].m_building_start);
				else if (col == 3)  val = Int2String(cols[row - 1].m_building_duration);
				else if (col == 4)  val = Double2String(cols[row - 1].m_building_cost);
				else if (col == 5) 	val = Int2String(cols[row - 1].m_payment_interval);
				else if (col == 6)  val = Int2String(cols[row - 1].m_payment_lag);
				else if (col == 7) {
					val = cols[row - 1].m_payment_time == 0 ? "期初" : "期末";
				}
				else if (col == 8) {
					val = Double2String(cols[row - 1].FutureValueOfPartitionedProject(parent.m_interest_rate));
				}
				else if (col == 9)	val = "修改（update）";
				else if (col == 10)	val = "删除（delete）";

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

	try {
		pGridCtrl->SetRowCount(cols.size() + 1);
		pGridCtrl->SetColumnCount(5 + 4);		//	额外增加4列 ： 序号/修改/删除/增加
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
				else if (col == 2)	val = "利率(%)(月)";
				else if (col == 3)	val = "最后一笔支付款收到月份";
				else if (col == 4)  val = "终值";
				else if (col == 5)	val = "现值";
				else if (col == 6)	val = "";
				else if (col == 7)	val = "";
				else if (col == 8)	val = "";

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
				if (col == 6 || col == 8) {
					Item.crFgClr = RGB(0, 120, 250);
					Item.mask |= GVIF_FGCLR;
				}
				if (col == 7) {
					Item.crFgClr = RGB(255, 0, 0);
					Item.mask |= GVIF_FGCLR;
				}

				if (col == 0)	val = Int2String(row);
				else if (col == 1) 	val = cols[row - 1].m_name.GetBuffer();
				else if (col == 2)  val = Double2String(cols[row - 1].m_interest_rate * 100, "%.2f");
				else if (col == 3)  val = Int2String(cols[row - 1].LatestPaymentTime());
				else if (col == 4)  val = Double2String(cols[row - 1].FutureValueOfWholeProject(), "%.2f");
				else if (col == 5) 	val = Double2String(cols[row - 1].PresentValueOfWholeProject() , "%.2f");
				else if (col == 6)	val = "修改（update）";
				else if (col == 7)	val = "删除（delete）";
				else if (col == 8)	val = "增加（create）";

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

