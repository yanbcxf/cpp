#pragma once

class CDllPtzPreset :
	public AbstractCommand
{
public:
	CDllPtzPreset(const sipd_preset_control * pPresetControl);
	~CDllPtzPreset(void);

	virtual int Execute(UsageEnvironment* pThis);

	virtual string CommandType() { return "CDllPtzPreset"; }

protected:
	sipd_preset_control  m_PresetControl;

};
