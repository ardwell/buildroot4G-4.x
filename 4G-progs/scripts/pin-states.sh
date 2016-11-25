#!/bin/bash

# Output the states of the three GPIO pins 36, 39 and 42

read-pin.sh 36
printf "IGT(36) state = $?\n"

read-pin.sh 39
printf "EMERG-OFF (39) state = $?\n"

read-pin.sh 42
printf "PWD-IND (42) state = $?\n"

