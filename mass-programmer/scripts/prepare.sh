#!/bin/bash

. pin-assignments.sh

# power-switch
gpio -g mode $POWER_SWITCH out
gpio -g write $POWER_SWITCH 0

# enable-Pin am Analogmux
gpio -g mode $MUX_ENABLE out
gpio -g write $MUX_ENABLE 1

# mode-switch
gpio -g mode $MUX_SWITCH out
gpio -g write $MUX_SWITCH 0

# reset
gpio -g mode $RESET_OUT out
gpio -g write $RESET_OUT 1
