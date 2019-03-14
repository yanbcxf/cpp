#pragma once



// CFfplaySdl2FormView form view

class CFfplaySdl2FormView : public CFormView
{
	DECLARE_DYNCREATE(CFfplaySdl2FormView)

protected:
	CFfplaySdl2FormView();           // protected constructor used by dynamic creation
	virtual ~CFfplaySdl2FormView();

public:
	enum { IDD = IDD_FFPLAYSDL2FORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStreamOpen();

	CStatic	m_black;
	afx_msg void OnPlayertestChangesize();
};


