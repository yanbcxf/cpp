#pragma once
#include "afxcmn.h"
#include "afxwin.h"
//#include "d:\workspace_c3\mytoolkit\stock\gridctrl_src\gridctrl.h"


// CStatsGovMenuDlg 对话框

class CStatsGovMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CStatsGovMenuDlg)

public:
	CStatsGovMenuDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStatsGovMenuDlg();

// 对话框数据
	enum { IDD = IDD_STATSGOV_MENU_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	virtual BOOL OnInitDialog();
	CGridCtrl m_Grid;
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);

public :
	vector<string>	m_vec_zb_code;
	vector<string>	m_vec_reg_code;
	string			m_str_db_code;

protected:
	void BtnProccess();
	void checkToTree(HTREEITEM hitem);
	void updateCheckBox(HTREEITEM hitem, bool newStatus);

public:
	CTreeCtrl m_TreeReg;
	afx_msg void OnBnClickedBtnZb();
	afx_msg void OnBnClickedBtnReg();
	afx_msg void OnNMClickTree2(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_btnZb;
	int m_nOutput;
};
