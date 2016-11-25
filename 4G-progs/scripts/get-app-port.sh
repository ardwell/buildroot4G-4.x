#!/bin/bash

# Look through all the devices as /dev/ttyACM* and locate the 'Application' port


for dev in `ls /dev/ttyACM*`; 
do 
  port=$(basename $dev)

  echo $port

  res=`modem-command $port "AT^SQPORT"`

  # This is command subsitution same as using back quotes ``
  export APPTTY=$(echo $res | awk -v port="$port" -F' ' '{if ($3 == "Application") print port}')

  if [ -n "$APPTTY" ]; then
    break 
  fi

  echo "$dev"; 
done
