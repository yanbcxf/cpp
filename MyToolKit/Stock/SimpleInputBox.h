#pragma once
#include "afxwin.h"


// CSimpleInputBox 对话框

class CSimpleInputBox : public CDialog
{
	DECLARE_DYNAMIC(CSimpleInputBox)

public:
	CSimpleInputBox(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSimpleInputBox();

// 对话框数据
	enum { IDD = IDD_SIMPLE_INPUT_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCaption;
	virtual BOOL OnInitDialog();
	CComboBox m_ComboBox;
	CString m_strInput;

	vector<string>	m_ComboString;
};
