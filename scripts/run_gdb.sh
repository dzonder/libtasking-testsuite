#!/bin/bash
ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/..
arm-none-linux-gnueabi-gdb -ex "source $ROOT_DIR/scripts/macros.gdb" $ROOT_DIR/build/$1.elf
