#!/bin/bash
# NOTE: -O2 causes runtime failure in qemu
CFLAGS='-fno-builtin -nostdlib -nostartfiles -nodefaultlibs -std=c99 -pedantic -Os -O1 -funroll-loops'
BUILD='../build'
LOOPDEV='/dev/loop7'

echo Compiling... && \
 gcc $CFLAGS -o $BUILD/main.o -c main.c && \
 gcc $CFLAGS -o $BUILD/kernel.o -c kernel.c && \
 gcc $CFLAGS -o $BUILD/sys.o -c sys.c && \
 gcc $CFLAGS -o $BUILD/stdlib.o -c stdlib.c && \
 gcc $CFLAGS -o $BUILD/jitlib-core.o -c myjit/jitlib-core.c && \
 echo Linking $BUILD/KERNEL64.SYS... && \
 ld -T kernel64.ld -o $BUILD/KERNEL64.SYS $BUILD/main.o $BUILD/kernel.o $BUILD/stdlib.o $BUILD/sys.o $BUILD/jitlib-core.o && \
 pushd $BUILD &>/dev/null && \
 echo Extracting os.img.gz... && \
 mkdir -p disk && \
 gzip -cd os.img.gz > os.img && \
 echo losetup $LOOPDEV os.img... && \
 (losetup -d $LOOPDEV &>/dev/null; true) && \
 losetup -o32256 $LOOPDEV os.img && \
 echo Mounting os.img... && \
 sudo mount -tvfat $LOOPDEV ./disk && \
 echo Updating KERNEL64.SYS... && \
 sudo cp KERNEL64.SYS ./disk/ && \
 sleep 0.1 && \
 echo Unmounting os.img... && \
 sudo umount ./disk && \
 losetup -d $LOOPDEV && \
 echo os.img setup complete. && \
 popd &>/dev/null
