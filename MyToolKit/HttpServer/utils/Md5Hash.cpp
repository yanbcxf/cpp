#include "StdAfx.h"
#include "Md5Hash.h"
#include "MD5.H"
#include <iomanip>

namespace ZvLibs
{

	string GetMd5HashStr(const string & szText)
	{
		MD5_CTX context;
		MD5Init(&context);
		MD5Update(&context, (unsigned char*)szText.c_str(), szText.length());
		MD5Final(&context);

		char buf[sizeof(DWORD)*4*2+1] = {0};

		for(int i=0; i<sizeof(DWORD)*4; i++)
		{
			unsigned char * p = ((unsigned char *)context.state)+i;
			sprintf(buf+2*i, "%02x", *p);
		}

		return buf;
	}

}
