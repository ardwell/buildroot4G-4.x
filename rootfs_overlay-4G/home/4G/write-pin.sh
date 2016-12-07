#! /bin/bash

# Access GPIO pin via sysfs
# There is no checking of input parameter value.

if [ $# = 0 ]; then
  echo No parameters for GPIO pin number and value entered
  echo eg write-pin 18 0
  echo where 18 is the gpio pin and 0 is the value to write to this pin
  exit 1
fi

# No check on the number entered.

# export the relevant GPIO pin
# Because we are invoking a sub-shell we have to use the relevant quotes to be able to pass the pin number and value

#sh -c "echo  $1 > /sys/class/gpio/export"
echo  $1 > /sys/class/gpio/export

 ls /sys/class/gpio

# Now write the required value 0 or 1
#sh -c "echo out  > /sys/class/gpio/gpio\"$1\"/direction"
echo out  > /sys/class/gpio/gpio"$1"/direction

# sh -c "echo $2  > /sys/class/gpio/gpio"$1"/value"
echo $2  > /sys/class/gpio/gpio"$1"/value

# Had to add this to get the 4G modem device to work. It seems as though without this small delay the 
# /sys/class/gpio/gpio$1 directoy may be disappearing before the 4G chip has a chance to read it.

sleep 0.2
 
# sh -c "echo  $1 > /sys/class/gpio/unexport"
echo  $1 > /sys/class/gpio/unexport

# ls /sys/class/gpio

exit 0

