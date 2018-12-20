CC = gcc
CFLAGS = -g 

all: ipc c_server c_user vm cw

ipc: ipc.o
	${CC} -o ipc ipc.o -lmach

c_server: c_server.o mig_server.o crypt_server.o 
	${CC} -o c_server c_server.o mig_server.o crypt_server.o -lmach
c_server.o: crypt.h

c_user: c_user.o crypt_user.o crypt.h
	${CC} -o c_user c_user.o crypt_user.o -lmach
c_user.o: crypt.h

crypt_user.c crypt_server.c crypt.h: crypt.defs
	mig -user crypt_user.c \
	-server crypt_server.c \
	-header crypt.h crypt.defs

vm: vm.o
	${CC} -o vm vm.o -lmach

cw: cw.o
	${CC} -o cw cw.o -lthreads -lmach
