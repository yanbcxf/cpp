#include "StdAfx.h"

/* 
 * File:   CSendMail.cpp
 * Author: root
 * 
 * Created on January 16, 2012, 6:14 PM
 */

#include "SendMail.h"

CSendMail::CSendMail() 
{
    m_strUser = "";
    m_strPsw = "";
    m_strSmtpServer = "";
    m_iPort = -1;
    m_RecipientList.clear();
    m_strMailFrom = "";
    m_MailContent.clear();
    m_iMailContentPos = 0;
}

CSendMail::CSendMail(  //create sendmail object with paremeter;
                const std::string & strUser,
                const std::string & strPsw, 
                const std::string & strSmtpServer, 
                int iPort, 
                const std::string & strMailFrom
            )
{
    m_strUser = strUser;
    m_strPsw = strPsw;
    m_strSmtpServer = strSmtpServer;
    m_iPort = iPort;
    m_RecipientList.clear();
    m_strMailFrom = strMailFrom;
    m_MailContent.clear();
    m_iMailContentPos = 0;
}

CSendMail::CSendMail(const CSendMail& orig) {
}

CSendMail::~CSendMail() {
}

size_t CSendMail::read_callback(void* ptr, size_t size, size_t nmemb, void* userp)
{
    CSendMail * pSm = (CSendMail *)userp;

    if(size*nmemb < 1)
        return 0;
    if(pSm->m_iMailContentPos < pSm->m_MailContent.size())
    {
        size_t len;
        len = pSm->m_MailContent[pSm->m_iMailContentPos].length();

        memcpy(ptr, pSm->m_MailContent[pSm->m_iMailContentPos].c_str(), pSm->m_MailContent[pSm->m_iMailContentPos].length());
        pSm->m_iMailContentPos++; /* advance pointer */
        return len;
    }
    return 0;
}

struct timeval CSendMail::tvnow()
{
  /*
  ** time() returns the value of time in seconds since the Epoch.
  */ 
    struct timeval now;
    now.tv_sec = (long)time(NULL);
    now.tv_usec = 0;
    return now;
}

long CSendMail::tvdiff(timeval newer, timeval older)
{
    return (newer.tv_sec-older.tv_sec)*1000+
        (newer.tv_usec-older.tv_usec)/1000;
}

bool CSendMail::ConstructHead(const std::string & strSubject, const std::string & strContent)
{
	m_MailContent.clear();
	m_iMailContentPos = 0;

    m_MailContent.push_back("MIME-Versioin: 1.0\n");
    std::string strTemp = "To: ";
    for(std::list<std::string >::iterator it = m_RecipientList.begin(); it != m_RecipientList.end();)
    {
        strTemp += *it;
        it++;
        if(it != m_RecipientList.end())
                strTemp += ",";
    }
    strTemp += "\n";
    m_MailContent.push_back(strTemp);
    if(strSubject != "")
    {
        strTemp = "Subject: ";
        strTemp += strSubject;
        strTemp += "\n";
        m_MailContent.push_back(strTemp);
    }
    m_MailContent.push_back("Content-Transfer-Encoding: 8bit\n");
    m_MailContent.push_back("Content-Type: text/html; \n Charset=\"UTF-8\"\n\n");
    if(strContent != "")
    {
        m_MailContent.push_back(strContent);
    }
    
    return true;
}

bool CSendMail::AddBodyContents(const std::string & strContent)
{
	if(strContent != "")
	{
		m_MailContent.push_back(strContent);
	}

	return true;
}


bool CSendMail::SendMail(const std::string& strSubject, const std::string& strMailBody) 
{
   /* m_MailContent.clear();
    m_iMailContentPos = 0;
    ConstructHead(strSubject, strMailBody);*/
    bool bRet = true;
    CURL *curl;
    CURLM *mcurl;
    int still_running = 1;
    struct timeval mp_start;
    char mp_timedout = 0;
    struct curl_slist* rcpt_list = NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (!curl)
    {
        printf("Init curl failed!\n");
        return false;
    }

    mcurl = curl_multi_init();
    if (!mcurl)
    {
        printf("Init mcurl failed!\n");
        return false;
    }
    for(std::list<std::string >::iterator it = m_RecipientList.begin(); it != m_RecipientList.end();it++)
    {
        rcpt_list = curl_slist_append(rcpt_list, it->c_str());
    }
    
    if(m_strSmtpServer == "" || m_iPort <= 0)
    {
        printf("smtp server couldn't be empty, or port must be large than 0!\n");
        
        curl_slist_free_all(rcpt_list);
        curl_multi_cleanup(mcurl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return false;
    }
    std::string strUrl = "smtp://" + m_strSmtpServer;
    strUrl += ":";
    char cPort[10];
    memset(cPort, 0, 10);
    sprintf(cPort, "%d", m_iPort);
    strUrl += cPort;
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
    
    if(m_strUser != "")
    {
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_strUser.c_str());
    }
    if(m_strPsw != "")
    {
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_strPsw.c_str());
    }
    
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &CSendMail::read_callback);
    
    if(m_strMailFrom == "")
    {
        printf("Mail from address couldn't be empty!\n");
        
        curl_slist_free_all(rcpt_list);
        curl_multi_cleanup(mcurl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, m_strMailFrom.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, rcpt_list);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long) CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_READDATA, this);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_SSLVERSION, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);

	/*这个配置很重要，否则会发送 VRFY 命令导致邮件发送不成功*/
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	
	CURLcode reCode = curl_easy_perform(curl);

	curl_slist_free_all(rcpt_list);
	curl_multi_cleanup(mcurl);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if(reCode == CURLE_OK)
		return true;
	else 
		return false;

	//////////////////////////////////////////////////////////////////////////
	//	以下为 curl 的异步操作，未成功

   
    //curl_multi_add_handle(mcurl, curl);
	
    mp_timedout = 0;
    mp_start = tvnow();

    /* we start some action by calling perform right away */
	while(CURLM_CALL_MULTI_PERFORM ==curl_multi_perform(mcurl, &still_running))
	{
		printf("running_handle_count ");
	}
	
    while (still_running) {
        struct timeval timeout;
        int rc; /* select() return code */

        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;

        long curl_timeo = -1;

        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);

        /* set a suitable timeout to play around with */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        curl_multi_timeout(mcurl, &curl_timeo);
        if (curl_timeo >= 0) {
            timeout.tv_sec = curl_timeo / 1000;
            if (timeout.tv_sec > 1)
                timeout.tv_sec = 1;
            else
                timeout.tv_usec = (curl_timeo % 1000) * 1000;
        }

        /* get file descriptors from the transfers */
        curl_multi_fdset(mcurl, &fdread, &fdwrite, &fdexcep, &maxfd);

        /* In a real-world program you OF COURSE check the return code of the
           function calls.  On success, the value of maxfd is guaranteed to be
           greater or equal than -1.  We call select(maxfd + 1, ...), specially in
           case of (maxfd == -1), we call select(0, ...), which is basically equal
           to sleep. */

        rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);

        if (tvdiff(tvnow(), mp_start) > MULTI_PERFORM_HANG_TIMEOUT) {
            fprintf(stderr, "ABORTING TEST, since it seems "
                    "that it would have run forever.\n");
            bRet = false;
            break;
        }

        switch (rc) {
            case -1:
                /* select error */
                printf("select error\n");
                bRet = false;
                break;
            case 0: /* timeout */
                printf("time out, retry again!\n");
                curl_multi_perform(mcurl, &still_running);
                break;
            default: /* action */
                while(CURLM_CALL_MULTI_PERFORM ==curl_multi_perform(mcurl, &still_running))
				{
					 printf("running_handle_count ");
				}
                break;
        }
    }

    curl_multi_remove_handle(mcurl, curl);
    curl_slist_free_all(rcpt_list);
    curl_multi_cleanup(mcurl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return bRet;
}

bool CSendMail::SendMail(const std::string & strSubject, const char* pMailBody, int len)
{
    std::string strMailContent;
    strMailContent.append(pMailBody, len);
    
    return SendMail(strSubject, strMailContent);
}

bool CSendMail::SendMail(const std::string& strUser, const std::string& strPsw, const std::string& strSmtpServer, int iPort, std::list<std::string>& recipientList, const std::string& strMailFrom, const std::string& strSubject, const std::string& strMailBody)
{
    m_strUser = strUser;
    m_strPsw = strPsw;
    m_strSmtpServer = strSmtpServer;
    m_iPort = iPort;
    std::copy(recipientList.begin(), recipientList.end(), m_RecipientList.begin());
    m_strMailFrom = strMailFrom;
    
    return SendMail(strSubject, strMailBody);
    
}

bool CSendMail::SendMail(const std::string& strUser, const std::string& strPsw, const std::string& strSmtpServer, int iPort, std::list<std::string>& recipientList, const std::string& strMailFrom, const std::string& strSubject, const char* pMailBody, int len)
{
    std::string strMailContent;
    strMailContent.append(pMailBody, len);
    return SendMail(strUser, strPsw, strSmtpServer, iPort, recipientList, strMailFrom, strSubject, strMailContent);
}

bool CSendMail::SendMail(const std::string& strUser, const std::string& strPsw, const std::string& strSmtpServer, int iPort, const std::string& strMailTo, const std::string& strMailFrom, const std::string& strSubject, const std::string& strMailBody)
{
    std::list<std::string> recipientList;
    recipientList.push_back(strMailTo);
    
    return SendMail(strUser, strPsw, strSmtpServer, iPort, recipientList, strMailFrom, strSubject, strMailBody);
}

bool CSendMail::SendMail(const std::string& strUser, const std::string& strPsw, const std::string& strSmtpServer, int iPort, const std::string& strMailTo, const std::string& strMailFrom, const std::string& strSubject, const char* pMailBody, int len)
{
    std::string strMailContent;
    strMailContent.append(pMailBody, len);
    return SendMail(strUser, strPsw, strSmtpServer, iPort, strMailTo, strMailFrom, strSubject, strMailContent);

}



