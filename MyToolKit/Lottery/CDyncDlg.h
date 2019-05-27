// DynDlg.h: interface for the DynDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNDLG_H__5611A542_2805_4285_96AC_B0CC2851255E__INCLUDED_)
#define AFX_DYNDLG_H__5611A542_2805_4285_96AC_B0CC2851255E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class /*AFX_EXT_CLASS*/ CDlgTemplateBuilder {
protected:
	WORD* m_pBuffer;							 // internal buffer holds dialog template
	WORD* m_pNext;								 // next WORD to copy stuff
	WORD* m_pEndBuf;							 // end of buffer

	// align ptr to nearest DWORD
	WORD* AlignDWORD(WORD* ptr) {
		ptr++;									 // round up to nearest DWORD
		LPARAM lp = (LPARAM)ptr;			 // convert to long
		lp &= 0xFFFFFFFC;						 // make sure on DWORD boundary
		return (WORD*)lp;
	}

	void AddItemTemplate(LPCTSTR className, DWORD dwStyle, const CRect& rc,
		WORD nID, DWORD dwStyleEx);

	void AddItemTemplate(WORD wType, DWORD dwStyle, const CRect& rc,
		WORD nID, DWORD dwStyleEx);

public:
	//	COMOBOX 以左为基本的类型（含combobox）, 右边是本程序扩展的
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb775491(v=vs.85).aspx
	// https://msdn.microsoft.com/en-us/library/aa367854(v=vs.85).aspx
	// Windows predefined atom names
	// BUTTON 的分支 BS_GROUPBOX， BS_CHECKBOX
	// STATIC 的分支 SS_BITMAP
	enum { BUTTON=0x0080, EDIT, STATIC, LISTBOX, SCROLLBAR, COMBOBOX, CONTROL,
		CHECKBOX, RADIOBUTTON, ICON, BITMAP, 
		SPIN_EDIT };

	CDlgTemplateBuilder(UINT nBufLen=4096);
	~CDlgTemplateBuilder();

	DLGTEMPLATE* GetTemplate() { return (DLGTEMPLATE*)m_pBuffer; }

	// functions to build the template
	DLGTEMPLATE* Begin(DWORD dwStyle, const CRect& rc, LPCTSTR caption, DWORD dwStyleEx=0);
	WORD* AddText(WORD* buf, LPCTSTR text);
	void AddItem(WORD wType, DWORD dwStyle, const CRect& rc,
		LPCTSTR text, WORD nID=-1, DWORD dwStyleEx=0);
	void AddItem(WORD wType, DWORD dwStyle, const CRect& rc,
		WORD nResID, WORD nID=-1, DWORD dwStyleEx=0);

	void AddItem(LPCTSTR className, DWORD dwStyle, const CRect& rc,
		LPCTSTR text, WORD nID = -1, DWORD dwStyleEx = 0);
};

#endif // !defined(AFX_DYNDLG_H__5611A542_2805_4285_96AC_B0CC2851255E__INCLUDED_)
