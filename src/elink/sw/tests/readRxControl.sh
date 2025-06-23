#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
echo $EXEPATH

echo "--Read ERX_CFG in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04

echo "--Read ERX_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04









