#!/bin/csh -f 
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
#  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they made and grant Carnegie Mellon the
# rights to redistribute these changes.
#

# HISTORY
# $Log:	us_setup.template.csh,v $
# Revision 2.3  94/07/13  16:30:49  mrt
# 	Add copyright.
# 	[94/07/13            mrt]
# 
# Revision 2.2  94/06/29  14:16:34  mrt
# 	Created.
# 	[94/06/27            mrt]
# 
#
# If you want to run the multi-server system along side a Unix
# single server, this script should be run once to set up links
# from the multi-server root back to the disk partitions that are
# managed by the single server. These partitions (/, /usr, /usr?) 
# can be mounted read-only by the multi-server. 

# Set up links from the multi-server root back to the single-server root
foreach i ( `ls /`) 
   echo "ln -s /slash/$i $i"
   ln -s /slash/$i $i
end

# Set up names to reference through the prefix table
# to the ufs_server handling a partition, These names
# should agree with the ones specified in prefix.config 
ln -s /slash_superroot/RFS/.LOCALROOT slash
ln -s ' TRANSPARENT /slash_superroot' slash_superroot
ln -s ' TRANSPARENT /slash_usr' slash_usr

# Enter a name in the root directory for the root nameserver
ln -s " TRANSPARENT /server" .

# Get rid of link to multi-server root
set curdir=`pwd`
rm -f $curdir:t

# Get rid of links for directories that must be local
rm -f tmp; mkdir tmp; chmod 777 tmp
rm -f dev; mkdir dev
rm -f lib; mkdir lib
rm -f etc; mkdir etc
rm -f usr
# /usr directory should be local 
# usr? directories are assumed to be mount points and
# so need prefix table names. This assumes the prefix name
# for /usri is usri. (See prefix.config)
foreach i (`ls usr*`)
  rm -f $i
  ln -s /$i $i
end
mkdir usr
cd usr
rm -f tmp
ln -s ../tmp .
 
cd ../lib
# Link to all the files in the single server /lib directory
foreach i ( `ls /lib`)
   echo "ln -s /slash/lib/$i $i"
   ln -s  /slash/lib/$i $i		
end
# add the emulation library, bsd_all.lib, to the local /lib directory
if ( -f /mach_servers/us/lib/bsd_all.lib ) then
	echo "cp -p /mach_servers/us/lib/bsd_all.lib"
	cp -p /mach_servers/us/lib/bsd_all.lib .

else
   echo ">>>>>bsd_all.lib must be installed into lib"
endif

cd ../etc
# Link to all the files in the single server /etc directory
foreach i ( `ls /etc`)
   echo "ln -s /slash/etc/$i $i"
   ln -s  /slash/etc/$i $i		
end
rm -f inetd.conf
echo "ftp	stream	tcp	nowait	root	/etc/ftpd	ftpd" > inetd.conf
echo "telnet	stream	tcp	nowait	root	/etc/telnetd	telnetd" >> inetd.conf

# Link to all the directories in the single server /usr directory
# When you want to add a multi-server user, delete the usr link if
# any and make a local directory
cd ../usr
foreach i ( `ls /usr`)
   echo "ln -s /slash_usr/$i $i "
   ln -s  /slash_usr/$i $i 
end

# Create links from the /dev directory to the server/ttys directory
# where the tty server actually puts entries for ttys. Unix programs
# will expect the names /dev/tty? /dev/pty? to exist
# The TRANSPARENT option means that the multi-server won't return
# the link name, even when asked to.
echo -n "making /dev links ..."
cd ../dev
ln -s 'DUMMY' null
ln -s ' TRANSPARENT /server/ttys/console' console
ln -s ' TRANSPARENT /server/ttys/console_htg' console_htg
ln -s ' TRANSPARENT /server/ttys/ptyQa' ptyQa
ln -s ' TRANSPARENT /server/ttys/ptyq4_htg' ptyq4_htg
ln -s ' TRANSPARENT /server/ttys/ttyPb' ttyPb
ln -s ' TRANSPARENT /server/ttys/ttyp5_htg' ttyp5_htg
ln -s ' TRANSPARENT /server/ttys/ptyP0' ptyP0
ln -s ' TRANSPARENT /server/ttys/ptyQa_htg' ptyQa_htg
ln -s ' TRANSPARENT /server/ttys/ptyq5' ptyq5
ln -s ' TRANSPARENT /server/ttys/ttyPb_htg' ttyPb_htg
ln -s ' TRANSPARENT /server/ttys/ttyp6' ttyp6
ln -s ' TRANSPARENT /server/ttys/ptyP0_htg' ptyP0_htg
ln -s ' TRANSPARENT /server/ttys/ptyQb' ptyQb
ln -s ' TRANSPARENT /server/ttys/ptyq5_htg' ptyq5_htg
ln -s ' TRANSPARENT /server/ttys/ttyPc' ttyPc
ln -s ' TRANSPARENT /server/ttys/ttyp6_htg' ttyp6_htg
ln -s ' TRANSPARENT /server/ttys/ptyP1' ptyP1
ln -s ' TRANSPARENT /server/ttys/ptyQb_htg' ptyQb_htg
ln -s ' TRANSPARENT /server/ttys/ptyq6' ptyq6
ln -s ' TRANSPARENT /server/ttys/ttyPc_htg' ttyPc_htg
ln -s ' TRANSPARENT /server/ttys/ttyp7' ttyp7
ln -s ' TRANSPARENT /server/ttys/ptyP1_htg' ptyP1_htg
ln -s ' TRANSPARENT /server/ttys/ptyQc' ptyQc
ln -s ' TRANSPARENT /server/ttys/ptyq6_htg' ptyq6_htg
ln -s ' TRANSPARENT /server/ttys/ttyPd' ttyPd
ln -s ' TRANSPARENT /server/ttys/ttyp7_htg' ttyp7_htg
ln -s ' TRANSPARENT /server/ttys/ptyP2' ptyP2
ln -s ' TRANSPARENT /server/ttys/ptyQc_htg' ptyQc_htg
ln -s ' TRANSPARENT /server/ttys/ptyq7' ptyq7
ln -s ' TRANSPARENT /server/ttys/ttyPd_htg' ttyPd_htg
ln -s ' TRANSPARENT /server/ttys/ttyp8' ttyp8
ln -s ' TRANSPARENT /server/ttys/ptyP2_htg' ptyP2_htg
ln -s ' TRANSPARENT /server/ttys/ptyQd' ptyQd
ln -s ' TRANSPARENT /server/ttys/ptyq7_htg' ptyq7_htg
ln -s ' TRANSPARENT /server/ttys/ttyPe' ttyPe
ln -s ' TRANSPARENT /server/ttys/ttyp8_htg' ttyp8_htg
ln -s ' TRANSPARENT /server/ttys/ptyP3' ptyP3
ln -s ' TRANSPARENT /server/ttys/ptyQd_htg' ptyQd_htg
ln -s ' TRANSPARENT /server/ttys/ptyq8' ptyq8
ln -s ' TRANSPARENT /server/ttys/ttyPe_htg' ttyPe_htg
ln -s ' TRANSPARENT /server/ttys/ttyp9' ttyp9
ln -s ' TRANSPARENT /server/ttys/ptyP3_htg' ptyP3_htg
ln -s ' TRANSPARENT /server/ttys/ptyQe' ptyQe
ln -s ' TRANSPARENT /server/ttys/ptyq8_htg' ptyq8_htg
ln -s ' TRANSPARENT /server/ttys/ttyPf' ttyPf
ln -s ' TRANSPARENT /server/ttys/ttyp9_htg' ttyp9_htg
ln -s ' TRANSPARENT /server/ttys/ptyP4' ptyP4
ln -s ' TRANSPARENT /server/ttys/ptyQe_htg' ptyQe_htg
ln -s ' TRANSPARENT /server/ttys/ptyq9' ptyq9
ln -s ' TRANSPARENT /server/ttys/ttyPf_htg' ttyPf_htg
ln -s ' TRANSPARENT /server/ttys/ttypa' ttypa
ln -s ' TRANSPARENT /server/ttys/ptyP4_htg' ptyP4_htg
ln -s ' TRANSPARENT /server/ttys/ptyQf' ptyQf
ln -s ' TRANSPARENT /server/ttys/ptyq9_htg' ptyq9_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ0' ttyQ0
ln -s ' TRANSPARENT /server/ttys/ttypa_htg' ttypa_htg
ln -s ' TRANSPARENT /server/ttys/ptyP5' ptyP5
ln -s ' TRANSPARENT /server/ttys/ptyQf_htg' ptyQf_htg
ln -s ' TRANSPARENT /server/ttys/ptyqa' ptyqa
ln -s ' TRANSPARENT /server/ttys/ttyQ0_htg' ttyQ0_htg
ln -s ' TRANSPARENT /server/ttys/ttypb' ttypb
ln -s ' TRANSPARENT /server/ttys/ptyP5_htg' ptyP5_htg
ln -s ' TRANSPARENT /server/ttys/ptyp0' ptyp0
ln -s ' TRANSPARENT /server/ttys/ptyqa_htg' ptyqa_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ1' ttyQ1
ln -s ' TRANSPARENT /server/ttys/ttypb_htg' ttypb_htg
ln -s ' TRANSPARENT /server/ttys/ptyP6' ptyP6
ln -s ' TRANSPARENT /server/ttys/ptyp0_htg' ptyp0_htg
ln -s ' TRANSPARENT /server/ttys/ptyqb' ptyqb
ln -s ' TRANSPARENT /server/ttys/ttyQ1_htg' ttyQ1_htg
ln -s ' TRANSPARENT /server/ttys/ttypc' ttypc
ln -s ' TRANSPARENT /server/ttys/ptyP6_htg' ptyP6_htg
ln -s ' TRANSPARENT /server/ttys/ptyp1' ptyp1
ln -s ' TRANSPARENT /server/ttys/ptyqb_htg' ptyqb_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ2' ttyQ2
ln -s ' TRANSPARENT /server/ttys/ttypc_htg' ttypc_htg
ln -s ' TRANSPARENT /server/ttys/ptyP7' ptyP7
ln -s ' TRANSPARENT /server/ttys/ptyp1_htg' ptyp1_htg
ln -s ' TRANSPARENT /server/ttys/ptyqc' ptyqc
ln -s ' TRANSPARENT /server/ttys/ttyQ2_htg' ttyQ2_htg
ln -s ' TRANSPARENT /server/ttys/ttypd' ttypd
ln -s ' TRANSPARENT /server/ttys/ptyP7_htg' ptyP7_htg
ln -s ' TRANSPARENT /server/ttys/ptyp2' ptyp2
ln -s ' TRANSPARENT /server/ttys/ptyqc_htg' ptyqc_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ3' ttyQ3
ln -s ' TRANSPARENT /server/ttys/ttypd_htg' ttypd_htg
ln -s ' TRANSPARENT /server/ttys/ptyP8' ptyP8
ln -s ' TRANSPARENT /server/ttys/ptyp2_htg' ptyp2_htg
ln -s ' TRANSPARENT /server/ttys/ptyqd' ptyqd
ln -s ' TRANSPARENT /server/ttys/ttyQ3_htg' ttyQ3_htg
ln -s ' TRANSPARENT /server/ttys/ttype' ttype
ln -s ' TRANSPARENT /server/ttys/ptyP8_htg' ptyP8_htg
ln -s ' TRANSPARENT /server/ttys/ptyp3' ptyp3
ln -s ' TRANSPARENT /server/ttys/ptyqd_htg' ptyqd_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ4' ttyQ4
ln -s ' TRANSPARENT /server/ttys/ttype_htg' ttype_htg
ln -s ' TRANSPARENT /server/ttys/ptyP9' ptyP9
ln -s ' TRANSPARENT /server/ttys/ptyp3_htg' ptyp3_htg
ln -s ' TRANSPARENT /server/ttys/ptyqe' ptyqe
ln -s ' TRANSPARENT /server/ttys/ttyQ4_htg' ttyQ4_htg
ln -s ' TRANSPARENT /server/ttys/ttypf' ttypf
ln -s ' TRANSPARENT /server/ttys/ptyP9_htg' ptyP9_htg
ln -s ' TRANSPARENT /server/ttys/ptyp4' ptyp4
ln -s ' TRANSPARENT /server/ttys/ptyqe_htg' ptyqe_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ5' ttyQ5
ln -s ' TRANSPARENT /server/ttys/ttypf_htg' ttypf_htg
ln -s ' TRANSPARENT /server/ttys/ptyPa' ptyPa
ln -s ' TRANSPARENT /server/ttys/ptyp4_htg' ptyp4_htg
ln -s ' TRANSPARENT /server/ttys/ptyqf' ptyqf
ln -s ' TRANSPARENT /server/ttys/ttyQ5_htg' ttyQ5_htg
ln -s ' TRANSPARENT /server/ttys/ttyq0' ttyq0
ln -s ' TRANSPARENT /server/ttys/ptyPa_htg' ptyPa_htg
ln -s ' TRANSPARENT /server/ttys/ptyp5' ptyp5
ln -s ' TRANSPARENT /server/ttys/ptyqf_htg' ptyqf_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ6' ttyQ6
ln -s ' TRANSPARENT /server/ttys/ttyq0_htg' ttyq0_htg
ln -s ' TRANSPARENT /server/ttys/ptyPb' ptyPb
ln -s ' TRANSPARENT /server/ttys/ptyp5_htg' ptyp5_htg
ln -s ' TRANSPARENT /server/ttys/tty00' tty00
ln -s ' TRANSPARENT /server/ttys/ttyQ6_htg' ttyQ6_htg
ln -s ' TRANSPARENT /server/ttys/ttyq1' ttyq1
ln -s ' TRANSPARENT /server/ttys/ptyPb_htg' ptyPb_htg
ln -s ' TRANSPARENT /server/ttys/ptyp6' ptyp6
ln -s ' TRANSPARENT /server/ttys/tty00_htg' tty00_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ7' ttyQ7
ln -s ' TRANSPARENT /server/ttys/ttyq1_htg' ttyq1_htg
ln -s ' TRANSPARENT /server/ttys/ptyPc' ptyPc
ln -s ' TRANSPARENT /server/ttys/ptyp6_htg' ptyp6_htg
ln -s ' TRANSPARENT /server/ttys/tty01' tty01
ln -s ' TRANSPARENT /server/ttys/ttyQ7_htg' ttyQ7_htg
ln -s ' TRANSPARENT /server/ttys/ttyq2' ttyq2
ln -s ' TRANSPARENT /server/ttys/ptyPc_htg' ptyPc_htg
ln -s ' TRANSPARENT /server/ttys/ptyp7' ptyp7
ln -s ' TRANSPARENT /server/ttys/tty01_htg' tty01_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ8' ttyQ8
ln -s ' TRANSPARENT /server/ttys/ttyq2_htg' ttyq2_htg
ln -s ' TRANSPARENT /server/ttys/ptyPd' ptyPd
ln -s ' TRANSPARENT /server/ttys/ptyp7_htg' ptyp7_htg
ln -s ' TRANSPARENT /server/ttys/tty02' tty02
ln -s ' TRANSPARENT /server/ttys/ttyQ8_htg' ttyQ8_htg
ln -s ' TRANSPARENT /server/ttys/ttyq3' ttyq3
ln -s ' TRANSPARENT /server/ttys/ptyPd_htg' ptyPd_htg
ln -s ' TRANSPARENT /server/ttys/ptyp8' ptyp8
ln -s ' TRANSPARENT /server/ttys/tty02_htg' tty02_htg
ln -s ' TRANSPARENT /server/ttys/ttyQ9' ttyQ9
ln -s ' TRANSPARENT /server/ttys/ttyq3_htg' ttyq3_htg
ln -s ' TRANSPARENT /server/ttys/ptyPe' ptyPe
ln -s ' TRANSPARENT /server/ttys/ptyp8_htg' ptyp8_htg
ln -s ' TRANSPARENT /server/ttys/tty03' tty03
ln -s ' TRANSPARENT /server/ttys/ttyQ9_htg' ttyQ9_htg
ln -s ' TRANSPARENT /server/ttys/ttyq4' ttyq4
ln -s ' TRANSPARENT /server/ttys/ptyPe_htg' ptyPe_htg
ln -s ' TRANSPARENT /server/ttys/ptyp9' ptyp9
ln -s ' TRANSPARENT /server/ttys/tty03_htg' tty03_htg
ln -s ' TRANSPARENT /server/ttys/ttyQa' ttyQa
ln -s ' TRANSPARENT /server/ttys/ttyq4_htg' ttyq4_htg
ln -s ' TRANSPARENT /server/ttys/ptyPf' ptyPf
ln -s ' TRANSPARENT /server/ttys/ptyp9_htg' ptyp9_htg
ln -s ' TRANSPARENT /server/ttys/ttyP0' ttyP0
ln -s ' TRANSPARENT /server/ttys/ttyQa_htg' ttyQa_htg
ln -s ' TRANSPARENT /server/ttys/ttyq5' ttyq5
ln -s ' TRANSPARENT /server/ttys/ptyPf_htg' ptyPf_htg
ln -s ' TRANSPARENT /server/ttys/ptypa' ptypa
ln -s ' TRANSPARENT /server/ttys/ttyP0_htg' ttyP0_htg
ln -s ' TRANSPARENT /server/ttys/ttyQb' ttyQb
ln -s ' TRANSPARENT /server/ttys/ttyq5_htg' ttyq5_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ0' ptyQ0
ln -s ' TRANSPARENT /server/ttys/ptypa_htg' ptypa_htg
ln -s ' TRANSPARENT /server/ttys/ttyP1' ttyP1
ln -s ' TRANSPARENT /server/ttys/ttyQb_htg' ttyQb_htg
ln -s ' TRANSPARENT /server/ttys/ttyq6' ttyq6
ln -s ' TRANSPARENT /server/ttys/ptyQ0_htg' ptyQ0_htg
ln -s ' TRANSPARENT /server/ttys/ptypb' ptypb
ln -s ' TRANSPARENT /server/ttys/ttyP1_htg' ttyP1_htg
ln -s ' TRANSPARENT /server/ttys/ttyQc' ttyQc
ln -s ' TRANSPARENT /server/ttys/ttyq6_htg' ttyq6_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ1' ptyQ1
ln -s ' TRANSPARENT /server/ttys/ptypb_htg' ptypb_htg
ln -s ' TRANSPARENT /server/ttys/ttyP2' ttyP2
ln -s ' TRANSPARENT /server/ttys/ttyQc_htg' ttyQc_htg
ln -s ' TRANSPARENT /server/ttys/ttyq7' ttyq7
ln -s ' TRANSPARENT /server/ttys/ptyQ1_htg' ptyQ1_htg
ln -s ' TRANSPARENT /server/ttys/ptypc' ptypc
ln -s ' TRANSPARENT /server/ttys/ttyP2_htg' ttyP2_htg
ln -s ' TRANSPARENT /server/ttys/ttyQd' ttyQd
ln -s ' TRANSPARENT /server/ttys/ttyq7_htg' ttyq7_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ2' ptyQ2
ln -s ' TRANSPARENT /server/ttys/ptypc_htg' ptypc_htg
ln -s ' TRANSPARENT /server/ttys/ttyP3' ttyP3
ln -s ' TRANSPARENT /server/ttys/ttyQd_htg' ttyQd_htg
ln -s ' TRANSPARENT /server/ttys/ttyq8' ttyq8
ln -s ' TRANSPARENT /server/ttys/ptyQ2_htg' ptyQ2_htg
ln -s ' TRANSPARENT /server/ttys/ptypd' ptypd
ln -s ' TRANSPARENT /server/ttys/ttyP3_htg' ttyP3_htg
ln -s ' TRANSPARENT /server/ttys/ttyQe' ttyQe
ln -s ' TRANSPARENT /server/ttys/ttyq8_htg' ttyq8_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ3' ptyQ3
ln -s ' TRANSPARENT /server/ttys/ptypd_htg' ptypd_htg
ln -s ' TRANSPARENT /server/ttys/ttyP4' ttyP4
ln -s ' TRANSPARENT /server/ttys/ttyQe_htg' ttyQe_htg
ln -s ' TRANSPARENT /server/ttys/ttyq9' ttyq9
ln -s ' TRANSPARENT /server/ttys/ptyQ3_htg' ptyQ3_htg
ln -s ' TRANSPARENT /server/ttys/ptype' ptype
ln -s ' TRANSPARENT /server/ttys/ttyP4_htg' ttyP4_htg
ln -s ' TRANSPARENT /server/ttys/ttyQf' ttyQf
ln -s ' TRANSPARENT /server/ttys/ttyq9_htg' ttyq9_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ4' ptyQ4
ln -s ' TRANSPARENT /server/ttys/ptype_htg' ptype_htg
ln -s ' TRANSPARENT /server/ttys/ttyP5' ttyP5
ln -s ' TRANSPARENT /server/ttys/ttyQf_htg' ttyQf_htg
ln -s ' TRANSPARENT /server/ttys/ttyqa' ttyqa
ln -s ' TRANSPARENT /server/ttys/ptyQ4_htg' ptyQ4_htg
ln -s ' TRANSPARENT /server/ttys/ptypf' ptypf
ln -s ' TRANSPARENT /server/ttys/ttyP5_htg' ttyP5_htg
ln -s ' TRANSPARENT /server/ttys/ttyp0' ttyp0
ln -s ' TRANSPARENT /server/ttys/ttyqa_htg' ttyqa_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ5' ptyQ5
ln -s ' TRANSPARENT /server/ttys/ptypf_htg' ptypf_htg
ln -s ' TRANSPARENT /server/ttys/ttyP6' ttyP6
ln -s ' TRANSPARENT /server/ttys/ttyp0_htg' ttyp0_htg
ln -s ' TRANSPARENT /server/ttys/ttyqb' ttyqb
ln -s ' TRANSPARENT /server/ttys/ptyQ5_htg' ptyQ5_htg
ln -s ' TRANSPARENT /server/ttys/ptyq0' ptyq0
ln -s ' TRANSPARENT /server/ttys/ttyP6_htg' ttyP6_htg
ln -s ' TRANSPARENT /server/ttys/ttyp1' ttyp1
ln -s ' TRANSPARENT /server/ttys/ttyqb_htg' ttyqb_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ6' ptyQ6
ln -s ' TRANSPARENT /server/ttys/ptyq0_htg' ptyq0_htg
ln -s ' TRANSPARENT /server/ttys/ttyP7' ttyP7
ln -s ' TRANSPARENT /server/ttys/ttyp1_htg' ttyp1_htg
ln -s ' TRANSPARENT /server/ttys/ttyqc' ttyqc
ln -s ' TRANSPARENT /server/ttys/ptyQ6_htg' ptyQ6_htg
ln -s ' TRANSPARENT /server/ttys/ptyq1' ptyq1
ln -s ' TRANSPARENT /server/ttys/ttyP7_htg' ttyP7_htg
ln -s ' TRANSPARENT /server/ttys/ttyp2' ttyp2
ln -s ' TRANSPARENT /server/ttys/ttyqc_htg' ttyqc_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ7' ptyQ7
ln -s ' TRANSPARENT /server/ttys/ptyq1_htg' ptyq1_htg
ln -s ' TRANSPARENT /server/ttys/ttyP8' ttyP8
ln -s ' TRANSPARENT /server/ttys/ttyp2_htg' ttyp2_htg
ln -s ' TRANSPARENT /server/ttys/ttyqd' ttyqd
ln -s ' TRANSPARENT /server/ttys/ptyQ7_htg' ptyQ7_htg
ln -s ' TRANSPARENT /server/ttys/ptyq2' ptyq2
ln -s ' TRANSPARENT /server/ttys/ttyP8_htg' ttyP8_htg
ln -s ' TRANSPARENT /server/ttys/ttyp3' ttyp3
ln -s ' TRANSPARENT /server/ttys/ttyqd_htg' ttyqd_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ8' ptyQ8
ln -s ' TRANSPARENT /server/ttys/ptyq2_htg' ptyq2_htg
ln -s ' TRANSPARENT /server/ttys/ttyP9' ttyP9
ln -s ' TRANSPARENT /server/ttys/ttyp3_htg' ttyp3_htg
ln -s ' TRANSPARENT /server/ttys/ttyqe' ttyqe
ln -s ' TRANSPARENT /server/ttys/ptyQ8_htg' ptyQ8_htg
ln -s ' TRANSPARENT /server/ttys/ptyq3' ptyq3
ln -s ' TRANSPARENT /server/ttys/ttyP9_htg' ttyP9_htg
ln -s ' TRANSPARENT /server/ttys/ttyp4' ttyp4
ln -s ' TRANSPARENT /server/ttys/ttyqe_htg' ttyqe_htg
ln -s ' TRANSPARENT /server/ttys/ptyQ9' ptyQ9
ln -s ' TRANSPARENT /server/ttys/ptyq3_htg' ptyq3_htg
ln -s ' TRANSPARENT /server/ttys/ttyPa' ttyPa
ln -s ' TRANSPARENT /server/ttys/ttyp4_htg' ttyp4_htg
ln -s ' TRANSPARENT /server/ttys/ttyqf' ttyqf
ln -s ' TRANSPARENT /server/ttys/ptyQ9_htg' ptyQ9_htg
ln -s ' TRANSPARENT /server/ttys/ptyq4' ptyq4
ln -s ' TRANSPARENT /server/ttys/ttyPa_htg' ttyPa_htg
ln -s ' TRANSPARENT /server/ttys/ttyp5' ttyp5
ln -s ' TRANSPARENT /server/ttys/ttyqf_htg' ttyqf_htg

echo "done"
