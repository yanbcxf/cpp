#include "stdafx.h"
#include "CDyncDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////
// Note: Make sure nBufLen is big enough to hold your entire dialog template!
//
CDlgTemplateBuilder::CDlgTemplateBuilder(UINT nBufLen)
{
	m_pBuffer = new WORD[nBufLen];
	m_pNext = m_pBuffer;
	m_pEndBuf = m_pNext + nBufLen;
}

CDlgTemplateBuilder::~CDlgTemplateBuilder()
{
	delete[] m_pBuffer;
}

//////////////////
// Create template (DLGTEMPLATE)
//
DLGTEMPLATE* CDlgTemplateBuilder::Begin(DWORD dwStyle, const CRect& rc,
	LPCTSTR text, DWORD dwStyleEx)
{
	ASSERT(m_pBuffer == m_pNext);			 // call Begin first and only once!

	DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
	hdr->style = dwStyle;					 // copy style..
	hdr->dwExtendedStyle = dwStyleEx;	 // ..and extended, too
	hdr->cdit = 0;								 // number of items: zero

												 // Set dialog rectangle.
	CRect rcDlg = rc;
	hdr->x = (short)rcDlg.left;
	hdr->y = (short)rcDlg.top;
	hdr->cx = (short)rcDlg.Width();
	hdr->cy = (short)rcDlg.Height();

	// Append trailing items: menu, class, caption. I only use caption.
	m_pNext = (WORD*)(hdr + 1);
	*m_pNext++ = 0;							 // menu (none)
	*m_pNext++ = 0;							 // dialog class (use standard)
	m_pNext = AddText(m_pNext, text);	 // append dialog caption

	ASSERT(m_pNext < m_pEndBuf);
	return hdr;
}

//////////////////
// Add dialog item (control).
//
void CDlgTemplateBuilder::AddItemTemplate(LPCTSTR className, DWORD dwStyle,
	const CRect& rc, WORD nID, DWORD dwStyleEx)
{
	ASSERT(m_pNext < m_pEndBuf);

	// initialize DLGITEMTEMPLATE 
	DLGITEMTEMPLATE& it = *((DLGITEMTEMPLATE*)AlignDWORD(m_pNext));
	it.style = dwStyle;
	it.dwExtendedStyle = dwStyleEx;

	CRect rcDlg = rc;
	it.x = (short)rcDlg.left;
	it.y = (short)rcDlg.top;
	it.cx = (short)rcDlg.Width();
	it.cy = (short)rcDlg.Height();
	it.id = nID;

	// add class 
	m_pNext = (WORD*)(&it + 1);
	m_pNext = AddText(m_pNext, className);
	ASSERT(m_pNext < m_pEndBuf);			 // check not out of range

											 // increment control/item count
	DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
	hdr->cdit++;
}

void CDlgTemplateBuilder::AddItemTemplate(WORD wType, DWORD dwStyle,
	const CRect& rc, WORD nID, DWORD dwStyleEx)
{
	ASSERT(m_pNext < m_pEndBuf);

	// initialize DLGITEMTEMPLATE 
	DLGITEMTEMPLATE& it = *((DLGITEMTEMPLATE*)AlignDWORD(m_pNext));
	it.style = dwStyle;
	it.dwExtendedStyle = dwStyleEx;

	CRect rcDlg = rc;
	it.x = (short)rcDlg.left;
	it.y = (short)rcDlg.top;
	it.cx = (short)rcDlg.Width();
	it.cy = (short)rcDlg.Height();
	it.id = nID;

	// add class (none)
	m_pNext = (WORD*)(&it + 1);
	*m_pNext++ = 0xFFFF;						 // next WORD is atom
	*m_pNext++ = wType;						 // ..atom identifier
	ASSERT(m_pNext < m_pEndBuf);			 // check not out of range

											 // increment control/item count
	DLGTEMPLATE* hdr = (DLGTEMPLATE*)m_pBuffer;
	hdr->cdit++;
}


//////////////////
// Add dialog item (control).
//
void CDlgTemplateBuilder::AddItem(WORD wType, DWORD dwStyle,
	const CRect& rc, LPCTSTR text, WORD nID, DWORD dwStyleEx)
{
	AddItemTemplate(wType, dwStyle, rc, nID, dwStyleEx);
	m_pNext = AddText(m_pNext, text);	 // append title
	*m_pNext++ = 0;							 // no creation data
	ASSERT(m_pNext < m_pEndBuf);
}

void CDlgTemplateBuilder::AddItem(LPCTSTR className, DWORD dwStyle,
	const CRect& rc, LPCTSTR text, WORD nID, DWORD dwStyleEx)
{
	AddItemTemplate(className, dwStyle, rc, nID, dwStyleEx);
	m_pNext = AddText(m_pNext, text);	 // append title
	*m_pNext++ = 0;							 // no creation data
	ASSERT(m_pNext < m_pEndBuf);
}

//////////////////
// Add dialog item (control).
//
void CDlgTemplateBuilder::AddItem(WORD wType, DWORD dwStyle,
	const CRect& rc, WORD wResID, WORD nID, DWORD dwStyleEx)
{
	AddItemTemplate(wType, dwStyle, rc, nID, dwStyleEx);
	*m_pNext++ = 0xFFFF;						 // next is resource id
	*m_pNext++ = wResID;						 // ..here it is
	*m_pNext++ = 0;							 // no extra stuff
	ASSERT(m_pNext < m_pEndBuf);
}

//////////////////
// Append text to buffer. Convert to Unicode if necessary.
// Return pointer to next character after terminating NULL.
//
WORD* CDlgTemplateBuilder::AddText(WORD* buf, LPCTSTR text)
{
	if (text) {
		USES_CONVERSION;
		wcscpy((WCHAR*)buf, T2W((LPTSTR)text));
		buf += wcslen((WCHAR*)buf) + 1;
	}
	else {
		*buf++ = 0;
	}
	return buf;
}
