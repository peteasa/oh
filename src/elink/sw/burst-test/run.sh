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
epiphany-elf-objdump -D bin/emain.elf > DUMP

cd $EXEPATH/bin

./main.elf

