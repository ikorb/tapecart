#!/bin/bash

. pin-assignments.sh

# first reset the chip
gpio -g write $MUX_SWITCH 0
gpio -g write $RESET_OUT 0
gpio -g write $RESET_OUT 1

#sleep 1

# then write the magic value using openocd
openocd -f openocd-rpi.conf -c init -c trigger_uartmode -c shutdown

# then reset and switch over to UART
gpio -g write $RESET_OUT 0
gpio -g write $RESET_OUT 1
gpio -g write $MUX_SWITCH 1
