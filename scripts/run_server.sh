#!/bin/bash
while :; do
	JLinkGDBServer -If SWD -Device MK22FN1M0xxx12 -Speed 20000 $1
done
