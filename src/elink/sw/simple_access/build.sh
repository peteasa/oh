#!/bin/bash

set -e

# DEVICE side paths
ESDK=${EPIPHANY_HOME}
EDIRS="-B ${ESDK}/lib -B ${ESDK}/lib/epiphany-elf/11.3.0"
EINCS="-I ${ESDK}/include"

## to get access to local memory we link the code into local and global memory
#ELDF=/usr/share/epiphany/bsps/current/fast.ldf

## to get access to global memory we link the code into local memory only (but loose access to snprintf)
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
${CROSS_PREFIX}gcc src/access.c -o bin/access.elf -I ../include -le-hal -le-loader -lpthread

# Build DEVICE side program
OPT=0
/usr/bin/epiphany-elf-gcc -O${OPT} -T ${ELDF} src/e_access_task.c -o bin/e_access_task.elf ${EINCS} ${EDIRS} -le-lib -lm -ffast-math

# Convert ebinary to SREC file
/usr/bin/epiphany-elf-objcopy --srec-forceS3 --output-target srec bin/e_access_task.elf bin/e_access_task.srec
