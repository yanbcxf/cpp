// DataOperation.cpp: implementation of the CDataOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataOperation.h"
#include <stdio.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataOperation::CDataOperation()
{
	m_pDataBuffer = NULL;
	m_bOwnBuffer = false;
	m_iBufferLen = 0;
	m_iBytePos = 0;
	m_iBitPos = 0;
}

CDataOperation::~CDataOperation()
{
	if(m_pDataBuffer!=NULL && m_bOwnBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = NULL;
	}
	m_bOwnBuffer = false;
	m_iBufferLen = 0;
	m_iBytePos = 0;
	m_iBitPos = 0;
}

void CDataOperation::CreatBuffer(unsigned long iBufferLen)
{
	if(iBufferLen <= 0)	return;

	char* tmp = NULL;
	tmp = new char[iBufferLen];
	if(tmp)	return;

	if(m_pDataBuffer!=NULL && m_bOwnBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = NULL;
	}

	m_pDataBuffer = tmp;
	m_bOwnBuffer = true;
}

void CDataOperation::SetBuffer(const char *pBuffer, unsigned long iBufferLen)
{
	if(1>iBufferLen || pBuffer == NULL)	return;

	if(m_pDataBuffer!=NULL && m_bOwnBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = NULL;
	}

	m_iBufferLen = iBufferLen;
	m_pDataBuffer = (char*)pBuffer;
	m_bOwnBuffer = false;
}

void CDataOperation::SetBytePos(unsigned long iBytePos)
{
	if(m_pDataBuffer== NULL || iBytePos<0 || iBytePos>=m_iBytePos)	return;
	
	m_iBytePos = iBytePos;
	m_iBitPos = 0;
}

void CDataOperation::SetBitPos(unsigned long iBitPos)
{
	if(m_pDataBuffer== NULL || iBitPos<0 || iBitPos>7)	return;
	m_iBitPos = iBitPos;
}	

//	将 “001000111” 等的比特串 以二进制形式写入流中
void CDataOperation::WriteBits(char *c)
{
	int len = strlen(c);
	
	int i=0;
	while(i<len)
	{
		WriteBit(c+i);
		i++;
	}
}

void CDataOperation::WriteBit(char* c)
{
	if(m_iBytePos >= m_iBufferLen)	return;

	switch(m_iBitPos)
	{
	case 0:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x80;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0x7F;
		break;
	case 1:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x40;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xBF;
		break;
	case 2:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x20;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xDF;
		break;
	case 3:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x10;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xEF;
		break;
	case 4:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x08;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xF7;
		break;
	case 5:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x04;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xFB;
		break;
	case 6:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x02;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xFD;
		break;
	case 7:
		if(*c == '1')
			*(m_pDataBuffer+m_iBytePos) |= 0x01;
		else
			*(m_pDataBuffer+m_iBytePos) &= 0xFE;
		break;
	}

	m_iBitPos++;
	if(m_iBitPos>=8)
	{
		m_iBytePos++;
		m_iBitPos = 0;
	}
}

void CDataOperation::WriteToFile(const char *szFileName , bool bOverWrite)
{
	if(m_pDataBuffer == NULL)	return;

	FILE* file = NULL;
	file = fopen(szFileName , "w");
	
	if(file==NULL)	return;
	
	fwrite(m_pDataBuffer , 1 , m_iBufferLen , file);
	fclose(file);
}

void CDataOperation::ReadBit(char *c)
{
	if(m_iBytePos>= m_iBufferLen)	return;

	char tmp = *(m_pDataBuffer+m_iBytePos);
	char cResult;
	switch(m_iBitPos)
	{
	case 0:
		cResult = tmp & 0x80;
		break;
	case 1:
		cResult = tmp & 0x40;
		break;
	case 2:
		cResult = tmp & 0x20;
		break;
	case 3:
		cResult = tmp & 0x10;
		break;
	case 4:
		cResult = tmp & 0x08;
		break;
	case 5:
		cResult = tmp & 0x04;
		break;
	case 6:
		cResult = tmp & 0x02;
		break;
	case 7:
		cResult = tmp & 0x01;
		break;
	}

	if(cResult)		*c='1';
	else			*c='0';

	m_iBitPos++;
	if(m_iBitPos>=8)
	{
		m_iBytePos++;
		m_iBitPos=0;
	}
}

void CDataOperation::ReadBits(char *c, const short shBitsLen)
{
	if(c== NULL || shBitsLen<=0)	return;

	int loop=0;
	while(loop<shBitsLen)
	{
		ReadBit(c+loop);
		loop++;
	}
}

void CDataOperation::ReadFromFile(const char *szFileName, const int iOff)
{
	if(iOff<0)	return;

	FILE* file = NULL;
	file = fopen(szFileName , "r");
	if(file == NULL)	return;

	fseek(file,0L,SEEK_END);
	long size = ftell(file);
	if(iOff >= size)	return;	

	if(m_pDataBuffer!=NULL && m_bOwnBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = NULL;
	}
	m_bOwnBuffer = false;
	m_iBufferLen = 0;
	m_iBytePos = 0;
	m_iBitPos = 0;

	fseek(file, iOff, SEEK_SET);
	m_pDataBuffer = new char[size-iOff+1];
	m_bOwnBuffer = true;
	m_iBufferLen = size-iOff;

	fread(m_pDataBuffer , 1 , size-iOff , file);
	fclose(file);
}

int CDataOperation::GetValueFromBits(const char *c)
{
	int iResult = 0;
	if(c == NULL)	return -1;

	int len = strlen(c);
	int loop=0;

	while(loop<len)
	{
		iResult <<= 1;
		if(*(c+loop) == '1')	iResult++;
		loop++;
	}

	return iResult;
}

//	将数值 转换为 字符串的形式 “011011011” 等
void CDataOperation::GetBitsFromValue(const int iValue, char *c , const short shBitsLen)
{
	memset(c , 0, shBitsLen);
	if(shBitsLen<=0 || iValue<0)	return;
	int tmp=iValue;
	int loop=0;

	for(int i=0 ; i<shBitsLen; i++)
		*(c+shBitsLen-1-i) = '0';

	while(true)
	{
		if(tmp>=2)
		{
			if(tmp%2 !=0)	*(c+shBitsLen-1-loop) = '1';
		}
		else
		{
				*(c+shBitsLen-1-loop) = '1';
				break;
		}

		tmp=tmp/2;
		loop++;
	}
}



bool CDataOperation::IsInitialized()
{
	if(m_pDataBuffer == NULL)	return false;
	return true;
}

int CDataOperation::NextStartCode(unsigned long &lOff)
{
	int re;
	char str[4];
	
	for(unsigned int i=m_iBytePos ; i<(m_iBufferLen-4) ; i++)
	{
		if(*(m_pDataBuffer+i) == 0x00)
		{
			if(*(m_pDataBuffer+i+1)==0x00)
			{
				if(*(m_pDataBuffer+i+2)==0x01)
				{					
					str[3]=0x00;
					str[2]=0x00;
					str[1]=0x01;
					str[0]=*(m_pDataBuffer+i+3);

					lOff = i;
					memcpy(&re , str , 4);
					m_iBytePos = i+4;
					return re;
				}
			}
			else i++;
		}
	}

	return 0;
}

void CDataOperation::SkipBytes(const unsigned long iNum)
{
	if(m_iBytePos+iNum>=m_iBufferLen)	return;

	m_iBytePos += iNum;
	m_iBitPos = 0;
}

void CDataOperation::SkipBits(const unsigned long iNum)
{
	unsigned long tmp = iNum/8;
	if(iNum%8>0)	tmp++;

	if(m_iBytePos+tmp>=m_iBufferLen)	return;

	m_iBytePos += iNum/8;
	m_iBitPos = 0;
	m_iBitPos += iNum%8;
}

void CDataOperation::GetOffSet(unsigned long &lBytesOff, unsigned long &lBitsOff)
{
	if(m_pDataBuffer == NULL)
	{
		lBytesOff = 0;
		lBitsOff = 0;
		return;
	}

	lBytesOff = m_iBytePos;
	lBitsOff = m_iBitPos;
}

void CDataOperation::ClearData()
{
	if(m_pDataBuffer!=NULL && m_bOwnBuffer)
	{
		delete[] m_pDataBuffer;
		m_pDataBuffer = NULL;
	}
	m_bOwnBuffer = false;
	m_iBufferLen = 0;
	m_iBytePos = 0;
	m_iBitPos = 0;
}

unsigned long CDataOperation::GetBufLenLeft()
{
	return m_iBufferLen - m_iBytePos;
}

/*void RollBackByte(const unsigned long iNum)
{
	if(iNum >= m_iBytePos)
	{
		m_iBytePos = 0;
		m_iBitPos = 0;
		return;
	}

	m_iBytePos -= iNum;
}*/
