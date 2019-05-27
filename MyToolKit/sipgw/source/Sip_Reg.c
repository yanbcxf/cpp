#include "sipd.h"

#define UA_STRING "SN SIPGW V1.0"
#define URI_MAX_LEN 100

static SIPD_GATEWAY_CFG g_Cfg;
static SIPD_GATEWAY_CFG g_Cfg_Change;  

static int g_keepalive_count = 0;




extern void Set_EvWaitFlg(int in);


/*注册信息*/
typedef struct regparam_t {
	int regid;  
	int expiry;
	int auth;
} regparam_t;

struct regparam_t regparam = { 0, 3600, 0 };


void Set_Reg_Id(int in)
{
	reg_id_lock();
	regparam.regid = in;
	reg_id_unlock();
}

int Get_Reg_Id()
{
	int out;
	reg_id_lock();
	out = regparam.regid;
	reg_id_unlock();

	return out;
}

static int para_change()
{
	if((strcmp((char *)g_Cfg_Change.local_domain_id, (char *)g_Cfg.local_domain_id) != 0)||
			(strcmp((char *)g_Cfg_Change.local_domain_name, (char *)g_Cfg.local_domain_name) != 0)||
			(strcmp((char *)g_Cfg_Change.register_upper_server_passwd, (char *)g_Cfg.register_upper_server_passwd) != 0)||
			(strcmp((char *)g_Cfg_Change.upper_server_id, (char *)g_Cfg.upper_server_id) != 0)||
			(g_Cfg_Change.upper_server_port!=  g_Cfg.upper_server_port)||
			(strcmp((char *)g_Cfg_Change.upper_server_ip, (char *)g_Cfg.upper_server_ip) != 0)||
			(g_Cfg_Change.register_keepalive!= g_Cfg.register_keepalive)||
			(g_Cfg_Change.keepalive_internal!= g_Cfg.keepalive_internal))
		return 1;
	
	return 0;
}

void Set_Para_Change(/*PRM_SIPD_CFG*/SIPD_GATEWAY_CFG *src)
{
	para_change_lock();
	memcpy(&g_Cfg_Change,  src, sizeof(g_Cfg_Change));
	para_change_unlock();
}



void SIPD_LogOut()
{
	osip_message_t *reg;

	context_eXosip_lock();
	eXosip_lock(context_eXosip); 	
	eXosip_register_build_register(context_eXosip, Get_Reg_Id(), 0, &reg);//
	//eXosip_register_send_register(context_eXosip, Get_Reg_Id(),&reg);

	osip_message_free(reg);

	eXosip_register_send_register(context_eXosip, Get_Reg_Id(),NULL);

	//regparam.auth = 0;
	//regparam.expiry = 3600;
	//Set_Reg_Id(0);
	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	//osip_message_free(reg);
}

void Setkeepalive_count(int in)
{
	keepalivecn_lock();
	g_keepalive_count = in;
	keepalivecn_unlock();
}

int SIPD_send_register_req(char *username, char *password, char *identity, char *registerer, char* contact)
{
	int ret = -1;
	int rid = 0;

	osip_message_t *reg = NULL;

	if((username == NULL)||(password == NULL)||(identity == NULL)||(registerer == NULL))
		return -1;

	context_eXosip_lock();
	eXosip_lock(context_eXosip);

	eXosip_set_user_agent(context_eXosip, UA_STRING);
	
	rid = eXosip_register_build_initial_register(context_eXosip, identity, registerer, 
				 contact, regparam.expiry, &reg);

	if (rid < 1) 
	{
		printf("eXosip_register_build_initial_register failed\n");
		eXosip_unlock(context_eXosip);
		context_eXosip_unlock();
		return -1;

	}

	printf("username: %s password: [removed]\n", username);

	eXosip_clear_authentication_info(context_eXosip);  //去除上次加入的错误认证信息
	if (eXosip_add_authentication_info(context_eXosip, username, username, password, "MD5", NULL)) 
	{
		printf("eXosip_add_authentication_info failed");
		osip_message_free(reg);
		eXosip_unlock(context_eXosip);
		context_eXosip_unlock();
		return -1;
		
	}

#if 1
	ret = eXosip_register_send_register(context_eXosip, rid, reg);


	if (ret != 0) 
	{
		printf("eXosip_register_send_register failed\n");
		osip_message_free(reg);
		eXosip_unlock(context_eXosip);
		context_eXosip_unlock();
		return -1;
	}
#endif

	eXosip_unlock(context_eXosip);
	context_eXosip_unlock();

	return 0;
}

#if defined(WIN32)
DWORD WINAPI SIPD_Reg_Proc(void *data)
#else
void *SIPD_Reg_Proc(void *arg)
#endif
{
	char username[MAX_SIPD_DEVICE_ID_LEN+1];
	char password[MAX_SIPD_NORMAL_LEN+1];
	char identity[URI_MAX_LEN];
	char registerer[URI_MAX_LEN];
	char contact[URI_MAX_LEN];
	int ret = -1;
	int sleeptime = 2;
	int maxRegErrCnt = 8;
	
ReGetPara:
	memset(username,0, MAX_SIPD_DEVICE_ID_LEN+1);
	memset(password,0, MAX_SIPD_NORMAL_LEN+1);
	memset(identity,0, URI_MAX_LEN);
	memset(registerer,0, URI_MAX_LEN);
	memset(contact, 0, URI_MAX_LEN);
	memset(&g_Cfg, 0, sizeof(g_Cfg));
	memset(&g_Cfg_Change, 0, sizeof(g_Cfg_Change));

	ret = get_PRM_SIPD_CFG(&g_Cfg);

	if(ret == -1)
	{
		printf("get_PRM_SIPD_CFG err\n");
		#if defined(WIN32)
		Sleep(2*1000);
		#else
		sleep(2);
		#endif
		goto ReGetPara;
	}

	Set_Para_Change(&g_Cfg);

	strcpy(username, (char*)g_Cfg.local_domain_id);
	
	memcpy(password, g_Cfg.register_upper_server_passwd, sizeof(password));

	
	snprintf(identity, URI_MAX_LEN, "sip:%s@%s", username, g_Cfg.local_domain_name);
	snprintf(registerer, URI_MAX_LEN, "sip:%s@%s:%d", g_Cfg.upper_server_id, g_Cfg.upper_server_ip, g_Cfg.upper_server_port);
	snprintf(contact, URI_MAX_LEN, "sip:%s@%s:%d",username, g_Cfg.local_ip, g_Cfg.local_port);

	strcpy(g_Cfg.local_domain_id, username);
	
	regparam.expiry = g_Cfg.register_keepalive;

	
	if(/*g_Cfg.enable*/1)
	{
		ret = SIPD_send_register_req(username, password, identity, registerer, contact);  //发送初始注册包
		if(ret == -1)
		{
			printf("%s line=%d SIPD_send_register_req ERROR!!!! ret=%d \n", __FUNCTION__, __LINE__, ret);

			/*
			while( ret != 0)
			{
				sleep(5);
				ret = SIPD_send_register_req(username, password, identity, registerer);  //发送初始注册包
				printf("%s line=%d SIPD_send_register_req ERROR!!!! ret=%d \n", __FUNCTION__, __LINE__, ret);
			}
			*/
		}
	}

	
	while(1)
	{	
		if(para_change() == 1)
		{
			Set_RegErrCnt(0);
			
			if(Get_Reg_Id() > 0)
			{
				//	参数发送变化，原先注册成功了，现在退出
				int i=0;
				SIPD_LogOut(); 
				printf("log_out\n");

				//	等待退出成功
				i=0;
				while(1)
				{
					if(Get_LogOutFlg() == 1)
					{
						break;
					}
					
					if(i >= 15)
					{
						printf("%s line=%d wait log out too loog, don't wait any more!\n", __FUNCTION__, __LINE__);
						i=0;
						break;
					}
					i++;

					#if defined(WIN32)
					Sleep(2*1000);
					#else
					sleep(2);
					#endif
				}

				Set_LogOutFlg(0);
			}

			Set_Dev_Online(0);
			Setkeepalive_count(0); 
			Set_NotifyFlg(0);
			memset(username,0, MAX_SIPD_DEVICE_ID_LEN+1);
			memset(password,0, MAX_SIPD_NORMAL_LEN+1);
			memset(identity,0, URI_MAX_LEN);
			memset(registerer,0, URI_MAX_LEN);
			memset(contact, 0, URI_MAX_LEN);

			memcpy(&g_Cfg, &g_Cfg_Change, sizeof(SIPD_GATEWAY_CFG));
			regparam.expiry = g_Cfg.register_keepalive;


			if(context_eXosip != NULL)
			{
#if 1				
				if(Get_Reg_Id()> 0)
				{
					context_eXosip_lock();
					eXosip_lock (context_eXosip);
					
					ret = eXosip_register_remove(context_eXosip, Get_Reg_Id());
					if(ret != OSIP_SUCCESS)
					{
						if(ret != OSIP_NOTFOUND)
						{
							printf("%s line=%d eXosip_register_remove failed! ret=%d\n", __FUNCTION__, __LINE__, ret);
						}
					}

					Set_Reg_Id(0);
					eXosip_unlock (context_eXosip);
					context_eXosip_unlock();
				}
#endif
				

				Set_EvWaitFlg(0);

				context_eXosip_lock();
				eXosip_quit(context_eXosip);
				context_eXosip_unlock();
			}

			// 参数变化了，可能使用了不同协议，重新初始化协议栈
			ret = init_sip_protocol_stack();
	        if (ret != 0)
	        {
		       	printf("%s line=%d init_sip_protocol_stack failed \n", __FUNCTION__, __LINE__);
				#if defined(WIN32)
				Sleep(2*1000);
				#else
				sleep(2);
				#endif
				continue;
	        }

			Set_EvWaitFlg(1);

			strcpy(username, (char*)g_Cfg.local_domain_id);
			strcpy(password, g_Cfg.register_upper_server_passwd);

			
			
			snprintf(identity, URI_MAX_LEN, "sip:%s@%s", username, g_Cfg.local_domain_name);
			snprintf(registerer, URI_MAX_LEN, "sip:%s@%s:%d", g_Cfg.upper_server_id, g_Cfg.upper_server_ip, g_Cfg.upper_server_port);
			snprintf(contact, URI_MAX_LEN, "sip:%s@%s:%d",username, g_Cfg.local_ip, g_Cfg.local_port);

			strcpy(g_Cfg.local_domain_id, username);

		}//	参数变化处理结束

		if(/*g_Cfg.enable*/1)
		{			
			if(Get_Dev_Online() == -2)  //设备重启
			{
				break;
			}
		
			if(Get_Dev_Online() == 0)
			{	
				ret = SIPD_send_register_req(username, password, identity, registerer, contact);  //发送初始注册包
				if(ret == -1)
				{
					printf("SIPD_send_register_req err\n");
				}

				Set_Dev_Online(-1);
			}
			else if (Get_Dev_Online() == -1)
			{
				/*register response is NULL*/
				RegErrCounter();

				if(Get_RegErrCnt() == maxRegErrCnt)
				{
					Set_RegErrCnt(0);
					Set_Dev_Online(0);
				}
			}
			else
			{
				Set_RegErrCnt(0);
			
				for(;;)
				{					
					if(para_change() == 1)
						break;

					if(Get_Dev_Online() == -1)
					{
						Set_Dev_Online(0);
					}

					if(Get_Dev_Online() == 0)
					{
						Setkeepalive_count(0);
						break;
					}

					if(Get_Dev_Online() == -2)  //设备重启
					{
						break;
					}

					if(g_keepalive_count == (int)(g_Cfg.keepalive_internal/sleeptime)) 
					{	
						context_eXosip_lock();
						eXosip_lock (context_eXosip);
						ret = eXosip_register_send_register (context_eXosip, Get_Reg_Id(), NULL);
						if (0 > ret) 
						{
							printf("eXosip_register_send_register keepalive err\n");
							Set_Dev_Online(0); 
						}
						else
						{
							Set_NotifyFlg(1);
						}
						
						Setkeepalive_count(0); 
					    eXosip_unlock (context_eXosip);
						context_eXosip_unlock();
					}	

					keepalivecn_lock();
					g_keepalive_count++;
					keepalivecn_unlock();

					#if defined(WIN32)
					Sleep(sleeptime*1000);
					#else
					sleep(sleeptime);
					#endif
				}
			}
		}

		#if defined(WIN32)
		Sleep(5*1000);
		#else
		sleep(5);
		#endif
	}
}





