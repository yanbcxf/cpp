#include "StdAfx.h"
#include "DllCallBye.h"

CDllCallBye::CDllCallBye(int playport):
AbstractCommand(false)
{
	m_playport = playport;
}

CDllCallBye::~CDllCallBye(void)
{
}


int CDllCallBye::Execute(UsageEnvironment* pEnviron)
{
	int nResult = -1;

	CDllMediaSession * pSession = (CDllMediaSession *)pEnviron->SearchSession("", -1 * m_playport);
	

	if(pSession)
	{
		
		nResult = pSession->SerailNumber();
		Sip_Call_Bye(pSession->m_cid, pSession->m_did);	

		//	Çå³şÏÖ³¡
		delete pSession;
	}

	return nResult;
}
