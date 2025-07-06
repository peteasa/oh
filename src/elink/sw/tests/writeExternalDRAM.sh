#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
echo $EXEPATH

echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_00000001_8f050000_05
echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_00000010_8f050004_05
echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_00000100_8f050008_05
echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_00001000_8f05000c_05
echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_00010000_8f050010_05
echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_00100000_8f050014_05
echo "--Write external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_01000000_8f05001c_05
echo "--Read external DRAM (heap)--"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f050000_04
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f050004_04
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f050008_04
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f05000c_04
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f050010_04
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f050014_04
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8f05001c_04
