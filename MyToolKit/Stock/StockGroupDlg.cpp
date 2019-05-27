// StockGroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "StockGroupDlg.h"


// CStockGroupDlg 对话框

IMPLEMENT_DYNAMIC(CStockGroupDlg, CDialog)

CStockGroupDlg::CStockGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStockGroupDlg::IDD, pParent)
	, m_RadioReg(-1)
	, m_RadioCsrc(-1)
{

}

CStockGroupDlg::~CStockGroupDlg()
{
}

void CStockGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_REG, m_TreeReg);
	DDX_Radio(pDX, IDC_RADIO_NATION, m_RadioReg);
	DDX_Radio(pDX, IDC_RADIO_CSRC_GATE, m_RadioCsrc);
	DDX_Control(pDX, IDC_TREE_CSRC, m_TreeCsrc);
}


BEGIN_MESSAGE_MAP(CStockGroupDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_NATION, &CStockGroupDlg::OnBnClickedRadioReg)
	ON_BN_CLICKED(IDC_RADIO_PROVINCE, &CStockGroupDlg::OnBnClickedRadioReg)
	ON_NOTIFY(NM_CLICK, IDC_TREE_REG, &CStockGroupDlg::OnNMClickTreeReg)
	ON_BN_CLICKED(IDC_RADIO_CSRC_GATE, &CStockGroupDlg::OnBnClickedRadioCsrc)
	ON_BN_CLICKED(IDC_RADIO_CSRC_BIG, &CStockGroupDlg::OnBnClickedRadioCsrc)
	ON_NOTIFY(NM_CLICK, IDC_TREE_CSRC, &CStockGroupDlg::OnNMClickTreeCsrc)
END_MESSAGE_MAP()


// CStockGroupDlg 消息处理程序



BOOL CStockGroupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//////////////////////////////////////////////////////////////////////////
	//	展示 各个省份
	{
		CStatsGovCn  statsGovCn;
		map<string, string> map_RegName = statsGovCn.GetOtherWds("test", "test");

		CString str;
		str.Format("%s-(%s)",  "中国", "86");
		HTREEITEM hClass = m_TreeReg.InsertItem(str, 1, 1);
		HTREEITEM hRoot = hClass;
		m_TreeReg.SetItemData(hClass, 0);

		map<string, string>::iterator it = map_RegName.begin();
		for(; it!=map_RegName.end(); it++)
		{
			str.Format("%s-(%s)",  it->second.c_str(), it->first.c_str());
			HTREEITEM hClass = m_TreeReg.InsertItem(str,hRoot, TVI_LAST);
			m_TreeReg.SetItemData(hClass, 1);
		}

		m_TreeReg.Expand(hRoot, TVE_EXPAND /*TVE_COLLAPSE*/);
		if(m_RadioReg<0)
			m_TreeReg.EnableWindow(FALSE);
	}

	//////////////////////////////////////////////////////////////////////////
	//	展示“证监会行业” 树
	{
		vector<CStockPlateData> vecStockPlateData;
		CStockPlateTree * pPlates = ((CStockApp *)AfxGetApp())->m_pCsindexCsrcStockPlates;

		if(pPlates)
		{	
			//	以某个节点为根，进行子树的搜索
			CStockPlateData treeNode;
			treeNode.code = Int2String(0, "%08d");
			treeNode.is_stock = 0;

			CStockPlateTree * pNode = pPlates->SearchSpecialNode(treeNode);
			if(pNode==NULL)
				pNode = pPlates;
			if(pNode)
			{
				//	该节点为根节点
				CStockPlateData spd = *pNode->get();
				spd.code = Int2String(0, "%08d");
				spd.parent_code = "-1";
				spd.is_stock = 0;
				spd.company_number = -1;		//	作为树的层级使用
				if(spd.name.empty())
					spd.name = "根目录";
				vecStockPlateData.push_back(spd);

				CStockPlateTree::pre_order_iterator pre_order_it = pNode->pre_order_begin();
				while(pre_order_it!=pNode->pre_order_end())
				{
					CStockPlateData spd = *pre_order_it;
					if(spd.is_stock==0)
					{
						if(pre_order_it.node()->parent()->is_root()==false)
						{
							spd.parent_code = pre_order_it.node()->parent()->get()->code;
						}
						else
						{
							spd.parent_code = Int2String(0, "%08d");
						}
						if(spd.code.length()==1)
							spd.company_number = 0;		//	门类
						else
							spd.company_number = 1;		//	大类
						vecStockPlateData.push_back(spd);
					}
					
					pre_order_it++;
				}

			}
		}

		HTREEITEM hRoot;
		map<string, HTREEITEM> mapHTree;
		for(int i =0 ; i<vecStockPlateData.size(); i++)
		{
			CString str;
			if(vecStockPlateData[i].is_stock==1)
			{
				str.Format("%s-%d(%s)",vecStockPlateData[i].name.c_str(),\
					vecStockPlateData[i].company_number, vecStockPlateData[i].code.c_str());
			}
			else
			{
				str.Format("%s-%d(%s)",vecStockPlateData[i].name.c_str(),\
					vecStockPlateData[i].company_number, vecStockPlateData[i].code.c_str());
			}

			if(vecStockPlateData[i].parent_code== "-1")
			{
				HTREEITEM hClass = m_TreeCsrc.InsertItem(str, 1, 1);
				hRoot = hClass;

				CString key;
				if(vecStockPlateData[i].is_stock==1)
					key.Format("%s-%d", vecStockPlateData[i].code.c_str(), vecStockPlateData[i].is_stock);
				else
					key.Format("%s-%d", vecStockPlateData[i].code.c_str(), vecStockPlateData[i].is_stock);
				mapHTree.insert(make_pair(string(key.GetBuffer()), hClass));

				m_TreeCsrc.SetItemData(hClass, (DWORD_PTR)vecStockPlateData[i].company_number);
			}
			else
			{
				CString key;
				key.Format("%s-%d", vecStockPlateData[i].parent_code.c_str(), 0);
				HTREEITEM hParent = mapHTree[string(key.GetBuffer())];
				HTREEITEM hClass = m_TreeCsrc.InsertItem(str, hParent, TVI_LAST);

				if(vecStockPlateData[i].is_stock==1)
					key.Format("%s-%d", vecStockPlateData[i].code.c_str(), vecStockPlateData[i].is_stock);
				else
					key.Format("%s-%d", vecStockPlateData[i].code.c_str(), vecStockPlateData[i].is_stock);
				mapHTree.insert(make_pair(string(key.GetBuffer()), hClass));

				m_TreeCsrc.SetItemData(hClass, (DWORD_PTR)vecStockPlateData[i].company_number);
			}
		}

		m_TreeCsrc.Expand(hRoot, TVE_EXPAND /*TVE_COLLAPSE*/);
		if(m_RadioCsrc<0)
			m_TreeCsrc.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CStockGroupDlg::DisplayCheckBoxByRadio(HTREEITEM hitem, int nRadio)
{
	if(hitem != NULL)
	{
		HTREEITEM old_hitem = hitem;

		int nLevel = (int)m_TreeReg.GetItemData(hitem);
		if(nLevel==nRadio)
		{
			// 显示“勾选”框，默认为“选中”状态
			m_TreeReg.SetItemState(hitem, 0x2000,  TVIS_STATEIMAGEMASK);
		}
		else
		{
			//	所有省份去除“勾选”框，不让选择
			m_TreeReg.SetItemState(hitem, 0,  TVIS_STATEIMAGEMASK);
		}

		hitem = m_TreeReg.GetChildItem(hitem);
		while(hitem)
		{
			DisplayCheckBoxByRadio(hitem, nRadio);
			hitem = m_TreeReg.GetNextItem(hitem, TVGN_NEXT);
		}

		if(m_TreeReg.ItemHasChildren(old_hitem))
			m_TreeReg.Expand(old_hitem, TVE_EXPAND /*TVE_COLLAPSE*/);
	}
}

//递归反向选择“勾选框”
void CStockGroupDlg::UpdateCheckBoxByRadio(HTREEITEM hitem, int nRadio)
{
	if(hitem!=NULL)
	{
		int nLevel = m_TreeReg.GetItemData(hitem);
		if (nLevel < nRadio)
		{
			HTREEITEM hChildItem = NULL; 
			if (m_TreeReg.ItemHasChildren(hitem)) 
			{ 
				hChildItem =m_TreeReg.GetChildItem(hitem); 
				while(hChildItem!=NULL) 
				{ 
					
					UpdateCheckBoxByRadio(hChildItem, nRadio);
					hChildItem =m_TreeReg.GetNextItem(hChildItem, TVGN_NEXT);    
				}    
			} 
		}
		else if(nLevel==nRadio)
		{
			bool oldStatus = m_TreeReg.GetCheck(hitem);
			m_TreeReg.SetCheck(hitem, !oldStatus);
		}
	}
	
}


void CStockGroupDlg::GetSelectedHitemByRadio(HTREEITEM hitem, int nRadio, vector<string> & vecItem)
{
	if(hitem != NULL)
	{
		int nLevel = (int)m_TreeReg.GetItemData(hitem);
		bool bChecked = m_TreeReg.GetCheck(hitem);
		if(nLevel==nRadio && bChecked)
		{
			CString text = m_TreeReg.GetItemText(hitem);
			vecItem.push_back(text.GetBuffer());
		}
		
		hitem = m_TreeReg.GetChildItem(hitem);
		while(hitem)
		{
			GetSelectedHitemByRadio(hitem, nRadio, vecItem);
			hitem = m_TreeReg.GetNextItem(hitem, TVGN_NEXT);
		}
	}
}

void CStockGroupDlg::OnBnClickedRadioReg()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);
	if(m_RadioReg>=0)
		m_TreeReg.EnableWindow(TRUE);

	/*遍历所有的树节点，对应的层级显示“勾选”框，其余隐藏“勾选”框*/
	HTREEITEM hroot = m_TreeReg.GetRootItem();
	DisplayCheckBoxByRadio(hroot, m_RadioReg);

	m_TreeReg.Expand(hroot, TVE_COLLAPSE /*TVE_COLLAPSE*/);
	m_TreeReg.Expand(hroot, TVE_EXPAND /*TVE_COLLAPSE*/);
}



void CStockGroupDlg::OnNMClickTreeReg(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	CPoint point;
	GetCursorPos(&point);			//获得鼠标点击的位置
	m_TreeReg.ScreenToClient(&point);	//转化为客户坐标
	UINT uFlags;

	HTREEITEM CurrentItem;
	CurrentItem=m_TreeReg.HitTest(point,&uFlags);//获得当前点击节点的ITEM

	UpdateData(TRUE);
	UpdateCheckBoxByRadio(CurrentItem,m_RadioReg);

	m_TreeReg.Expand(CurrentItem, TVE_COLLAPSE /*TVE_COLLAPSE*/);
	m_TreeReg.Expand(CurrentItem, TVE_EXPAND /*TVE_COLLAPSE*/);

	*pResult = 0;
}



void CStockGroupDlg::OnBnClickedRadioCsrc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(m_RadioCsrc>=0)
		m_TreeCsrc.EnableWindow(TRUE);

	/*遍历所有的树节点，对应的层级显示“勾选”框，其余隐藏“勾选”框*/
	HTREEITEM hroot = m_TreeCsrc.GetRootItem();
	DisplayCheckBoxByRadio(hroot, m_RadioCsrc);

	m_TreeCsrc.Expand(hroot, TVE_COLLAPSE /*TVE_COLLAPSE*/);
	m_TreeCsrc.Expand(hroot, TVE_EXPAND /*TVE_COLLAPSE*/);

	
}


void CStockGroupDlg::OnNMClickTreeCsrc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;
	GetCursorPos(&point);			//获得鼠标点击的位置
	m_TreeCsrc.ScreenToClient(&point);	//转化为客户坐标
	UINT uFlags;

	HTREEITEM CurrentItem;
	CurrentItem=m_TreeCsrc.HitTest(point,&uFlags);//获得当前点击节点的ITEM

	UpdateData(TRUE);
	UpdateCheckBoxByRadio(CurrentItem,m_RadioCsrc);

	m_TreeCsrc.Expand(CurrentItem, TVE_COLLAPSE /*TVE_COLLAPSE*/);
	m_TreeCsrc.Expand(CurrentItem, TVE_EXPAND /*TVE_COLLAPSE*/);

	*pResult = 0;
}


void CStockGroupDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData(TRUE);

	HTREEITEM hroot = m_TreeReg.GetRootItem();
	if(m_RadioReg==1)
		GetSelectedHitemByRadio(hroot, m_RadioReg, m_vecSelectedItemReg);

	hroot = m_TreeCsrc.GetRootItem();
	if(m_RadioCsrc==0)
		GetSelectedHitemByRadio(hroot, m_RadioCsrc, m_vecSelectedItemCsrcGate);
	if(m_RadioCsrc==1)
		GetSelectedHitemByRadio(hroot, m_RadioCsrc, m_vecSelectedItemCsrcBig);

	CDialog::OnOK();
}