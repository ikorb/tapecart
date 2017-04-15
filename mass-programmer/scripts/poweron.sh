#!/bin/bash

. pin-assignments.sh

# strom einschalten
gpio -g write $POWER_SWITCH 1

# Analogmux auf JTAG und aktivieren
gpio -g write $MUX_SWITCH 0
gpio -g write $MUX_ENABLE 0
