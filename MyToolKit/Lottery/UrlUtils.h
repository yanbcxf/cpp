#pragma once

#include <curl/curl.h>
//#include <curl/types.h>
#include <curl/easy.h>

//#include <boost/circular_buffer.hpp> 

class CUrlUtils
{
public:
	CUrlUtils(void);
	~CUrlUtils(void);

public:


	static string PostUrl(string url,char* data);

	static string PostUrlAuth(string url,char* data,char* user,char* pwd);

	static string PostMultipartUrl(string url,list< map<string,curl_forms> > formData);

	
	static int PostUrlWithJson(string url, unsigned char* & pdata);

	

private:
public:


	static char errorBuffer[CURL_ERROR_SIZE];

	static void Init(CURL *&conn, string url,string *p_buffer);

	static int  Writer(char *, size_t, size_t, string *);

	static void ThrowError(string str);

	/*static void InitOfSouhu(CURL *&conn, string url, void   *p_buffer, \
		boost::circular_buffer<unsigned char>  *header_buffer);*/

	static int  WriterOfSouhu(char *, size_t, size_t,  void  *);

	/*static int  HeaderOfSouhu(char *, size_t, size_t, boost::circular_buffer<unsigned char> *);*/

	static string PostUrlOfSouhu(string url, char* data);
};
