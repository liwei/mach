#
# Distributed as part of the Mach Operating System
#
# 
# Mach Operating System
# Copyright (c) 1993,1992 Carnegie Mellon University
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
# 
# HISTORY
# $Log:	osf.mach3.mk,v $
# Revision 2.9  93/08/02  13:40:10  mrt
# 	Defined new profiling library, LIBPROF1 (libprof1.a)
# 	[93/07/20  15:24:15  bershad]
# 
# Revision 2.8  93/06/16  17:05:02  mrt
# 	Removed the check for NOT_USING_GCC2 from .o.s rule.
# 	The .s => .S could be used for other platforms as well
# 	if the include "" in .s files were changed to <>.
# 	[93/06/10            mrt]
# 
# Revision 2.7  93/05/31  12:27:32  mrt
# 		Corrected the .s.o rule for old gcc (.s -> .S -> .o)
# 	Removed commented out LDFLAFS.
# 	Added -DBYTE_MSF for Sun3 so we don't have to depend on
# 	compiler defining this.
# 	[93/05/28            mrt]
# 
# Revision 2.6  93/05/14  23:14:15  mrt
# 	Added .S.s rule for old gcc. Added .s_p.o rule for profiled
# 	assembly. Removed .cs.o rule.
# 	[93/05/14            mrt]
# 
# Revision 2.5  93/05/11  18:04:27  mrt
# 	Added definition for MONCRT0.
# 	Removed definition of PC535_ASCPP -from Ian Dall
# 	Dropped -g from the default CC_OPT_LEVEL
# 	Made the default CC_OPT_LEVEL conditional on NOT_USING_GCC2.
# 	Added CC_OPT_EXTRA to CC_OPT_LEVEL for additions.
# 	Dumped the -e __start flag from LDFLAGS.
# 	Added definitions of the profiling libraries.
# 	[93/04/01            mrt]
# 
# Revision 2.4  93/03/20  00:36:38  mrt
# 	Added -P to ASCPP definitions. With gcc this makes output that
# 	the assembler can read. (no #line directives ).
# 	Added CPATH and _MDFLAGS_ to print_makevars
# 	[93/02/21            mrt]
# 
# 	Added definitions of libraries and library dependency generating
# 	rules. Moved mig rules to osf.mig.mk. Defined the compiler 
# 	programs, from Rich's Makefile-common. Added definitions for
# 	WH and XSTRIP
# 	[92/08/14            mrt]
# 
# Revision 2.3  92/07/08  18:05:43  mrt
# 	Added conditional definition of DEF_MIGFLAGS to -MD
# 	[92/07/03            mrt]
# 
# Revision 2.2  92/05/20  20:15:59  mrt
# 	First checkin
# 	[92/05/20  14:48:16  mrt]
# 

#
#  Project rules for building Mach 3.0 sources
#

# If you want to export symlinks define the following in the Makefile
#EXPORT_USING_TAR	=

# First define a few programs that osf.std.mk does not

WH	?= wh
XSTRIP	?= `wh -q xstrip`
SIZE	?= size

#
#  C compiler variations
#	osf.std.mk defines the compiler programs, _CC_, _CPP_, _AS_,
#	_LD_,_AR_,_RANLIB_ to  be funtions of _CCTYPE_ which can be
#	one of HOST, ANSI, TRADITIONAL, or WRITEABLE_STRINGS. _CCTYPE_
#	defaults to ANSI but can be overidden by individual Makefiles
#	for all files or on a per-file basis. All these programs default
#	to the obvious values: cc, cpp, etc., If desired, other definitions
#	should be specified in this file.
#
#  Define the following values here to be used in osf.std.mk
#	ANSI_CC		- normal C compiler for the target machine
#	DEF_CFLAGS 	- default flags for host and target compiler, 
#			  added to all other flags
#	_CCDEFS_	- CC defines, applies only to target compiler
#	HOST_CC		- C compiler for the host machine
#	HOST_CFLAGS 	- flags for HOST_CC
#	ANSI_CPP	- C preprocessor for the target machine
#	HOST_CPP_FLAGS	- Flags for target machine CPP for code
#		  	   to be compiled for and run on host machine
#	ANSI_AS		- Assembler for the target machine
#	HOST_AS 	- Assembler for the host machine
#	ANSI_ASCPP	- Preprocessor for assembly language
#	HOST_ASCPP 	- Preprocessor for host assembly language
#	ASFLAGS		- flags for the assembler
#	HOST_ASFLAGS 	- flags for the host assembler
#	ANSI_LD		- Loader for the target machine
#	HOST_LD 	- Loader for the host machine
#	LDFLAGS		- flags for the loader
#	LDLIBS		- libraries needed when using the loader
#	ANSI_AR		- Library archiver for the target machine
#	HOST_AR		- Library archiver for the host machine
#
#  This file defines defaults for gcc 2.3.3. for most machine types. The
#  defaults for the i860 and alpha are sometime different. The ${TARGET_MACHINE}_foo
#  defaults can be set in the run-time envrionment via the ${host_context}/setvar.csh
#  or ${host_X_target_context}/script.

SUN3_CC ?= gcc -m68020 
I860_CC ?= cc860
SUN4_CC ?= gcc -fno-builtin 
${TARGET_MACHINE}_CC ?= gcc
${HOST_MACHINE}_CC ?= gcc

ANSI_CC = ${${TARGET_MACHINE}_CC}
TRADITIONAL_CC ?=${ANSI_CC} -traditional
WRITABLE_STRINGS_CC?=${ANSI_CC} -fwritable-strings
# C compiler for compiling for the host machine
HOST_CC = ${${HOST_MACHINE}_HOST_CC:U${${HOST_MACHINE}_CC}}


# compile everything with optimization
# CC_OPT_LEVEL and/or ${TARGET_MACHINE}_OPT_LEVEL can 
# be set in the Makefile or the envionment, otherwise these are the defaults

.ifdef NOT_USING_GCC2
CC_OPT_LEVEL ?= ${${TARGET_MACHINE}_OPT_LEVEL:U-O}
.else
CC_OPT_LEVEL ?= ${${TARGET_MACHINE}_OPT_LEVEL:U-O2}
LD_OPT_LEVEL ?=
.endif
CC_OPT_LEVEL += ${CC_OPT_EXTRA}

SUN4_CFLAGS ?= -DBYTE_MSF -Dsun
SUN3_CFLAGS ?= -DBYTE_MSF -Dsun
I860_CFLAGS ?= -DiPSC860
LUNA88K_CFLAGS ?= -DBYTE_MSF -Dm88k -Wcomment
PMAX_CFLAGS ?= -Dmips
DEF_CCFLAGS ?= -MD -DMACH -DCMU -DCMUCS
_CCDEFS_=  -D${target_machine} ${${TARGET_MACHINE}_CFLAGS}
HOST_CFLAGS ?= -D${host_machine} ${${HOST_MACHINE}_HOST_CFLAGS:U${${HOST_MACHINE}_CFLAGS}}


# C preprocessor for C files - defined only in this file
I860_CPP ?= /lib/cpp
${TARGET_MACHINE}_CPP ?= ${ANSI_CC} -x c -E
${HOST_MACHINE}_CPP ?= ${HOST_CC} -x c -E

ansi_CPP = ${${TARGET_MACHINE}_CPP}
HOST_CPP = ${${HOST_MACHINE}_HOST_CPP:U${${HOST_MACHINE}_CPP}}
host_CCP = ${HOST_CPP}
traditional_CCP?=${ansi_CCP} -traditional
writeable_strings_CCP?=${ansi_CCP}
_CPP_=${${_CCTYPE_}_CPP}

target_CPP_FLAGS = ${${TARGET_MACHINE}_CPP_FLAGS}
host_CPP_FLAGS = ${${HOST_MACHINE}_HOST_CPP_FLAGS:U${${HOST_MACHINE}_CPP_FLAGS}}
_CPP_FLAGS_ = ${${_CCTYPE_}_CPP_FLAGS}

# C preprocessor for assembly files - defined only in this file
I860_ASCPP ?= /lib/cpp
VAX_ASCPP ?= vax_ascover ${_CPP_} -P
${TARGET_MACHINE}_ASCPP ?= ${ANSI_CC} -x c -E -P
${HOST_MACHINE}_ASCPP ?= ${HOST_CC} -x c -E -P

ansi_ASCPP = ${${TARGET_MACHINE}_ASCPP}
host_ASCPP = ${${HOST_MACHINE}_HOST_ASCPP:U${${HOST_MACHINE}_ASCPP}}
_ASCPP_= ${${_CCTYPE_}_ASCPP}

# Assembler with no preprocessor
I860_AS ?= as860
SUN3_AS ?= as -m68020
${TARGET_MACHINE}_AS ?= as
${HOST_MACHINE}_AS ?= as
ansi_AS = ${${TARGET_MACHINE}_AS}
host_AS = ${${HOST_MACHINE}_HOST_AS:U${${HOST_MACHINE}_AS}}
_AS_= ${${_CCTYPE_}_AS}

PMAX_ASFLAGS ?= -nocpp
SUN4_ASFLAGS ?= -DBYTE_MSF 
ansi_ASFLAGS = ${${TARGET_MACHINE}_ASFLAGS}
host_ASFLAGS = ${${HOST_MACHINE}_HOST_ASFLAGS:U${${HOST_MACHINE}_ASFLAGS}}
_ASFLAGS_ = ${${_CCTYPE_}_ASFLAGS}

${TARGET_MACHINE}_LD ?= ${ANSI_CC} -nostdlib
${HOST_MACHINE}_LD ?= ${HOST_CC} -nostdlib
ANSI_LD = ${${TARGET_MACHINE}_LD}
TRADITIONAL_LD?=${ANSI_LD}
WRITABLE_STRINGS_LD?=${ANSI_LD}
HOST_LD = ${${HOST_MACHINE}_HOST_LD:U${${HOST_MACHINE}_LD}}

ALPHA_LDFLAGS ?=  ${ALPHA_LD_OPTS}
${TARGET_MACHINE}_LDFLAGS ?=
${HOST_MACHINE}_LDFLAGS ?= 
ANSI_LDFLAGS = ${${TARGET_MACHINE}_LDFLAGS}
HOST_LDFLAGS = ${${HOST_MACHINE}_HOST_LDFLAGS:U${${HOST_MACHINE}_LDFLAGS}}

# Some architectures need additional libraries defined when linking with
# ${LD} (e.g, compiler support libraries).


.ifdef NOT_USING_GCC2
LIBGCC = -lgcc
.else
LIBGCC = `${ANSI_CC} -print-libgcc-file-name`
.endif
ALPHA_LDLIBS ?= -ldivide ${LIBGCC}
LDLIBS = ${${TARGET_MACHINE}_LDLIBS:U${LIBGCC}}


I860_AR ?= ar860
${TARGET_MACHINE}_AR ?= ar
${HOST_MACHINE}_AR ?= ar
ANSI_AR = ${${TARGET_MACHINE}_AR}
TRADITIONAL_AR?=${ANSI_AR}
WRITABLE_STRINGS_AR?=${ANSI_AR}
HOST_AR = ${${HOST_MACHINE}_HOST_AR:U${${HOST_MACHINE}_AR}}

I860_RANLIB ?= ranlib860
${TARGET_MACHINE}_RANLIB ?= ranlib
${HOST_MACHINE}_RANLIB ?= ranlib
ANSI_RANLIB = ${${TARGET_MACHINE}_RANLIB}
TRADITIONAL_RANLIB?=${ANSI_RANLIB}
WRITABLE_STRINGS_RANLIB?=${ANSI_RANLIB}
HOST_RANLIB = ${${HOST_MACHINE}_HOST_RANLIB:U${${HOST_MACHINE}_RANLIB}}

_host_GENINC_= -I.
_ansi_GENINC_= -I. 
_traditional_GENINC_= -I.
_writable_strings_GENINC_= -I.
_CC_GENINC_?=${_${_CCTYPE_}_GENINC_:!${GENPATH} ${_${_CCTYPE_}_GENINC_}!}


.SUFFIXES: .o .s .ss .b 

.if ( ${host_context} == "i386_bnr" )
.s.o:
	@echo ""
	-@${RM} -f ${.TARGET:S/.o$/.S/}
	cp ${.IMPSRC} ${.TARGET:S/.o$/.S/}
	${_CC_} -c -DASSEMBLER ${_CCFLAGS_} ${.TARGET:S/.o$/.S/}

.else
.s.o:
	@echo ""
	${_ASCPP_} -DASSEMBLER ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET:S/.o$/.as/}
	${_AS_} ${_ASFLAGS_} -o ${.TARGET} ${.TARGET:S/.o$/.as/}
	${RM} ${.TARGET:S/.o$/.as/}
.endif

.if defined(LIBRARIES_P)
.SUFFIXES:	_p.o
.if ( ${host_context} == "i386_bnr" )
.s_p.o:
	@echo ""
	-@${RM} -f ${.TARGET:S/.o$/.S/}
	cp ${.IMPSRC} ${.TARGET:S/.o$/.S/}
	${_CC_} -c -DASSEMBLER ${_CCFLAGS_} -DPROF -g ${.TARGET:S/.o$/.S/} -o ${.TARGET}
.else
.s_p.o:
	@echo " "
	${_ASCPP_} -DASSEMBLER ${_CCFLAGS_} -DGPROF -g  ${.IMPSRC} > ${.TARGET:S/.o$/.as/}
	${_AS_} ${_ASFLAGS_} -o ${.TARGET} ${.TARGET:S/.o$/.as/}
	${RM} ${.TARGET:S/.o$/.as/}
.endif
.endif

.ss.o:
	${_AS_} ${_ASFLAGS_} -o ${.TARGET} ${.IMPSRC}

DECODE = uudecode

.b.o:
	${DECODE} $*.b

CRT0 		= `wh -Lq crt0.o`
MONCRT0 	= `wh -Lq moncrt0.o`
HOST_CRT0	= `wh -qp ${HOST_LPATH}  crt0.o`

LIBMACH		= -lmach
LIBMACH_SA	= -lmach_sa
LIBMACH_P	= -lmach_p
LIBMACH_SA_P	= -lmach_sa_p
LIBTHREADS	= -lthreads
LIBTHREADS_P	= -lthreads_p
LIBMACHID	= -lmachid
LIBNETNAME	= -lnetname
LIBNETMEMORY	= -lnetmemory
LIBSERVICE	= -lservice
LIBXMM		= -lxmm
LIBL		= -ll
LIBCS		= -lcs
LIBCMUCS	= -lcmucs
LIBM		= -lm
LIBTERMCAP	= -ltermcap
LIBCURSES	= -lcurses
LIBREADLINE	= -lreadline
LIBENVMGR	= -lenv
LIBPROF1	= -lprof1

LIB_DEPS	= ${LIBS:S/^-l/lib/g:S/$/.a/g}

print_makevars:
	@echo MAKETOP ${MAKETOP}
	@echo VPATH is: ${VPATH}
	@echo INCDIRS is: ${INCDIRS}
	@echo _GENINC_ is: ${_GENINC_}
	@echo _CC_INCDIRS_ is: ${_CC_INCDIRS_}
	@echo CPATH is: ${CPATH}
	@echo _CC_ is : ${_CC_}
	@echo CPP is: ${CPP}
	@echo _CPP_ is: ${_CPP_}
	@echo CC_OPT_LEVEL, _CC_OL_: ${CC_OPT_LEVEL}, ${_CC_OL_}
	@echo DEF_CCFLAGS: ${DEF_CCFLAGS}
	@echo CFLAGS: ${CFLAGS}
	@echo _CCFLAGS_: ${_CCFLAGS_}
	@echo _CCDEFS_: ${_CCDEFS_}
	@echo _MDFLAGS_: ${_MDFLAGS_}
	@echo HOST_CC ${HOST_CC}
	@echo HOST_CFLAGS: ${HOST_CFLAGS}
	@echo _CC_CFLAGS_: ${_CC_CFLAGS_}
	@echo HOST_CPP ${HOST_CPP}
	@echo host_ASCPP ${host_ASCPP}
	@echo host_AS ${host_AS}
	@echo HOST_LD ${HOST_LD}
	@echo HOST_AR ${HOST_AR}
	@echo HOST_RANLIB ${HOST_RANLIB}


