SRCPATH=.
prefix=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/FFmpeg-sources/ffmpeg_build
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
SYS_ARCH=ARM
SYS=LINUX
CC=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-gcc
CFLAGS=-Wno-maybe-uninitialized -Wshadow -O3 -ffast-math  -Wall -I. -I$(SRCPATH) -march=armv6 -mfloat-abi=hard -std=gnu99 -fomit-frame-pointer -fno-tree-vectorize
COMPILER=GNU
COMPILER_STYLE=GNU
DEPMM=-MM -g0
DEPMT=-MT
LD=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-gcc -o 
LDFLAGS= -march=armv6 -mfloat-abi=hard -lm -lpthread -ldl
LIBX264=libx264.a
AR=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-ar rc 
RANLIB=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-ranlib
STRIP=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-strip
INSTALL=install
AS=/mnt/users/ed/build/software/bitvu/buildroot4G-4.x/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-gcc
ASFLAGS= -I. -I$(SRCPATH) -c -DSTACK_ALIGNMENT=16 -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8
RC=
RCFLAGS=
EXE=
HAVE_GETOPT_LONG=1
DEVNULL=/dev/null
PROF_GEN_CC=-fprofile-generate
PROF_GEN_LD=-fprofile-generate
PROF_USE_CC=-fprofile-use
PROF_USE_LD=-fprofile-use
HAVE_OPENCL=yes
default: cli
install: install-cli
default: lib-static
install: install-lib-static
LDFLAGSCLI = -ldl 
CLI_LIBX264 = $(LIBX264)
