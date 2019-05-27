#include "sipd.h"





int gw_init(void)
{
	int ret = -1;
	
	printf("gw_init \n");
	
	ret = init_sip_protocol_stack();
       if (ret != 0)
       {
       	printf("gw_init init_sip_protocol_stack failed \n");
		return -1;
       }

	if (gw_create_threads() != 0)
	{
		printf("gw_init gw_create_threads failed! \n");
	}

	if(gw_register_callback() != 0)
	{
		printf("gw_init gw_register_callback failed! \n");
	}

	printf("gw_init successful! \n");

	return 0;
}


