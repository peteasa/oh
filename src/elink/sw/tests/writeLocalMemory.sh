#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
echo $EXEPATH

echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_00000001_80805000_05
echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_00000010_80805004_05
echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_00000100_80805008_05
echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_00001000_8080500c_05
echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_00010000_80805010_05
echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_00100000_80805014_05
echo "--Write local memory--"
$EXEPATH/../bin/e-access 00000000_01000000_8080501c_05
echo "--Read local memory--"
$EXEPATH/../bin/e-access 00000000_00000150_80805000_04
$EXEPATH/../bin/e-access 00000000_00000150_80805004_04
$EXEPATH/../bin/e-access 00000000_00000150_80805008_04
$EXEPATH/../bin/e-access 00000000_00000150_8080500c_04
$EXEPATH/../bin/e-access 00000000_00000150_80805010_04
$EXEPATH/../bin/e-access 00000000_00000150_80805014_04
$EXEPATH/../bin/e-access 00000000_00000150_8080501c_04
