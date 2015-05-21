#!/bin/sh

if [ $# -ne 1 -o ]; then
	echo "Usage: $0 flash_image.bin"
	exit
fi

if [ ! -f "$1" ]; then
	echo "Error: file $1 not found."
	exit 1
fi

echo "\
	rsettype 2\n\
	rx 100\n\
	r\n\
	loadbin $1, 0\n\
	r\n\
	g\n\
	exit\n\
	" | JLinkExe -If SWD -Device MK22FN1M0xxx12 -Speed 20000
