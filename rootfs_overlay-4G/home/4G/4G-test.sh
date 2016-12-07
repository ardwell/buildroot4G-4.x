#!/bin/bash

function read-pin()
{
  # read GPIO pin value
  # 
  echo $1 > /sys/class/gpio/export
  pinvalue=`cat /sys/class/gpio/gpio"$1"/value`
  echo $1 > /sys/class/gpio/unexport
  # return $pinvalue

  if [ $pinvalue -eq 1 ]; then
    echo -e "\033[1;31mOFF "
  else
    echo -e "\033[1;32mON "
  fi
}


show_menu()
{
    NORMAL=`echo "\033[m"`
    MENU=`echo "\033[0;36m"` #Blue
    NUMBER=`echo "\033[0;33m"` #yellow
    FGRED=`echo "\033[41m"`
    RED_TEXT=`echo "\033[0;31m"`
    GREEN_TEXT=`echo "\033[0;32m"`
    ENTER_LINE=`echo "\033[0;33m"`
    echo -e "${MENU}************ ${NUMBER}PLS-8 Power Status `read-pin 42 `${MENU}****************************${NORMAL}"
    echo -e "${MENU}**${NUMBER}  1)${MENU} Start PLS-8 ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  2)${MENU} Show tty ports ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  3)${MENU} Show PLS-8 information ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  4)${MENU} Put PLS-8 into dual USB mode${NORMAL}"
    echo -e "${MENU}**${NUMBER}  5)${MENU} Put PLS-8 into 'airplane' mode ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  6)${MENU} Check write to Application port ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  7)${MENU} Check write to Modem port ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  8)${MENU} Get IMEI number ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  9)${MENU} USIM card status ${NORMAL}"
    echo -e "${MENU}**${NUMBER} 10)${MENU} Check for WiFi+BT chip ${NORMAL}"
    echo -e "${MENU}*********************************************${NORMAL}"
    echo -e "${ENTER_LINE}Please enter a menu option and enter or ${RED_TEXT}'x' to exit. ${NORMAL}"
    # read opt
}


function option_picked() 
{
    COLOR='\033[01;31m' # bold red
    RESET='\033[00;00m' # normal white
    MESSAGE=${@:-"${RESET}Error: No message passed"}
    echo -e "${COLOR}${MESSAGE}${RESET}"
}

clear
show_menu
while [ opt != '' ]
  do
 
  read -t 5 opt
    
  if [ $opt = "" ] && [ $? -eq 0 ]; then 
    exit;
  else
    case $opt in
    1) clear;
        option_picked "Option 1 Picked";
        echo "Starting PLS-8"
        ./start-pls8.sh
        sleep 2 &&
        clear;
        show_menu;
        ;;

    2) clear;
        option_picked "Option 2 Picked";
        devices=`ls /dev/ttyACM*`
        echo $devices
        sleep 2 &&
        clear;
        show_menu;
        ;;

    3) clear;
        option_picked "Option 3 Picked";
        ./modem-command ttyACM1 ATI1
        sleep 2 &&
        clear;
        show_menu;
        ;;

    4) clear;
        option_picked "Option 4 Picked";
        ./modem-command ttyACM1 AT^SSRVSET="actSrvSet",2
        sleep 2 &&
        clear;
        show_menu;
        ;;

    5) clear;
        option_picked "Option 5 Picked";
        ./modem-command ttyACM1 AT+CFUN=4
        sleep 2 &&
        clear;
        show_menu;
        ;;

    6) clear;
        option_picked "Option 6 Picked";
        ./modem-command ttyACM1 AT^SQPORT
        sleep 2 &&
        clear;
        show_menu;
        ;;

    7) clear;
        option_picked "Option 7 Picked";
        ./modem-command ttyACM0 AT^SQPORT
        sleep 2 &&
        clear;
        show_menu; 
        ;;

    8) clear;
        option_picked "Option 8 Picked";
        ./modem-command ttyACM1 AT+CGSN
        sleep 2 &&
        clear;
        show_menu;
        ;;

    9) clear;
        option_picked "Option 9 Picked";
        ./modem-command ttyACM1 AT^SCKS?
        sleep 2 &&
        clear;
        show_menu;
        ;;

   10) clear;
        option_picked "Option 10 Picked";
        lsusb | awk '{ if($6=="0bda:b720") { print "RTL8723BU WiFi + BT chip present" } }'
        sleep 2 &&
        clear;
        show_menu;
        ;;

    x)exit;
      ;;

    \n)exit;
      ;;

    *) clear;
       # option_picked "Pick an option from the menu";
      show_menu;
      ;;
    esac
  fi
done
