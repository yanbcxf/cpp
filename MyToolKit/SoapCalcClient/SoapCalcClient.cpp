// SoapCalcClient.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "calc.nsmap" // XML namespace mapping table (only needed once at the global level)
#include "soapH.h"    // client stubs, serializers, etc.

/* the Web service endpoint URL */
//const char server[] = "http://websrv.cs.fsu.edu/~engelen/calcserver.cgi";
const char server[] = "http://127.0.0.1:8080";

int main()
{
	struct soap *soap = soap_new1(SOAP_XML_INDENT); /* new context */
	soap->connect_timeout = 10; // connect within 10s
	soap->send_timeout = 5;     // send timeout is 5s
	soap->recv_timeout = 5;     // receive timeout is 5s

	double a, b, result;

	if (0)
	{
		fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");
		exit(0);
	}
	/*a = strtod(argv[2], NULL);
	b = strtod(argv[3], NULL);*/
	a = 11.1;
	b = 22.2;
	char argv = 'a';
	switch (argv)
	{
	case 'a':
		soap_call_ns2__add(soap, server, "", a, b, &result);
		break;
	case 's':
		soap_call_ns2__sub(soap, server, "", a, b, &result);
		break;
	case 'm':
		soap_call_ns2__mul(soap, server, "", a, b, &result);
		break;
	case 'd':
		soap_call_ns2__div(soap, server, "", a, b, &result);
		break;
	case 'p':
		soap_call_ns2__pow(soap, server, "", a, b, &result);
		break;
	default:
		fprintf(stderr, "Unknown command\n");
		exit(0);
	}
	if (soap->error)
		soap_print_fault(soap, stderr);
	else
		printf("result = %g\n", result);
	soap_destroy(soap); /* delete deserialized objects */
	soap_end(soap);     /* delete heap and temp data */
	soap_free(soap);    /* we're done with the context */

	system("pause");
	return 0;
}

