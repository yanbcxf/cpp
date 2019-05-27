#pragma once



class CDllCallPlayPlaybackDownload :
	public AbstractCommand
{
public:
	CDllCallPlayPlaybackDownload( sipd_media_session* pMediaInfo);
	~CDllCallPlayPlaybackDownload(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllCallPlayPlaybackDownload"; }

protected:
	sipd_media_session  m_MediaInfo;

};
