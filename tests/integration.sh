#!/bin/sh

# setup

BIN=webserv
TEST_FOLDER=tests/integration/
CONFIG_FOLDER=tests/integration/configs/

run_server ()
{
	./$BIN ./"$CONFIG_FOLDER"$2 1>/dev/null 2>/dev/null &
	PID=$!
	python3 "$TEST_FOLDER"$1
	EXITCODE=$?
	kill $PID
	return $EXITCODE
}

# end setup

run_server status_code.py status_code.conf

run_server multiple.py multiple.conf