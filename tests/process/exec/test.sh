#!/bin/sh

if ! ./exec_test test_pgm arg1 arg2 arg3 arg4 ; then
	exit 1
fi

exit 0


