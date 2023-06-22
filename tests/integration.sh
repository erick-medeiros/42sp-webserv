#!/bin/sh

BIN=webserv
TEST_FOLDER=tests/integration/

run() {
	TEST=$1
	PY="$TEST_FOLDER""$TEST"/test.py
	CONF="$TEST_FOLDER""$TEST"/config.conf
	./$BIN ./$CONF 1>/dev/null 2>/dev/null &
	PID=$!
	export PYTHONPATH=$PYTHONPATH:$(pwd)/$TEST_FOLDER
	python3 $PY
	EXITCODE=$?
	kill $PID
	echo -n "TEST:" $TEST " "
	if [ $EXITCODE != 0 ]; then echo ❌; exit $EXITCODE; fi
	echo ✅
	return $EXITCODE
}

run status_code

run multiple_servers

run limit_client_body_size