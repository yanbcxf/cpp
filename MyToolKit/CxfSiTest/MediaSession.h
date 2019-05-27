#pragma once

class CMediaSession
{
public:
	CMediaSession(void);
	~CMediaSession(void);

public:
	sipd_media_session	m_invite_adaptor;
};
