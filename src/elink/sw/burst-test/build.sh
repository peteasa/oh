#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/lib -B ${ESDK}/lib/epiphany-elf/11.3.0"
EINCS="-I ${ESDK}/include"
ELDF=/usr/share/epiphany/bsps/current/internal.ldf

# Create the binaries directory
mkdir -p bin/

CROSS_COMPILE=

# Build HOST side application
${CROSS_COMPILE}gcc src/main.c -g -o bin/main.elf -le-hal -le-loader -lpthread

# Build DEVICE side program
OPT=3
/usr/bin/epiphany-elf-gcc -g -T ${ELDF} -O${OPT} src/emain.c src/etest.S -o bin/emain.elf ${EINCS} ${EDIRS} -le-lib
