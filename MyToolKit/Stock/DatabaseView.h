#pragma once



// CDatabaseView 窗体视图

class CDatabaseView : public CFormView
{
	DECLARE_DYNCREATE(CDatabaseView)

protected:
	CDatabaseView();           // 动态创建所使用的受保护的构造函数
	virtual ~CDatabaseView();

public:
	enum { IDD = IDD_DATABASEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};


