#!/bin/bash


#Toggle the IGT pin
# Pull IGT low for 200ms and set high again. We can't do fractions of a second so use 1 second sleep
# If PLS8 is power on then the IND pin should be low and the EMERG_OFF should be high


write-pin.sh 36 0
sleep 0.2 

write-pin.sh 36 1

# read-pin.sh 39
# read-pin.sh 42 


