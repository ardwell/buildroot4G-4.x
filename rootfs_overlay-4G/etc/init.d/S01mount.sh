#!/bin/bash
# 
# Mount/unmount block devices

start() 
{
  mkdir -p /mnt/shadow-rootfs
  mkdir -p /mnt/data1
  mkdir -p /mnt/data2
  mkdir -p /mnt/upgrade

  # Don't just automatically mount mmcblk0p3! This could be the current _live_ root-fs.
  # Check which partition is being used from boot partition (/dev/mmcblk0p1) cmdline.txt file
  #
  # mount  -t ext4 /dev/mmcblk0p3 /mnt/shadow-rootfs
  mount -t ext4 /dev/mmcblk0p5 /mnt/data1
  mount -t ext4 /dev/mmcblk0p6 /mnt/data2
  mount -t ext4 /dev/mmcblk0p7 /mnt/upgrade
}

stop() 
{
  umount  /mnt/data1
  umount  /mnt/data2
  umount  /mnt/upgrade
}

case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  restart|reload)
        stop
        start
        ;;
  *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
esac

