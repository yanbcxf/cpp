
// HiredisTestDlg.h: 头文件
//

#pragma once


// CHiredisTestDlg 对话框
class CHiredisTestDlg : public CDialog
{
// 构造
public:
	CHiredisTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HIREDISTEST_DIALOG };
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
	afx_msg void OnBnClickedBtnStringTest();
	afx_msg void OnBnClickedBtnListTest();
	afx_msg void OnBnClickedBtnSubscribeTest();
	afx_msg void OnBnClickedBtnPublishTest();

public:
	bool	m_bListening;
	bool	m_bWatching;
	afx_msg void OnBnClickedBtnTransactionTest();
	afx_msg void OnBnClickedBtnWatchTest();
	afx_msg void OnBnClickedBtnLockTest();

	bool	m_bThread_1;
	bool	m_bThread_2;
	bool	m_bThread_3;
	bool	m_bThread_4;
	bool	m_bThread_5;

	bool	m_bThread_6;
	bool	m_bThread_7;
	bool	m_bThread_8;
	bool	m_bThread_9;
	bool	m_bThread_10;
	afx_msg void OnBnClickedBtnSemaphoreTest();
};

