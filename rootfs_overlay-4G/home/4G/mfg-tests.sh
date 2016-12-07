#!/bin/bash

# This script has been modified from the original at
# .../buildrooot4G/progs/4G-test.sh


function is_connected()
{

	# We look for two strings to identify the WiFi adapter. A search for jsut ip= will fail
	# if there are two network devices i.e. eth0 and wlan0
  ip=$(lshw -c network 2>&1 | awk '/ip=/ && /driver=rtl/ { print $4}' | awk -F = '{ print $2 }')

  if [ ! -z $ip ]; then
    echo -e "is connected"
  else
    echo -e "is NOT connected"
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
    echo -e "${MENU}************ ${NUMBER}WiFi Manufacture Tests (2016)${MENU} ****************************${NORMAL}"
		echo -e ""
		echo -e "${GREEN_TEXT}>>>>>>>>      WiFi Device "`is_connected`" ${NORMAL}"
    echo -e ""
    echo -e "${MENU}**${NUMBER}  1)${MENU} WiFi Login ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  2)${MENU} Show WiFi signal level ${NORMAL}"
    echo -e "${MENU}**${NUMBER}  3)${MENU} Play short video${NORMAL}"
    echo -e "${MENU}*********************************************${NORMAL}"
    echo -e "${ENTER_LINE}Please enter a menu option 1 to 3 or ${RED_TEXT}'x'${ENTER_LINE} to exit"
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
 
  read -n 1 opt
    
  if [ "$opt" = "x" ] && [ $? -eq 0 ]; then 
    exit;
  else
    case $opt in
    1) clear;
        # option_picked "Option 1 Picked";
        /etc/init.d/S41wpa_supplicant restart  2>&1 > /dev/null
        clear;
        show_menu;
       ;;

    2) clear;
			 char=''
       until [ "$char" = "q" ];do
          awk 'NR==3 { print "Signal  level = ",$4; print "Quality level = ", $3  }''{}' /proc/net/wireless
					# sleep 1
        # watch -n 1 -t  "awk 'NR==3 {print \$4 \"00 %\"}''' /proc/net/wireless"

				echo -e "\n\nEnter 'q' to terminate"

				read -t 1 -n 1 char
				clear
				done

        clear;
        show_menu;
        ;;

    3) clear;
        option_picked "Option 3 Picked";
				/root/omxplayer -p --fps 60 -o hdmi /mnt/data1/big_buck_bunny_720p_50mb.mp4 2>&1 > /dev/null
        clear;
        show_menu;
        ;;

    x)exit;
    #	RESET='\033[00;00m' # normal white
    #  echo -e ${$RESET}
      `echo "\033[m"`
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
