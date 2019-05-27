#pragma once


// CIndi10jqkaBonusView 视图

class CIndi10jqkaBonusView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CIndi10jqkaBonusView)

protected:
public:
	CIndi10jqkaBonusView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndi10jqkaBonusView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();

	void RedrawGridCtrl(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);
};


