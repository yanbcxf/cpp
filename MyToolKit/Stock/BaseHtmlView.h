#pragma once

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CHtmlView��"
#endif 

// CBaseHtmlView Html ��ͼ

class CBaseHtmlView : public CHtmlView
{
	DECLARE_DYNCREATE(CBaseHtmlView)

protected:
public:
	CBaseHtmlView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CBaseHtmlView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

