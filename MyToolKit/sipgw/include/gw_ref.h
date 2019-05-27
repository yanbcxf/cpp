#ifndef __GW_REF_H_73890389892721990290__
#define __GW_REF_H_73890389892721990290__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sipd_cms_layer.h"
#include "pplgwutility.h"


#ifdef WIN32
#define 	   MSG_ID_GW_BASE							(WM_USER)
#else
#define 	   MSG_ID_GW_BASE							(99<<16)
#endif

/*云台向上*/
#define	   MSG_ID_GW_PTZ_UP_REQ					(MSG_ID_GW_BASE+1)
/*云台向下*/
#define	   MSG_ID_GW_PTZ_DOWN_REQ				(MSG_ID_GW_BASE+2)
/*云台向左*/
#define	   MSG_ID_GW_PTZ_LEFT_REQ					(MSG_ID_GW_BASE+3)
/*云台向右*/
#define	   MSG_ID_GW_PTZ_RIGHT_REQ				(MSG_ID_GW_BASE+4)
/*云台左上*/
#define	   MSG_ID_GW_PTZ_LU_REQ					(MSG_ID_GW_BASE+5)
/*云台左下*/
#define	   MSG_ID_GW_PTZ_LD_REQ					(MSG_ID_GW_BASE+6)
/*云台右上*/
#define	   MSG_ID_GW_PTZ_RU_REQ					(MSG_ID_GW_BASE+7)
/*云台右下*/
#define	   MSG_ID_GW_PTZ_RD_REQ					(MSG_ID_GW_BASE+8)
/*云台停止*/
#define	   MSG_ID_GW_PTZ_STOP_REQ				(MSG_ID_GW_BASE+9)
/*ZOOM -*/
#define	   MSG_ID_GW_PTZ_ZOOMTELE_REQ			(MSG_ID_GW_BASE+10)
/*ZOOM +*/
#define	   MSG_ID_GW_PTZ_ZOOMWIDE_REQ			(MSG_ID_GW_BASE+11)
/*FOCUS -*/
#define	   MSG_ID_GW_PTZ_FOCUS_NEAR_REQ			(MSG_ID_GW_BASE+12)
/*FOCUS +*/
#define	   MSG_ID_GW_PTZ_FOCUS_FAR_REQ			(MSG_ID_GW_BASE+13)
/*HALLO -*/
#define	   MSG_ID_GW_PTZ_HALLO_SMALL_REQ		(MSG_ID_GW_BASE+14)
/*HALLO +*/
#define	   MSG_ID_GW_PTZ_HALLO_LARGE_REQ		(MSG_ID_GW_BASE+15)

/*添加巡航*/
#define	   MSG_ID_GW_TOUR_ADD_REQ				(MSG_ID_GW_BASE+16)
/*删除巡航*/
#define	   MSG_ID_GW_TOUR_DEL_REQ				(MSG_ID_GW_BASE+17)
/*开始巡航*/
#define	   MSG_ID_GW_TOUR_START_REQ				(MSG_ID_GW_BASE+18)
/*巡航停留时间*/
#define	   MSG_ID_GW_TOUR_SET_STAYTIME_REQ		(MSG_ID_GW_BASE+19)
/*巡航速度*/
#define	   MSG_ID_GW_TOUR_SET_SPEED_REQ			(MSG_ID_GW_BASE+20)


/*预置点添加*/
#define	   MSG_ID_GW_PRESET_ADD_REQ				(MSG_ID_GW_BASE+21)
/*删除预置点*/
#define	   MSG_ID_GW_PRESET_DEL_REQ				(MSG_ID_GW_BASE+22)
/*调用预置点*/
#define	   MSG_ID_GW_PRESET_GOTO_REQ				(MSG_ID_GW_BASE+23)

/*自动扫描*/
#define	   MSG_ID_GW_AUTOSCAN_START_REQ			(MSG_ID_GW_BASE+24)


/*远程重启*/
#define	   MSG_ID_GW_REBOOT_REQ					(MSG_ID_GW_BASE+25)
/*录像控制*/
#define	   MSG_ID_GW_RECORD_CONTROL_REQ			(MSG_ID_GW_BASE+26)
/*报警控制*/
#define	   MSG_ID_GW_ALARM_GUARD_REQ			(MSG_ID_GW_BASE+27)
/*报警复位*/
#define	   MSG_ID_GW_ALARM_CONTROL_REQ			(MSG_ID_GW_BASE+28)


/*邋IO报警通知*/
#define	   MSG_ID_GW_IO_ALARM_IND				(MSG_ID_GW_BASE+29)
/*移动侦测报警通知*/
#define	   MSG_ID_GW_MOTO_ALARM_IND				(MSG_ID_GW_BASE+30)
/*回放或下载结束通知*/
#define	   MSG_ID_GW_FILE_TO_END_IND				(MSG_ID_GW_BASE+31)


/*获取设备信息*/
#define	   MSG_ID_GW_DEVICEINFO_REQ				(MSG_ID_GW_BASE+32)
/*获取状态信息*/
#define	   MSG_ID_GW_STATEINFO_REQ				(MSG_ID_GW_BASE+33)
/*获取目录信息*/
#define	   MSG_ID_GW_CATALOGINFO_REQ				(MSG_ID_GW_BASE+34)
/*检索录像文件*/
#define	   MSG_ID_GW_RECORDINFO_REQ				(MSG_ID_GW_BASE+35)

#define	MSG_ID_GW_CONFIG_IND					(MSG_ID_GW_BASE+36)
#define	MSG_ID_GW_CONTROL_RSP					(MSG_ID_GW_BASE+37)

#define	MSG_ID_GW_INFO_RSP_DEVICE				(MSG_ID_GW_BASE+38)
#define	MSG_ID_GW_INFO_RSP_STATE				(MSG_ID_GW_BASE+39)
#define	MSG_ID_GW_INFO_RSP_CATALOG				(MSG_ID_GW_BASE+40)
#define	MSG_ID_GW_INFO_RSP_RECORD				(MSG_ID_GW_BASE+41)


#define	MSG_ID_GW_MEDIA_RSP						(MSG_ID_GW_BASE+42)
#define	MSG_ID_GW_INVITE_CTRL_IND				(MSG_ID_GW_BASE+43)
#define	MSG_ID_GW_INVITE_INFO_IND				(MSG_ID_GW_BASE+44)

#define	MSG_ID_GW_SET_TIME_REQ					(MSG_ID_GW_BASE+45)

/*自动扫描*/
#define	   MSG_ID_GW_AUTOSCAN_SET_LEFT_REQ		(MSG_ID_GW_BASE+46)
#define	   MSG_ID_GW_AUTOSCAN_SET_RIGHT_REQ		(MSG_ID_GW_BASE+47)
#define	   MSG_ID_GW_AUTOSCAN_SET_SPEED_REQ		(MSG_ID_GW_BASE+48)


#define	   MSG_ID_GW_CRUISE_STOP_REQ			(MSG_ID_GW_BASE+49)

#define	   MSG_ID_GW_OTHER_ALARM_IND			(MSG_ID_GW_BASE+50)

/*预制点查询*/
#define	   MSG_ID_GW_PERSET_QUERY_REQ			(MSG_ID_GW_BASE+51)
#define	   MSG_ID_GW_PERSET_QUERY_RSP			(MSG_ID_GW_BASE+52)

/*目录订阅 */
#define	   MSG_ID_GW_DESCRIBE_CATALOG_REQ		(MSG_ID_GW_BASE+53)
#define	   MSG_ID_GW_DESCRIBE_CATALOG_RSP		(MSG_ID_GW_BASE+54)

/*报警订阅 */
#define	   MSG_ID_GW_DESCRIBE_ALARM_REQ			(MSG_ID_GW_BASE+55)
#define	   MSG_ID_GW_DESCRIBE_ALARM_RSP			(MSG_ID_GW_BASE+56)


/*判断初始路由*/
extern int handle_gw_route(const char* request_uri, int* pResult);

extern int init_domain_server(void);

extern int gw_get_domain_server_cfg_init(SIPD_GATEWAY_CFG* pCfg);

extern int gw_get_domain_server_cfg(SIPD_GATEWAY_CFG* pCfg);

extern int gw_set_domain_server_cfg(const SIPD_GATEWAY_CFG* pCfg);


extern int gw_init(void);

extern int unInit_gw(void);

extern int gw_reinit_cfg(void);



#endif /*__GW_REF_H_73890389892721990290__*/
