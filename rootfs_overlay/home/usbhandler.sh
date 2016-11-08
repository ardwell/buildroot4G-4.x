#!/bin/bash

# Handler that is called when a usb device is inserted or removed. 
# We are concerned for the rtl8192eu usb dongle.

# This script is not part of the init.d sequence

# set >> /root/usb-hotplug

echo $ACTION >> /root/usb-hotplug
echo $PRODUCT >> /root/usb-hotplug
echo $MODALIAS >> /root/usb-hotplug
echo ""; echo ""

if [ -n "$ACTION" ] && [ "$PRODUCT" = "2357/109/200" ]; then

	if [ "$ACTION" = "add" ]; then

		# echo "here 3" >> /root/usb-hotplug
		modprobe -abq $MODALIAS

		# Restart relevant init.d script
		. /etc/default/wifi-mode

  	if [ "$WIFI_MODE" = "AP" ]; then
			echo "restarting S41ap" >> /root/usb-hotplug
			/etc/init.d/S41ap start
		else
			echo "Restarting S41wpa_supplicant" >> /root/usb-hotplug

			# Why do we need this small delay?
			sleep 1
			/etc/init.d/S41wpa_supplicant start
		fi

	elif [ "$ACTION" = "remove" ]; then

		# echo "here 4" >> /root/usb-hotplug
		. /etc/default/wifi-mode

	  if [ "$WIFI_MODE" = "AP" ]; then
			echo "stopping S41ap " >> /root/usb-hotplug
			/etc/init.d/S41ap stop 
		else
    	echo "Stopping S41wpa_supplicant" >> /root/usb-hotplug
			/etc/init.d/S41wpa_supplicant stop
		fi
	fi
fi

exit 0


