#pragma once
#include "medium.h"

class CInviteOfBase /*: public Medium*/
{
public:
	CInviteOfBase(CGbUsageEnvironment& env, sipd_media_session * pMediaSession);
	virtual ~CInviteOfBase(void);

	virtual void Notify_3PCC_Finished(string ssrc) = 0;

	CGbUsageEnvironment& envir() const {return fEnviron;}

private:
	CGbUsageEnvironment& fEnviron;
};
