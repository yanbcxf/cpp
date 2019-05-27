 #if !defined _HYPERLINK_H_
#define _HYPERLINK_H_

#define  BN_CLICK_STATIC  WM_USER + 1300

class CHyperLink : public CStatic 
{ 
// Construction/destruction 
public: 
    CHyperLink(); 
    virtual ~CHyperLink(); 
 
// Attributes 
public: 
 
// Operations 
public:  
    void SetURL(CString strWindowText,CString strURL,BOOL bClick = TRUE); 
    CString GetURL() const; 
 
    void SetColours(COLORREF crLinkColour, COLORREF crVisitedColour, COLORREF crHoverColour = -1); 
    COLORREF GetLinkColour() const; 
    COLORREF GetVisitedColour() const; 
    COLORREF GetHoverColour() const; 
 
    void SetVisited(BOOL bVisited = TRUE); 
    BOOL GetVisited() const; 
 
    void SetLinkCursor(HCURSOR hCursor); 
    HCURSOR GetLinkCursor() const; 
 
    void SetUnderline(BOOL bUnderline = TRUE); 
    BOOL GetUnderline() const; 
 
// Overrides 
    // ClassWizard generated virtual function overrides 
    //{{AFX_VIRTUAL(CHyperLink) 
    public: 
    virtual BOOL PreTranslateMessage(MSG* pMsg); 
    protected: 
    virtual void PreSubclassWindow(); 
    //}}AFX_VIRTUAL 
 
// Implementation 
protected: 
    HINSTANCE GotoURL(LPCTSTR url, int showcmd); 
    void SetDefaultCursor(); 
 
// Protected attributes 
protected: 
    COLORREF m_crLinkColour, m_crVisitedColour;     // Hyperlink colours 
    COLORREF m_crHoverColour;                       // Hover colour 
    BOOL     m_bOverControl;                        // cursor over control? 
    BOOL     m_bVisited;                            // Has it been visited? 
    BOOL     m_bUnderline;                          // underline hyperlink?   
    CString  m_strURL;                              // hyperlink URL
	CString  m_strWindowText;						// 
    HCURSOR  m_hLinkCursor;                         // Cursor for hyperlink 
    CToolTipCtrl m_ToolTip;                         // The tooltip 
	BOOL	m_bClick;								// Enable Click Event.
 
    // Generated message map functions 
protected: 
    //{{AFX_MSG(CHyperLink) 
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor); 
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message); 
    afx_msg void OnMouseMove(UINT nFlags, CPoint point); 
    //}}AFX_MSG 
    afx_msg void OnClicked(); 
    DECLARE_MESSAGE_MAP() 
};  

#endif


