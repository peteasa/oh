#!/bin/bash

## Enable user code to mmap and write to elink registers
echo 1 > /sys/module/epiphany/parameters/unsafe_access
echo "INFO: epiphany kernel driver unsafe_access enabled?"
cat /sys/module/epiphany/parameters/unsafe_access
echo

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
echo $EXEPATH

echo "-- ELINK_CLKCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810F0204_04
echo "-- ELINK_CHIPID --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810F0208_04
echo "-- ELINK_VERSION --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810F020C_04
echo "-- ELINK_TXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810F0210_04
echo "-- ELINK_RXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810F0300_04
echo "-- ELINK_RXOFFSET --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810F030C_04
echo "-- ELINK_TXMMU --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810E0000_04
echo "-- ELINK_RXMMU --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810E8000_04

echo "-- CONFIG (datasheet)--"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0F00_04
echo "-- CONFIG (architecture spec)--"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0400_04
echo "-- LINKCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0300_04
echo "-- LINKTXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0304_04
echo "-- LINKRXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0308_04
#echo "-- LINKDEBUG --"
#$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0338_04
echo "-- MESHCONFIG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0700_04
echo "-- COREID --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0704_04
echo "-- MULTICAST --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0708_04
echo "-- CMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0710_04
echo "-- XMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0714_04
echo "-- RMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_808F0718_04
echo "** NORTH eCore**"
echo "-- CONFIG (datasheet)--"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0F00_04
echo "-- CONFIG (architecture spec)--"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0400_04
echo "-- LINKCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0300_04
echo "-- LINKTXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0304_04
echo "-- LINKRXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0308_04
#need to prepare ctrlmode to access these registers:- they are not in the eCore!
$EXEPATH/../bin/e-access 00000000_00000000_810f0210_05
$EXEPATH/../bin/e-access 00000000_00000110_810f0210_05
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810f0210_04
$EXEPATH/../bin/e-access 00000000_03FFFFFF_80AF030C_05
echo "-- IOCONFIG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF030C_04
echo "-- IOFLAG --"
$EXEPATH/../bin/e-access 00000000_00000001_80AF0318_05
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0318_04
echo "-- LINKDEBUG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0338_04
echo "-- restore normal routing to the eCore --"
$EXEPATH/../bin/e-access 00000000_00000000_810F0210_05
echo "-- MESHCONFIG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0700_04
echo "-- COREID --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0704_04
echo "-- MULTICAST --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0708_04
echo "-- CMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0710_04
echo "-- XMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0714_04
echo "-- RMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_80AF0718_04
echo "** EAST eCore**"
echo "-- LINKCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0300_04
echo "-- LINKTXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0304_04
echo "-- LINKRXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0308_04
#need to prepare ctrlmode to access these registers:- they are not in the eCore!
$EXEPATH/../bin/e-access 00000000_00000000_810f0210_05
$EXEPATH/../bin/e-access 00000000_00000150_810f0210_05
$EXEPATH/../bin/e-access 00000000_DEADBEAF_810f0210_04
$EXEPATH/../bin/e-access 00000000_03FFFFFF_88BF030C_05
echo "-- IOCONFIG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF030C_04
echo "-- IOFLAG --"
$EXEPATH/../bin/e-access 00000000_00000001_88BF0318_05
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0318_04
echo "-- restore normal routing to the eCore --"
$EXEPATH/../bin/e-access 00000000_00000000_810F0210_05
echo "-- MESHCONFIG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0700_04
echo "-- COREID --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0704_04
echo "-- MULTICAST --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0708_04
echo "-- CMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0710_04
echo "-- XMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0714_04
echo "-- RMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_88BF0718_04
echo "-- LINKCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0300_04
echo "-- LINKTXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0304_04
echo "-- LINKRXCFG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0308_04
echo "-- MESHCONFIG --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0700_04
echo "-- COREID --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0704_04
echo "-- MULTICAST --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0708_04
echo "-- CMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0710_04
echo "-- XMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0714_04
echo "-- RMESHROUTE --"
$EXEPATH/../bin/e-access 00000000_DEADBEAF_8CBF0718_04
