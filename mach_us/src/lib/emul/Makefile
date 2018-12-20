# 
# Mach Operating System
# Copyright (c) 1993-1988 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon 
# the rights to redistribute these changes.
#  

# File:        Makefile
#
# HISTORY: 
# $Log:	Makefile,v $
# Revision 1.42  94/06/29  15:01:01  mrt
# 	Updated for odemake. Removed ancient history.
# 	[94/02/24            mrt]
# 
# Revision 1.41  94/05/17  14:06:13  jms
# 	Add -lgcc to places where "_LDFLAGS_" were used
# 	Use "normal" loader instead of "/bin/ld"
# 	[94/04/28  18:13:00  jms]
# 
# Revision 1.39.2.1  94/02/18  11:19:47  modh
# 	Needed to add -lgcc for new 2.3.3 g++ compiler
# 
# 
# Revision 1.39  93/01/20  17:36:30  jms
# 	Remove out of date "libcs.a" gyrations.  Just get it where everyone else does.
# 	[93/01/18  15:55:15  jms]
# 
# Revision 1.38  92/07/05  23:24:17  dpj
# 	Removed libmachobj++.
# 	[92/05/10  00:21:04  dpj]
# 
# 
# Revision 1.20  90/01/02  21:30:37  dorr
# 	Completely reorganize.  now, produce a real emul library
# 	with i/o, signal, uid, process mgt, etc., split into
# 	different archive elements.  in addition, allow an emulation
# 	"type" and "sys" to be defined to customize which syscalls and setup
# 	methods are to be used by a given emulation library.  All this
# 	is to allow easy generation of different kinds of emulation
# 	libraries.
# 
######################################################################

DEPENDENCIES		=

CONFIGURATION		= MACH3_US

IDIR			= /us/lib/

OTHERS			= bsd_all.lib
ILIST			= bsd_all.lib

SUBDIRS			= emul_init_prog

EMUL_OBJECTS		= emul_basic.o emul_time.o emul_io.o emul_file.o \
			  emul_socket.o emul_proc.o emul_uid.o emul_misc.o \
			  emul_error.o inherit_ports.o emul_signal.o \
			  emul_vector.o	emul_machdep.o syscall_common.o \
			  emul_resource.o emul_exec.o \
			  emul_address_space.o loader.o loader_md.o
MAIN_OBJECTS		= bsd_syscall_table.o emul_all.o
MEM_OBJECTS		= emul_mem_base.o emul_memory.o

OBJECTS			= ${EMUL_OBJECTS} ${MAIN_OBJECTS} ${MEM_OBJECTS}

VPATH			= ${target_cpu}

INCFLAGS		= -Iemul_init_prog -I${UNIX_INCLUDE}
CFLAGS			= ${DEBUGFLAGS} -DFAKE_EVENTS=1 \
			  -DSTACK_SWITCH=1 -DSIGCHLD_WAIT=1 -DTaskMaster=1 

CPlusPlusFLAGS		= ${CFLAGS}

LIBS			=  -lux -lproxies -lus++ \
			  ${LIBTHREADS} ${LIBCPlusPlus} ${LIBMACH3} -lcs ${LIBCPlusPlus}

.include <${RULES_MK}>


bsd_all.lib: ${OBJECTS} emul_addr
	@ pwd
	@ trap 'rm -f version.c version.d version.o; exit 1' 1 2 3 15; \
	    echo "char version[] = \"Emulation library 'bsd_all'/"${CONFIGURATION}" of `date`; branch $${BCSBRANCH-TRUNK} on `hostname` by $$USER\";" > version.c; \
	    $(_CC_) $(_CCFLAGS_)  -c version.c ; rm -f version.d
	${_LD_} -Xlinker -X -z -T ` ./emul_addr ` -e _emul_initialize \
		$(_LDFLAGS_) $(_MACH3_bLIBDIRS_) \
		-o $@ ${OBJECTS} version.o ${_LIBS_}

bsd_syscalls.dat: bsd_syscalls.cpp
	$(_CPP_) ${_CCP_FLAGS_} -I${UNIX_INCLUDE} ${bsd_syscalls.cpp:P} | \
	sed -e '/^#/d' -e '/^[	 ]*$$/d' | \
	sort -n > bsd_syscalls.dat

# CC with target includes, load to run on host
gen_syscalls: gen_syscalls.o
	$(_host_CC_) $(_LDFLAGS_) -o gen_syscalls gen_syscalls.o

bsd_syscall_table.c: bsd_syscalls.dat bsd_emul_all gen_syscalls
	./gen_syscalls bsd_syscalls.dat ${bsd_emul_all:P} > ${.TARGET}

# CC with target includes, load to run on host
emul_addr: emul_addr.o
	$(_host_CC_) $(_LDFLAGS_) -o emul_addr emul_addr.o

emul_all.o:	emul_config.h

emul_config.h:
	@echo "configuring for type all"
	@echo "#define emul_type_all 1" > emul_config.h
	@echo '#define EMUL_NAME "bsd_all"' >> emul_config.h



SHORTNAMES			= emul_mem_base.cc 
emul_mem_base.cc_LONGNAME	= emul_memory_base.cc


${SHORTNAMES}: $${${.TARGET}_LONGNAME}
	${RM} -f ${.TARGET}
	${CP} ${${${.TARGET}_LONGNAME}:P} ${.TARGET}

# Hacks to compile emul_vector.s
#.s.o:
#	@echo ""
#	${_ASCPP_} -DASSEMBLER ${_CPPFLAGS_} ${.IMPSRC} > ${.TARGET:S/.o$/.as/}; \
#	${_AS_} ${_ASFLAGS_} -o ${.TARGET} ${.TARGET:S/.o$/.as/}
#	${RM} -f ${.TARGET:S/.o$/.as/}
