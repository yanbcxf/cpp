#include "StdAfx.h"
#include "BufferManager.h"

BufferManager::BufferManager(void)
{
	m_fMutex.Init();
}

BufferManager::~BufferManager(void)
{
	
}

 
BOOL BufferManager::ReleaseBuffer(CIOCPBuffer *pBuff)
{
	if(pBuff==NULL)
		return FALSE;

	JMutexAutoLock lock(m_fMutex);

	if( m_BufferList.size()< MaxNumOfBuffer)
	{
		m_BufferList.push_back(pBuff);
	}
	else
	{
		if ( pBuff!=NULL )
			delete pBuff;
	}
	pBuff=NULL;
	return TRUE;
}



CIOCPBuffer* BufferManager::AllocateBuffer(int nType)
{
	CIOCPBuffer *pBuff=NULL;
	stringstream ss;
	
	JMutexAutoLock lock(m_fMutex);

	if(!m_BufferList.empty())
	{
		pBuff = *m_BufferList.begin();
		m_BufferList.pop_front();
	}

	// We have to create a new buffer. 
	if(pBuff==NULL)
	{
		pBuff= new CIOCPBuffer();
		if( !pBuff )
		{ 

			return NULL;	
		}	
	}

	if(pBuff!=NULL) 
	{
		pBuff->EmptyUsed();
		pBuff->SetOperation(nType);
		pBuff->SetSequenceNumber(0);
		
		return pBuff;
	}
	return NULL; 
}
