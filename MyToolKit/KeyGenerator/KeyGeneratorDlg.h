
// KeyGeneratorDlg.h : 头文件
//

#pragma once

#include "Step1.h"
#include "Step2.h"
#include "Step3.h"

// CKeyGeneratorDlg 对话框
class CKeyGeneratorDlg : public CDialog
{
// 构造
public:
	CKeyGeneratorDlg(CWnd* pParent = NULL);	// 标准构造函数

	CStep1 step1;
	CStep2 step2;
	CStep3 step3;

// 对话框数据
	enum { IDD = IDD_TEST_DIALOG };

	CString	m_str1;
	CString	m_str2;
	CString	m_str3;
	CString	m_key;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg HRESULT OnShowStepWnd(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnCloseWindow(WPARAM wParam, LPARAM lParam);
	afx_msg HRESULT OnGetSLC(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	void ShowStepWnd(int nStep);
};
