/*
 *      Das Hauptmodul des Verschluesselungsservers  'c_server.c'
 *      Port kreieren und systemweit bekanntmachen
 *      Ankommende Strings verschluesseln
 *
 *      main module oft crypt server:
 *      creating port and anouncing it to the whole system
 *      crypting incoming strings
 */

#include <stdio.h>
#include <mach.h>
#include <mach/message.h>
#include <mach/std_types.h>
#include <servers/netname.h>
#include <string.h>
#include <mach_error.h>

#include "crypt.h"

extern msg_return_t mig_server();
extern boolean_t crypt_server();

extern char *crypt(char *, char *);

kern_return_t encode(port_t service, string256 str)
{

        strcpy((char *) str, crypt((char *) str, "  ") + 2);
        return KERN_SUCCESS;
}

kern_return_t compare(port_t service, string256 str1, 
                string256 str2, int *flag)
{
        *flag = strcmp((char *) str1, (char *) str2);
        return KERN_SUCCESS;
}
        
void main(int argc, char *argv [])
{
        port_name_t server;
        port_set_name_t set;
        kern_return_t ret;
        if (argc > 1) {
                fprintf(stderr, "%s benutzt keine Argumente\n");
                fprintf(stderr, "%s uses no arguments\n");
                exit(-1);
        }
        mach_init();
        ret = port_allocate(task_self(), &server);

        if (ret != KERN_SUCCESS) {
		fprintf(stderr, "konnte Port nicht anlegen\n");
                fprintf(stderr, "couldn't allocate port\n");
		exit(-1);
	}
	ret = port_set_allocate(task_self(), &set);
	if (ret != KERN_SUCCESS) {
		fprintf(stderr, "konnte Set nicht anlegen\n");
                fprintf(stderr, "couldn't allocate set\n");
		exit(-1);
	}	
	ret = port_set_add(task_self(), set, server);
	if (ret != KERN_SUCCESS) {
		fprintf(stderr, "konnte Set nicht erweitern\n");
                fprintf(stderr, "couldn't extend set\n");
		exit(-1);
	}
	ret = port_set_add(task_self(), set, task_notify());
	if (ret != KERN_SUCCESS) {
		fprintf(stderr, "konnte Set nicht erweitern(notify)\n");
                fprintf(stderr, "couldn't extend set (notify)\n");
		exit(-1);
	}
	ret = netname_check_in(name_server_port, "CryptServer", 
		PORT_NULL, server);
	if (ret != KERN_SUCCESS) {
                fprintf(stderr, "netname-checkin nicht moeglich:%d\n", ret);
                fprintf(stderr, "netname-checkin impossible:%d\n", ret);
		exit(-1);
	}
	ret = mig_server(set, crypt_server);
        fprintf(stderr, "Unerwartete Rueckkehr aus mig_server, Abbruch\n");
        fprintf(stderr, "unexpected return from mig_server: %s, exiting\n", mach_error_string(ret));
	exit(-1);
}
