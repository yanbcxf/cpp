#pragma once
#include "XmlBase.h"

class CDevConfigReq : public CHttpHandle
{
public:
	CDevConfigReq(CHttpSvr * pHttpSvr);
	~CDevConfigReq(void);

	virtual bool Handle(CHttpRequest & Request);
	void SendJsonTree(CHttpRequest & Request);
};
