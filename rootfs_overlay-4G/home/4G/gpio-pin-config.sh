#! /bin/bash


# GPIO pin state reader.


printf "Pin Direction Edge  Active-Low  value\n"
printf "\n"


for pin in 36 39 42
do

  sh -c "echo  '$pin' > /sys/class/gpio/export"

  printf " $pin  "

  printf " `cat /sys/class/gpio/gpio$pin/direction`     "
  printf " `cat /sys/class/gpio/gpio$pin/edge`   "
  printf " `cat /sys/class/gpio/gpio$pin/active_low`         "
  printf " `cat /sys/class/gpio/gpio$pin/value`    "

  sh -c "echo '$pin' > /sys/class/gpio/unexport"
  
  printf "\n"

done
