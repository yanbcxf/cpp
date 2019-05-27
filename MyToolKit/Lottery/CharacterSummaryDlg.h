#pragma once
#include "afxcmn.h"
#include "../GridCtrl_src/GridCtrl.h"
#include "../GridCtrl_src/TreeGridCtrl.h"


// CPDFStructureDlg 对话框

class CPDFStructureDlg : public CDialog
{
	DECLARE_DYNAMIC(CPDFStructureDlg)

public:
	CPDFStructureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPDFStructureDlg();

// 对话框数据
	enum { IDD = IDD_PDF_STRUCTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	CTreeGridCtrl m_Grid;
	string		m_announcementId;

	virtual BOOL OnInitDialog();
};
