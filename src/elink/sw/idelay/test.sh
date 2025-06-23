#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


$EXEPATH/bin/e-main.elf $EXEPATH/bin/e-task.srec > e-main.log

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval

