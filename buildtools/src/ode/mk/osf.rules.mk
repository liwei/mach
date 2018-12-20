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
# $Log:	osf.rules.mk,v $
# Revision 2.4  93/05/11  18:04:37  mrt
# 	Added LIBRARIES_P
# 	[93/05/09            mrt]
# 
# Revision 2.3  93/03/20  00:36:50  mrt
# 	Added include of osf.mig.mk
# 	[92/08/14            mrt]
# 
# Revision 2.2  92/05/20  20:16:12  mrt
# 	First checkin
# 

.if !defined(_OSF_RULES_MK_)
_OSF_RULES_MK_=

.include <osf.std.mk>
.if !defined(TOSTAGE)
.if defined(MIG_HDRS) || defined(MIG_USRS) || defined(MIG_SRVS)
.include <osf.mig.mk>
.endif
.if defined(PROGRAMS)
.include <osf.prog.mk>
.endif
.if defined(LIBRARIES) || defined(LIBRARIES_P) || defined(SHARED_LIBRARIES)
.include <osf.lib.mk>
.endif
.if defined(BINARIES)
.include <osf.obj.mk>
.endif
.if defined(SCRIPTS)
.include <osf.script.mk>
.endif
.if defined(MANPAGES)
.include <osf.man.mk>
.endif
.if defined(DOCUMENTS)
.include <osf.doc.mk>
.endif
# We need to pick up osf.depend.mk even on an install
.endif
.if defined(DEPENDENCIES)
.include <osf.depend.mk>
.endif


.endif
