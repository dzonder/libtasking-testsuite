#!/bin/bash
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/..
PROG=$1

set -m

JLinkGDBServer -If SWD -Device MK22FN1M0xxx12 -Speed 20000 &> $ROOT_DIR/JLinkServer.log &
PID_JLINK=$!

sleep 5

arm-none-linux-gnueabi-gdb $ROOT_DIR/build/$PROG.elf -ex "source $ROOT_DIR/scripts/macros.gdb" -ex "load"

kill $PID_JLINK
