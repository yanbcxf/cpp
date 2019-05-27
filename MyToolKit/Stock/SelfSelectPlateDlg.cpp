// SelfSelectPlateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "SelfSelectPlateDlg.h"


// CSelfSelectPlateDlg 对话框

IMPLEMENT_DYNAMIC(CSelfSelectPlateDlg, CDialog)

CSelfSelectPlateDlg::CSelfSelectPlateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelfSelectPlateDlg::IDD, pParent)
{

}

CSelfSelectPlateDlg::~CSelfSelectPlateDlg()
{
}

void CSelfSelectPlateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
}


BEGIN_MESSAGE_MAP(CSelfSelectPlateDlg, CDialog)
END_MESSAGE_MAP()


// CSelfSelectPlateDlg 消息处理程序

BOOL CSelfSelectPlateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_nPlateCode = -1;
	vector<CStockPlateData> vecStockPlateData;

	//	先序遍历
	{
		JMutexAutoLock lock(((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_mutex);
		CStockPlateTree * pPlates = ((CStockApp *)AfxGetApp())->m_pCSelfSelectStockPlates->m_pTree;

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

	//	展开根节点
	if(vecStockPlateData.size()>0)
		m_Tree.Expand(hRoot, TVE_EXPAND /*TVE_COLLAPSE*/);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelfSelectPlateDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	HTREEITEM hTreeItem = m_Tree.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CString text = m_Tree.GetItemText(hTreeItem);
		//AfxMessageBox(text);

		vector<string> firstMatch;
		string strPattern = string("\\(\\d+\\)");
		if(Pcre2Grep(strPattern.c_str(), text.GetBuffer(),firstMatch )<=0)
		{
			AfxMessageBox("获取代码错误");		
			return ;
		}

		int is_stock = (int)m_Tree.GetItemData(hTreeItem);
		string::size_type pos1 = firstMatch[0].find(")");
		string codeStr = firstMatch[0].substr(1, pos1-1);
		int code = atoi(codeStr.c_str());

		if(code!=0 && is_stock==0)
		{
			m_nPlateCode = code;
			
			m_sPlateName = string(text.GetBuffer());
			pos1 = m_sPlateName.find(firstMatch[0]);
			if(pos1!=string::npos)
			{	
				m_sPlateName = m_sPlateName.substr(0, pos1);
			}


			CDialog::OnOK();
			return;
		}
	}
	AfxMessageBox(_T("请选择需要的自选板块!"));
}
