#include "stdafx.h"
#include "HttpHandle.h"
#include "HttpSvr.h"
#include "UriCodec.h"
#include "HttpRequest.h"

namespace ZvLibs
{
	CHttpHandle::CHttpHandle(CHttpSvr * pHttpSvr, HTTP_HANDLE_TYPE HandleType, const tstring & szPath)
	{
		m_szPath = szPath;
		m_pHttpSvr = pHttpSvr;
		m_HandleType = HandleType;
		m_pHttpSvr->AddHandle(*this);
	}

	CHttpHandle::~CHttpHandle()
	{
		m_pHttpSvr->RemoveHandle(*this);
	}

	bool CHttpHandle::Handle(CHttpRequest & Request)
	{
		return Request.SendFile(Request.m_szRealPath, false);
	}

}