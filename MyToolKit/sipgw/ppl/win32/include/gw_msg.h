#ifndef _MSG_H
#define _MSG_H
#include <Windows.h>

#define MAX_QUEQUE_NUM	100
enum
{
	MOD_MMI,	/*0	MMI                    */
	MOD_RPC,	/*1	RPC and remote manager */
	MOD_WEB,	/*2	WEB server             */
	MOD_FWK,	/*3	Framework              */
	MOD_FMG,	/*4 	File manager           */
	MOD_VAM,	/*5	vidio and auio manager */
	MOD_PTZ,	/*6	cloud terrace          */
	MOD_ALM,	/*7	system alarm           */
	MOD_SCM,	/*8	schem                  */
	MOD_PRM,	/*9	parameter manager      */
	MOD_MCU,	/*10	slave C51 mcu          */
	MOD_NET,	/*11	Network protocl stack  */
	MOD_DRV,	/*12	system driver          */
	MOD_TRC,	/*13 	Debug trace manage     */
	MOD_DEC,	/*14	playback               */
	MOD_QLOG,	/*15	query log			*/
	MOD_WLOG,	/*16	write log			*/
	MOD_GSM,	/*17	GMS			*/
	MOD_PRV,	/*18	preview			*/
	MOD_VOA,	/*19       voicetalk		*/
	MOD_SLC,	/*20	slice*/
	MOD_PMG,	/*21*/
	MOD_MMG,	/*22	MMI Message Manager*/
	MOD_OWSP,  /*23 */
	MOD_CGI,	/*24*/
	MOD_SENSOR, /*25*/
	MOD_AVB,	/*26 AVBuffer*/
	MOD_RTSP,	/*27 RTSP*/
	MOD_VSP,     /*28 VSP */
	MOD_VOIP, /*29 VOIP*/
	MOD_ONVIF, /*30 ONVIF*/
	MOD_SIPD, /*31 GB28181*/
	MOD_NUM 	/*32*/
};

#define MOD_SIPD_REG (MOD_NUM+1)
#define MOD_SIPD_PROTOC (MOD_NUM+2)
#define MOD_SIPD_RECORD_QUERY (MOD_NUM + 3)
#define MOD_SIPD_CATALOG_QUERY  (MOD_NUM + 4)
#define MOD_SIPD_QUERY  (MOD_NUM + 5)
#define MOD_SIPD_PERSET_QUERY  (MOD_NUM + 6)
#define MOD_SIPD_SUBSCRIBE  (MOD_NUM + 7)


void SetQueque(int mod, DWORD queque);
DWORD GetQueue(int mod);
void SendMessageEx(int srcid, int destid, int msgtype, void *data, int msg_len);

#pragma pack(push) //保存对齐状态
#pragma pack (1)     /*指定按1字节对齐*/

typedef struct
{
	int thread_id;		//发送的thead ID专用
	int msg_len;
	char para[1];	//消息参数，不同的消息对应不同的消息参数
} SN_MSG;

#pragma pack(pop)//恢复对齐状态
#endif