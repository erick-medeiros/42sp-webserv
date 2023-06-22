#!/bin/sh

BIN=webserv
TEST_FOLDER=tests/integration/

run() {
	TEST=$1
	PY="$TEST_FOLDER""$TEST"/test.py
	CONF="$TEST_FOLDER""$TEST"/config.conf
	if [ $# -eq 1 ]; then # If an args was passed to the script don't redirect the output
		./$BIN ./$CONF &
	else # Otherwise redirect the output to /dev/null
		./$BIN ./$CONF 1>/dev/null 2>/dev/null &
	fi
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

# If an args was passed to the script, run only the test with the same name
if [ $# -eq 1 ]; then
	run $1
	exit $?
fi

run "status_code"
run "multiple_servers"
run "default_error_pages"
run "limit_client_body_size"