// File: tasks.h
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Classes Reference and related electronic
// documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft C++ Libraries products.
#ifndef TASKS_H
#define TASKS_H

#pragma once
#include <Windows.h>
#include <atlutil.h>
#include "../GWInterfaceDef.h"
///////////////////////////////////////////////////////////////////////////////
class CTaskBase
{

public:
	virtual void DoTask(void *pvParam, OVERLAPPED *pOverlapped)=0;
};

///////////////////////////////////////////////////////////////////////////////
class CTask1 : public CTaskBase
{
private:
	int m_nTaskType;
	void *m_param1;
	void *m_param2;
	void *m_param3;
	/*******************回调函数**********************/
	sip_config_pcb_func m_pConfigCB; //配置信息变更通知回调函数
	sip_alarm_ind_pcb_func m_pAlarmCB;	//报警通知回调函数
	sip_control_req_pcb_func m_pControlReqCB; //控制返回通知回调函数
	sip_info_req_pcb_func m_pInfoReqCB; //命令返回通知回调函数
	sip_media_pcb_func m_pMediaCB; //媒体交互通知回调函数

	
public:
	virtual void DoTask(void *pvParam, OVERLAPPED *pOverlapped)
	{
		printf("[%d]: CTask1::DoTask(pvParam=%d, pOverlapped=%d)\n",
			::GetCurrentThreadId(), (DWORD_PTR)pvParam, (DWORD_PTR)pOverlapped);


	}

	
};


///////////////////////////////////////////////////////////////////////////////
class CTask2 : public CTaskBase
{
private:
	DWORD m_dwBegin;
public:
	virtual void DoTask(void *pvParam, OVERLAPPED *pOverlapped)
	{
		printf("[%d]: CTask2::DoTask(pvParam=%d, pOverlapped=%d)\n",
			::GetCurrentThreadId(), (DWORD_PTR)pvParam, (DWORD_PTR)pOverlapped);
	}

	void SetParam(DWORD dwParam)
	{
		m_dwBegin = dwParam;
	}

	DWORD GetRunTime()
	{
		return GetTickCount()-m_dwBegin;
	}
};



#endif