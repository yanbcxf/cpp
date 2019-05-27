#pragma once
#include "afxwin.h"


// CFindStockByCode 对话框

class CFindStockByCode : public CDialog
{
	DECLARE_DYNAMIC(CFindStockByCode)

public:
	CFindStockByCode(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindStockByCode();

// 对话框数据
	enum { IDD = IDD_FIND_BY_CODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void SetStockInfo(CStockInfo * pStockInfo){ m_pStockInfo = pStockInfo; }
public:
	CStockInfo * m_pStockInfo;

	CComboBoxExt	m_Combo1;
	CString			m_strCode;
	CString			m_strCaption;
};
