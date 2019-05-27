#include "stdafx.h"
#include "HttpService.h"
#include "HttpHandle.h"
#include "HttpSvr.h"
#include "../utils/ZvUtils.h"
#include "HttpRequest.h"
#include "HttpSocket.h"

namespace ZvLibs
{

	CHttpService::CHttpService(CHttpSvr * pHttpSvr, SOCKET sClient, sockaddr_in & AddrClient)
				 :CHttpSocket(true, sClient)
	{
		m_pHttpSvr = pHttpSvr;
		m_AddrClient = AddrClient;
		m_Socket = sClient;
		_beginthread(ThreadService, 0, this);
	}

	CHttpService::~CHttpService()
	{
	}

	bool CHttpService::HandleFilterBefore(CHttpRequest & Request)
	{
		vector<CHttpHandle* > vtHandleFilter = m_pHttpSvr->m_vtHandleFilterBefore;

		for(int i=0; i<(int)vtHandleFilter.size(); i++)
		{
			if(!vtHandleFilter[i]->Handle(Request))
			{
				Request.SendBody("");
				return false;
			}
		}
		return true;
	}

	bool CHttpService::HandleFilterAfter(CHttpRequest & Request)
	{
		vector<CHttpHandle* > vtHandleFilter = m_pHttpSvr->m_vtHandleFilterAfter;

		for(int i=0; i<(int)vtHandleFilter.size(); i++)
		{
			if(!vtHandleFilter[i]->Handle(Request))
			{
				Request.SendBody("");
				return false;
			}
		}
		return true;
	}


	void CHttpService::ThreadService(LPVOID pParam)
	{
		CHttpService * pThis = (CHttpService*)pParam;

		map<string, CHttpHandle*> & mapHandleContent = pThis->m_pHttpSvr->m_mapHandleContent;
		
		while(1)
		{
			CHttpRequest Request(pThis);
			bool b = false;

			if(!Request.RecvRequest())
			{
				Request.SendError("400 Bad Request");
				break;
			}

			Request.SetParam("Connection", "Close");

			// Filters before
			if(!pThis->HandleFilterBefore(Request)) break;

			// Content Generator
			CHttpHandle * pHandle = NULL;

			if(mapHandleContent.find(Request.m_szScriptName)!=mapHandleContent.end())
			{
				pHandle = mapHandleContent[Request.m_szScriptName];
				b = pHandle->Handle(Request);
			}
			else if(mapHandleContent.find(Request.m_szPath)!=mapHandleContent.end())
			{
				pHandle = mapHandleContent[Request.m_szPath];
				b = pHandle->Handle(Request);
			}
			else
			{
				CHttpHandle HttpHandle(pThis->m_pHttpSvr, HTTP_HANDLE_IMM);
				b = HttpHandle.Handle(Request);
			}

			if(Request.IsChunked())
			{
				Request.SendBody("");
			}

			// Filters after
			if(!pThis->HandleFilterAfter(Request)) break;

			if(!Request.m_bKeepAlive || !b)
				break;
		}

		delete pThis;
	}

}