# 
# Mach Operating System
# Copyright (c) 1994 Carnegie Mellon University
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
# File:        init_env.template.csh
#
# 
# Purpose:  Set some environment variables for the first root process
# 
# HISTORY: 
# $Log:	init_env.template.csh
#

set home=/
setenv USER root
alias ll 'ls -l'
alias ty '/usr/ucb/more'
set term=at386
stty newcrt erase ^h kill ^k -tabs
set editmode=emacs
set history=100
set savehist=50
set prompt='# '
setpath PATH -i99 /bin
setpath -i99 /usr
setpath PATH -i99 /usr/ucb
setpath PATH -i99 /etc
