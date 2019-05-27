#include "stdafx.h"
#include "Gb28181_Layer.h"

#include <eXosip.h>
#include <osip_headers.h>
#include <osip_mt.h>

bool parse_deviceId_and_SN(mxml_node_t *tree, mxml_node_t *query_node, string & deviceId, string & SN);

int Sip_send_message(const char *to, const char *from, const char * buffer)
{
	int ret = OSIP_UNDEFINED_ERROR;

	osip_message_t *message = NULL;

	eXosip_lock(context_eXosip);
	ret = eXosip_message_build_request(context_eXosip, &message, "MESSAGE",\
		to, from, NULL);
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		LOG4CPLUS_DEBUG(gb_Logger, "Sip_PTZ_Move eXosip_message_build_request failed");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		//context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_body(message, buffer, strlen(buffer));
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		LOG4CPLUS_DEBUG(gb_Logger, "Sip_PTZ_Move osip_message_set_body failed");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		//context_eXosip_unlock();
		return -1;
	}

	ret = osip_message_set_content_type (message, "Application/MANSCDP+xml");
	if(ret != OSIP_SUCCESS)
	{
		eXosip_unlock(context_eXosip);
		LOG4CPLUS_DEBUG(gb_Logger, "Sip_PTZ_Move osip_message_set_content_type failed");
		if(message != NULL)
		{
			osip_message_free(message);
		}
		//context_eXosip_unlock();
		return -1;
	}

	ret = eXosip_message_send_request (context_eXosip, message);
	eXosip_unlock(context_eXosip);
	return ret;
}


int Sip_record_control(const sipd_record_control * pRecordControl)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<RecordCmd>%s</RecordCmd>\r\n"\
		"</Control>",
		pRecordControl->SN, pRecordControl->device_id,
		pRecordControl->cmd_type == SIPD_RECORD_CONTROL_START? "Record": "StopRecord");

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		ret = Sip_send_message(pRecordControl->to_uri, pRecordControl->from_uri,buffer);
	}

	/*返回 tid >0*/

	stringstream ss;
	ss << "Sip_record_control, SN = "<< string(pRecordControl->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}

int Sip_alarm_control (const sipd_alarm_control * pAlarmControl)
{
	int ret = OSIP_UNDEFINED_ERROR;

	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	memset(buffer, 0, sizeof(buffer));

	if(pAlarmControl->cmd_type == SIPD_ALARM_CONTROL_RESET_ALARM)
	{
		//	报警复位
		snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Control>\r\n"\
			"<CmdType>DeviceControl</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<AlarmCmd>%s</AlarmCmd>\r\n"\
			"</Control>",
			pAlarmControl->SN, pAlarmControl->device_id, "ResetAlarm");
	}
	else
	{
		//	报警布撤防
		snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
			"<?xml version=\"1.0\"?>\r\n"\
			"<Control>\r\n"\
			"<CmdType>DeviceControl</CmdType>\r\n"\
			"<SN>%s</SN>\r\n"\
			"<DeviceID>%s</DeviceID>\r\n"\
			"<GuardCmd>%s</GuardCmd>\r\n"\
			"</Control>",
			pAlarmControl->SN, pAlarmControl->device_id,
			pAlarmControl->cmd_type == SIPD_ALARM_CONTROL_SET_GUARD? "SetGuard": "ResetGuard");
	}

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		ret = Sip_send_message(pAlarmControl->to_uri, pAlarmControl->from_uri,buffer);
	}

	/*返回 tid >0*/
	stringstream ss;
	ss << "Sip_alarm_control, SN = "<< string(pAlarmControl->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}



int Sip_PTZ_Move(const sipd_ptz_control* pControl)
{
	int ret = OSIP_UNDEFINED_ERROR;
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	char toHex[32] = { 0 };
	memset(buffer, 0, sizeof(buffer));

	unsigned char tmp;
	struct sipd_PTZCMD  ptzcode;

	ptzcode.firstByteA5 = 0xA5;
	ptzcode.combineCode1 = 0x00;
	tmp = (ptzcode.firstByteA5 >>4) + (ptzcode.firstByteA5 & 0x0f) + (ptzcode.combineCode1 >>4);
	ptzcode.combineCode1 = ptzcode.combineCode1 | (tmp & 0x0f);

	ptzcode.addr = 0x00;
	ptzcode.data1 = 0x00;	//	字节 5
	ptzcode.data2 = 0x00;	//	字节 6
	ptzcode.combineCode2 = 0x00;

	switch(pControl->cmd_type)
	{
	case SIPD_PTZ_MOVE_UP:
		{
			ptzcode.instruction = 0x08;
			ptzcode.data2 = (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_DOWN:
		{
			ptzcode.instruction = 0x04;
			ptzcode.data2 =  (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_LEFT:
		{
			ptzcode.instruction = 0x02;
			ptzcode.data1 =  (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_RIGHT:
		{
			ptzcode.instruction = 0x01;
			ptzcode.data1 =  (unsigned char)pControl->speed;
		}
		break;

	case SIPD_PTZ_MOVE_LU:
		{
			ptzcode.instruction = 0x0A;
			ptzcode.data1 =  (unsigned char)pControl->speed;
			ptzcode.data2 =  (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_LD:
		{
			ptzcode.instruction = 0x06;
			ptzcode.data1 =  (unsigned char)pControl->speed;
			ptzcode.data2 =  (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_RU:
		{
			ptzcode.instruction = 0x09;
			ptzcode.data1 =  (unsigned char)pControl->speed;
			ptzcode.data2 =  (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_RD:
		{
			ptzcode.instruction = 0x05;
			ptzcode.data1 =  (unsigned char)pControl->speed;
			ptzcode.data2 =  (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_MOVE_STOP:
		{
			ptzcode.instruction = 0x00;
		}
		break;


	case SIPD_PTZ_ZOOMTELE:
		{
			//	镜头变倍放大（IN）, 字节 7 的高 4 位代表 速度
			ptzcode.instruction = 0x10;
			ptzcode.combineCode2 = (pControl->speed << 4) & 0xf0;
		}
		break;
	case SIPD_PTZ_ZOOMWIDE:
		{

			ptzcode.instruction = 0x20;
			ptzcode.combineCode2 = (pControl->speed << 4) & 0xf0;
		}
		break;
	case SIPD_PTZ_FOCUS_NEAR:
		{
			ptzcode.instruction = 0x42;
			ptzcode.data1 = (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_FOCUS_FAR:
		{
			ptzcode.instruction = 0x41;
			ptzcode.data1 = (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_HALLO_SMALL:
		{
			//	缩小光圈
			ptzcode.instruction = 0x48;
			ptzcode.data2 = (unsigned char)pControl->speed;
		}
		break;
	case SIPD_PTZ_HALLO_LARGE:
		{
			ptzcode.instruction = 0x44;
			ptzcode.data2 = (unsigned char)pControl->speed;
		}
		break;
	}

	// 生成校验码
	tmp = ptzcode.firstByteA5 + ptzcode.combineCode1 + ptzcode.addr + ptzcode.instruction + ptzcode.data1;
	ptzcode.check_code = tmp + ptzcode.data2 + ptzcode.combineCode2;

	
	sprintf_s (toHex, 3, "%02X", ptzcode.firstByteA5);
	sprintf_s (&toHex[2], 3, "%02X", ptzcode.combineCode1);
	sprintf_s (&toHex[4], 3, "%02X", ptzcode.addr);
	sprintf_s (&toHex[6], 3, "%02X", ptzcode.instruction);
	sprintf_s (&toHex[8], 3, "%02X", ptzcode.data1);
	sprintf_s (&toHex[10], 3, "%02X", ptzcode.data2);
	sprintf_s (&toHex[12], 3, "%02X", ptzcode.combineCode2);
	sprintf_s (&toHex[14], 3, "%02X", ptzcode.check_code);

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<PTZCmd>%s</PTZCmd>\r\n"\
		"</Control>",
		pControl->SN, pControl->device_id, toHex);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		ret = Sip_send_message(pControl->to_uri, pControl->from_uri,buffer);
	}

	/*返回 tid >0*/

	stringstream ss;
	ss << "Sip_PTZ_Move, SN = "<< string(pControl->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;

}


int Sip_PTZ_Preset(const sipd_preset_control * pPresetControl)
{
	int ret = OSIP_UNDEFINED_ERROR;
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	char toHex[32] = { 0 };
	memset(buffer, 0, sizeof(buffer));

	unsigned char tmp;
	struct sipd_PTZCMD  ptzcode;

	ptzcode.firstByteA5 = 0xA5;
	ptzcode.combineCode1 = 0x00;
	tmp = (ptzcode.firstByteA5 >>4) + (ptzcode.firstByteA5 & 0x0f) + (ptzcode.combineCode1 >>4);
	ptzcode.combineCode1 = ptzcode.combineCode1 | (tmp & 0x0f);

	ptzcode.addr = 0x00;
	ptzcode.data1 = 0x00;	//	字节 5
	ptzcode.data2 = 0x00;	//	字节 6
	ptzcode.combineCode2 = 0x00;

	switch(pPresetControl->cmd_type)
	{
	case SIPD_PTZ_PRESET_ADD:
		{
			ptzcode.instruction = 0x081;
			ptzcode.data2 = pPresetControl->preset_id;
		}
		break;
	case SIPD_PTZ_PRESET_DEL:
		{
			ptzcode.instruction = 0x082;
			ptzcode.data2 = pPresetControl->preset_id;
		}
		break;
	case SIPD_PTZ_PRESET_GOTO:
		{
			ptzcode.instruction = 0x083;
			ptzcode.data2 = pPresetControl->preset_id;
		}
		break;
	}

	// 生成校验码
	tmp = ptzcode.firstByteA5 + ptzcode.combineCode1 + ptzcode.addr + ptzcode.instruction + ptzcode.data1;
	ptzcode.check_code = tmp + ptzcode.data2 + ptzcode.combineCode2;


	sprintf_s (toHex, 3, "%02X", ptzcode.firstByteA5);
	sprintf_s (&toHex[2], 3, "%02X", ptzcode.combineCode1);
	sprintf_s (&toHex[4], 3, "%02X", ptzcode.addr);
	sprintf_s (&toHex[6], 3, "%02X", ptzcode.instruction);
	sprintf_s (&toHex[8], 3, "%02X", ptzcode.data1);
	sprintf_s (&toHex[10], 3, "%02X", ptzcode.data2);
	sprintf_s (&toHex[12], 3, "%02X", ptzcode.combineCode2);
	sprintf_s (&toHex[14], 3, "%02X", ptzcode.check_code);

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<PTZCmd>%s</PTZCmd>\r\n"\
		"</Control>",
		pPresetControl->SN, pPresetControl->device_id, toHex);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		ret = Sip_send_message(pPresetControl->to_uri, pPresetControl->from_uri,buffer);
	}

	/*返回 tid >0*/
	stringstream ss;
	ss << "Sip_PTZ_Preset, SN = "<< string(pPresetControl->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}

/*巡航操作命令*/
int Sip_PTZ_Tour(const sipd_tour_control * pTourControl)
{
	int ret = OSIP_UNDEFINED_ERROR;
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	char toHex[32] = { 0 };
	memset(buffer, 0, sizeof(buffer));

	unsigned char tmp;
	struct sipd_PTZCMD  ptzcode;

	ptzcode.firstByteA5 = 0xA5;
	ptzcode.combineCode1 = 0x00;
	tmp = (ptzcode.firstByteA5 >>4) + (ptzcode.firstByteA5 & 0x0f) + (ptzcode.combineCode1 >>4);
	ptzcode.combineCode1 = ptzcode.combineCode1 | (tmp & 0x0f);

	ptzcode.addr = 0x00;
	ptzcode.data1 = 0x00;	//	字节 5
	ptzcode.data2 = 0x00;	//	字节 6
	ptzcode.combineCode2 = 0x00;

	switch(pTourControl->tour_cmd)
	{
	case SIPD_PTZ_TOUR_ADD:
		{
			ptzcode.instruction = 0x084;
			ptzcode.data1 = pTourControl->tour_id;
			ptzcode.data2 = pTourControl->preset_id;
		}
		break;
	case SIPD_PTZ_TOUR_DEL:
		{
			ptzcode.instruction = 0x085;
			ptzcode.data1 = pTourControl->tour_id;
			ptzcode.data2 = pTourControl->preset_id;
		}
		break;
	case SIPD_PTZ_TOUR_START:
		{
			ptzcode.instruction = 0x088;
			ptzcode.data1 = pTourControl->tour_id;
		}
		break;
	case SIPD_PTZ_TOUR_SET_STAYTIME:
		{
			ptzcode.instruction = 0x087;
			ptzcode.data1 = pTourControl->tour_id;
			ptzcode.data2 = pTourControl->time & 0x0ff;
			ptzcode.combineCode2 = ((pTourControl->time >> 8) & 0x0f) << 4;
		}
		break;
	case SIPD_PTZ_TOUR_SET_SPEED:
		{
			ptzcode.instruction = 0x086;
			ptzcode.data1 = pTourControl->tour_id;
			ptzcode.data2 = pTourControl->speed & 0x0ff;
			ptzcode.combineCode2 = ((pTourControl->speed >> 8) & 0x0f) << 4;
		}
		break;
	case SIPD_PTZ_TOUR_STOP:
		{
			ptzcode.instruction = 0x00;
		}
		break;
	}

	// 生成校验码
	tmp = ptzcode.firstByteA5 + ptzcode.combineCode1 + ptzcode.addr + ptzcode.instruction + ptzcode.data1;
	ptzcode.check_code = tmp + ptzcode.data2 + ptzcode.combineCode2;


	sprintf_s (toHex, 3, "%02X", ptzcode.firstByteA5);
	sprintf_s (&toHex[2], 3, "%02X", ptzcode.combineCode1);
	sprintf_s (&toHex[4], 3, "%02X", ptzcode.addr);
	sprintf_s (&toHex[6], 3, "%02X", ptzcode.instruction);
	sprintf_s (&toHex[8], 3, "%02X", ptzcode.data1);
	sprintf_s (&toHex[10], 3, "%02X", ptzcode.data2);
	sprintf_s (&toHex[12], 3, "%02X", ptzcode.combineCode2);
	sprintf_s (&toHex[14], 3, "%02X", ptzcode.check_code);

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<PTZCmd>%s</PTZCmd>\r\n"\
		"</Control>",
		pTourControl->SN, pTourControl->device_id, toHex);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		ret = Sip_send_message(pTourControl->to_uri, pTourControl->from_uri,buffer);
	}

	/*返回 tid >0*/

	stringstream ss;
	ss << "Sip_PTZ_Tour, SN = "<< string(pTourControl->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());
	return ret;
}



/*自动扫描命令*/
int Sip_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl)
{
	int ret = OSIP_UNDEFINED_ERROR;
	char buffer[MAX_SIPD_RECORD_INFO_LEN+1] = {0};
	char toHex[32] = { 0 };
	memset(buffer, 0, sizeof(buffer));

	unsigned char tmp;
	struct sipd_PTZCMD  ptzcode;

	ptzcode.firstByteA5 = 0xA5;
	ptzcode.combineCode1 = 0x00;
	tmp = (ptzcode.firstByteA5 >>4) + (ptzcode.firstByteA5 & 0x0f) + (ptzcode.combineCode1 >>4);
	ptzcode.combineCode1 = ptzcode.combineCode1 | (tmp & 0x0f);

	ptzcode.addr = 0x00;
	ptzcode.data1 = 0x00;	//	字节 5
	ptzcode.data2 = 0x00;	//	字节 6
	ptzcode.combineCode2 = 0x00;

	switch(pAutoScanControl->cmd_type)
	{
	case SIPD_PTZ_AUTOSCAN_START:
		{
			ptzcode.instruction = 0x89;
			ptzcode.data1 = 0;	//	扫描组号
		}
		break;

	}

	// 生成校验码
	tmp = ptzcode.firstByteA5 + ptzcode.combineCode1 + ptzcode.addr + ptzcode.instruction + ptzcode.data1;
	ptzcode.check_code = tmp + ptzcode.data2 + ptzcode.combineCode2;


	sprintf_s (toHex, 3, "%02X", ptzcode.firstByteA5);
	sprintf_s (&toHex[2], 3, "%02X", ptzcode.combineCode1);
	sprintf_s (&toHex[4], 3, "%02X", ptzcode.addr);
	sprintf_s (&toHex[6], 3, "%02X", ptzcode.instruction);
	sprintf_s (&toHex[8], 3, "%02X", ptzcode.data1);
	sprintf_s (&toHex[10], 3, "%02X", ptzcode.data2);
	sprintf_s (&toHex[12], 3, "%02X", ptzcode.combineCode2);
	sprintf_s (&toHex[14], 3, "%02X", ptzcode.check_code);

	snprintf(buffer,MAX_SIPD_RECORD_INFO_LEN,
		"<?xml version=\"1.0\"?>\r\n"\
		"<Control>\r\n"\
		"<CmdType>DeviceControl</CmdType>\r\n"\
		"<SN>%s</SN>\r\n"\
		"<DeviceID>%s</DeviceID>\r\n"\
		"<PTZCmd>%s</PTZCmd>\r\n"\
		"</Control>",
		pAutoScanControl->SN, pAutoScanControl->device_id, toHex);

	if(/*Get_EvWaitFlg() ==*/ 1)
	{
		ret = Sip_send_message(pAutoScanControl->to_uri, pAutoScanControl->from_uri, buffer);
	}

	/*返回 tid >0*/

	stringstream ss;
	ss << "Sip_PTZ_AutoScan, SN = "<< string(pAutoScanControl->SN) << ", tid = " << ret;
	LOG4CPLUS_DEBUG(gb_Logger,ss.str());

	return ret;
}