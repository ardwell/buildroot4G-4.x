#!/bin/bash

# Access GPIO pin via sysfs
# There is no checking of input parameter value.

# The return value $? is the value of the read pin

if [ $# = 0 ]; then
  echo No parameters for GPIO pin number to read 
  echo eg read-pin 18
  echo where 18 is the gpio pin number
  exit 1
fi

# No check on the number entered.

# export the relevant GPIO pin
# Not using subshell now - Because we are invoking a sub-shell we have to use the relevant quotes to be able to pass the pin number and value

echo $1 > /sys/class/gpio/export

# Now read the pin

echo "in" > /sys/class/gpio/gpio"$1"/direction
#sh -c "$VALUE(`cat /sys/class/gpio/gpio\'$1\'/value`)"
 
# echo " pin value = "
# sh -c "cat /sys/class/gpio/gpio"$1"/value"
# cat /sys/class/gpio/gpio"$1"/value

VALUE=`cat /sys/class/gpio/gpio"$1"/value`
# sh -c "echo -n "value for gpio pin $1 = cat /sys/class/gpio/gpio'$1'/value"

echo  $1 > /sys/class/gpio/unexport

# ls /sys/class/gpio
# echo "Pin value = $VALUE"

exit $VALUE
