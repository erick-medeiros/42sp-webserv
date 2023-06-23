#!/bin/sh

BIN=webserv
TEST_FOLDER=tests/integration/

run() {
	TEST=$1
	DEBUG=$2
	echo -n "TEST: $TEST "
	if [ ! -d $TEST ]; then
		echo ❌
		exit
	fi
	PY="$TEST"/test.py
	CONF="$TEST"/config.conf
	if [ $DEBUG = true ]; then
		./$BIN ./$CONF &
	else
		./$BIN ./$CONF 1>/dev/null 2>/dev/null &
	fi
	PID=$!
	export PYTHONPATH=$PYTHONPATH:$(pwd)/$TEST_FOLDER
	python3 $PY
	EXITCODE=$?
	kill $PID
	if [ $EXITCODE != 0 ]; then
		echo ❌
		exit $EXITCODE
	fi
	echo ✅
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
		run $f false
	done
}

run_single_test() {
	run $TEST_FOLDER$1 true
}

if [ -n "$1" ]; then
	run_single_test $1
else
	run_all_tests
fi
