#pragma once

#define MaxNumOfBuffer 100

class BufferManager
{
public:
	BufferManager(void);

	~BufferManager(void);

	// deletes the buffer or just put it in the FreeBufferList to optimze performance. 
	BOOL ReleaseBuffer(CIOCPBuffer *pBuff);

	// Creates a new buffer or returns a buffer from the FreeBufferList and configure the buffer. 
	CIOCPBuffer* AllocateBuffer(int nType);

private:
	JMutex	m_fMutex;

	list<CIOCPBuffer *> m_BufferList;

};
