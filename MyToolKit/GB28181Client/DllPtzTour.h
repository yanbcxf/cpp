#pragma once

class CDllPtzTour :
	public AbstractCommand
{
public:
	CDllPtzTour(const sipd_tour_control * pTourControl);
	~CDllPtzTour(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllPtzTour"; }

protected:
	sipd_tour_control  m_TourControl;
};
