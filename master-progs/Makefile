# This Makefile is not used in association with buildroot.
# Instead we are using the tool chain from a successful buildroot run.
#
# If you want to use these test/try programs then patch up the sysroot and PATH variables to point to the right places.
sysroot := ../buildroot/output/staging
toolchain_prefix := arm-buildroot-linux-gnueabihf-

PATH := /mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/:$(PATH)

# hello-world is trivial but it's a starting point to get the compiler and linker working.
# at-cmd comiles but does not function correctly. It has been superceeded by canon-input.c
# This small programs waits forever for input from a serial device. It reads 
# the input _from_ a serial device in raw mode i.e. one char at a time. When sending input _to_ the 
# serial device is uses canonical mode i.e. input terminated with a \r character. 
# e.g echo -e 'AT\r' > /dev/ttyACM1  # Don't forget the '-e' switch for the echo command.

all: gettime hello gpio-interrupt

hello : hello-world.cpp
	$(toolchain_prefix)gcc -o hello $? --sysroot=$(sysroot)

gpio-interrupt : gpio-interrupt.c
	$(toolchain_prefix)gcc -o gpio-interrupt $? --sysroot=$(sysroot)

gettime : gettime.c
	$(toolchain_prefix)gcc -o gettime $? --sysroot=$(sysroot)

clean:
	touch *.c *.cpp	
