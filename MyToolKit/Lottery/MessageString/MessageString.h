// MessageString.h: interface for the CMessageString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGESTRING_H__8D4B2F14_C5DB_49C0_B7C1_2E4B3D22440E__INCLUDED_)
#define AFX_MESSAGESTRING_H__8D4B2F14_C5DB_49C0_B7C1_2E4B3D22440E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MessageStringManager.h"

// this is the result we will return from SendMessage
// if the target window is not in this process
#define MESSAGESTRING_NOT_SENT			-99999

// this is the result returned by receivers if an invalid
// CMessageString is send
#define MESSAGESTRING_NOT_RECEIVED		-99998

class CMessageString : public CString
{
public:
	CMessageString();
	CMessageString(const CString& string);
	CMessageString( TCHAR ch, int nRepeat = 1 );
	CMessageString( LPCTSTR lpch, int nLength );
	CMessageString( const unsigned char* psz );
	CMessageString( LPCWSTR lpsz );
	CMessageString( LPCSTR lpsz );
	virtual ~CMessageString();

public:
	void ForceCleanup();
	static BOOL IsStringValid(CMessageString* pStringToCheck);
	LRESULT SendAsWParam(HWND hwndTarget, UINT uiMessage, LPARAM lParam = NULL);
	BOOL PostAsWParam(HWND hwndTarget, UINT uiMessage, LPARAM lParam = NULL);

	LRESULT SendAsLParam(HWND hwndTarget, UINT uiMessage, WPARAM wParam = NULL);
	BOOL PostAsLParam(HWND hwndTarget, UINT uiMessage, WPARAM wParam = NULL);

protected:
	BOOL IsWindowInThisProcess(HWND hWnd);
	static CMessageStringManager m_Manager;
};

#endif // !defined(AFX_MESSAGESTRING_H__8D4B2F14_C5DB_49C0_B7C1_2E4B3D22440E__INCLUDED_)
