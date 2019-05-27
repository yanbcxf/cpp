// SLCdll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include "ansides.h"



void compress2(unsigned char abTarget[], const unsigned char abSource[],  int wLen)
{
  unsigned int wI;
  unsigned char bT1, bT2;

  for (wI = 0; wI < wLen; wI++) {
    bT1 = abSource[wI*2];
    bT1 = (bT1 >= 'A') ? bT1-7 : bT1;
    bT2 = abSource[wI*2+1];
    bT2 = (bT2 >= 'A') ? bT2-7 : bT2;
    abTarget[wI] = ((bT1&0x0f)<<4) | (bT2&0x0f);
  }
}

/***********************************************************
 * Function: To turn BCD code into ASC code                *
 * Input:    abTarget - The buffer used to store ASC code  *
 *           abSource - The BCD code need be turned into   *
 *           wLen - The BCD code lenth                     *
 * Return:   NULL                                          *
 ***********************************************************/
void split(unsigned char abTarget[],unsigned  char abSource[], int wLen)
{
  unsigned int  wI;

  for (wI = 0; wI < wLen; wI++) {
    if ((abTarget[wI*2] = ((abSource[wI]&0xf0)>>4)|0x30) > '9')
      abTarget[wI*2] += 7;
    if ((abTarget[wI*2+1] = (abSource[wI]&0x0f)|0x30) > '9')
      abTarget[wI*2+1] += 7;
  }
}

/*3DES加密函数
//输入：
//		unsigned char *Sourcetr 要进行加密的串(无长度要求)
//		unsigned char *Keystr      加密用的密钥长度为16位
//输出:
//		unsigned char *Deststr  输出的加密后的字符串(无长度要求，但要与Sourcetr的长度统一)
//返回：
//      Ret==0 正常，可以取值
//      Ret==1 密钥长度不够
//      Ret==2 放加密串的空间不足
*/
int  Encrypt(unsigned char *Deststr,unsigned char *Sourcetr,unsigned char *Keystr)
{
	int Ret=0;
	unsigned char key[8],abInBuffer[8],abOutBuffer[8];
	for (int wI = 0; wI < strlen((char *)Sourcetr); wI += 8)
    {
		memcpy(abInBuffer,&Sourcetr[wI],8);//取8位数据

		memcpy(key,Keystr,8); //取8位key
		des(abInBuffer,abOutBuffer,key,0); //加密

		memcpy(key,&Keystr[8],8);			   //取后8位key
		des(abOutBuffer,abOutBuffer,key,1);//解密

		memcpy(key,Keystr,8);				   //取前8位key
		des(abOutBuffer,abOutBuffer,key,0);//加密

		memcpy(&Deststr[wI],abOutBuffer,8);//放入
	}
	return Ret;
}

/*3DES解函数
//输入：
//		unsigned char *Sourcetr 要进行解密的串
//		unsigned char *key      加密用的密钥
//输出:
//		unsigned char *Deststr  输出的解密后的字符串
//返回：
//      Ret==0 正常，可以取值
//      Ret==1 密钥长度不够
//      Ret==2 放加密串的空间不足
//*/
int  Decrypt(unsigned char *Deststr,unsigned char *Sourcetr,unsigned char *Keystr)
{
	int Ret=0;
	unsigned char key[8],abInBuffer[8],abOutBuffer[8];
	for (int wI = 0; wI < strlen((char *)Sourcetr); wI += 8)
    {
		memcpy(abInBuffer,&Sourcetr[wI],8);//取8位数据

		memcpy(key,Keystr,8); //取8位key
		des(abInBuffer,abOutBuffer,key,1); //解密

		memcpy(key,&Keystr[8],8);			   //取后8位key
		des(abOutBuffer,abOutBuffer,key,0);//加密

		memcpy(key,Keystr,8);				   //取前8位key
		des(abOutBuffer,abOutBuffer,key,1);//解密

		memcpy(&Deststr[wI],abOutBuffer,8);//放入
	}
	return Ret;
}

extern "C" bool  EncryptData(unsigned char *Deststr,unsigned char *Sourcetr,unsigned char *Keystr)
{
    unsigned char srcBuffer[512],Inbuffer[512],keyBuffer[16];
	memset(srcBuffer,0x00,sizeof(srcBuffer));
	memset(Inbuffer,0x00,sizeof(Inbuffer));
	memset(keyBuffer,0x00,sizeof(keyBuffer));

	memcpy(srcBuffer,Sourcetr,24);

	int len = strlen((char*)Keystr);
	if (len>=16)
	{
		memcpy((char *)keyBuffer,Keystr,16);
	}
	else
	{
		memcpy(keyBuffer,Keystr,len);
	}
	
	int rr = Encrypt(Inbuffer,srcBuffer,keyBuffer);
	
	int srclen = strlen((char*)Sourcetr);
	if (srclen<=8)
		len = 8;
	else if (srclen<=16)
		len = 16;
	else if (srclen<=24)
		len = 24;
	else len = 24;
	split(Deststr,Inbuffer,len);

	return true;
}


// 如果 Sourcetr 的长度不足 24， 要求后面要填写 000000 
extern "C" bool  DecryptData(unsigned char *Deststr,unsigned char *Sourcetr,unsigned char *Keystr)
{
    unsigned char keyBuffer[16],Inbuffer[512];

	memset(keyBuffer,0x00,sizeof(keyBuffer));
	memset(Inbuffer,0x00,sizeof(Inbuffer));
	int len = strlen((char*)Keystr);
	if (len>=16)
	{
		memcpy((char *)keyBuffer,Keystr,16);
	}
	else
	{
		memcpy(keyBuffer,Keystr,len);
	}

	compress2(Inbuffer,Sourcetr,24);//
	int rr = Decrypt(Deststr,Inbuffer,keyBuffer);
	return true;

}
