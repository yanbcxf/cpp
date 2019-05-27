//slc.h


/************************************************************************/
/* 加密函数 
参数：
     Deststr：加密得到的密文
     Sourcetr：明文
	 Keystr: 密钥                                                       */
/************************************************************************/
extern "C" bool  EncryptData(unsigned char *Deststr,unsigned char *Sourcetr,unsigned char *Keystr);

/************************************************************************/
/* 解密函数 
参数：
     Deststr：解密得到的明文
     Sourcetr：加密信息
	 Keystr: 密钥                                                       */
/************************************************************************/
extern "C" bool  DecryptData(unsigned char *Deststr,unsigned char *Sourcetr,unsigned char *Keystr);

