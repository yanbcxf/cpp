
// ShxShowDlg.h: 头文件
//

#pragma once


// CShxShowDlg 对话框
class CShxShowDlg : public CDialog
{
// 构造
public:
	CShxShowDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHXSHOW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_chShx;
	ShapeFont* m_tssdFont;
	unsigned short m_tssdCode;
	afx_msg void OnBnClickedShow();
	afx_msg void OnBnClickedCreateLff();
	afx_msg void OnBnClickedShowTssd();
	CString m_strPatch;
	afx_msg void OnBnClickedConvertTssdToLff();
};
