
#include "gw_ref.h"
//#include "proxycore.h"


//#include "ppl_init.h"
#include "pplgwutility.h"

extern void SetMyException();

#if 0
#ifdef HAVE_SETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif
#endif

void proxycore_exit (int process_exit_value);

#if 0
#define PSP_SERVER_BASEVENDOR "Star-Net"
#define PSP_SERVER_BASEPRODUCT "ProxyCore"

#ifdef WIN32
#define PSP_SERVER_BASEREVISION PSP_VERSION
#define PSP_SERVER_BASEREVISION PSP_VERSION

#if defined(WIN32) && defined(HAVE_NT_SERVICE_MANAGER)
extern HANDLE started_event, stopped_event;
void ReportPartysipShutdownComplete ();
void CommenceStartupProcessing (LPCTSTR lpszServiceName,
				void (*stopper) (int));

#endif

#else
#define PSP_SERVER_BASEREVISION VERSION
#define PSP_SERVER_BASEREVISION VERSION
#endif

#define PSP_SERVER_BASEVERSION PSP_SERVER_BASEPRODUCT "/" PSP_SERVER_BASEREVISION
#define PSP_SERVER_VERSION  PSP_SERVER_BASEVERSION

#define PSP_SERVER_INSTALL_DIR PSP_SERVER_PREFIX


#if defined(__DATE__) && defined(__TIME__)
static const char server_built[] = __DATE__ " " __TIME__;
#else
static const char server_built[] = "unknown";
#endif

#define PSP_SERVER_WHO_AM_I PSP_SERVER_BASEVENDOR "/" PSP_SERVER_VERSION

#define PSP_PLUGIN_API "1.0"

#define PSP_SERVER_HEADER   "StarNet/ProxyCore-" PSP_SERVER_BASEREVISION
#endif

FILE *log_file = NULL;


void
proxycore_exit (int process_exit_value)
{

#if 0
  psp_core_free ();
//  psp_config_unload ();
#endif

  unInit_gw();

#if 0
#if defined(WIN32) && defined(HAVE_NT_SERVICE_MANAGER)
  ReportPartysipShutdownComplete ();
  osip_usleep (1000000); /* is it still needed? */
#endif
#endif

  OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_INFO1, NULL,
			  "program has terminated.\n"));

  if (log_file != NULL && log_file != stdout)
    fclose (log_file);


#if defined(WIN32) && defined(HAVE_NT_SERVICE_MANAGER)
  return;
#else
  /* in case of services, the method MUST return */
  exit (process_exit_value);
#endif
}


#ifdef WIN32
char *
simple_readline (int descr)
{
  int i = 0;
  char *tmp = (char *) osip_malloc (201);

  fgets (tmp, 200, stdin);
  osip_clrspace (tmp);
  return tmp;
}
#else
static char *
simple_readline (int descr)
{
  int ret;
  fd_set fset;

  FD_ZERO (&fset);
  FD_SET (descr, &fset);
  ret = select (descr + 1, &fset, NULL, NULL, NULL);

  if (FD_ISSET (descr, &fset))
    {
      char *tmp;
      int i;

      tmp = (char *) osip_malloc (201);
      i = read (descr, tmp, 200);
      tmp[i] = '\0';
      if (i > 0)
	osip_clrspace (tmp);
      return tmp;
    }
  return NULL;
}
#endif


static void
main_run ()
{
  char *tmp;

  while (1)
    {
      printf ("(proxycore)");
      fflush (stdout);
      tmp = simple_readline (1);
      if (tmp != NULL)
	{
	  if (strlen (tmp) >= 6 && 0 == strncmp (tmp, "plugin", 6))
	    ;
	  else if (strlen (tmp) >= 4 && 0 == strncmp (tmp, "help", 4))
	    ;
#ifdef ENABLE_MPATROL
	  else if (strlen (tmp) == 1 && 0 == strncmp (tmp, "s", 1))
	    {
	      static int on=0;
	      if (on==0)
		{
		  __mp_clearleaktable();
		  __mp_startleaktable();
		  on = 1;
		}
	      else
		{
		  __mp_stopleaktable();
		  __mp_leaktable(0, MP_LT_UNFREED, 0);
		  on = 0;
		}
	    }
#endif
	  else if (strlen (tmp) == 1 && 0 == strncmp (tmp, "q", 1))
	    {
	    	 #if 0 /*FOR NOW*/
	      osip_free (tmp);
	      proxycore_exit (0);
		  #endif
	    }
	  else if (strlen (tmp) == 4 && 0 == strncmp (tmp, "quit", 4))
	    {
	    	  #if 0 /*FOR NOW*/
	      osip_free (tmp);
	      proxycore_exit (0);
		  #endif
	    }
	  else
	  {
	    //printf ("error: %s: command not found!\n", tmp);
	  }
	  osip_free (tmp);
	}
      else
	proxycore_exit (1);
    }

}


#if 0
static int
main_load_plugins (const char* port)
{
  int i;
  psp_plugin_t *psp_plugin;

  /*
     The config file MUST contain the list of plugins as in:
     plugins = udp ......
     just do it below for now!
   */
   if(NULL == port)
   {
   	 i = psp_plugin_load (&psp_plugin, "udp", "5060");
   }
   else
   {
   	 i = psp_plugin_load (&psp_plugin, "udp", port);
   }
    if (i != 0)
   {
	  OSIP_TRACE (osip_trace (__FILE__, __LINE__, OSIP_ERROR, NULL,
				  "failed to load plugin %s\n", i));
	  return -1;
   }
	  
  
  return 0;
}
#endif

#if defined(WIN32) && defined(HAVE_NT_SERVICE_MANAGER)

int WINAPI
ServiceMain (DWORD argc, LPTSTR * argv)
#else

int
main (int argc, const char *const argv[])
#endif
{
  char c;
  int i;
  //ppl_status_t rv;
  int interactive_mode = 1;
  int cf_debug_level = 6;
  const char *cf_log_file = "gw.log";
  const char *optarg;
  SIPD_GATEWAY_CFG sip_cfg;
  char c_port[MAX_SIPD_NORMAL_LEN+1];

  SetMyException();


#if 0
#if defined(WIN32) && defined(HAVE_NT_SERVICE_MANAGER)
  interactive_mode = 2;
  CommenceStartupProcessing ("GB Proxy Server", proxycore_exit);
  cf_debug_level = 6;
#endif

#ifdef HAVE_SETRLIMIT
  {
    struct rlimit rlimit;
    rlimit.rlim_cur = 2097151;
    rlimit.rlim_max = 2097151;
    if (setrlimit (RLIMIT_CORE,&rlimit) == -1)
      {
	perror("Could not reset new core size (2097151) with setrlimit");
	exit(1);
      }
  }
#endif
#endif


 if( init_domain_server() != 0)
 {
 	printf("main init_domain_server ERROR, return \n");
	return -1;
 }

 memset(&sip_cfg, 0, sizeof(sip_cfg));
 gw_get_domain_server_cfg_init(&sip_cfg);
 

#if 0
 /* ppl_initialize(); */
  ppl_init_open ( );
#endif

 

  /*********************************/
  /*   Load Config File            */

 /*Find serverip, servername(hostname)*/
/*TODO ... ...
   get local serverip and hostname, and del psp_core_load_all_ipv4
*/


  /* check some mandatory element in the config file */
  /*ignore for now*/



  /*********************************/
  /* INIT Log File and Log LEVEL   */

  if (cf_debug_level > 0)
    {
#ifdef ENABLE_SYSLOG
      osip_trace_initialize_syslog (cf_debug_level, "sipserver");
#else
      if (/*cf_log_file != NULL*/ 0)
	{
	  log_file = fopen (cf_log_file, "w+");
	  if (NULL == log_file)
	    {
	      printf ("Log name:           %s\n", cf_log_file);
	      perror ("ERROR: Could not open log file");
	      exit (1);
	    }
	}
      else
	log_file = NULL;
      TRACE_INITIALIZE (cf_debug_level, log_file);
#endif
    }

#if 0
  i = psp_core_init (sip_cfg.local_ip,  sip_cfg.local_domain_name,  sip_cfg.local_port);
  if (i != 0)
    {
      fprintf (stderr, "ERROR: Could not initialize proxycore!\n");
      goto main_error;
    }

  /* load general user configuration */
  /*ignore for now*/

  /* from here, let's load plugins */
  memset(c_port, 0, sizeof(c_port));
  snprintf(c_port, MAX_SIPD_NORMAL_LEN, "%d", sip_cfg.local_port);
  i = main_load_plugins (c_port);
  if (i != 0)
    {
      fprintf (stderr, "ERROR: Could not load plugins!\n");
      proxycore_exit (0);
    }

#if defined(WIN32) && defined(HAVE_NT_SERVICE_MANAGER)
  SetEvent (started_event);
#endif

  /* start all modules that we want as thread */
  i = psp_core_start (interactive_mode);	/* block if interactive_mode==0 */
  if (i != 0)
    {
      fprintf (stderr, "ERROR: Could not start modules of proxycore!\n");
      goto main_error;
    }
  #endif

  gw_init();
  printf("gw_init done! \n");

  if (interactive_mode == 1)
    main_run ();



  if (interactive_mode == 1)
    proxycore_exit (0);		/* else, this is done somewhere else */

  return 0;
main_error:
  proxycore_exit (0);
  return -1;
}
