#pragma once


// CMyDialog 对话框

class CMyDialog : public CDialog
{
public:
	CMyDialog(UINT id, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDialog();

	virtual void ProcessMessage(string strMsg);
};
