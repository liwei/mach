/*
 *	Beispielklient fuer die Benutzung des Verschluesselungs-
 *      Servers  'c_user.c'
 *
 *      example for a client using the crypt server
 */

#include <stdio.h>
#include <mach.h>
#include <servers/netname.h>
#include <mach/std_types.h>
#include <string.h>

#include "crypt.h"


void main(int argc, char *argv[])
{
	port_t service;
	kern_return_t ret;
	int flag;
	char line1 [256], line2 [256];

	if (argc > 1) {
		fprintf(stderr, "%s benutzt keine Argumente", argv [0]);
                fprintf(stderr, "%s uses no arguments", argv [0]);
		exit(-1);
	}

	ret = netname_look_up(name_server_port, "", "CryptServer", 
		&service);
	if (ret != KERN_SUCCESS) {
		fprintf(stderr, "konnte Crypt-Server Port nicht finden");
                fprintf(stderr, "couldn't find crypt server port");
		exit(-1);
	}
        printf("1. String: \n");
        printf("1st string:");
	gets(line1);
	encode(service, line1);
	printf("1. String verschluesselt: %s\n", line1);
        printf("1st string crypted: %s \n", line1);
        printf("2. String: \n");
        printf("2nd string:");
	gets(line2);
	encode(service, line2);
	printf("2. String verschluesselt: %s\n", line2);
        printf("2nd string crypted: %s \n", line2);

	compare(service, line1,  line2, 
		&flag);
	if (flag == 0){
		printf("1. String und 2. String gleich\n");
                printf("1st and 2nd string are equal\n");
	}
	else {
		printf("1. String und 2. String nicht gleich\n");
                printf("1st and 2nd string are not equal\n");
	}
}
