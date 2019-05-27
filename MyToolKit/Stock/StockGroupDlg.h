#pragma once
#include "afxcmn.h"


// CStockGroupDlg 对话框

class CStockGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(CStockGroupDlg)

public:
	CStockGroupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStockGroupDlg();

// 对话框数据
	enum { IDD = IDD_STOCK_GROUP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void DisplayCheckBoxByRadio(HTREEITEM hitem, int nRadio);
	void UpdateCheckBoxByRadio(HTREEITEM hitem, int nRadio);
	void GetSelectedHitemByRadio(HTREEITEM hitem, int nRadio, vector<string> & vecItem);

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioReg();
	afx_msg void OnNMClickTreeReg(NMHDR *pNMHDR, LRESULT *pResult);
	
	vector<string>	m_vecSelectedItemReg;	//	选中的区域编码
	vector<string>	m_vecSelectedItemCsrcGate;	//	选中的证监会行业 门类编码
	vector<string>	m_vecSelectedItemCsrcBig;	//	选中的证监会行业 大类编码
protected:
	virtual void OnOK();

public:
	int m_RadioCsrc;
	int m_RadioReg;
	CTreeCtrl m_TreeCsrc;
	CTreeCtrl m_TreeReg;
	afx_msg void OnBnClickedRadioCsrc();
	afx_msg void OnNMClickTreeCsrc(NMHDR *pNMHDR, LRESULT *pResult);
};
