#!/bin/sh

BIN=$(pwd)/webserv
TEST_FOLDER=$(pwd)/tests/bad_config/
DEBUG_LOG="$(pwd)/tests/debug_bad_config.log"
SHOW_DEBUG=0

cd $TEST_FOLDER

fail() {
	EXITCODE=$1
	if [ $RUNNING != 0 ]; then
		cat $DEBUG_LOG
	fi
	echo "FAIL: $TESTNAME ❌"
	rm $DEBUG_LOG
	exit $EXITCODE
}

success() {
	echo "SUCCESS! ✅"
	rm $DEBUG_LOG
}

run() {
	CONF=$1
	TESTNAME="$(basename $(dirname $CONF))/$(basename $CONF)"
	$BIN $CONF 1>$DEBUG_LOG 2>$DEBUG_LOG &
	PID=$!
	sleep 0.1
	ps -p $PID 1>/dev/null 2>/dev/null
	RUNNING=$?
	if [ $RUNNING -eq 0 ]; then
		echo "ERROR: webserv is running" >> $DEBUG_LOG
		kill $PID
		fail 1
	fi
}

run_all_tests ()
{
	for config in $(ls $TEST_FOLDER); do
		run $config
	done
	success
}

run_all_tests