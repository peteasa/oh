#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

#dumping disassembly
epiphany-elf-objdump -D bin/e_access_task.elf > DUMP

#running program
cd $EXEPATH/bin
# script to test simple_access 
./access.elf --user_time=791050 --dmesg_time=873443 ../tests/test_simple_access_parser.emf

#./access.elf ../tests/writeLocalMemory.emf
#./access.elf ../tests/set_halfspeed.emf
#./access.elf ../tests/toggle_led.emf
#./access.elf ../tests/readRxControl.emf
#./access.elf ../tests/readRegOfInterest.emf
#./access.elf ../tests/readLinkRegisters.emf
#./access.elf ../tests/writeExternalDRAM.emf
#./access.elf ../tests/test_loopback.emf
