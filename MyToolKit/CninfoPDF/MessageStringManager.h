// MessageStringManager.h: interface for the CMessageStringManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGESTRINGMANAGER_H__5FB82C91_F2B3_4A52_A078_0F623172E7AB__INCLUDED_)
#define AFX_MESSAGESTRINGMANAGER_H__5FB82C91_F2B3_4A52_A078_0F623172E7AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMessageString;

class CMessageStringManager  
{
public:
	CMessageStringManager();
	virtual ~CMessageStringManager();

public:
	BOOL IsValid(CMessageString* pString);
	void ForceCleanup();
	void Delete(CMessageString* pString);
	void Add(CMessageString* pString);

protected:
	CPtrList	m_CurrentStrings;
	BOOL		m_bCleaningUp;
	JMutex		m_mutex;
};

#endif // !defined(AFX_MESSAGESTRINGMANAGER_H__5FB82C91_F2B3_4A52_A078_0F623172E7AB__INCLUDED_)
