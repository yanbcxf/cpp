// IOCPBuffer.h: interface for the CIOCPBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CIOCPBUFFER_HH
#define _CIOCPBUFFER_HH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*
 * This Class is used to pass around the buffer and the operation that 
 * is done asynchronously. 
 *
 */

class CIOCPBuffer   
{
public:
	void Init();	
	PBYTE GetPayLoadBuffer();
	
	// Used with overlapped.. 
	OVERLAPPED			m_ol;
	BOOL Flush(UINT nBytesToRemove);
	
	WSABUF * GetWSABuffer();
	// sets the SequenceNumber of this object. 
	void SetSequenceNumber(int nr);
	// Get The sequensNumber of the buffer object. 
	int GetSequenceNumber();
	// Setup Different Types in buffer. 
	void SetupWrite();
	void SetupRead();
	void SetupZeroByteRead();
	int GetOperation();
	void SetOperation(int op);
	void EmptyUsed();
	UINT Use(UINT nSize);
	UINT GetUsed();
	//UINT GetSize();
	BOOL IsValid();
	// DUMP the buffer. (For Debug)
	void DUMP();
	// Write different types of variabels to the buffer
	BOOL AddData(const BYTE * pData,UINT nSize);
	BOOL AddData(const char * pData,UINT nSize);
	BOOL AddData(BYTE data);
	BOOL AddData(UINT data);
	BOOL AddData(unsigned short data);
	

	// returns A pointer to the Buffer 
	PBYTE GetBuffer();

	CIOCPBuffer();
	virtual ~CIOCPBuffer();
	
private:
	int m_iSequenceNumber;
	// The Type of Operation. 
    int		m_Operation;
	// Size of the Buffer 
	//UINT	m_nSize;
	// number of bytes that are Used. 
	UINT    m_nUsed;
	// The buffer. 
	BYTE m_Buffer[MAXIMUMPACKAGESIZE];
	//Holds the Buffer Used for send/receive
	WSABUF m_wsabuf;
	
};

#endif 
