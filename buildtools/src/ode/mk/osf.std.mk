#
# Distributed as part of the Mach Operating System
#
# Copyright (c) 1990, 1991, 1992  
# Open Software Foundation, Inc. 
#  
# Permission is hereby granted to use, copy, modify and freely distribute 
# the software in this file and its documentation for any purpose without 
# fee, provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation.  Further, provided that the name of Open 
# Software Foundation, Inc. ("OSF") not be used in advertising or 
# publicity pertaining to distribution of the software without prior 
# written permission from OSF.  OSF makes no representations about the 
# suitability of this software for any purpose.  It is provided "as is" 
# without express or implied warranty. 
#
#
# ODE 2.1.1

#
# HISTORY
# $Log:	osf.std.mk,v $
# Revision 2.6  93/06/02  17:11:26  mrt
# 	Made the spelling of _NOSTRIP_ consistent.
# 	[93/06/02            mrt]
# 
# Revision 2.5  93/05/20  20:57:26  mrt
# 	Silenced part of the export rule.
# 	[93/05/15            mrt]
# 
# Revision 2.4  93/05/11  18:04:40  mrt
# 	Changed LIBRARIES to _LIBRARIES_ which may include profiled 
# 	libraries.  If PROFILING is set, changed ILIST to release
# 	only the profiled libraries.
# 	[93/05/08            mrt]
# 
# Revision 2.3  93/03/20  00:37:09  mrt
# 	Jan-6-93  Mary Thompson (mrt) at Carnegie-Mellon University
# 	Rewrote conditional assigments as :U modifiers.
# 	Made all assigments conditional, so that defaults could
# 	be overridden in the project.mk file.
# 	Added setup rules from ODE2.1.1
# 
# Revision 2.2  92/05/20  20:16:16  mrt
# 	First checkin
# 
# 	$EndLog$
# 	[92/05/20  14:56:34  mrt]
# 

.if !defined(_OSF_STD_MK_)
_OSF_STD_MK_=

#
#  Default rule - All other rules appear after variable definitions
#
.MAIN: build_all

#
#  Debugging entry for checking environment
#
print_env:
	@printenv

#
#  Use this as a dependency for any rule which should always be triggered
#  (e.g. recursive makes).
#
ALWAYS=

#
#  Shortened for macro definitions, not to be used within a Makefile.
#
_T_M_=${TARGET_MACHINE}

#
#  Definitions for object file format - A_OUT, COFF or MACHO
#
${_T_M_}_OBJECT_FORMAT?=MACHO
OBJECT_FORMAT?=${${_T_M_}_OBJECT_FORMAT}

#
#  Definitions for archive file format - ASCARCH or COFF
#
${_T_M_}_ARCHIVE_FORMAT?=COFF
ARCHIVE_FORMAT?=${${_T_M_}_ARCHIVE_FORMAT}

#
#  Set defaults for input variables which are not already defined
#
DEF_RMFLAGS?=-f
DEF_ARFLAGS?=crl

ROOT_OWNER?=root
KMEM_GROUP?=kmem
TTY_GROUP?=tty
SETUID_MODE?=4711
SETGID_MODE?=2711

OWNER?=bin
IOWNER?=${OWNER}
GROUP?=bin
IGROUP?=${GROUP}
IMODE?=755

#
#  Program macros
#
AR?=whatar
AS?=as
AWK?=awk
CC?=whatcc
CHMOD?=chmod
CP?=cp
CTAB?=ctab
ECHO?=echo
GENCAT?=gencat
GENPATH?=genpath
LD?=whatld
LEX?=lex
LIBLOC?=libloc
LINT?=lint
LN?=ln
MAKE?=odemake
MAKEPATH?=makepath
MD?=md
MKCATDEFS?=xmkcatdefs
MV?=mv
RANLIB?=whatranlib
RELEASE?=release
RM?=rm
SED?=sed
SORT?=sort
TAGS?=ctags
TAR?=tar
TOUCH?=touch
TR?=tr
UUDECODE?=uudecode
XSTR?=xstr
YACC?=yacc

#
#  Define ${_T_M_}_VA_ARGV to be either VA_ARGV_IS_RECAST
#  to recast to char **, otherwise define VA_ARGV_IS_ROUTINE
#  If not defined here, we become VA_ARGV_UNKNOWN which should invoke
#  a #error directive where needed.
#
HP_M68K_VA_ARGV=VA_ARGV_IS_RECAST
HP300_VA_ARGV=VA_ARGV_IS_RECAST
IBMRT_VA_ARGV=VA_ARGV_IS_RECAST
MACII_VA_ARGV=VA_ARGV_IS_RECAST
MMAX_VA_ARGV=VA_ARGV_IS_RECAST
PMAX_VA_ARGV=VA_ARGV_IS_RECAST
SUN3_VA_ARGV=VA_ARGV_IS_RECAST
SUN4_VA_ARGV=VA_ARGV_IS_RECAST
SUN_VA_ARGV=VA_ARGV_IS_RECAST
VAX_VA_ARGV=VA_ARGV_IS_RECAST
AT386_VA_ARGV=VA_ARGV_IS_RECAST
I386_VA_ARGV=VA_ARGV_IS_RECAST
RIOS_VA_ARGV=VA_ARGV_IS_RECAST
${_T_M_}_VA_ARGV?=VA_ARGV_UNKNOWN

#
#  Defined whether characters are sign or zero extended
#
HP_M68K_CHAR_EXTEND=CHARS_EXTEND_ZERO
HP300_CHAR_EXTEND=CHARS_EXTEND_SIGN
IBMRT_CHAR_EXTEND=CHARS_EXTEND_ZERO
MACII_CHAR_EXTEND=CHARS_EXTEND_SIGN
MMAX_CHAR_EXTEND=CHARS_EXTEND_ZERO
PMAX_CHAR_EXTEND=CHARS_EXTEND_ZERO
SUN3_CHAR_EXTEND=CHARS_EXTEND_SIGN
SUN4_CHAR_EXTEND=CHARS_EXTEND_SIGN
SUN_CHAR_EXTEND=CHARS_EXTEND_SIGN
VAX_CHAR_EXTEND=CHARS_EXTEND_SIGN
AT386_CHAR_EXTEND=CHARS_EXTEND_SIGN
I386_CHAR_EXTEND=CHARS_EXTEND_SIGN
RIOS_CHAR_EXTEND=CHARS_EXTEND_ZERO
${_T_M_}_CHAR_EXTEND?=CHARS_EXTEND_UNKNOWN

#
#  Include project specific information
#
.include <osf.${project_name}.mk>

#
#  C compiler variations
#
CCTYPE?=ansi
_CCTYPE_=${${.TARGET}_CCTYPE:U${CCTYPE}}

_host_CC_=${HOST_CC:Ucc}
_ansi_CC_=$(ANSI_CC:Ucc)
_traditional_CC_=${TRADITIONAL_CC:Ucc}
_writable_strings_CC_=${WRITABLE_STRINGS_CC:Ucc}
_CC_=${_${_CCTYPE_}_CC_}

_host_LD_=${HOST_LD:Uld}
_ansi_LD_=${ANSI_LD:Uld}
_traditional_LD_=${TRADITIONAL_LD:Uld}
_writable_strings_LD_=${WRITABLE_STRINGS_LD:Uld}
_LD_=${_${_CCTYPE_}_LD_}

_ansi_LPATH_=${LPATH:U""}
_host_LPATH_=${HOST_LPATH:U${_ansi_LPATH_}}

_host_AR_=${HOST_AR:Uar}
_ansi_AR_=${ANSI_AR:Uar}
_traditional_AR_=${TRADITIONAL_AR:Uar}
_writable_strings_AR_=${WRITABLE_STRINGS_AR:Uar}
_AR_=${_${_CCTYPE_}_AR_}

_host_RANLIB_=${HOST_RANLIB:Uranlib}
_ansi_RANLIB_=${ANSI_RANLIB:Uranlib}
_traditional_RANLIB_=${TRADITIONAL_RANLIBL:Uranlib}
_writable_strings_RANLIB_=${WRITABLE_STRINGS_RANLIB:Uranlib}
_RANLIB_=${_${_CCTYPE_}_RANLIB_}

_host_CFLAGS_=${DEF_CCFLAGS} ${HOST_CFLAGS}
_ansi_CFLAGS_=${DEF_CCFLAGS} ${_O_F_CFLAGS_} ${_CCDEFS_} ${_SHCCDEFS_}
_traditional_CFLAGS_=${_ansi_CFLAGS_}
_writable_strings_CFLAGS_=${_ansi_CFLAGS_}
_CC_CFLAGS_=${_${_CCTYPE_}_CFLAGS_}

_host_INCDIRS_= ${HOST_INCDIRS}
_ansi_INCDIRS_=${INCDIRS}
_traditional_INCDIRS_=${INCDIRS}
_writable_strings_INCDIRS_=${INCDIRS}
_CC_INCDIRS_=${_${_CCTYPE_}_INCDIRS_}

_host_LDFLAGS_= ${HOST_LDFLAGS}
_ansi_LDFLAGS_= ${ANSI_LDFLAGS}
_traditional_LDFLAGS_=
_writable_strings_LDFLAGS_=
_CC_LDFLAGS_=${_${_CCTYPE_}_LDFLAGS_}

_host_LIBDIRS_= ${HOST_LIBDIRS}
_ansi_LIBDIRS_=${LIBDIRS}
_traditional_LIBDIRS_=${_ansi_LIBDIRS_}
_writable_strings_LIBDIRS_=${_ansi_LIBDIRS_}
_CC_LIBDIRS_=${_${_CCTYPE_}_LIBDIRS_}

#
#  Compilation optimization level.  This should be set to whatever
#  combination of -O and -g flags you desire.  Defaults to -O.
#
#  Allow these flags to be overridden per target
#
.if defined(OPT_LEVEL)
_CC_OL_=${${.TARGET}_OPT_LEVEL:U${OPT_LEVEL}}
_LD_OL_=${${.TARGET}_OPT_LEVEL:U${OPT_LEVEL}}
.else
CC_OPT_LEVEL?=-O
_CC_OL_=${${.TARGET}_OPT_LEVEL:U${${.TARGET}_CC_OPT_LEVEL:U${CC_OPT_LEVEL}}}
LD_OPT_LEVEL?=
_LD_OL_=${${.TARGET}_OPT_LEVEL:U${${.TARGET}_LD_OPT_LEVEL:U${LD_OPT_LEVEL}}}
.endif

#
#  Program flags for makefile, environment and command line args
#
_INCFLAGS_=\
	${${.TARGET}_INCARGS:U${INCARGS}}\
	${${.TARGET}_INCFLAGS:U${INCFLAGS}}\
	${${.TARGET}_INCENV:U${INCENV}}
_GENINC_=\
	${_CC_GENINC_} ${_INCFLAGS_:!${GENPATH} ${_INCFLAGS_}!}
_LIBFLAGS_=\
	${${.TARGET}_LIBARGS:U${LIBARGS}}\
	${${.TARGET}_LIBFLAGS:U${LIBFLAGS}}\
	${${.TARGET}_LIBENV:U${LIBENV}}
_GENLIB_=\
	${_CC_GENLIB_} ${_LIBFLAGS_:!${GENPATH} ${_LIBFLAGS_}!}
_LIBS_=	${${.TARGET}_LIBSENV:U${LIBSENV}}\
	${${.TARGET}_LIBS:U${LIBS}}\
	${${.TARGET}_LIBSARGS:U${LIBSARGS}} ${TARGET_LIBS}
_CCFLAGS_=\
	${_CC_CFLAGS_}\
	${_CC_OL_}\
	${${.TARGET}_CENV:U${CENV}}\
	${${.TARGET}_CFLAGS:U${CFLAGS}} ${TARGET_FLAGS}\
	${${.TARGET}_CARGS:U${CARGS}}\
	${_CC_NOSTDINC_} ${_GENINC_} ${_CC_INCDIRS_} ${_CC_PICLIB_}
_COMMON_LDFLAGS_=\
	${_CC_LDFLAGS_}\
	${_LD_OL_}\
	${${.TARGET}_LDENV:U${LDENV}}\
	${${.TARGET}_LDFLAGS:U${LDFLAGS}}\
	${${.TARGET}_LDARGS:U${LDARGS}}\
	${_CC_NOSTDLIB_} ${_GENLIB_} ${_CC_LIBDIRS_}
_LDFLAGS_=\
	${_CC_GLUE_} ${_COMMON_LDFLAGS_}
_SHLDFLAGS_=\
	-R ${EXPORTS} ${_COMMON_LDFLAGS_}

_LFLAGS_=\
	${${.TARGET}_LENV:U${LENV}}\
	${${.TARGET}_LFLAGS:U${LFLAGS}}\
	${${.TARGET}_LARGS:U${LARGS}}
_YFLAGS_=\
	${${.TARGET}_YENV:U${YENV}}\
	${${.TARGET}_YFLAGS:U${YFLAGS}}\
	${${.TARGET}_YARGS:U${YARGS}}
_LINTFLAGS_=\
	${${.TARGET}_LINTENV:U${LINTENV}}\
	${${.TARGET}_LINTFLAGS:U${LINTFLAGS}}\
	${${.TARGET}_LINTARGS:U${LINTARGS}}\
	${_GENINC_} ${_CC_INCDIRS_}
_TAGSFLAGS_=\
	${${.TARGET}_TAGSENV:U${TAGSENV}}\
	${${.TARGET}_TAGSFLAGS:U${TAGSFLAGS}}\
	${${.TARGET}_TAGSARGS:U${TAGSARGS}}
_RMFLAGS_=\
	${${.TARGET}_DEF_RMFLAGS:U${DEF_RMFLAGS}}

#
#  Define these with default options added
#
_RELEASE_=${RELEASE_PREFIX}${RELEASE} ${RELEASE_OPTIONS}

#
#  Change a few definitions if PROFILING is set, so
#  that only profiling libraries are built, exported and released.
#

.if defined(PROFILING)
# build, export and release profiled libraries only
LIBRARIES_P	= ${LIBRARIES:S/.a$/_p.a/g}
_LIBRARIES_	= ${LIBRARIES_P}
_ILIST_		:= ${_LIBRARIES_} ${ILIST:N*.a}
.else
_ILIST_		:= ${ILIST}
_LIBRARIES_	= ${LIBRARIES} ${LIBRARIES_P}
.endif

#
#  Define binary targets
#
.if defined(PROGRAMS)
BINARIES+=${PROGRAMS}
.endif
.if defined(LIBRARIES)
BINARIES+=${_LIBRARIES_}
.if defined(SHARED_LIBRARIES)
BINARIES+=${SHARED_LIBRARIES}
.endif
.endif
.if defined(OBJECTS)
BINARIES+=${OBJECTS}
.endif

#
#  Definitions for clean/rmtarget/clobber
#
_CLEAN_TARGET=${.TARGET:S/^clean_//}
.if !defined(CLEANFILES)
_CLEAN_DEFAULT_=\
	${_CLEAN_TARGET}.X\
	${OFILES:U${${_CLEAN_TARGET}_OFILES:U${_CLEAN_TARGET}.o}}\
	${${_CLEAN_TARGET}_GARBAGE:U${GARBAGE}}
_CLEANFILES_=\
	${CLEANFILES:U${${_CLEAN_TARGET}_CLEANFILES:U${_CLEAN_DEFAULT_}}}
.endif

_RMTARGET_TARGET=${.TARGET:S/^rmtarget_//}

_CLOBBER_TARGET=${.TARGET:S/^clobber_//}
_CLOBBER_DEFAULT_=\
	${_CLOBBER_TARGET}.X\
	${OFILES:U${${_CLOBBER_TARGET}_OFILES:U${_CLOBBER_TARGET}.o}}\
	${${_CLOBBER_TARGET}_GARBAGE:U${GARBAGE}}
_CLOBBERFILES_=${_CLOBBER_TARGET} \
	${CLEANFILES:U${${_CLOBBER_TARGET}_CLEANFILES:U${_CLOBBER_DEFAULT_}}}

#
#  Definitions for lint
#
_LINT_TARGET=${.TARGET:S/^lint_//}
.if !defined(LINTFILES)
_LINT_OFILES_=${OFILES:U${${_LINT_TARGET}_OFILES:U${_LINT_TARGET}.o}}
LINTFILES=${${_LINT_TARGET}_LINTFILES:U${_LINT_OFILES_:.o=.c}}
.endif

#
#  Definitions for tags
#
_TAGS_TARGET=${.TARGET:S/^tags_//}
.if !defined(TAGSFILES)
_TAGS_OFILES_=${OFILES:U${${_TAGS_TARGET}_OFILES:U${_TAGS_TARGET}.o}}
TAGSFILES?=${${_TAGS_TARGET}_TAGSFILES:U${_TAGS_OFILES_:.o=.c}}
.endif

#
#  Definitions for setup
#
TOOLS	= ${EXPORTBASE}/bin
_SETUP_TARGET=${.TARGET:S/^setup_//}
_SETUPFILES_=${TOOLS}/${_SETUP_TARGET}

#
#  Definitions for export
#
_EXPORT_TARGET=${.TARGET:S/^export_//}
_EXPDIR_=${${_EXPORT_TARGET}_EXPDIR:U${EXPDIR:U${${_EXPORT_TARGET}_IDIR:U${IDIR:U/_MISSING_EXPDIR_/}}}}
_EXPLINKS_=${${_EXPORT_TARGET}_EXPLINKS:U${EXPLINKS}}
_DO_EXPLINKS_=\
	(cd ${EXPORTBASE}${_EXPDIR_:H};\
	 ${RM} ${_RMFLAGS_} ${_EXPLINKS_}\
	 ${_EXPLINKS_:@.LINK.@; ${LN} ${_EXPORT_TARGET} ${.LINK.}@})
.if defined(EXPLINKS)
_MAKE_EXPLINKS_=${_DO_EXPLINKS_}
.else
_MAKE_EXPLINKS_=${${_EXPORT_TARGET}_EXPLINKS:U@true:D${_DO_EXPLINKS_}}
.endif
_EXPFILES_=${EXPORTBASE}${_EXPDIR_}${_EXPORT_TARGET}

#
#  Definitions for install/release
#
_INSTALL_TARGET=${.TARGET:S/^install_//}
.if defined(TOSTAGE)

.if defined(NOSTRIP)
_NOSTRIP_=-nostrip
.else
_NOSTRIP_=${${_INSTALL_TARGET}_NOSTRIP:D-nostrip}
.endif

_IDIR_=${${_INSTALL_TARGET}_IDIR:U${IDIR:U/_MISSING_IDIR_/}}

.endif

#
#  Default single suffix compilation rules
#
.if !defined(TOSTAGE)
.SUFFIXES: .o .s .pp .c .h .y .l .sh .csh .txt .uu

.uu:
	${UUDECODE} ${.IMPSRC}
.endif
#
#  Special rules
#

#
#  Use this as a dependency for any rule which should always be triggered
#  (e.g. recursive makes).
#
#${ALWAYS}:

#
#  Include pass definitions for standard targets
#
.include <osf.${project_name}.passes.mk>

#
#  Compilation rules
#
all: build_all;@

build_all: $${_all_targets_};@

comp_all: $${_all_targets_};@

#
#  Clean up rules
#
clean_all: $${_all_targets_}
	-${RM} ${_RMFLAGS_} core

.if !empty(_CLEAN_TARGETS_:Mclean_*)
${_CLEAN_TARGETS_:Mclean_*}:
	-${RM} ${_RMFLAGS_} ${_CLEANFILES_}
.endif

rmtarget_all: $${_all_targets_}
	-${RM} ${_RMFLAGS_} core

.if !empty(_RMTARGET_TARGETS_:Mrmtarget_*)
${_RMTARGET_TARGETS_:Mrmtarget_*}:
	-${RM} ${_RMFLAGS_} ${_RMTARGET_TARGET}
.endif

clobber_all: $${_all_targets_}
	-${RM} ${_RMFLAGS_} core depend.mk

.if !empty(_CLOBBER_TARGETS_:Mclobber_*)
${_CLOBBER_TARGETS_:Mclobber_*}:
	-${RM} ${_RMFLAGS_} ${_CLOBBERFILES_}
.endif

#
#  Lint rules
#
lint_all: $${_all_targets_};@

.if !empty(_LINT_TARGETS_:Mlint_*)
${_LINT_TARGETS_:Mlint_*}: $${LINTFILES}
	${LINT} ${_LINTFLAGS_} ${.ALLSRC}
.endif

#
#  Tags rules
#
tags_all: $${_all_targets_};@

.if !empty(_TAGS_TARGETS_:Mtags_*)
${_TAGS_TARGETS_:Mtags_*}: $${TAGSFILES}
	${TAGS} ${_TAGSFLAGS_} ${.ALLSRC}
.endif

#
# Setup rules
#

setup_all: $${_all_targets_};@
.if !empty(_SETUP_TARGETS_:Msetup_*)
${_SETUP_TARGETS_:Msetup_*}: $${_SETUPFILES_};@
${_SETUP_TARGETS_:Msetup_*:S/^setup_//g:@_SETUP_TARGET@${_SETUPFILES_}@}:\
               ${.TARGET:T}
	-@${RM} ${_RMFLAGS_} ${.TARGET}
	@${MAKEPATH} ${.TARGET}
.if     defined(EXPORT_USING_TAR)
	(cd ${.ALLSRC:H}; ${TAR} cf - ${.ALLSRC:T}) | \
	(cd ${.TARGET:H}; ${TAR} xf -)
.else
	${CP} -p -R ${.ALLSRC} ${.TARGET}
.endif
.endif
#
#  Export rules
#
.if !defined(EXPORTBASE)

export_all: ${ALWAYS}
	@echo "You must define EXPORTBASE to do an ${.TARGET}"

.if !empty(_EXPORT_TARGETS_:Mexport_*)
${_EXPORT_TARGETS_:Mexport_*}: ${ALWAYS}
	@echo "You must define EXPORTBASE to do an ${.TARGET}"
.endif

.else

export_all: $${_all_targets_}

.if !empty(_EXPORT_TARGETS_:Mexport_*)
${_EXPORT_TARGETS_:Mexport_*}: $${_EXPFILES_};@

${_EXPORT_TARGETS_:Mexport_*:S/^export_//g:@_EXPORT_TARGET@${_EXPFILES_}@}:\
		${.TARGET:T}
	-@${RM} ${_RMFLAGS_} ${.TARGET}
	@${MAKEPATH} ${.TARGET}
.if	defined(EXPORT_USING_TAR)
	(cd ${.ALLSRC:H}; ${TAR} cf - ${.ALLSRC:T}) | \
	(cd ${.TARGET:H}; ${TAR} xf -)
.else
	${CP} -p -R ${.ALLSRC} ${.TARGET}
.endif
	${.ALLSRC:T:@_EXPORT_TARGET@${_MAKE_EXPLINKS_}@}
.endif

.endif

#
#  Installation/release rules
#
.if !defined(TOSTAGE)

install_all: ${ALWAYS}
	@echo "You must define TOSTAGE to do an ${.TARGET}"

.if !empty(_INSTALL_TARGETS_:Minstall_*)
${_INSTALL_TARGETS_:Minstall_*}: ${ALWAYS}
	@echo "You must define TOSTAGE to do an ${.TARGET}"
.endif

.else

install_all: $${_all_targets_};@

.if !empty(_INSTALL_TARGETS_:Minstall_*)

.if defined(FROMSTAGE)

${_INSTALL_TARGETS_:Minstall_*}: ${ALWAYS}
	${_RELEASE_} ${_NOSTRIP_}\
		-o ${${_INSTALL_TARGET}_IOWNER:U${IOWNER}}\
		-g ${${_INSTALL_TARGET}_IGROUP:U${IGROUP}}\
		-m ${${_INSTALL_TARGET}_IMODE:U${IMODE}}\
		-tostage ${TOSTAGE}\
		-fromstage ${FROMSTAGE}\
		${_IDIR_}${_INSTALL_TARGET}\
		${${_INSTALL_TARGET}_ILINKS:U${ILINKS}}

.else

${_INSTALL_TARGETS_:Minstall_*}: ${_INSTALL_TARGET}
	${_RELEASE_} ${_NOSTRIP_}\
		-o ${${_INSTALL_TARGET}_IOWNER:U${IOWNER}}\
		-g ${${_INSTALL_TARGET}_IGROUP:U${IGROUP}}\
		-m ${${_INSTALL_TARGET}_IMODE:U${IMODE}}\
		-tostage ${TOSTAGE}\
		-fromfile ${${_INSTALL_TARGET}:P}\
		${_IDIR_}${_INSTALL_TARGET}\
		${${_INSTALL_TARGET}_ILINKS:U${ILINKS}}

.endif

.endif

.endif

.endif
