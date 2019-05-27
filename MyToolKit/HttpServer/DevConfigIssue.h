#pragma once
#include "XmlBase.h"

class CDevConfigIssue : public CHttpHandle
{
public:
	CDevConfigIssue(CHttpSvr * pHttpSvr);
	~CDevConfigIssue(void);

	virtual bool Handle(CHttpRequest & Request);
	void SendJsonTree(CHttpRequest & Request);
};
