#!/bin/bash

set -e

EINCS="-I ../include"
SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin

CROSS_PREFIX=

# Build all tests
${CROSS_PREFIX}gcc src/e-access.c src/elink.c  -o bin/e-access ${EINCS}
${CROSS_PREFIX}gcc src/loop.c src/elink.c  -o bin/loop ${EINCS}



