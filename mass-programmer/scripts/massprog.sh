#!/bin/bash
#
# sample script for mass-programming tapecarts
# with firmware, loader and a onefiler PRG in flash

if [ x"$3" == x ]; then
    echo Usage: $0 firmware.bin loader.bin flashdata.prg
    exit 1
fi

./poweron.sh &&
    sudo openocd -f openocd-rpi.conf -c "program $1" -c shutdown &&
    sudo ./reset-to-uart.sh &&
    ./tcprog.rb -l $2 $3
./poweroff.sh
