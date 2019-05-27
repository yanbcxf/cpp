// MessageString.cpp: implementation of the CMessageString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageString.h"

#include "MessageStringManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////

// we will overide all of the standard CString defined constructors
// and implement the m_Manager.Add method to put the CMessageString
// under management to prevent it from being leaked
CMessageString::CMessageString()
{
	m_Manager.Add(this);
}

CMessageString::CMessageString(const CString& string)
	: CString(string)
{
	m_Manager.Add(this);
}

CMessageString::CMessageString( TCHAR ch, int nRepeat )
	: CString(ch, nRepeat)	
{
	m_Manager.Add(this);
}

CMessageString::CMessageString( LPCTSTR lpch, int nLength )
	: CString(lpch, nLength)
{
	m_Manager.Add(this);
}

CMessageString::CMessageString( const unsigned char* psz )
	: CString (psz)
{
	m_Manager.Add(this);
}

CMessageString::CMessageString( LPCWSTR lpsz )
	: CString(lpsz)
{
	m_Manager.Add(this);
}

CMessageString::CMessageString( LPCSTR lpsz )
	: CString (lpsz)
{
	m_Manager.Add(this);
}

//////////////////////////////////////////////////////////////////////
// Destruction
//////////////////////////////////////////////////////////////////////

CMessageString::~CMessageString()
{
	m_Manager.Delete(this);
}

// static member CMessageStringManager used as a
// garbage collector to insure app does not leak
// any CMessageString objects
CMessageStringManager CMessageString::m_Manager;

// the following functions are used to post and send
// CMessageString objects via the WPARAM or LPARAM paramater
LRESULT CMessageString::SendAsWParam(HWND hwndTarget, UINT uiMessage, LPARAM lParam)
{
	// we can only send messages to windows within this process
	// allowing a message to be sent otherwise, could cause very bad results
	if (IsWindowInThisProcess(hwndTarget))
	{
		return ::SendMessage(hwndTarget, uiMessage, (WPARAM)this, lParam);	
	}
	else
	{
		TRACE("Attempted to send (as wParam) to invalid window. Address: %u, Text: %s\n", (DWORD)this, (LPCTSTR)*this);
		ASSERT(FALSE);
		return MESSAGESTRING_NOT_SENT;
	}
}

BOOL CMessageString::PostAsWParam(HWND hwndTarget, UINT uiMessage, LPARAM lParam)
{
	// we can only send messages to windows within this process
	// allowing a message to be sent otherwise, could cause very bad results
	if (IsWindowInThisProcess(hwndTarget))
	{
		return ::PostMessage(hwndTarget, uiMessage, (WPARAM)this, lParam);
	}
	else
	{
		TRACE("Attempted to post (as wParam) to invalid window. Address: %u, Text: %s\n", (DWORD)this, (LPCTSTR)*this);
		//ASSERT(FALSE);
		return FALSE;
	}
}

LRESULT CMessageString::SendAsLParam(HWND hwndTarget, UINT uiMessage, WPARAM wParam)
{
	// we can only send messages to windows within this process
	// allowing a message to be sent otherwise, could cause very bad results
	if (IsWindowInThisProcess(hwndTarget))
	{
		return ::SendMessage(hwndTarget, uiMessage, wParam, (LPARAM)this);	
	}
	else
	{
		TRACE("Attempted to send (as lParam) to invalid window. Address: %u, Text: %s\n", (DWORD)this, (LPCTSTR)*this);
		ASSERT(FALSE);
		return MESSAGESTRING_NOT_SENT;
	}
}

BOOL CMessageString::PostAsLParam(HWND hwndTarget, UINT uiMessage, WPARAM wParam)
{
	// we can only send messages to windows within this process
	// allowing a message to be sent otherwise, could cause very bad results
	if (IsWindowInThisProcess(hwndTarget))
	{
		return ::PostMessage(hwndTarget, uiMessage, wParam, (LPARAM)this);	
	}
	else
	{
		TRACE("Attempted to post (as lParam) to invalid window. Address: %u, Text: %s\n", (DWORD)this, (LPCTSTR)*this);
		ASSERT(FALSE);
		return FALSE;
	}
}

// helper function used to insure target thread is
// in same process
BOOL CMessageString::IsWindowInThisProcess(HWND hWnd)
{
	BOOL bReturn = TRUE;
	DWORD dwWindowProcessID = 0;

	// assert window is valid
	//ASSERT(::IsWindow(hWnd));
	if(::IsWindow(hWnd))
	{
		// get the process ID of the specified window
		// ignore the thread ID
		GetWindowThreadProcessId(hWnd, &dwWindowProcessID);

		// verify returned process id against current process id
		return (dwWindowProcessID == GetCurrentProcessId());
	}
	else
		return FALSE;
	
}

// helper function used by receivers to insure that the
// received CMessageString pointer is a valid one
// NOTE: this is a static function
BOOL CMessageString::IsStringValid(CMessageString *pStringToCheck)
{
	return m_Manager.IsValid(pStringToCheck);
}

// helper function used to access the CMessageStringManager
// this function is useful
void CMessageString::ForceCleanup()
{
	m_Manager.ForceCleanup();
}
