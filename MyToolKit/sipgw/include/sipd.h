#ifndef __SIPD_H_9430049238203489__
#define __SIPD_H_9430049238203489__


#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pplgwutility.h"
#include "mxml.h"
#include "sipd_def.h"
#include "sipd_cms_layer.h"
#include "sip_reg.h"
#include "gw_ref.h"



extern struct eXosip_t *context_eXosip;

#define SIPD_MAX2(x,y)       ( (x)>(y) ? (x):(y) )


#ifdef WIN32
#define snprintf _snprintf
#endif

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp  strnicmp 
#endif

#define SIPD_DEVICE_TYPE_IPC			("132")
#define SIPD_DEVICE_TYPE_DVR			("111")
#define SIPD_DEVICE_TYPE_NVR			("118")

#define SIPD_DEVICE_TYPE_ALARM_IN		("134")
#define SIPD_DEVICE_TYPE_ALARM_OUT	("135")

#define SIPD_CENTER_SERVER				("200")

#define SIPD_DEVICE_TYPE				SIPD_DEVICE_TYPE_IPC

#if defined(WIN32)
DWORD WINAPI sipd_protocolevent(void *data);
DWORD WINAPI SIPD_Reg_Proc(void *data);
#else
extern void *sipd_protocolevent(void* data);
extern void *SIPD_Reg_Proc(void *data); 
#endif


extern int init_sip_protocol_stack(void);
extern int build_and_send_device_control_response_message(char* from, char* to, unsigned int sn, char* device_id, int result);
extern int build_and_send_alarm_message(char* from, char* to, unsigned int sn, char* device_id, int alarm_level, int alarm_method, char* description, char* AlarmTime, char* longitude, char* latitiude);
extern int strip_from(char* from, char* newfrom);
extern int strip_to(char* to, char* newto);
extern int build_and_send_file_to_end_message(int tid, int did, const char* deviceID);
extern int build_and_send_invite_200_ok(const sipd_sender_sdp_info* pInviteRspSdp);


extern int sipd_get_play_originator_device_id(char* pdeviceid);
extern void sipd_dev_stat_notify();
extern void sipd_dev_stat_notify();
extern void sipd_dev_stat_rsp(const sipd_stateinfo_adaptor_rsp* pRsp);
extern void sipd_dev_info_rsp(const sipd_deviceinfo_rsp* pRsp);
extern int sipd_dev_catalog_rsp(const sipd_catainfo_adaptor_rsp* pRsp );
extern int sipd_dev_catalog_notify(const sipd_catainfo_adaptor_rsp* pRsp );
extern int build_and_send_alarm_notify(const sipd_alarm_ind  *palarm_info_rsp);
extern int build_and_send_recordinfo_rsp(const sipd_recordinfo_adaptor_rsp *pRsp);

extern int GMTStrToTime_t(char *timestr, PRM_TIME *outtime);

extern int get_PRM_SIPD_CFG(SIPD_GATEWAY_CFG* pCfg);




#endif

