# This Makefile is not used in association with buildroot.
# Instead we are using the tool chain from a successful buildroot run.
#
# If you want to use these test/try programs then patch up the sysroot and PATH variables to point to the right places.
sysroot := ../buildroot/output/staging
toolchain_prefix := arm-buildroot-linux-gnueabihf-

#PATH := /mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/:$(PATH)
PATH := $(CURDIR)/../buildroot/output/host/usr/bin/:$(PATH)

# hello-world is trivial but it's a starting point to get the compiler and linker working.
# at-cmd comiles but does not function correctly. It has been superceeded by canon-input.c
# This small programs waits forever for input from a serial device. It reads 
# the input _from_ a serial device in raw mode i.e. one char at a time. When sending input _to_ the 
# serial device is uses canonical mode i.e. input terminated with a \r character. 
# e.g echo -e 'AT\r' > /dev/ttyACM1  # Don't forget the '-e' switch for the echo command.

.PHONY: check all

check:
	@if [ ! -d "./bin" ]; then mkdir bin; fi

all: hello at-cmd canon-input modem-command non-canon-input urc-input gpio-interrupt
	if [ ! -d "./bin" ]; then echo "bin does not exist"; fi

hello : hello-world.cpp
	$(toolchain_prefix)gcc -o bin/hello $? --sysroot=$(sysroot)

at-cmd : at-cmd.c
	$(toolchain_prefix)gcc -o bin/at-cmd $? --sysroot=$(sysroot)

canon-input : canon-input.c
	$(toolchain_prefix)gcc -o bin/canon-input $? --sysroot=$(sysroot)

urc-input : urc-input.c
	$(toolchain_prefix)gcc -o bin/urc-input $? --sysroot=$(sysroot)

non-canon-input : non-canon-input.c
	$(toolchain_prefix)gcc -o bin/non-canon-input $? --sysroot=$(sysroot)

modem-command : modem-command.c
	$(toolchain_prefix)gcc -o bin/modem-command $? --sysroot=$(sysroot)

gpio-interrupt : gpio-interrupt.c
	$(toolchain_prefix)gcc -o bin/gpio-interrupt $? --sysroot=$(sysroot)

mmal-main: mmal-main.c
	$(toolchain_prefix)gcc -o $@ $? -I$(sysroot)/usr/include/interface/vcos/pthreads/ -I$(sysroot)/usr/include/interface/vmcs_host/linux/ \
  -lbcm_host -lmmal_util -lmmal_core -lmmal_vc_client -lmmal -lmmal_components -l:libEGL_static.a -l:libGLESv2_static.a --sysroot=$(sysroot)


clean:
	touch *.c *.cpp
	rm -f bin/*	
