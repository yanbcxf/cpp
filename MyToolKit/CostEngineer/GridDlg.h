#pragma once
#include "afxcmn.h"
#include "../GridCtrl_src/GridCtrl.h"
#include "../GridCtrl_src/TreeGridCtrl.h"


// CPDFStructureDlg 对话框

class CGridDlg : public CDialog
{
	DECLARE_DYNAMIC(CGridDlg)

public:
	CGridDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGridDlg();

// 对话框数据
	enum { IDD = IDD_GRID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	CTreeGridCtrl m_Grid;
	
	CString					m_strTitle;
	vector<string>			m_vecHeader;
	vector<vector<string>>	m_vecData;

	virtual BOOL OnInitDialog();
};
