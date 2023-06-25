#!/bin/sh

BIN=webserv
TEST_FOLDER=tests/integration/
DEBUG_LOG="$TEST_FOLDER"debug.log

fail() {
	EXITCODE=$1
	cat $DEBUG_LOG
	echo "FAIL! ❌"
	rm $DEBUG_LOG
	exit $EXITCODE
}

success() {
	echo "SUCCESS! ✅"
	rm $DEBUG_LOG
}

run() {
	TEST=$1
	echo "TEST: $TEST"
	if [ ! -d $TEST ]; then
		fail 1
	fi
	PY="$TEST"/test.py
	CONF="$TEST"/config.conf
	./$BIN ./$CONF 1>$DEBUG_LOG 2>$DEBUG_LOG &
	PID=$!
	export PYTHONPATH=$PYTHONPATH:$(pwd)/$TEST_FOLDER
	python3 $PY
	EXITCODE=$?
	kill $PID
	if [ $EXITCODE != 0 ]; then
		fail $EXITCODE
	fi
	return $EXITCODE
}

folders() {
	for f in "$1"*; do
		if [ -d "$f" ]; then
			if [ -z $(echo $f | grep __pycache__) ]; then
				echo $f
			fi
		fi
	done
}

subfolders() {
	for f in $(folders $1); do
		folders "$f"/
	done
}

run_all_tests() {
	for f in $(subfolders $TEST_FOLDER); do
		run $f
	done
	success
}

run_single_test() {
	run $TEST_FOLDER$1
	success
}

if [ -n "$1" ]; then
	run_single_test $1
else
	run_all_tests
fi
