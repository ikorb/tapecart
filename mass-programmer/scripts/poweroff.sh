#!/bin/bash

. pin-assignments.sh

# Analogmux abschalten
gpio -g write $MUX_ENABLE 1

# strom ausschalten
gpio -g write $POWER_SWITCH 0
