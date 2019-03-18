// DataOperation.h: interface for the CDataOperation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAOPERATION_H__0126D9B0_072F_46AC_9E46_872E71B59CC7__INCLUDED_)
#define AFX_DATAOPERATION_H__0126D9B0_072F_46AC_9E46_872E71B59CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDataOperation  
{
private:

	char* m_pDataBuffer;
	unsigned long m_iBytePos;
	bool m_bOwnBuffer;
	unsigned int m_iBufferLen;
	
	unsigned short m_iBitPos;

private:
	void ReadBit(char* c);
	void WriteBit(char* c);

protected:
	void GetBitsFromValue(const int iValue , char* c , const short shBitsLen);
	int GetValueFromBits(const char* c);
	
	void ReadBits(char* c , const short shBitsLen);
	void ReadFromFile(const char* szFileName , const int iOff = 0);
	
	void WriteBits(char* c);
	void WriteToFile(const char* szFileName , bool bOverWrite);

	void GetOffSet(unsigned long &lBytesOff , unsigned long &lBitsOff);
	void SetBitPos(unsigned long iBitPos);
	void SetBytePos(unsigned long iBytePos);
	void SetBuffer(const char* pBuffer , unsigned long iBufferLen);
	void CreatBuffer(unsigned long iBufferLen);


	void SkipBits(const unsigned long iNum);
	void SkipBytes(const unsigned long iNum);
	void RollBackByte(const unsigned long iNum);

	int NextStartCode(unsigned long &lOff);
	
	bool IsInitialized();
	void ClearData();

protected:
	unsigned long GetBufLenLeft();
	CDataOperation();
	virtual ~CDataOperation();
	
};

#endif // !defined(AFX_DATAOPERATION_H__0126D9B0_072F_46AC_9E46_872E71B59CC7__INCLUDED_)
