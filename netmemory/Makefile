all	: master slave 

CC	= gcc
CFLAGS	= -g

# The library interface to netname_check_in requires that the task have
# receive rights to the port that it wishes to check-in. This is unecessary
# and does not work for this program as it is checking in a netmsgserver_object.

master	: master.o  netname_check_in.o
	@-rm master
	${CC} -o master master.o netname_check_in.o -lnetmemory -lnetname -lmach

slave	: master
	@-rm slave
	ln -s master slave

