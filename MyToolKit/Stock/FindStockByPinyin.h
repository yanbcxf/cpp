#pragma once
#include "afxwin.h"


// CFindStockByPinyin 对话框

class CFindStockByPinyin : public CDialog
{
	DECLARE_DYNAMIC(CFindStockByPinyin)

public:
	CFindStockByPinyin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindStockByPinyin();

// 对话框数据
	enum { IDD = IDD_FIND_BY_PINYIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBoxExt m_Combo1;
	CString m_strPinyin;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
