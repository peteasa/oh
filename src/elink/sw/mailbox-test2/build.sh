#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-B ${ESDK}/lib -B ${ESDK}/lib/epiphany-elf/11.3.0"
EINCS="-I ../include -I ${ESDK}/include"
ELDF=/usr/share/epiphany/bsps/current/internal.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

CROSS_PREFIX=
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
${CROSS_PREFIX}gcc src/main.c -o bin/main.elf -I ../include -le-hal -le-loader -lpthread

# Build DEVICE side program
OPT=3
/usr/bin/epiphany-elf-gcc -O${OPT} -T ${ELDF} src/emain.c -o bin/emain.elf ${EINCS} ${ELIBS} -le-lib -lm -ffast-math

# Convert ebinary to SREC file
/usr/bin/epiphany-elf-objcopy --srec-forceS3 --output-target srec bin/emain.elf bin/emain.srec
