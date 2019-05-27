#ifndef __PPL_GW_UTILITY_893902002__
#define __PPL_GW_UTILITY_893902002__

#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
//#include "ppl.h"
#include "gw_msg.h"
#include "sipd.h"


extern char* get_local_host_ip (void);
extern int gw_create_threads(void);
extern int gw_register_callback(void);
extern int sipd_init_ptz_variable(void);
extern unsigned int get_device_sn(void);
extern void Set_EvWaitFlg(int in);
extern int Get_EvWaitFlg();
extern void context_eXosip_lock();
extern void context_eXosip_unlock();
extern void SetAlmWorkFlg(int in);
extern int GetAlmWorkFlg();
extern void Set_LogOutFlg(int in);
extern int Get_LogOutFlg();
extern void Set_RegErrCnt(int in);
extern int Get_RegErrCnt();
extern void RegErrCounter();
extern void Set_NotifyFlg(int in);
extern int Get_NotifyFlg();
extern void reg_id_lock();
extern void reg_id_unlock();
extern void para_change_lock();
extern void para_change_unlock();
extern void Set_Dev_Online(int isonline);
extern int Get_Dev_Online();
extern void keepalivecn_lock();
extern void keepalivecn_unlock();
extern void gw_g_variable_lock();
extern void gw_g_variable_unlock();




#endif
