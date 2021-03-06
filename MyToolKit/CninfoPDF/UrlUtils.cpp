
#include "StdAfx.h"

#include "zlib.h"
#include "UrlUtils.h"

//////////////////////////////////////////////////////////////////////////

class SavedData {
public:
	SavedData(unsigned char* buf, unsigned size)
		: next(NULL), data(buf), dataSize(size), numBytesUsed(0) 
	{

	}

	virtual ~SavedData() {
		delete[] data;
		if(next)
			delete next;
	}

	long	seq;	//	数据包的序号
	SavedData* next;	
	unsigned char* data;
	unsigned dataSize, numBytesUsed;
};


class MyBuffer {
public:
	MyBuffer() {
		head = NULL;
		tail = NULL;
		totalBytesUsed = 0;
		totalSize = 0;
	}

	~MyBuffer() {
		if(head)
			delete head;
	}
	
	void InputData(unsigned char * pData, int len)
	{

		totalBytesUsed += len;

		SavedData* savedData = NULL;
		if(head==NULL)
		{
			int packLength = len > 1024 * 1024 ? len : 1024 * 1024;
			totalSize += packLength;

			unsigned char* buf = new unsigned char[packLength];
			memcpy(buf, pData, len);
			savedData  = new SavedData(buf, packLength);
			savedData->next = NULL;
			savedData->numBytesUsed = len;
			head = savedData;
			tail = savedData;
		}
		else
		{
			//	先保存在 最后的包中
			savedData = tail;
			int remainSize = savedData->dataSize - savedData->numBytesUsed;
			if(remainSize >0)
			{
				int copySize = remainSize >= len ? len : remainSize; 
				memcpy(savedData->data + savedData->numBytesUsed, pData, copySize);
				savedData->numBytesUsed += copySize;

				pData += copySize;
				len -= copySize;

			}
			//	最后的包放不下时，生成新的包
			if(len>0)
			{
				int packLength = len > 1024 * 1024 ? len : 1024 * 1024;
				totalSize += packLength;

				unsigned char* buf = new unsigned char[packLength];
				memcpy(buf, pData, len);
				savedData  = new SavedData(buf, packLength);
				savedData->next = NULL;
				savedData->numBytesUsed = len;

				tail->next = savedData;
				tail = savedData;
			}										
		}
	}

	unsigned char * linearize()
	{
		if(totalBytesUsed>0)
		{
			unsigned char* buf = new unsigned char[totalBytesUsed];

			unsigned char * buf1 = buf;
			SavedData* tmpSavedData = head;
			while(tmpSavedData!=NULL)
			{
				memcpy(buf1, tmpSavedData->data, tmpSavedData->numBytesUsed );
				buf1 += tmpSavedData->numBytesUsed;
				tmpSavedData = tmpSavedData->next;
			}
			return buf;
		}
		else 
			return NULL;
	}

	SavedData *  head;
	SavedData *  tail;
	long		totalBytesUsed;
	long		totalSize;

};

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
	curl_easy_setopt(conn,CURLOPT_TIMEOUT_MS, 90 * 1000);

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

//中财网
string CUrlUtils::PostUrlOfCfi(string url,char* data)
{
	CURL *conn = NULL;
	CURLcode code;
	boost::circular_buffer<unsigned char> buffer = boost::circular_buffer<unsigned char>(1024 * 1024);

	boost::circular_buffer<unsigned char> header = boost::circular_buffer<unsigned char>(2048);

	//curl_global_init(CURL_GLOBAL_DEFAULT);

	CUrlUtils::InitOfSouhu(conn,url,&buffer, &header);
	if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}

	curl_easy_setopt(conn, CURLOPT_REFERER,"http://data.cfi.cn/cfidata.aspx?sortfd=&sortway=&curpage=1&fr=content&ndk=A0A1934A1939A1957A1966A1983&xztj=&mystock=");
	curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	//curl_easy_setopt(conn, CURLOPT_ACCEPT_LANGUAGE, "zh-CN");

	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	string HqJs = "";

	if (code != CURLE_OK) //CUrlUtils::ThrowError(errorBuffer);
		return HqJs;

	//	解析http 回复头, 如果包含头 Content-Length 则说明不是通过 deflate 算法压缩的
	//	如果 包含 “Transfer-Encoding: chunked”，但是不包含 Content-Encoding: gzip，说明时分段但不压缩

	if(header.size()<=0)
		return  HqJs;

	string strHeader((char *)header.linearize());
	if(strHeader.find("Content-Encoding: gzip")==string::npos )
	{
		void * tmpBuffer = malloc(buffer.size() + 1);
		if(tmpBuffer!=NULL)
		{
			memset(tmpBuffer, 0, buffer.size()+1);
			memcpy(tmpBuffer, buffer.linearize(), buffer.size() );
			HqJs = string((char *)tmpBuffer);
			free(tmpBuffer);
		}

		HqJs = Utf8_GBK(HqJs);
		return HqJs;
	}

	unsigned char destBuf[409600] = {0};
	unsigned int len = 409600;
	//unsigned char * pp = buffer.linearize();


	//////////////////////////////////////////////////////////////////////////
	//	解压缩，初始化

	int ret;
	z_stream strm;
	strm.zalloc = myalloc;
	strm.zfree = myfree;
	strm.opaque = Z_NULL;

	strm.next_in = 0;
	strm.avail_in = Z_NULL;
	ret = inflateInit2(&strm, 15 + 16);
	if (ret != Z_OK) {
		return HqJs;
	}

	// 执行解压

	unsigned char in[1];
	strm.next_out = (Bytef *)destBuf;
	strm.avail_out = len;
	int index = 0;
	do {
		if(index >=buffer.size())
			break;

		in[0] = buffer[index];
		index++;
		strm.next_in = in;
		strm.avail_in = 1;
		//buffer.erase(buffer.begin(), buffer.begin()+1);
		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_DATA_ERROR) {
			string strError = string(strm.msg);
			return HqJs;
		}
		if (ret == Z_STREAM_END)
			inflateReset(&strm);
	} while (strm.avail_out);

	len = len - strm.avail_out;

	//	反初始化
	inflateEnd(&strm);

	HqJs = string((char *)destBuf);
	HqJs = Utf8_GBK(HqJs);
	return HqJs;
}


int  CUrlUtils::PostUrlOfCsindex(string url,unsigned char* & pdata)
{

	if(url=="http://hq.stock.sohu.com/pl/1881-1.html?uid=1512222240560727&7027194625967")
	{
		int ttt = 1;
	}

	CURL *conn = NULL;
	CURLcode code;
	// boost::circular_buffer<unsigned char> buffer = boost::circular_buffer<unsigned char>(1024 * 1024);
	MyBuffer buffer;

	boost::circular_buffer<unsigned char> header = boost::circular_buffer<unsigned char>(2048);

	//curl_global_init(CURL_GLOBAL_DEFAULT);

	CUrlUtils::InitOfSouhu(conn,url,&buffer, &header);
	/*if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}*/

	curl_easy_setopt(conn, CURLOPT_REFERER,"http://q.stock.sohu.com/cn/300067/lshq.shtml");
	curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	//curl_easy_setopt(conn, CURLOPT_ACCEPT_LANGUAGE, "zh-CN");

	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	
	if (code != CURLE_OK) //CUrlUtils::ThrowError(errorBuffer);
		return 0;

	//	解析http 回复头, 

	if(header.size()<=0)
		return  0;

	string strHeader((char *)header.linearize());
	if(strHeader.find("404 Not Found")!=string::npos)
	{
		return -1;
	}

	if(strHeader.find("Content-Length")!=string::npos ||
		strHeader.find("x-zip-compressed")!=string::npos )
	{
		pdata = buffer.linearize();
		return buffer.totalBytesUsed;

		/*
		int k = buffer.size();
		void * tmpBuffer = malloc(buffer.size());
		if(tmpBuffer!=NULL)
		{
			memset(tmpBuffer, 0, buffer.size());
			memcpy(tmpBuffer, buffer.linearize(), buffer.size() );
			pdata = (unsigned char *)tmpBuffer;

			return buffer.size();
		}*/
	}
	return 0;

}


int  CUrlUtils::PostUrlOfSzse(string url,unsigned char* & pdata)
{

	if(url=="http://hq.stock.sohu.com/pl/1881-1.html?uid=1512222240560727&7027194625967")
	{
		int ttt = 1;
	}

	CURL *conn = NULL;
	CURLcode code;
	boost::circular_buffer<unsigned char> buffer = boost::circular_buffer<unsigned char>(1024 * 1024);

	boost::circular_buffer<unsigned char> header = boost::circular_buffer<unsigned char>(2048);

	//curl_global_init(CURL_GLOBAL_DEFAULT);

	CUrlUtils::InitOfSouhu(conn,url,&buffer, &header);
	/*if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}*/

	curl_easy_setopt(conn, CURLOPT_REFERER,"http://q.stock.sohu.com/cn/300067/lshq.shtml");
	curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	//curl_easy_setopt(conn, CURLOPT_ACCEPT_LANGUAGE, "zh-CN");

	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	
	if (code != CURLE_OK) //CUrlUtils::ThrowError(errorBuffer);
		return 0;

	//	解析http 回复头, 

	if(header.size()<=0)
		return  0;

	string strHeader((char *)header.linearize());
	if(strHeader.find("404 Not Found")!=string::npos)
	{
		return -1;
	}

	if(strHeader.find("ms-excel")!=string::npos )
	{
		int k = buffer.size();
		void * tmpBuffer = malloc(buffer.size());
		if(tmpBuffer!=NULL)
		{
			memset(tmpBuffer, 0, buffer.size());
			memcpy(tmpBuffer, buffer.linearize(), buffer.size() );
			pdata = (unsigned char *)tmpBuffer;

			return buffer.size();
		}
	}
	return 0;

}



string CUrlUtils::PostUrlOfCffex(string url,char* data)
{

	if(url=="http://hq.stock.sohu.com/pl/1881-1.html?uid=1512222240560727&7027194625967")
	{
		int ttt = 1;
	}

	CURL *conn = NULL;
	CURLcode code;
	boost::circular_buffer<unsigned char> buffer = boost::circular_buffer<unsigned char>(1024 * 1024);

	boost::circular_buffer<unsigned char> header = boost::circular_buffer<unsigned char>(2048);

	//curl_global_init(CURL_GLOBAL_DEFAULT);

	CUrlUtils::InitOfSouhu(conn,url,&buffer, &header);
	if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}

	curl_easy_setopt(conn, CURLOPT_REFERER,"http://q.stock.sohu.com/cn/300067/lshq.shtml");
	curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	//curl_easy_setopt(conn, CURLOPT_ACCEPT_LANGUAGE, "zh-CN");

	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	string HqJs = "";

	if (code != CURLE_OK) //CUrlUtils::ThrowError(errorBuffer);
		return HqJs;

	//	解析http 回复头, 如果包含头 Content-Length 则说明不是通过 deflate 算法压缩的
	//	如果 包含 “Transfer-Encoding: chunked”，但是不包含 Content-Encoding: gzip，说明时分段但不压缩

	if(header.size()<=0)
		return  HqJs;

	string strHeader((char *)header.linearize());

	if(strHeader.find("http://www.cffex.com.cn/error_page/error_404.html")!=string::npos)
	{
		return "error_404";
	}

	if(strHeader.find("Content-Length")!=string::npos ||
		strHeader.find("Content-Encoding: gzip")==string::npos )
	{
		void * tmpBuffer = malloc(buffer.size() + 1);
		if(tmpBuffer!=NULL)
		{
			memset(tmpBuffer, 0, buffer.size()+1);
			memcpy(tmpBuffer, buffer.linearize(), buffer.size() );
			HqJs = string((char *)tmpBuffer);
			free(tmpBuffer);
		}

		return HqJs;
	}

	unsigned char destBuf[409600] = {0};
	unsigned int len = 409600;
	//unsigned char * pp = buffer.linearize();


	//////////////////////////////////////////////////////////////////////////
	//	解压缩，初始化

	int ret;
	z_stream strm;
	strm.zalloc = myalloc;
	strm.zfree = myfree;
	strm.opaque = Z_NULL;

	strm.next_in = 0;
	strm.avail_in = Z_NULL;
	ret = inflateInit2(&strm, 15 + 16);
	if (ret != Z_OK) {
		return HqJs;
	}

	// 执行解压

	unsigned char in[1];
	strm.next_out = (Bytef *)destBuf;
	strm.avail_out = len;
	int index = 0;
	do {
		if(index >=buffer.size())
			break;

		in[0] = buffer[index];
		index++;
		strm.next_in = in;
		strm.avail_in = 1;
		//buffer.erase(buffer.begin(), buffer.begin()+1);
		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_DATA_ERROR) {
			string strError = string(strm.msg);
			return HqJs;
		}
		if (ret == Z_STREAM_END)
			inflateReset(&strm);
	} while (strm.avail_out);

	len = len - strm.avail_out;

	//	反初始化
	inflateEnd(&strm);

	HqJs = string((char *)destBuf);
	return HqJs;
}

//////////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------------------
//post normal param to url
//------------------------------------------------------------------------------
string CUrlUtils::PostUrlOfSouhu(string url,char* data)
{

	if(url=="http://hq.stock.sohu.com/pl/1881-1.html?uid=1512222240560727&7027194625967")
	{
		int ttt = 1;
	}

	CURL *conn = NULL;
	CURLcode code;
	boost::circular_buffer<unsigned char> buffer = boost::circular_buffer<unsigned char>(1024 * 1024);

	boost::circular_buffer<unsigned char> header = boost::circular_buffer<unsigned char>(2048);

	//curl_global_init(CURL_GLOBAL_DEFAULT);

	CUrlUtils::InitOfSouhu(conn,url,&buffer, &header);
	if(0!=data){
		curl_easy_setopt(conn, CURLOPT_POSTFIELDS,data);
	}

	curl_easy_setopt(conn, CURLOPT_REFERER,"http://q.stock.sohu.com/cn/300067/lshq.shtml");
	curl_easy_setopt(conn, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
	//curl_easy_setopt(conn, CURLOPT_ACCEPT_LANGUAGE, "zh-CN");

	code = curl_easy_perform(conn); 
	curl_easy_reset(conn);
	curl_easy_cleanup(conn);

	string HqJs = "";

	if (code != CURLE_OK) //CUrlUtils::ThrowError(errorBuffer);
		return HqJs;

	//	解析http 回复头, 如果包含头 Content-Length 则说明不是通过 deflate 算法压缩的
	//	如果 包含 “Transfer-Encoding: chunked”，但是不包含 Content-Encoding: gzip，说明时分段但不压缩
	
	if(header.size()<=0)
		return  HqJs;
	
	string strHeader((char *)header.linearize());
	if(strHeader.find("Content-Length")!=string::npos ||
		strHeader.find("Content-Encoding: gzip")==string::npos )
	{
		void * tmpBuffer = malloc(buffer.size() + 1);
		if(tmpBuffer!=NULL)
		{
			memset(tmpBuffer, 0, buffer.size()+1);
			memcpy(tmpBuffer, buffer.linearize(), buffer.size() );
			HqJs = string((char *)tmpBuffer);
			free(tmpBuffer);
		}
		
		return HqJs;
	}

	unsigned char destBuf[409600] = {0};
	unsigned int len = 409600;
	//unsigned char * pp = buffer.linearize();
	
	
	//////////////////////////////////////////////////////////////////////////
	//	解压缩，初始化

	int ret;
	z_stream strm;
	strm.zalloc = myalloc;
	strm.zfree = myfree;
	strm.opaque = Z_NULL;

	strm.next_in = 0;
	strm.avail_in = Z_NULL;
	ret = inflateInit2(&strm, 15 + 16);
	if (ret != Z_OK) {
		return HqJs;
	}

	// 执行解压
	
	unsigned char in[1];
	strm.next_out = (Bytef *)destBuf;
	strm.avail_out = len;
	int index = 0;
	do {
		if(index >=buffer.size())
			break;

		in[0] = buffer[index];
		index++;
		strm.next_in = in;
		strm.avail_in = 1;
		//buffer.erase(buffer.begin(), buffer.begin()+1);
		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_DATA_ERROR) {
			string strError = string(strm.msg);
			return HqJs;
		}
		if (ret == Z_STREAM_END)
			inflateReset(&strm);
	} while (strm.avail_out);

	len = len - strm.avail_out;
	
	//	反初始化
	inflateEnd(&strm);
		
	HqJs = string((char *)destBuf);
	return HqJs;
}

int CUrlUtils::WriterOfSouhu(char *data, size_t size, size_t nmemb, void * writerData)
{
	MyBuffer * buffer = (MyBuffer *)writerData;
	unsigned long sizes = size * nmemb;
	if (buffer == NULL) return 0;

	buffer->InputData((unsigned char *)data, sizes);
	return sizes;
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
		writerData->rset_capacity(capa+ sizes + 1024 * 1024);
	}
	writerData->insert(writerData->end(), data, data + sizes);
	return sizes;
}
*/

int CUrlUtils::HeaderOfSouhu(char *data, size_t size, size_t nmemb, boost::circular_buffer<unsigned char> * headerData)
{
	unsigned long sizes = size * nmemb;
	if (headerData == NULL) return 0;
	headerData->insert(headerData->end(), data, data + sizes);
	return sizes;
}

void CUrlUtils::InitOfSouhu(CURL *&conn, string url, void *p_buffer, \
							boost::circular_buffer<unsigned char>  *header_buffer)
{
	string logstr;
	CURLcode code;

	conn = curl_easy_init();

	if(conn==NULL)CUrlUtils::ThrowError("Failed to create CURL connection\n");

	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to init errBuffer\n");

	//url="http://134.128.4.79:7001/video2web/serviceAction.do";
	curl_easy_setopt(conn,CURLOPT_CONNECTTIMEOUT_MS, 2000);
	curl_easy_setopt(conn,CURLOPT_TIMEOUT_MS, 90 * 1000);

	code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set URL\n");

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set redirect option \n");

	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION,CUrlUtils::WriterOfSouhu);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set writer\n");

	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, p_buffer);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set writer data\n");

	code = curl_easy_setopt(conn, CURLOPT_HEADERFUNCTION,CUrlUtils::HeaderOfSouhu);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set writer\n");

	code = curl_easy_setopt(conn, CURLOPT_HEADERDATA, header_buffer);
	if (code != CURLE_OK)CUrlUtils::ThrowError("Failed to set writer data\n");
}



