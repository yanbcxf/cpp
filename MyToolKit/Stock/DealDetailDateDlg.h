#pragma once

// CDealDetailDateDlg 对话框

class CDealDetailDateDlg : public CDialog
{
	DECLARE_DYNAMIC(CDealDetailDateDlg)

public:
	CDealDetailDateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDealDetailDateDlg();

// 对话框数据
	enum { IDD = IDD_DEAL_DETAIL_DATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_Grid;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();

public:
	string	m_strDealDetailDate;
};
