#pragma once
#include "XmlBase.h"

class CDevStateReq : public CHttpHandle
{
public:
	CDevStateReq(CHttpSvr * pHttpSvr);
	~CDevStateReq(void);

	virtual bool Handle(CHttpRequest & Request);
	void SendJsonTree(CHttpRequest & Request);
};
