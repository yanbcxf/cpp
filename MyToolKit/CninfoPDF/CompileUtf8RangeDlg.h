#pragma once


// CCompileUtf8RangeDlg 对话框

class CCompileUtf8RangeDlg : public CMyDialog
{
	DECLARE_DYNAMIC(CCompileUtf8RangeDlg)

public:
	CCompileUtf8RangeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCompileUtf8RangeDlg();

// 对话框数据
	enum { IDD = IDD_COMPILE_UTF8_RANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strStart;
	CString m_strEnd;
	CString m_strResult;
	afx_msg void OnBnClickedBtnCaculate();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
