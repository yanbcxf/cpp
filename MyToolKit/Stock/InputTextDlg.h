#pragma once


// CInputTextDlg 对话框

class CInputTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputTextDlg)

public:
	CInputTextDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputTextDlg();

// 对话框数据
	enum { IDD = IDD_INPUTTEXTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strInputText;
};
