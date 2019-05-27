
// CninfoPDFDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CCninfoPDFDlg 对话框
class CCninfoPDFDlg : public CMyDialog
{
// 构造
public:
	CCninfoPDFDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CNINFOPDF_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	
	virtual BOOL OnInitDialog();

	virtual void ProcessMessage(string strMsg);


// 实现
protected:

	DECLARE_MESSAGE_MAP()

	
public:
	CRichEditCtrl m_ctlEdit;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	
};
