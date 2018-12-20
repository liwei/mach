#include <mach.h>
#include <stdio.h>
#include <servers/netname.h>

main(argc, argv)
    int argc;
    char **argv;
{
    char *hostname, *objectname;
    port_t object;
    kern_return_t kr;

    if (argc != 3) {
	fprintf(stderr, "usage: checkout hostname objectname\n");
	exit(1);
    }
    hostname = argv[1];
    objectname = argv[2];
    kr = netname_look_up(name_server_port, hostname, objectname, &object);
    if (kr) {
	mach_error("netname_look_up", kr);
	exit(1);
    }
    kr = netname_check_out(name_server_port, objectname, PORT_NULL);
    if (! kr) {
	exit(0);
    }
    kr = netname_check_out(name_server_port, objectname, object);
    if (! kr) {
	exit(0);
    }
    mach_error("netname_check_out", kr);
    exit(1);
}
