#pragma once
#include "afxcmn.h"


// CStockRangeSet 对话框

class CStockRangeSet : public CDialog
{
	DECLARE_DYNAMIC(CStockRangeSet)

public:
	CStockRangeSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStockRangeSet();

// 对话框数据
	enum { IDD = IDD_STOCKRANGESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	COleDateTime m_StartDate;
	COleDateTime m_End_Date;
	CListCtrl m_listViewType;
	string		m_strViewType;

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
