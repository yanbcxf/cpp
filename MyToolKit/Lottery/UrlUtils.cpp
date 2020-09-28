
#include "StdAfx.h"

#include "zlib.h"
#include "UrlUtils.h"
#include "MyBuffer.h"


//////////////////////////////////////////////////////////////////////////

CUrlUtils::CUrlUtils(void)
{
}

CUrlUtils::~CUrlUtils(void)
{
}

char CUrlUtils::errorBuffer[CURL_ERROR_SIZE];
//------------------------------------------------------------------------------
//private fun
//------------------------------------------------------------------------------
void CUrlUtils::Init(CURL *&conn, string url,string *p_buffer)
{
	string logstr;
	CURLcode code;

	conn = curl_easy_init();

	if(conn==NULL)CUrlUtils::ThrowError("Failed to create CURL connection\n");

	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to init errBuffer\n");

	//url="http://134.128.4.79:7001/video2web/serviceAction.do";
	curl_easy_setopt(conn,CURLOPT_CONNECTTIMEOUT_MS, 2000);
	curl_easy_setopt(conn,CURLOPT_TIMEOUT_MS, 60000);

	code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set URL\n");

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set redirect option \n");

	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION,CUrlUtils::Writer);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set writer\n");

	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, p_buffer);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set writer data\n");	
}


int CUrlUtils::Writer(char *data, size_t size, size_t nmemb, string *writerData)
{
	unsigned long sizes = size * nmemb;
	if (writerData == NULL) return 0;
	writerData->append(data, sizes);
	return sizes;
}

void CUrlUtils::ThrowError(string str)
{
	string logstr=str.insert(0,"UrlUtils::").c_str();
	throw runtime_error(logstr);
}

//------------------------------------------------------------------------------
//public function
//------------------------------------------------------------------------------
string CUrlUtils::PostUrlAuth(string url,char* data,char* user,char* pwd)
{
	CURL *conn = NULL;
	CURLcode code;
	string buffer;

	CUrlUtils::Init(conn,url,&buffer);

	stringstream ss;
	ss<<user<<":"<<pwd;
	string auth=ss.str();
	curl_easy_setopt(conn, CURLOPT_USERPWD, auth.c_str());

	if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}
	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	if (code != CURLE_OK)CUrlUtils::ThrowError(errorBuffer);

	buffer=code != CURLE_OK?"":buffer;
	return buffer;
}



//------------------------------------------------------------------------------
//post normal param to url
//------------------------------------------------------------------------------
string CUrlUtils::PostUrl(string url,char* data)
{
	CURL *conn = NULL;
	CURLcode code;
	string buffer;

	//curl_global_init(CURL_GLOBAL_DEFAULT);

	CUrlUtils::Init(conn,url,&buffer);
	if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}

	curl_easy_setopt(conn, CURLOPT_REFERER,"http://www.sse.com.cn/assortment/stock/list/stockdetails/capital/index.shtml?COMPANY_CODE=600008&SecurityCode=-");
	curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	//curl_easy_setopt(conn, CURLOPT_ACCEPT_LANGUAGE, "zh-CN");

	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	if (code != CURLE_OK)CUrlUtils::ThrowError(errorBuffer);

	buffer=code != CURLE_OK?"":buffer;
	return buffer;
}

//------------------------------------------------------------------------------
//post multipart/formdata to url
//------------------------------------------------------------------------------
string CUrlUtils::PostMultipartUrl(string url,list< map<string,curl_forms> > formData)
{

	CURL *conn = NULL;
	CURLcode code;
	string buffer;

	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	static const char buf[] = "Expect:";

	//curl_global_init(CURL_GLOBAL_DEFAULT);
	CUrlUtils::Init(conn,url,&buffer);

	list< map<string,curl_forms> >::iterator iter;
	for(iter=formData.begin();iter!=formData.end();iter++)
	{
		map<string,curl_forms> tmap=*iter;
		map<string,curl_forms>::iterator miter;
		for(miter=tmap.begin();miter!=tmap.end();miter++)
		{
			string name=miter->first;
			curl_forms cfs=miter->second;
			curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME,name.c_str(),
				cfs.option,cfs.value,
				CURLFORM_END);
		}
	}

	curl_easy_setopt(conn, CURLOPT_HTTPPOST, formpost);
	code = curl_easy_perform(conn);

	curl_easy_reset(conn);
	curl_easy_cleanup(conn);
	curl_formfree(formpost);
	curl_slist_free_all (headerlist);
	if (code != CURLE_OK)CUrlUtils::ThrowError(errorBuffer);
	//buffer=code != CURLE_OK?"":buffer;

	return buffer;
}

//////////////////////////////////////////////////////////////////////////


void * myalloc(void *q, unsigned n, unsigned m)

{
	q = Z_NULL;
	return calloc(n, m);
}

void myfree(void *q, void *p)
{
	q = Z_NULL;
	free(p);
}



int  CUrlUtils::PostUrlWithJson(string url,unsigned char* & pdata)
{

	
	return 0;

}





//////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
//post normal param to url
//------------------------------------------------------------------------------
string CUrlUtils::PostUrlOfSouhu(string url, char* data)
{
	return "";
}

/*
int CUrlUtils::WriterOfSouhu(char *data, size_t size, size_t nmemb, boost::circular_buffer<unsigned char> * writerData)
{
unsigned long sizes = size * nmemb;
if (writerData == NULL) return 0;
int capa = writerData->capacity();
int cur_size = writerData->size();
if(cur_size + sizes >= capa)
{
//	2016-11-15 ,增大内存可以显著提高下载速度
writerData->rset_capacity(capa+ sizes + 1024 * 1024);
}
writerData->insert(writerData->end(), data, data + sizes);
return sizes;
}
*/

