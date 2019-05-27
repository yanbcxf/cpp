#pragma once
#include "tabsheet.h"

// CTabDialog 对话框

class CTabDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabDialog)

public:
	CTabDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTabDialog();

// 对话框数据
	enum { IDD = IDD_TAB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	HICON m_hIcon;

	// 生成的消息映射函数
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT	OnNotifyIcon(WPARAM wParam,LPARAM lParam);
	void DeleteTray();
	void ToTray();

	DECLARE_MESSAGE_MAP()
public:
	CTabSheet m_TabCtrl;

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
