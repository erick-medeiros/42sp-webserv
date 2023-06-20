#!/bin/sh

# setup

BIN=webserv
TEST_FOLDER=tests/integration/

run_server ()
{
	TEST=$1
	PY="$TEST_FOLDER""$TEST"/test.py
	CONF="$TEST_FOLDER""$TEST"/config.conf
	echo "TEST:" $TEST
	./$BIN ./$CONF 1>/dev/null 2>/dev/null &
	PID=$!
	export PYTHONPATH=$PYTHONPATH:$(pwd)/$TEST_FOLDER
	python3 $PY
	EXITCODE=$?
	kill $PID
	return $EXITCODE
}

# end setup

run_server status_code

run_server multiple_servers