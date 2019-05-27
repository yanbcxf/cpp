#ifndef __GWINTERFACE_H__
#define __GWINTERFACE_H__
#include "GWInterfaceDef.h"

#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------
/*����˵��:��ʼ��
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Init();

//-----------------------------------------------------------------------
/*����˵��:����ʼ
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Start();

//-----------------------------------------------------------------------
/*����˵��:��ȡ����������Ϣ
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_GetSIPServerCfg (SIPD_GATEWAY_CFG* pCfg);

//-----------------------------------------------------------------------
/*����˵��:ע�������Ϣ����ص�
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Register_ServerConfig_CB (sip_config_pcb_func config_fun);

//-----------------------------------------------------------------------
/*����˵��:ע�ᱨ����Ϣ�ص�
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Register_AlarmInd_CB (sip_alarm_ind_pcb_func alarm_ind_fun);

//-----------------------------------------------------------------------
/*����˵��:����ƽ̨ʱ��
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_settime(const sipd_id_info * pDeviceID,  const SIPD_TIME* pTime);

//-----------------------------------------------------------------------
/*����˵��:��̨�ƶ�����
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_PTZ_Move(const sipd_ptz_control* pControl);

//-----------------------------------------------------------------------
/*����˵��:��̨Ѳ������
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_PTZ_Tour(const sipd_tour_control * pTourControl);

//-----------------------------------------------------------------------
/*����˵��:Ԥ�õ����
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_PTZ_Preset(const sipd_preset_control * pPresetControl);

//-----------------------------------------------------------------------
/*����˵��:�Զ�ҡ��
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_PTZ_AutoScan(const sipd_autoscan_control * pAutoScanControl);

//-----------------------------------------------------------------------
/*����˵��:�����豸
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Reboot(const sipd_id_info * pIdInfo);

//-----------------------------------------------------------------------
/*����˵��:¼�����
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_record_control (const sipd_record_control * pRecordControl);

//-----------------------------------------------------------------------
/*����˵��:��������
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_alarm_control (const sipd_alarm_control * pAlarmControl);

//-----------------------------------------------------------------------
/*����˵��:ע��������ƻص�
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Register_ControlRsp_CB(sip_control_req_pcb_func control_rsp_fun);

//-----------------------------------------------------------------------
/*����˵��:�豸�б���ѯ
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_cataloginfo_request(const sipd_catalog_req * pCatalogReq);

//-----------------------------------------------------------------------
/*����˵��:�豸��Ϣ��ѯ
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_deviceinfo_request(const sipd_deviceinfo_req * pDeviceinfoReq);

//-----------------------------------------------------------------------
/*����˵��:�豸״̬��ѯ
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_stateinfo_request(const sipd_devicestate_req * pStateInfoReq);

//-----------------------------------------------------------------------
/*����˵��:ע���ѯ����ص�
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Register_InfoRsp_CB(sip_info_req_pcb_func rsp_fun);

//-----------------------------------------------------------------------
/*����˵��:¼���ѯ
/*����:��
/*����ֵ:0��ʾ�ɹ���<0��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_search_recordfile_req (sipd_recordinfo_req * pRecordFileReq);

//-----------------------------------------------------------------------
/*����˵��:ע��ý����ƻص�
/*����:��
/*����ֵ:0��ʾ�ɹ�������ֵ��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Register_MediaControl_CB(sip_media_pcb_func media_control_fun);

//-----------------------------------------------------------------------
/*����˵��:�ط�֪ͨ
/*����:��
/*����ֵ:0��ʾ�ɹ�������ֵ��ʾʧ��*/
//-----------------------------------------------------------------------
int Invite_play_playback_download(const sipd_media_session* pMediaInfo);

//-----------------------------------------------------------------------
/*����˵��:֪ͨý�����ؽ���ý�����
/*����:��
/*����ֵ:0��ʾ�ɹ�������ֵ��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_media_control_notify_CMS(const int did, const int tid, const enum SIPD_TO_CMS_CMD_TYPE cmd_type, const sipd_playback_mans_rtsp_ind* playback_rtsp_control);
//-----------------------------------------------------------------------
/*����˵��:ֹͣ
/*����:��
/*����ֵ:0��ʾ�ɹ�������ֵ��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Stop();

//-----------------------------------------------------------------------
/*����˵��:����ʼ��
/*����:��
/*����ֵ:0��ʾ�ɹ�������ֵ��ʾʧ��*/
//-----------------------------------------------------------------------
int Sipd_Uninit();

#ifdef __cplusplus
}
#endif

#endif
