#include <mach.h>
#include <stdio.h>
#include <sys/time.h>

#define	MAP_ADDR	0x50000000	
char *object_name	= "netmemoryserver.test.object";

vm_offset_t
map_object(size, hostname)
    int size;
    char *hostname;
{
    vm_offset_t addr;
    kern_return_t kr;
    char *netmemory_server_name, *getenv();
    port_t netmemory_server;
    port_t memory_object;
    port_t netmemory_object;
    port_t netmemory_control;

    /*
     * Find the netmemory server.
     */
    netmemory_server_name = getenv("NETMEMORYSERVER_NAME");
    if (! netmemory_server_name) {
	netmemory_server_name = "netmemoryserver";
    }
    kr = netname_look_up(name_server_port, "", netmemory_server_name,
			 &netmemory_server);
    if (kr) {
	mach_error("netname_look_up(netmemoryserver)", kr);
	exit(1);
    }

    /*
     * Slave: find the object.
     * Master: create and register the object.
     *
     * Only the slave provides a hostname, which is the location of the object.
     */
    if (hostname) {
	kr = netname_look_up(name_server_port, hostname, object_name,
			     &netmemory_object);
	if (kr) {
	    mach_error("netname_look_up(object)", kr);
	    exit(1);
	}
    } else {
	kr = netmemory_create(netmemory_server, size, &netmemory_object,
			      &netmemory_control);
	if (kr) {
	    mach_error("netmemory_create", kr);
	    exit(1);
	}
	kr = netname_check_in(name_server_port, object_name, PORT_NULL,
			      netmemory_object);
	if (kr) {
	    mach_error("netname_check_in", kr);
	    netmemory_destroy(netmemory_control);
	    exit(1);
	}
    }

    /*
     * Map the object.
     */
    kr = netmemory_cache(netmemory_server, netmemory_object, &memory_object);
    if (kr) {
	mach_error("netmemory_cache", kr);
	exit(1);
    }
    if (! size) {
	return (vm_offset_t) memory_object;
    }
    addr = MAP_ADDR;
    kr = vm_map(task_self(), &addr, size, 0, FALSE, memory_object, 0, FALSE,
		VM_PROT_ALL, VM_PROT_ALL, VM_INHERIT_SHARE);
    if (kr) {
	mach_error("vm_map", kr);
	exit(1);
    }
    return addr;
}

master_check(addr, size)
    char *addr;
    int size;
{
    int i;

    for (i = 0; i < size; i++) {
	addr[i] = i % 113;
    }
}

slave_check(addr, size)
    char *addr;
    int size;
{
    int i;

    for (i = 0; i < size; i++) {
	if (addr[i] != i % 113) {
	    printf("%d: %d vs %d!\n", i, addr[i], i % 113);
	}
    }
}

#define SPIN_SWTCH	1

master_spin(addr, size)
    char *addr;
    int size;
{
    int *ip = (int *) addr;
    int count = size / vm_page_size;
    int i;
    boolean_t verbose = (count & 01);

    for (i = 1; i <= count; i++) {
	while (*ip != 0) {
#if SPIN_SWTCH
	    swtch();
#endif
	}
	*ip = i;
	if (verbose) {
	    printf("%d ", i);
	    fflush(stdout);
	}
    }
}

slave_spin(addr, size)
    char *addr;
    int size;
{
    int *ip = (int *) addr;
    int count = size / vm_page_size;
    int i;
    boolean_t verbose = (count & 01);

    for (i = 1; i <= count; i++) {
	while (*ip == 0) {
#if SPIN_SWTCH
	    swtch();
#endif
	}
	*ip = 0;
	if (verbose) {
	    printf(". ");
	    fflush(stdout);
	}
    }
}

do_read(addr, size)
    char *addr;
    int size;
{
    int i;
    char c;

    for (i = 0; i < size; i += vm_page_size) {
	c = addr[i];
    }
}

do_write(addr, size)
    char *addr;
    int size;
{
    int i;

    for (i = 0; i < size; i += vm_page_size) {
	addr[i] = 1;
    }
}

do_null(addr, size)
    char *addr;
    int size;
{
    int i;
    char c;

    for (i = 0; i < size; i += vm_page_size) {
	c = (char) i;
    }
}

master_usage()
{
    fprintf(stderr,
	    "usage: master [spin | check | read | write | null] size(KB)\n");
    exit(1);
}

master(argc, argv)
    int argc;
    char **argv;
{
    char *addr;
    int size, (*use)();

    if (argc != 3) {
	master_usage();
    }
    if (! strcmp(argv[1], "spin")) {
	use = master_spin;
    } else if (! strcmp(argv[1], "check")) {
	use = master_check;
    } else if (! strcmp(argv[1], "read")) {
	use = do_read;
    } else if (! strcmp(argv[1], "write")) {
	use = do_write;
    } else if (! strcmp(argv[1], "null")) {
	use = do_null;
    } else {
	master_usage();
    }
    size = 1024 * atoi(argv[2]);
    size = (size + vm_page_size - 1) & ~(vm_page_size - 1);
    addr = (char *) map_object(size, 0);

    printf("working... ");
    fflush(stdout);

    (*use)(addr, size);

    printf("done.\n");
    for (;;) {
	sleep(1000000);
    }
}

slave_usage()
{
    fprintf(stderr, "usage: slave [spin | check | read | write] size(KB)\n");
    exit(1);
}

slave(argc, argv)
    int argc;
    char **argv;
{
    char *addr;
    int size, (*use)();
    struct timeval tv1, tv2;
    float total;

    if (argc != 4) {
	slave_usage();
    }
    if (! strcmp(argv[1], "spin")) {
	use = slave_spin;
    } else if (! strcmp(argv[1], "check")) {
	use = slave_check;
    } else if (! strcmp(argv[1], "read")) {
	use = do_read;
    } else if (! strcmp(argv[1], "write")) {
	use = do_write;
    } else {
	slave_usage();
    }
    size = 1024 * atoi(argv[3]);
    size = (size + vm_page_size - 1) & ~(vm_page_size - 1);
    addr = (char *) map_object(size, argv[2]);

    printf("working... ");
    fflush(stdout);
    gettimeofday(&tv1, 0);

    (*use)(addr, size);

    gettimeofday(&tv2, 0);
    printf("done.\n");
    total  = 1.0      * (tv2.tv_sec  - tv1.tv_sec);
    total += 0.000001 * (tv2.tv_usec - tv1.tv_usec);
    printf("total_time: %.3f sec\n", total);
    printf("%.3f KB/sec\n", size / 1024.0 / total);
    printf("%.3f msec/msg\n", total * 1000.0 / (size / vm_page_size));
    exit(0);
}

stats_usage()
{
    fprintf(stderr, "usage: stats host [ object-name ]\n");
    exit(1);
}

stats(argc, argv)
    int argc;
    char **argv;
{
    port_t memory_object;
    vm_offset_t addr;
    vm_size_t size;

    if (argc < 2 || argc > 3) {
	stats_usage();
    } else if (argc == 3) {
	object_name = argv[2];
    }
    memory_object = (port_t) map_object(0, argv[1]);
    netmemory_statistics(memory_object, &addr, &size);
    exit(0);
}

main(argc, argv)
    int argc;
    char **argv;
{
    int len;
    char *s, *a0 = argv[0];

    for (s = &a0[strlen(a0)]; s > a0; s--) {
	if (*s == '/') {
	    s++;
	    break;
	}
    }
    if (! strcmp(s, "master")) {
	master(argc, argv);
    } else if (! strcmp(s, "slave")) {
	slave(argc, argv);
    } else if (! strcmp(s, "stats")) {
	stats(argc, argv);
    } else {
	fprintf(stderr, "usage: [master | slave] ...\n");
	exit(1);
    }
}
