#!/bin/bash

set -e

# DEVICE side paths
ESDK=${EPIPHANY_HOME}
EDIRS="-B ${ESDK}/lib -B ${ESDK}/lib/epiphany-elf/11.3.0"
EINCS="-I ${ESDK}/include"
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
${CROSS_PREFIX}gcc src/e-main.c -o bin/e-main.elf -I ../include -le-hal -le-loader -lpthread

# Build DEVICE side program
/usr/bin/epiphany-elf-gcc -O0 -T ${ELDF} src/e-task.c -o bin/e-task.elf ${EINCS} ${EDIRS} -le-lib -lm -ffast-math

# Convert ebinary to SREC file
/usr/bin/epiphany-elf-objcopy --srec-forceS3 --output-target srec bin/e-task.elf bin/e-task.srec
