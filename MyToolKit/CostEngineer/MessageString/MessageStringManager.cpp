// MessageStringManager.cpp: implementation of the CMessageStringManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MessageStringManager.h"
#include "MessageString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageStringManager::CMessageStringManager()
{
	// the m_bCleaningUp member is used to prevent
	// the Delete function from removing an item
	// from the list while the ForceCleanup function
	// is running.  If this happened, problems would
	// occur
	m_bCleaningUp = FALSE;
	m_mutex.Init();
}

CMessageStringManager::~CMessageStringManager()
{
	// force cleanup
	ForceCleanup();
}

void CMessageStringManager::Add(CMessageString *pString)
{
	// we only add CMessageString* which are not already
	// under manageement
	JMutexAutoLock lock(m_mutex);
	if (!m_CurrentStrings.Find(pString))
		m_CurrentStrings.AddTail(pString);
}

void CMessageStringManager::Delete(CMessageString *pString)
{
	// if we are not in the ForceCleanup function, find
	// and remove CMessageString from the list
	JMutexAutoLock lock(m_mutex);
	if (!m_bCleaningUp)
	{
		POSITION Pos = m_CurrentStrings.Find(pString);
		ASSERT(Pos);

		if (Pos)
			m_CurrentStrings.RemoveAt(Pos);
	}
}

void CMessageStringManager::ForceCleanup()
{
	JMutexAutoLock lock(m_mutex);
	m_bCleaningUp = TRUE;

	POSITION Pos = m_CurrentStrings.GetHeadPosition();
	while (Pos)
	{
		CMessageString* pString = (CMessageString*)m_CurrentStrings.GetNext(Pos);

		// put out a trace message when in debug
#ifdef _DEBUG
		TRACE("CMessageString not deleted by handler, will be automatically deleted. Address: %u; Text: %s\n", (DWORD)pString, (LPCTSTR)*pString);
#endif

		delete pString;
	}
	m_CurrentStrings.RemoveAll();

	m_bCleaningUp = FALSE;
}

BOOL CMessageStringManager::IsValid(CMessageString *pString)
{
	JMutexAutoLock lock(m_mutex);
	return (m_CurrentStrings.Find(pString) != NULL);
}
