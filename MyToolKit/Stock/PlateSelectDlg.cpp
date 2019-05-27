// PlateSelectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "PlateSelectDlg.h"


// CPlateSelectDlg 对话框

IMPLEMENT_DYNAMIC(CPlateSelectDlg, CDialog)

CPlateSelectDlg::CPlateSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlateSelectDlg::IDD, pParent)
{

}

CPlateSelectDlg::~CPlateSelectDlg()
{
}

void CPlateSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE2, m_Tree);
}


BEGIN_MESSAGE_MAP(CPlateSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_GRAPH, &CPlateSelectDlg::OnBnClickedBtnGraph)
	ON_BN_CLICKED(IDC_BTN_TABLE, &CPlateSelectDlg::OnBnClickedBtnTable)
	ON_NOTIFY(NM_CLICK, IDC_TREE2, &CPlateSelectDlg::OnNMClickTree2)
END_MESSAGE_MAP()


// CPlateSelectDlg 消息处理程序

BOOL CPlateSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	HTREEITEM hRoot;
	//	展示 各个省份
	{
		map<string, string> map_RegName = GetPlateCodes(m_nIs_Stock);

		CString str;
		str.Format("%s-(%s)",  "中国", "86");
		HTREEITEM hClass = m_Tree.InsertItem(str, 1, 1);
		hRoot = hClass;
		m_Tree.SetItemData(hClass, 0);

		map<string, string>::iterator it = map_RegName.begin();
		for(; it!=map_RegName.end(); it++)
		{
			str.Format("%s-(%s)",  it->second.c_str(), it->first.c_str());
			HTREEITEM hClass = m_Tree.InsertItem(str,hRoot, TVI_LAST);
			m_Tree.SetItemData(hClass, 1);
		}

		m_Tree.Expand(hRoot, TVE_EXPAND /*TVE_COLLAPSE*/);

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

map<string, string> CPlateSelectDlg::GetPlateCodes(int & is_stock)
{
	JMutexAutoLock lock(((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_mutex);

	map<string, string>		map_codes;

	CStockPlateTree * pPlates = NULL;
	if(m_nPlateType==ID_PLATE_TYPE_SOHU)
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_pCSouhuStockPlates;
	}
	else if(m_nPlateType == ID_PLATE_TYPE_SHENZHEN_INDEXINFO)
	{
		pPlates = ((CStockApp *)AfxGetApp())->m_ShenzhenIndexList.m_pTree;
	}
	else
	{
		return map_codes;
	}
	

	CStockPlateData treeNode;
	treeNode.is_stock = 0;
	treeNode.code = m_nCode;

	CStockPlateTree * pNode = pPlates->SearchSpecialNode(treeNode);
	if(pNode==NULL)
		pNode = pPlates;		

	CStockPlateData spd = *pNode->get();
	
	CStockPlateTree::iterator child_it = pNode->begin();
	while(child_it!=pNode->end())
	{
		CStockPlateData spd = *child_it;
		map_codes[spd.code] = spd.name;
		is_stock = spd.is_stock;
		child_it++;
	}
	return map_codes;
}

void CPlateSelectDlg::OnBnClickedBtnGraph()
{
	// TODO: 在此添加控件通知处理程序代码

	//	递归获取选中的 节点
	m_vec_code.clear();

	HTREEITEM root = m_Tree.GetRootItem();
	checkToTree(root);

	if(m_vec_code.size()>0)
	{
		m_strGraphOrTable = "Graph";
		CDialog::OnOK();
	}

}

void CPlateSelectDlg::OnBnClickedBtnTable()
{
	// TODO: 在此添加控件通知处理程序代码

	m_vec_code.clear();

	HTREEITEM root = m_Tree.GetRootItem();
	checkToTree(root);

	if(m_vec_code.size()>0)
	{
		m_strGraphOrTable = "Table";
		CDialog::OnOK();
	}
}



void CPlateSelectDlg::updateCheckBox(HTREEITEM hitem, bool newStatus)
{
	if(hitem != NULL)
	{
		hitem = m_Tree.GetChildItem(hitem);
		while(hitem)
		{
			m_Tree.SetCheck(hitem, newStatus);
			updateCheckBox(hitem, newStatus);
			hitem = m_Tree.GetNextItem(hitem, TVGN_NEXT);
		}
	}
}



void CPlateSelectDlg::checkToTree(HTREEITEM hitem)
{
	if(hitem != NULL)
	{
		hitem = m_Tree.GetChildItem(hitem);
		while(hitem)
		{
			// is_stock 代表是否为 叶子节点
			int is_stock = (int)m_Tree.GetItemData(hitem);
			if(m_Tree.GetCheck(hitem) && is_stock ==1)
			{
				CString text = m_Tree.GetItemText(hitem);

				vector<string> firstMatch;
				string strPattern = string("\\-\\([A-Za-z\\d]+\\)");
				if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )>0)
				{
					string::size_type pos1 = firstMatch[0].find(")");
					string codeStr = firstMatch[0].substr(2, pos1-2);
					m_vec_code.push_back(codeStr);
				}
			}
			checkToTree(hitem);
			hitem = m_Tree.GetNextItem(hitem, TVGN_NEXT);
		}
	}
}


void CPlateSelectDlg::OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult)
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

		int is_stock = (int)m_Tree.GetItemData(CurrentItem);
		if(is_stock==0)
		{
			bool oldStatus = m_Tree.GetCheck(CurrentItem);
			m_Tree.SetCheck(CurrentItem, !oldStatus);
			updateCheckBox(CurrentItem, !oldStatus);
		}
		else
		{
			m_Tree.SetCheck(CurrentItem, !m_Tree.GetCheck(CurrentItem));
		}
	}

	*pResult = 0;
}
