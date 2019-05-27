#include "stdafx.h"
#include "Error.h"

namespace ZvLibs
{
	using namespace std;

	string CError::m_szOperation;
	string CError::m_szReason;

	CError::CError(void)
	{
	}

	CError::~CError(void)
	{
	}

	void CError::LogErr()
	{
	}

	void CError::SetErr(const string & pReason, const string & pOperation, bool bLogIt)
	{
		m_szOperation = pOperation;
		m_szReason = pReason;
		
		if(bLogIt) LogErr();
	}

	string CError::GetOperation()
	{
		return m_szOperation;
	}

	string CError::GetReason()
	{
		return m_szReason;
	}

	string CError::GetWinErr()
	{
		string szErr;

		LPVOID lpMsgBuf;
		if(GetLastError()!=ERROR_SUCCESS &&
			FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL )>0)
		{
			szErr += (char*)lpMsgBuf;
			LocalFree( lpMsgBuf );
		}

		return szErr;
	}

	string CError::GetErr()
	{
		if(m_szOperation.length()>0)
			return m_szOperation+"£¬"+m_szReason;

		return m_szReason;
	}
}