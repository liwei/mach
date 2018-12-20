#!/bin/csh -f -b
# 
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
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

#
# Purpose:
#	This script is used to take an "Abstract Class Interface" definition
#	and create the "Default Proxy Class" "_ifc.h" and ".cc" files.
#
#	It is assumed that the abstract class is of the "standard" format
#	for such classes, most notably, that the remote routines are marked
#	with the "REMOTE" macro, there are no argument identifiers given,
#	and that the file conforms to c++ syntax.
#
# HISTORY
# $Log:	proxy_default.csh,v $
# Revision 2.2  94/06/16  17:09:36  mrt
# 	Moved from utils.
# 
# 
# Revision 2.4.1.1  94/05/20  16:03:08  mrt
# 	Changed to take -cpp <cpp-name> as optional input arg.
#	Moved from utils to bin.
#
# 	[94/03/01            mrt]
# 
# Revision 2.4  94/01/11  18:13:19  jms
# 	Modify heavily to be able to build all of the "default"
# 	proxies.  Even those that inherit from more than to parent classes.
# 	Part of the creation of the "lib/proxies" separation from "lib/us++"
# 	[94/01/10  14:04:50  jms]
# 
# Revision 2.3  93/01/20  17:41:08  jms
# 	Add "cflags" arg to be used when running "cc -E"
# 	[93/01/18  18:11:01  jms]
# 
# Revision 2.2  92/07/05  23:37:31  dpj
# 	Converted for new C++ RPC package.
# 	[92/07/05  19:04:46  dpj]
# 
# 	First version.
# 	[92/06/24  18:24:58  jms]
# 


set after_cflags = (`echo $argv | fgrep '.h' | tr ' ' '\012' | sed -e '1,/\.h/{' -e '/\.h/'\!\d -e '}'`)

if ($#after_cflags < 3) then
    echo 'Usage: proxy_default [-v] [-cpp <cpp> ] <cflags> <abstract_class_interface> <parent_proxy_class_interface> ... <target_base_name>'
    echo '	e.g.: proxy_default -v us_tm_task_ifc.h us_item_proxy_ifc.h us_tm_task_proxy'
    echo '		Output would got to us_tm_task_proxy{_ifc.h,.cc}'
    echo '		-v means "No virtual class inheritance" for compilier bug workaround'
    echo '		-cpp <cpp> specifies version of cpp to use; default is cc -E'
    exit -1
endif

set arg_index = 1
set use_virtual_class = 1
set cpp = 'cc -E'
set done = 1
while ( $done )
switch ($argv[$arg_index])
   case '-v'
     set use_virtual_class = 0
     @ arg_index++
     breaksw
   case '-cpp'
     @ arg_index++
     set cpp  = $argv[$arg_index]
     @ arg_index++
     breaksw
   default:
     set done = 0
endsw
end

@ last_cflag = $#argv - $#after_cflags
set cflags = ($argv[$arg_index-$last_cflag])
set in_file = $after_cflags[1]
@ pproxy_done = $#after_cflags - 1
set pproxy_files = ($after_cflags[2-$pproxy_done])
set out_name = $after_cflags[$#after_cflags]
set out_incl = "$out_name"_ifc.h
set out_code = "$out_name".cc

set tmp1 = /tmp/"$out_name"_default.tmp1
#set tmp1 = /tmp/$$_default.tmp1
set clean_txt = /tmp/"$out_name"_default.clean_txt
#set clean_txt = /tmp/$$_default.clean_txt
set pnd = '#'

rm -f $tmp1 >& /dev/null
rm -f $clean_txt >& /dev/null

# Build a clean text version of the input file
cat $in_file | \
sed '/^#[ 	]*include/d' > $tmp1
$cpp $cflags $tmp1 > $clean_txt

# Get the class/parent
set classes = (`sed -e 's/{/{/g' $clean_txt | tr '' '\012' | sed -e '/^[ 	]*class/,/\{/'\!\d -e '/\{/d' | tr ':,' '\012' | sed -e 's/^.*[ 	]\([^ 	][^ 	]*\)[ 	]*$/\1/'`)

if (2 > $#classes) then
    echo Error: could not find class and parent class strings in abstract class interface
    exit -1
endif

set class = $classes[1]
set parents = ($classes[2-$#classes])

# Get the name

#
# Build the proxy interface file
#

cat > $out_incl << XXX_INCL_1
/*
 * This proxy interface:
 *			${class}_proxy, $out_incl:t
 *
 * was machine generated from the pure abstract external interface:
 *			$class, $in_file:t
 *
 * `date`
 */

${pnd}ifndef	_${out_name:t}_ifc_h
${pnd}define	_${out_name:t}_ifc_h

${pnd}include <$in_file:t>
XXX_INCL_1

echo $pproxy_files | \
  tr ' ' '\012' | \
  sed -e 's@^.*/\([^/][^/]*\)$@\1@' | \
  sed -e 's@\(.*\)$@#include <\1>@' >> $out_incl

echo  ' ' >> $out_incl
#if ($#parents > 1) then
if ($use_virtual_class) then
  set V2 = VIRTUAL2' '
  set V5 = VIRTUAL5' '
else
  set V2
  set V5

  echo "//class ${class}_proxy: public VIRTUAL2 $class," >> $out_incl

  echo $parents | \
    tr ' ' '\012' | \
    sed 's@\(.*\)@//	public VIRTUAL5 \1_proxy,@' | \
    sed '$,$s@,@ {@' >> $out_incl
endif

echo "class ${class}_proxy: public $V2$class," >> $out_incl
echo $parents | \
  tr ' ' '\012' | \
  sed 's@\(.*\)@	public '"$V5"'\1_proxy,@' | \
  sed '$,$s@,@ {@' >> $out_incl

cat >> $out_incl << XXX_INCL_1_a

      public:
	DECLARE_PROXY_MEMBERS(${class}_proxy);
	${class}_proxy() {};
	~${class}_proxy() {};

XXX_INCL_1_a

# Get the remotes
set remote_cnt = (`fgrep -c REMOTE $clean_txt`)
if ($remote_cnt) then
	cat $clean_txt | \
	sed 's/(/(/' | \
	tr '' '\012' | \
	sed \
	    -e '/REMOTE/,/0;/'\!\d | \
	sed 's/,/,/g' | tr '' '\012' | \
	sed '/^[ 	]*$/d' | \
	sed  \
	    -e 's/^[ 	]*//' \
	    -e 's/^\(.*\),/	    \1,/' \
	    -e 's/^\(.*\))/	    \1)/' \
	    -e 's/)[^a-zA-Z1-9]*;/);/' | \
	tr '' '\012' | \
	cat >> $out_incl
endif

cat << XXX_INCL_2 >> $out_incl
};

${pnd}endif	_${out_name:t}_ifc_h
XXX_INCL_2

@ castdown_cnt = $#parents + 1
set parent_commap = (`echo "$parents"_proxy | sed 's/ /_proxy, /g'`)

# GXXCASTDOWN: castdowns of three or more may not work under g++1.37
if (3 <= $castdown_cnt) then
    set castdown_cnt = 2
    set parent_commap = (`echo $parent_commap[1] | sed 's/,//'`)
endif

cat > $out_code << XXX_CODE_TOP
/*
 * This proxy default implementation:
 *			${class}_proxy, $out_code:t
 *
 * was machine generated from the pure abstract external interface:
 *			$class, $in_file:t
 *
 * `date`
 */

${pnd}include <${out_incl:t}>

DEFINE_PROXY_CLASS(${class}_proxy)
DEFINE_CASTDOWN${castdown_cnt}(${class}_proxy, ${class}, ${parent_commap})

void ${class}_proxy::init_class(usClass* class_obj)
{
XXX_CODE_TOP

echo $parents | \
  tr ' ' '\012' | \
  sed -e 's/\(.*\)/	\1_proxy::init_class(class_obj);/' \
>> $out_code

cat >> $out_code << XXX_CODE_TOP_a
        ${class}::init_class(class_obj);

XXX_CODE_TOP_a

if ($remote_cnt) then
	echo '	BEGIN_SETUP_METHOD_WITH_ARGS('"$class"'_proxy);' >> $out_code

	fgrep REMOTE $out_incl | \
	sed \
	    -e 's/^.*mach_error_t //' \
	    -e 's/(//' -e 's/\(.*\)/	SETUP_METHOD_WITH_ARGS('"$class"'_proxy, \1);/' \
	>> $out_code

	echo '	END_SETUP_METHOD_WITH_ARGS;' >> $out_code
endif

cat >> $out_code << XXX_CODE_SETUP_END
}

XXX_CODE_SETUP_END

if (0 == $remote_cnt) goto NO_REMOTE_CALLS
cat $out_incl | \
sed \
    -e '/REMOTE/,/;/'\!\d \
    -e 's/^[ 	]*//' \
    -e 's/);/);/' | \
tr '\012' ' \012' | \
sed \
    -e 's/^ //' \
    -e 's/^\(REMOTE[ 	]*\)\(.*\)/\1\2RETURN \2/' | \
tr '' '\012' | \
sed \
    -e 's/[ 	][ 	]*/ /g' \
    -e 's/ virtual / /' \
    -e 's/\([^ (]\) *)/\1,)/' \
    -e 's/ *,/ arg_0_0987890_/' \
    -e 's/ *,/ arg_1_0987890_/' \
    -e 's/ *,/ arg_2_0987890_/' \
    -e 's/ *,/ arg_3_0987890_/' \
    -e 's/ *,/ arg_4_0987890_/' \
    -e 's/ *,/ arg_5_0987890_/' \
    -e 's/ *,/ arg_6_0987890_/' \
    -e 's/ *,/ arg_7_0987890_/' \
    -e 's/ *,/ arg_8_0987890_/' \
    -e 's/ *,/ arg_9_0987890_/' \
    -e 's/ *,/ arg_10_0987890_/' \
    -e 's/ *,/ arg_11_0987890_/' \
    -e 's/ *,/ arg_12_0987890_/' \
    -e 's/ *,/ arg_13_0987890_/' \
    -e 's/ *,/ arg_14_0987890_/' \
    -e 's/ *,/ arg_15_0987890_/' \
    -e 's/ *,/ arg_16_0987890_/' \
    -e 's/ *,/ arg_17_0987890_/' \
    -e 's/ *,/ arg_18_0987890_/' \
    -e 's/ *,/ arg_19_0987890_/' \
    -e 's/_0987890_/,/g' \
    -e 's/,)/)/' \
    -e '/^RETURN/s/ *\*\** */ /g' \
    -e '/^RETURN/s/\([(,]\) [^,]* \(arg_[0-9][0-9]*\)/\1 \2/g' \
    -e '/^RETURN/s/^RETURN mach_error_t \([^(]*\)( *\(.*\)/	return outgoing_invoke(		mach_method_id(\1), \2}/' \
    -e '/^REMOTE/s/^REMOTE mach_error_t \(.*\);.*/mach_error_t '"$class"'_proxy::\1{/' \
    -e 's/, *)/)/' \
    -e 's/( /(		/' \
    -e 's/, /,		/g' | \
tr '' '\012' | \
cat >> $out_code

NO_REMOTE_CALLS:

#cat $out_code
rm -f $clean_txt $tmp1

