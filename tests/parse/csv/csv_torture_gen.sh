#! /bin/bash

datafile=$1
resultfile=$2


[ -z "$datafile" -o -z "$resultfile" ] && echo "usage: csv_torture_gen <datafile> <resultfile>" && exit 1

cols=50
rows=20
total=0

RANGE=40000

echo -n > $datafile
echo -n > $resultfile


for ((row=0;row<rows;row++)) ; do
	for ((col=0;col<cols;col++)) ; do
		((col!=0)) && echo -n "," >> $datafile

		number=$RANDOM
		let "number %= $RANGE"

		quoted=$RANDOM
		let "quoted %= 2"

		(( quoted == 1 )) && echo -n "\"" >> $datafile
		echo -n $number >> $datafile
		(( quoted == 1 )) && echo -n "\"" >> $datafile

		((total+=number))
	done
	echo >> $datafile
done

echo "$cols $rows $total" >> $resultfile


