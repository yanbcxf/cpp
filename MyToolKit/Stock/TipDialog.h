#pragma once



// CTipDialog 对话框

class CTipDialog : public CDialog
{
	DECLARE_DYNAMIC(CTipDialog)

public:
	CTipDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTipDialog();

// 对话框数据
	enum { IDD = IDD_TIP_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_Grid;
	list<CTipRecord>	m_lstTip;
	virtual BOOL OnInitDialog();
};
