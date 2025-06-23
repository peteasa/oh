#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
echo $EXEPATH

echo "--Read ETX_MONITOR in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f021c_04
echo "--Read ETX_STATUS in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0214_04
echo "--Read ETX_PACKET in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0220_04
echo "--Read ETX_CFG in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0210_04
echo "--Read ERX_STATUS in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0304_04
echo "--Read ERX_CFG in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0300_04
echo "--Read ERX_TESTDATA in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0318_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
echo "--Read E_MAILBOXSTAT in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0738_04
echo "--Read E_MAILBOXLO in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0730_04
echo "--Read E_MAILBOXHI in FPGA--"
$EXEPATH/../bin/e-access 00000000_00000150_810f0734_04
