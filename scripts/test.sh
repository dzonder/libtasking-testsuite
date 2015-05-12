#!/bin/bash
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/..
PROG=$1

echo "======= Running test '$PROG' ======="

JLinkGDBServer -If SWD -Device MK22FN1M0xxx12 -Speed 20000 &> $ROOT_DIR/JLinkServer.log &
PID_JLINK=$!

sleep 5

netcat localhost 2332 | tee $ROOT_DIR/$PROG.out &
PID_NETCAT=$!

arm-none-linux-gnueabi-gdb $ROOT_DIR/build/$PROG.elf -quiet \
	-ex "source $ROOT_DIR/scripts/macros.gdb" \
	-ex "b Reset_Handler:hang" \
	-ex "lrst" \
	-ex "bt" \
	-ex "d" \
	-ex "q" \
	2>&1 | tee $ROOT_DIR/gdb.log

kill $PID_JLINK $PID_NETCAT
wait $PID_JLINK $PID_NETCAT 2>&1

if [ -e "$ROOT_DIR/tests/$PROG/test.py" ]; then
	PYTHONPATH=$ROOT_DIR/scripts python $ROOT_DIR/tests/$PROG/test.py $ROOT_DIR/$PROG.out $ROOT_DIR/gdb.log 2>&1
	RESULT=$?

	if [ "$RESULT" -eq "0" ]; then
		echo "&&&&&&& PASSED $PROG"
	else
		echo "&&&&&&& FAILED $PROG"
	fi

	exit $RESULT
fi
