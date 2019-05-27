#pragma once
#include "medium.h"

class CMessageTransfer :
	public Medium
{
public:
	CMessageTransfer(CGbUsageEnvironment& env);
	~CMessageTransfer(void);

	void sipd_request(sipd_message_query_or_response  * pReq,const char * route);
};
