#pragma once
#include "XmlBase.h"

class CJsonNotify : public CHttpHandle
{
public:
	CJsonNotify(CHttpSvr * pHttpSvr);
	~CJsonNotify(void);

	virtual bool Handle(CHttpRequest & Request);
	void SendJsonTree(CHttpRequest & Request);
};
