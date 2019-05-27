#pragma once
#include "afxcmn.h"


// CPlateSelectDlg 对话框

class CPlateSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlateSelectDlg)

public:
	CPlateSelectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlateSelectDlg();

// 对话框数据
	enum { IDD = IDD_PLATE_SELECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	void updateCheckBox(HTREEITEM hitem, bool newStatus);
	void checkToTree(HTREEITEM hitem);
	map<string, string>	GetPlateCodes(int & is_stock);
public:
	//	决定 展示出来提供选择的板块信息
	string	m_nCode;
	int		m_nPlateType;

	//	保存对话框选择的结果
	vector<string>	m_vec_code;
	string	m_strGraphOrTable;	//	图表 或者 表格展示
	int		m_nIs_Stock;		//	结果中保存的 代码类型 ， 0 板块  1 个股

public:
	CTreeCtrl m_Tree;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnGraph();
	afx_msg void OnBnClickedBtnTable();
	afx_msg void OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult);
};
