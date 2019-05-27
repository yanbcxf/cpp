#pragma once
#include "afxcmn.h"


// CSelfSelectPlateDlg 对话框

class CSelfSelectPlateDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelfSelectPlateDlg)

public:
	CSelfSelectPlateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelfSelectPlateDlg();

// 对话框数据
	enum { IDD = IDD_SELFSELECTPLATEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	int		m_nPlateCode;
	string	m_sPlateName;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
