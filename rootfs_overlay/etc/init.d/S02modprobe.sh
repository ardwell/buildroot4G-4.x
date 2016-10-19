#!/bin/sh
#
# Load modules 
#

start()
{
  modprobe --syslog brcmfmac
  modprobe --syslog 8192cu
}

stop()
{
  modprobe -r brcmfmac
  modprobe -r 8192cu
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

return $?

