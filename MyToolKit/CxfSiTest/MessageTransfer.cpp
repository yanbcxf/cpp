#include "StdAfx.h"
#include "GbUsageEnvironment.h"
#include "CatalogQuery.h"
#include "MessageTransfer.h"


CMessageTransfer::CMessageTransfer(CGbUsageEnvironment& env)
:Medium(env)
{
}

CMessageTransfer::~CMessageTransfer(void)
{
}



void CMessageTransfer::sipd_request(sipd_message_query_or_response  * pReq , const char * route)
{
	send_message_response(pReq, route);

}