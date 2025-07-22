#! /bin/bash

test_file=int_test_gen.h

unsigned_fmt_list="d x X"
signed_fmt_list="d"



declare -i index=0
declare -i test_id=1




# $1 - type name
# $2 - 1 - signed, 0 - unsigned
function generate_type()
{
       char_type="char"

cat << HERE >> $test_file
const FormatElement<$type> fmt$test_id[] = {
HERE


	# for all types
	#if ((signed)) ; then
	#	format_type_list="d"
	#else
	#	format_type_list="d x X"
	#fi


	format_type_prefix=""
	format_type="d"

	# for all widths

	#for format_type in $format_type_list ; do


	# generate signed number
	for (( i=0 ; i<10 ; i++ )) ; do
		
		#
		# decide format type
		#
		if ((signed)) ; then
			format_type="d" ;
		else
			format_type="u" ;
		fi

		if ((sizeof==2)) ; then 
			format_type_prefix="h"
		elif ((sizeof==4)) ; then
			format_type_prefix="l" ;
		elif ((sizeof==8)) ; then 
			format_type_prefix="ll" ;
		fi


		if ((!signed)) ; then
			x=`bc << !
				obase=16; ($RANDOM%256*2^24) + ($RANDOM%256*2^16) + $RANDOM
!
`
		else
			x=`bc << !
				obase=16; ($RANDOM%128*2^24) + ($RANDOM%256*2^16) + ($RANDOM)
!
`
		fi

		y=`bc << !
			obase=16; ($RANDOM%256*2^24) + ($RANDOM%256*2^16) + ($RANDOM%256*2^8) + ($RANDOM%256)
!
`

		#fill-up with zeroes
		while (( ${#x} < 8 )) ; do
			x="0$x"
		done

		x="$x$y"
		x="0x${x:0:$sizeof*2}"

		if ((signed&&$RANDOM%2==1)) ; then x="-$x" ;  fi
		if ((!signed&&sizeof==4)) ; then x="${x}UL" ; fi
		if ((signed&&sizeof==4)) ; then x="${x}L" ; fi
		if ((!signed&&sizeof==8)) ; then x="${x}ULL" ; fi
		if ((signed&&sizeof==8)) ; then x="${x}LL" ; fi



cat << HERE >> $test_file
	{ $index, "%$format_type_prefix$format_type", $x },
HERE
		((index++))
        done

cat << HERE >> $test_file
} ;
HERE

((index++))
((test_id++))
}


#template <class T>
#struct FormatElement {
#	int index ;
#	const char *fmt ;
#	T val ;
#} ;


cat << HERE > $test_file
/*
 * Automatically generated file
 */

#pragma once

#include "process_test.h"

HERE

cat << HERE >> $test_file

// make all types as one string, without spaces
using long_long = long long ;
using unsigned_int = unsigned int ;
using unsigned_long = unsigned long ;
using unsigned_long_long = unsigned long long ;


HERE

signed=1
sizeof=1
type_list="int long long_long"
for type in $type_list ; do
	generate_type
	((sizeof=sizeof*2))
done

signed=0
sizeof=1
type_list="unsigned_int unsigned_long unsigned_long_long"
echo "Generating..."
for type in $type_list ; do
	echo $type
	generate_type
	((sizeof=sizeof*2))
done
echo "Done."


cat << HERE >> $test_file

inline
int processAll()
{
HERE
	for (( i=1 ; i<test_id ; i++)) ; do
cat << HERE >> $test_file
	if ( int ret = processTest ( fmt$i, sizeof(fmt$i)/sizeof(fmt$i[0]) ) ; ret != 0 )
		return ret ;
HERE
	done
cat << HERE >> $test_file
	return 0 ;
}
HERE


#cat << HERE >> $test_file
#struct unsigned_type_test {
#  int index ;
#  const char *fmt ;
#  unsigned 
#}
#HERE

#for unsigned_type in $unsigned_type_list


