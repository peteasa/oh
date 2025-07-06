#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")

#dumping disassembly
epiphany-elf-objdump -D bin/emain.elf > DUMP

#running program
cd $EXEPATH/bin
./main.elf

