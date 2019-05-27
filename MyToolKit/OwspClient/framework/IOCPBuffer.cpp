// IOCPBuffer.cpp: implementation of the CIOCPBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOCPBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOCPBuffer::CIOCPBuffer()
{
	Init(); // Never called if the class is allocated with VirtualAlloc.. 	
}

void CIOCPBuffer::Init()
{
	m_nUsed=0;
	m_Operation=-1;
	ZeroMemory(&m_ol, sizeof(OVERLAPPED));
	ZeroMemory(&m_Buffer,sizeof(m_Buffer));
}


CIOCPBuffer::~CIOCPBuffer()
{
	// Never called if the class is allocated with VirtualAlloc.. 
	m_nUsed=0;
	//FreeBuffer();
}



/*
* Returns the pointer to the Buffer.  
*
*/
PBYTE CIOCPBuffer::GetBuffer()
{
	return (PBYTE)m_Buffer;
}

PBYTE CIOCPBuffer::GetPayLoadBuffer()
{
	return m_nUsed>MINIMUMPACKAGESIZE ? GetBuffer()+MINIMUMPACKAGESIZE : NULL;	
}

/*
* Adds a singel BYTE to the data. 
*
*/

BOOL CIOCPBuffer::AddData(BYTE data)
{
	return AddData(&data, 1);
}

BOOL CIOCPBuffer::AddData(UINT data)
{
return AddData(reinterpret_cast<const BYTE*>(&data), sizeof(UINT));
}


BOOL CIOCPBuffer::AddData(unsigned short data)
{
	return AddData(reinterpret_cast<const BYTE*>(&data), sizeof(unsigned short));
}


/*
* Adds a stream of char to the buffer. 
*
*/
BOOL CIOCPBuffer::AddData(const char *const pData, UINT nSize)
{ 
	return AddData(reinterpret_cast<const BYTE*>(pData),nSize);
}

/*
* Adds a stream of BYTES to the buffer. 
*
*/

BOOL CIOCPBuffer::AddData(const BYTE *const pData, UINT nSize)
{
	if ( nSize > MAXIMUMPACKAGESIZE-m_nUsed )
		return FALSE;
	else
	{
		memcpy(m_Buffer + m_nUsed, pData, nSize);
		m_nUsed += nSize;
		return TRUE;
	}
}


// cheks if the Buffer is valid. 
BOOL CIOCPBuffer::IsValid()
{
	return TRUE;
}

UINT CIOCPBuffer::GetUsed()
{
	return m_nUsed;
}

// Used to indicate that we did have  a successfull Receive 
UINT CIOCPBuffer::Use(UINT nSize)
{
	m_nUsed+=nSize;
	return m_nUsed;
}

// Empty A used structure. 
void CIOCPBuffer::EmptyUsed()
{
	m_nUsed=0;
}

// Sets the current Operation. 
void CIOCPBuffer::SetOperation(int op)
{
	ZeroMemory(&m_ol, sizeof(OVERLAPPED));
	m_Operation=op;
}


// Setup the buffer for a ZeroByteRead. 
void CIOCPBuffer::SetupZeroByteRead()
{
 m_wsabuf.buf =(char*)m_Buffer;
 m_wsabuf.len = 0; 
}

// Setup Setup the buffer for a Read. 
void CIOCPBuffer::SetupRead()
{
if (m_nUsed == 0)
   {
      m_wsabuf.buf = reinterpret_cast<char*>(m_Buffer);
      m_wsabuf.len = MAXIMUMPACKAGESIZE; 
   }
   else // We have received some of the data but not all .. 
   {
      m_wsabuf.buf = reinterpret_cast<char*>(m_Buffer) + m_nUsed;
      m_wsabuf.len = MAXIMUMPACKAGESIZE - m_nUsed; 
   }
}

// Setup the buffer for a Write
void CIOCPBuffer::SetupWrite()
{
   m_wsabuf.buf = reinterpret_cast<char*>(m_Buffer);
   m_wsabuf.len = m_nUsed;
}

// Returns the WSABUF used with WsaRead and WsaWrite. 
WSABUF * CIOCPBuffer::GetWSABuffer()
{
	return  const_cast<WSABUF*>(&m_wsabuf);
}

//Get the Operation 
int CIOCPBuffer::GetOperation()
{
	return m_Operation;
}



// removes nSize byte from the Buffer. 
BOOL CIOCPBuffer::Flush(UINT nBytesToRemove)
{
	if ((nBytesToRemove > MAXIMUMPACKAGESIZE) || (nBytesToRemove > m_nUsed) ) 
	{
		//TRACE("ERROR BOOL CIOCPBuffer::Flush(UINT nBytesToRemove)");
		return FALSE;
	}
	m_nUsed-=nBytesToRemove;
	memmove(m_Buffer, m_Buffer + nBytesToRemove, m_nUsed);
	return TRUE;
}



// Sets the sequence number of the buffer..
void CIOCPBuffer::SetSequenceNumber(int nr)
{
	m_iSequenceNumber=nr;
}


// Gets the sequence number of the buffer. 
int CIOCPBuffer::GetSequenceNumber()
{
	return m_iSequenceNumber;
}




void CIOCPBuffer::DUMP()
{
	for(int i=0;i<GetUsed();i++)
	{
		CString txt;
		BYTE b=*(GetBuffer()+i);
		txt.Format("%d:%s;%d",i,GetBuffer()+i,b);
		//TRACE("%s\r\n",txt);
	}
}


