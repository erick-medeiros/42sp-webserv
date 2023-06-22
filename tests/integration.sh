#!/bin/sh

BIN=webserv
TEST_FOLDER=tests/integration/

run() {
	TEST=$1
	PY="$TEST_FOLDER""$TEST"/test.py
	CONF="$TEST_FOLDER""$TEST"/config.conf
	echo "TEST:" $TEST
	# ./$BIN ./$CONF 1>/dev/null 2>/dev/null &
	./$BIN ./$CONF &
	PID=$!
	export PYTHONPATH=$PYTHONPATH:$(pwd)/$TEST_FOLDER
	python3 $PY
	EXITCODE=$?
	kill $PID
	if [ $EXITCODE != 0 ]; then exit $EXITCODE; fi
	return $EXITCODE
}

run status_code

# run multiple_servers

# run limit_client_body_size