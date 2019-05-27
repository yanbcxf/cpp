#include "StdAfx.h"
#include "CallbackDllCallDataResponse.h"

CCallbackDllCallDataResponse::CCallbackDllCallDataResponse(int commandport, unsigned char * pBuffer, int nBufSize)
{
	m_commandport = commandport;
	m_pBuffer = NULL;
	m_nBufSize = 0;

	if(pBuffer && nBufSize > 0)
	{
		m_pBuffer = new unsigned char[nBufSize];
		if(m_pBuffer)
		{
			memcpy(m_pBuffer, pBuffer, nBufSize);
			m_nBufSize = nBufSize;
		}
	}
}

CCallbackDllCallDataResponse::~CCallbackDllCallDataResponse(void)
{
	if(m_pBuffer)
		delete[] m_pBuffer;
}


void CCallbackDllCallDataResponse::Execute()
{
	if(g_Dll_Call_Response_func && m_pBuffer)
	{
		unsigned long tvBegin = GetTickCount();

		g_Dll_Call_Response_func(m_commandport, 1, 0, m_pBuffer,  m_nBufSize);

		unsigned long tvNow = GetTickCount();
		long delay = tvNow - tvBegin; 

		if(delay > 16)
		{
			char tmp[256] = { 0 };
			sprintf_s(tmp, 256, "[%08d], Dll_Call_Response£¬ÓÃÊ± %d ms", m_commandport, delay);
			LOG4CPLUS_DEBUG(gb_Logger, string(tmp) );
		}
	}
}

int CCallbackDllCallDataResponse::FrameType()
{
	if(((H264_PARAM *)m_pBuffer)->knd==0)
		return m_commandport;
	else
		return -1;
}