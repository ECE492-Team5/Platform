#!/bin/bash

# Adapted from Rocketboards' Guide on Development with DE0 Nano SoC
# https://rocketboards.org/foswiki/view/Documentation/EmbeddedLinuxBeginnerSGuide
# Authored by Devon Andade on Dec 21 2015

# Modified by Satyen Akolkar
# March 22 2017

# Generate New Header File From QSYS SOPCINFO
# Header file contains Base addresses for system components' registers
echo ''
echo 'Generating Header File...'
sopc-create-header-files output/soc_system.sopcinfo \
    --single output/hps_0.h \
    --module hps_0

# Generate Device Tree Source File
echo ''
echo 'Generating Device Tree Source...'
sopc2dts --input output/soc_system.sopcinfo \
    --output output/soc_system.dts \
    --type dts \
    --board board-info/soc_system_board_info.xml \
    --board board-info/hps_common_board_info.xml \
    --bridge-removal all \
    --clocks

# Compile Device Tree Source to Binary
echo ''
echo 'Compiling Device Tree Source'
dtc -f -I dts -O dtb -o output/soc_system.dtb output/soc_system.dts

# Complete
echo ''
echo 'Complete. Make sure to update the .rbf file through Quartus.'
