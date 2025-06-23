#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

state=$1

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
echo $EXEPATH

echo "--Set EAST routing ctrlmode in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0210_05

echo "--Set EAST Epiphany link to half speed--"
$EXEPATH/../bin/e-access 00000000_00000001_88bf0300_05

echo "--Clear ctrlmode in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000000_810f0210_05


