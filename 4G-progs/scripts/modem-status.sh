#!/bin/bash


# Trying to remember a list of AT commands is pointless as the syntax for each command can be different
# I've gathered a few of the more relevant commands that I hope are useful to determine the operational status
# of the Gemalto PLS8-E modem. To understand the meaning of the commands will require access to the PLS8's 
# command set users' pdf file.


declare -a devinfo_commands=(
"AT&V"
"AT^SQPORT?" 
"AT+CGMI" 
"AT+CGMR"
"AT+CNUM"
"AT+GCAP"
"AT+CBC"
"AT+IPR?"
"AT+CFUN?"
"AT+CPAS"
"AT+IFC?"
"AT+CCLK?"
"AT+CGSN"
"AT+CSCS?"
"AT+CMUT?"
)

declare -a status_commands=(
"AT+COPS?"
"AT+CSQ"
"AT+CREG?"
"AT+CGACT?"
"AT+CGDCONT?"
"AT+CGATT?"
"AT+CGACT?"
"AT+CGQREQ?"
)
declare -a sim_commands=(
"AT+CPIN?"
)


TTY=ttyACM0

if [ ! -z "$1" ]
then 
  TTY="$1"
fi

# echo "TTY is $TTY"

for i in "${devinfo_commands[@]}" "${status_commands[@]}" "${sim_commands[@]}"
do
  ./modem-command $TTY "$i"
done

