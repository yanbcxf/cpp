#ifndef _URLUTILS_HPP
#define _URLUTILS_HPP


#include "StandardInc.hpp"
#include "CurlInc.hpp"




class  UrlUtils
{
  public:


    __declspec(dllimport) static string PostUrl(string url,char* data);

	__declspec(dllimport) static string PostUrlAuth(string url,char* data,char* user,char* pwd);

    __declspec(dllimport) static string PostMultipartUrl(string url,list< map<string,curl_forms> > formData);

  private:
    

    static char errorBuffer[CURL_ERROR_SIZE];

    static void Init(CURL *&conn, string url,string *p_buffer);

    static int  Writer(char *, size_t, size_t, string *);

    static void ThrowError(string str);

};

#endif  //_URLUTILS_HPP


