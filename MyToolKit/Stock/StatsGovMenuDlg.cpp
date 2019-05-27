// StatsGovMenuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "StatsGovMenuDlg.h"


// CStatsGovMenuDlg 对话框

IMPLEMENT_DYNAMIC(CStatsGovMenuDlg, CDialog)

CStatsGovMenuDlg::CStatsGovMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatsGovMenuDlg::IDD, pParent)
	, m_nOutput(1)	//	默认为 图表展示
{

}

CStatsGovMenuDlg::~CStatsGovMenuDlg()
{
}

void CStatsGovMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_TREE2, m_TreeReg);
	DDX_Control(pDX, IDC_BTN_ZB, m_btnZb);
	DDX_Radio(pDX, IDC_RADIO_OUPUT, m_nOutput);
}


BEGIN_MESSAGE_MAP(CStatsGovMenuDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CStatsGovMenuDlg::OnNMClickTree1)
	ON_BN_CLICKED(IDC_BTN_ZB, &CStatsGovMenuDlg::OnBnClickedBtnZb)
	ON_BN_CLICKED(IDC_BTN_REG, &CStatsGovMenuDlg::OnBnClickedBtnReg)
	ON_NOTIFY(NM_CLICK, IDC_TREE2, &CStatsGovMenuDlg::OnNMClickTree2)
END_MESSAGE_MAP()


// CStatsGovMenuDlg 消息处理程序

BOOL CStatsGovMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	if(m_str_db_code.find("hg")!=string::npos)
	{
		m_btnZb.EnableWindow(false);
		m_TreeReg.EnableWindow(FALSE);
	}

	vector<CStockPlateData> vecStockPlateData;

	//	先序遍历
	CStockPlateTree * pPlates = NULL;

	if(m_str_db_code=="hgyd")
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCStatsGovHgydPlates;
		this->SetWindowText("全国 月度数据");
	}
	else if(m_str_db_code=="hgjd")
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCStatsGovHgjdPlates;
		this->SetWindowText("全国 季度数据");
	}
	else if(m_str_db_code=="hgnd")
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCStatsGovHgndPlates;
		this->SetWindowText("全国 年度数据");
	}
	else if(m_str_db_code=="fsyd")
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCStatsGovFsydPlates;
		this->SetWindowText("分省 月度数据");
	}
	else if(m_str_db_code=="fsjd")
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCStatsGovFsjdPlates;
		this->SetWindowText("分省 季度数据");
	}
	else if(m_str_db_code=="fsnd")
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCStatsGovFsndPlates;
		this->SetWindowText("分省 年度数据");
	}

	if(pPlates)
	{
		
		CStockPlateTree::pre_order_iterator pre_order_it = pPlates->pre_order_begin();
		while(pre_order_it!=pPlates->pre_order_end())
		{
			CStockPlateData spd = *pre_order_it;
			if(pre_order_it.node()->parent()->is_root()==false)
			{
				spd.parent_code = pre_order_it.node()->parent()->get()->code;
			}
			else
			{
				spd.parent_code = "-1";
			}

			vecStockPlateData.push_back(spd);
			pre_order_it++;
		}
	}


	HTREEITEM hRoot;
	map<string, HTREEITEM> mapHTree;
	for(int i =0 ; i<vecStockPlateData.size(); i++)
	{
		CString str;
		str.Format("%s-(%s)",vecStockPlateData[i].name.c_str(), vecStockPlateData[i].code.c_str());

		if(vecStockPlateData[i].parent_code=="-1")
		{
			HTREEITEM hClass = m_Tree.InsertItem(str, 1, 1);
			hRoot = hClass;

			CString key;
			key.Format("%s-%d", vecStockPlateData[i].code.c_str(), vecStockPlateData[i].is_stock);
			mapHTree.insert(make_pair(string(key.GetBuffer()), hClass));

			m_Tree.SetItemData(hClass, (DWORD_PTR)vecStockPlateData[i].is_stock);
		}
		else
		{
			CString key;
			key.Format("%s-%d", vecStockPlateData[i].parent_code.c_str(), 0);
			HTREEITEM hParent = mapHTree[string(key.GetBuffer())];
			HTREEITEM hClass = m_Tree.InsertItem(str, hParent, TVI_LAST);

			key.Format("%s-%d", vecStockPlateData[i].code.c_str(), vecStockPlateData[i].is_stock);
			mapHTree.insert(make_pair(string(key.GetBuffer()), hClass));

			m_Tree.SetItemData(hClass, (DWORD_PTR)vecStockPlateData[i].is_stock);
		}
	}

	if(vecStockPlateData.size()>0)
	{
		//	展开根节点
		m_Tree.Expand(hRoot, TVE_EXPAND /*TVE_COLLAPSE*/);
	}


	//	展示 各个省份
	{
		CStatsGovCn  statsGovCn;
		map<string, string> map_RegName = statsGovCn.GetOtherWds("test", "test");

		CString str;
		str.Format("%s-(%s)",  "中国", "86");
		HTREEITEM hClass = m_TreeReg.InsertItem(str, 1, 1);
		hRoot = hClass;
		m_TreeReg.SetItemData(hClass, 0);

		map<string, string>::iterator it = map_RegName.begin();
		for(; it!=map_RegName.end(); it++)
		{
			str.Format("%s-(%s)",  it->second.c_str(), it->first.c_str());
			HTREEITEM hClass = m_TreeReg.InsertItem(str,hRoot, TVI_LAST);
			m_TreeReg.SetItemData(hClass, 1);
		}

		m_TreeReg.Expand(hRoot, TVE_EXPAND /*TVE_COLLAPSE*/);

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CStatsGovMenuDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	string codeStr;
	
	CPoint point;
	GetCursorPos(&point);			//获得鼠标点击的位置
	m_Tree.ScreenToClient(&point);	//转化为客户坐标
	UINT uFlags;

	HTREEITEM CurrentItem;
	CurrentItem=m_Tree.HitTest(point,&uFlags);//获得当前点击节点的ITEM


	if (CurrentItem != NULL)
	{
		CString text = m_Tree.GetItemText(CurrentItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\-\\([A-Za-z\\d]+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		int is_stock = (int)m_Tree.GetItemData(CurrentItem);
		string::size_type pos1 = firstMatch[0].find(")");
		codeStr = firstMatch[0].substr(2, pos1-2);
		int kkk = 1;

		if(is_stock==0)
			return;
	}

	*pResult = 0;

	vector<string>	zb_code;
	vector<string>	zb_name;
	vector<string>	unit;
	try{
		string test = "select zb_code, zb_name, unit from statsgov" + m_str_db_code + 
			"  where pid = \'" + codeStr + "\' GROUP BY zb_code, zb_name, unit";
		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		while (st.fetch())
		{
			zb_code.insert(zb_code.end(), r.get<string>("zb_code"));
			zb_name.insert(zb_name.end(), Utf8_GBK(r.get<string>("zb_name")));
			unit.insert(unit.end(), Utf8_GBK(r.get<string>("unit")));
		}
	}
	catch(...){}

	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

	m_Grid.SetRowCount(zb_code.size() + 1);
	m_Grid.SetColumnCount(4);

	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnSelection(FALSE);
	m_Grid.SetFixedRowSelection(FALSE);
	m_Grid.EnableColumnHide();
	m_Grid.SetEditable(FALSE);
	m_Grid.SetHeaderSort(TRUE);

	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.col = 0;
	Item.strText.Format(_T("%s"), "指标名称");
	m_Grid.SetItem(&Item);

	Item.col = 1;
	Item.strText.Format(_T("%s"), "单位");
	m_Grid.SetItem(&Item);

	Item.col = 2;
	Item.strText.Format(_T("%s"), "指标后缀");
	m_Grid.SetItem(&Item);

	Item.col = 3;
	Item.strText.Format(_T("%s"), "指标编码");
	m_Grid.SetItem(&Item);

	for(int i=0; i<zb_code.size(); i++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		Item.row = i+1;
		Item.col = 0;
		Item.strText.Format(_T("%s"), zb_name[i].c_str());
		m_Grid.SetItem(&Item);

		Item.col = 1;
		Item.strText.Format(_T("%s"), unit[i].c_str());
		m_Grid.SetItem(&Item);

		string strZbExt = "";
		string::size_type pos = zb_name[i].find_last_of("_");
		if(pos!=string::npos)
		{
			strZbExt = zb_name[i].substr(pos + 1);
		}
		Item.col = 2;
		Item.strText.Format(_T("%s"), strZbExt.c_str());
		m_Grid.SetItem(&Item);

		Item.col = 3;
		Item.strText.Format(_T("%s"), zb_code[i].c_str());
		m_Grid.SetItem(&Item);
	}


	m_Grid.AutoSize();
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellTextCompare);
}


void CStatsGovMenuDlg::checkToTree(HTREEITEM hitem)
{
	if(hitem != NULL)
	{
		hitem = m_TreeReg.GetChildItem(hitem);
		while(hitem)
		{
			int is_stock = (int)m_TreeReg.GetItemData(hitem);
			if(m_TreeReg.GetCheck(hitem) && is_stock ==1)
			{
				CString text = m_TreeReg.GetItemText(hitem);

				vector<string> firstMatch;
				string strPattern = string("\\-\\([A-Za-z\\d]+\\)");
				if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )>0)
				{
					string::size_type pos1 = firstMatch[0].find(")");
					string codeStr = firstMatch[0].substr(2, pos1-2);
					m_vec_reg_code.push_back(codeStr);
				}
			}
			checkToTree(hitem);
			hitem = m_TreeReg.GetNextItem(hitem, TVGN_NEXT);
		}
	}
}


void CStatsGovMenuDlg::updateCheckBox(HTREEITEM hitem, bool newStatus)
{
	if(hitem != NULL)
	{
		hitem = m_TreeReg.GetChildItem(hitem);
		while(hitem)
		{
			m_TreeReg.SetCheck(hitem, newStatus);
			updateCheckBox(hitem, newStatus);
			hitem = m_TreeReg.GetNextItem(hitem, TVGN_NEXT);
		}
	}
}

void CStatsGovMenuDlg::OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	string codeStr;

	CPoint point;
	GetCursorPos(&point);			//获得鼠标点击的位置
	m_TreeReg.ScreenToClient(&point);	//转化为客户坐标
	UINT uFlags;

	HTREEITEM CurrentItem;
	CurrentItem=m_TreeReg.HitTest(point,&uFlags);//获得当前点击节点的ITEM


	if (CurrentItem != NULL)
	{
		CString text = m_TreeReg.GetItemText(CurrentItem);

		int is_stock = (int)m_TreeReg.GetItemData(CurrentItem);
		if(is_stock==0)
		{
			bool oldStatus = m_TreeReg.GetCheck(CurrentItem);
			m_TreeReg.SetCheck(CurrentItem, !oldStatus);
			updateCheckBox(CurrentItem, !oldStatus);
		}
		else
		{
			m_TreeReg.SetCheck(CurrentItem, !m_TreeReg.GetCheck(CurrentItem));
		}
	}

	*pResult = 0;
}


void CStatsGovMenuDlg::BtnProccess()
{
	// TODO: 在此添加专用代码和/或调用基类

	m_vec_zb_code.clear();

	int selected = m_Grid.GetSelectedCount();
	map<string ,string>  codes;
	if(selected>0)
	{
		for (int row = 0; row < m_Grid.GetRowCount(); row++)
		{
			for (int col = 0; col < m_Grid.GetColumnCount(); col++)
			{
				if(m_Grid.IsCellSelected(row, col))
				{
					/* 通过第三列获取指标代码 */
					CString str = m_Grid.GetItemText(row, 3);
					string  zb_code = str.GetBuffer();
					str = m_Grid.GetItemText(row, 0);
					codes[zb_code] = str.GetBuffer();
				}
			}
		}

		map<string ,string >::iterator it = codes.begin();
		for(; it!=codes.end(); it++)
		{
			m_vec_zb_code.push_back(it->first);
		}
	}

	//	递归获取选中的 节点
	m_vec_reg_code.clear();

	HTREEITEM root = m_TreeReg.GetRootItem();
	checkToTree(root);

	if(m_str_db_code.find("hg")!=string::npos)
	{
		m_vec_reg_code.push_back("1111111");
	}

}


void CStatsGovMenuDlg::OnBnClickedBtnZb()
{
	// TODO: 在此添加控件通知处理程序代码
	BtnProccess();

	if(m_vec_zb_code.size()>0 && m_vec_reg_code.size()>0)
	{
		//	保持指标唯一
		string strCode = m_vec_zb_code[0];
		m_vec_zb_code.clear();
		m_vec_zb_code.push_back(strCode);
		CDialog::OnOK();
	}
	else if(m_vec_reg_code.size()<=0)
	{
		AfxMessageBox("请选择查询的区域");
	}
	else
		AfxMessageBox("请选择查询的指标");

	
}

void CStatsGovMenuDlg::OnBnClickedBtnReg()
{
	// TODO: 在此添加控件通知处理程序代码
	BtnProccess();

	if(m_vec_reg_code.size()>0 && m_vec_zb_code.size()>0)
	{
		//	保持地区唯一
		string strCode = m_vec_reg_code[0];
		m_vec_reg_code.clear();
		m_vec_reg_code.push_back(strCode);
		CDialog::OnOK();
	}
	else if(m_vec_reg_code.size()<=0)
	{
		AfxMessageBox("请选择查询的区域");
	}
	else
		AfxMessageBox("请选择查询的指标");
}
