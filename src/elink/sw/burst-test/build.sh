#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-B ${ESDK}/lib -B ${ESDK}/lib/epiphany-elf/11.3.0"
EINCS="-I ${ESDK}/include"
ELDF=/usr/share/epiphany/bsps/current/internal.ldf

# Create the binaries directory
mkdir -p bin/

CROSS_COMPILE=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac

# Build HOST side application
${CROSS_COMPILE}gcc src/main.c -g -o bin/main.elf -le-hal -le-loader -lpthread

# Build DEVICE side program
OPT=3
/usr/bin/epiphany-elf-gcc -g -fno-dwarf2-cfi-asm -T ${ELDF} -O${OPT} src/emain.c src/etest.S -o bin/emain.elf ${EINCS} ${ELIBS} -le-lib 
