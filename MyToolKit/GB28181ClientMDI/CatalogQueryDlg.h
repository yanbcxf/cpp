#pragma once


// CCatalogQueryDlg 对话框

class CCatalogQueryDlg : public CDialog
{
	DECLARE_DYNAMIC(CCatalogQueryDlg)

public:
	CCatalogQueryDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCatalogQueryDlg();

// 对话框数据
	enum { IDD = IDD_CATALOGQUERYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
