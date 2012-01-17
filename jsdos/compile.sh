#!/bin/bash
CFLAGS='-fno-builtin -nostdlib -nostartfiles -nodefaultlibs -std=c99 -pedantic'
BUILD='../build'
LOOPDEV='/dev/loop7'

gcc $CFLAGS -o $BUILD/main.o -c main.c && \
 gcc $CFLAGS -o $BUILD/kernel.o -c kernel.c && \
 gcc $CFLAGS -o $BUILD/sys.o -c sys.c && \
 gcc $CFLAGS -o $BUILD/stdlib.o -c stdlib.c && \
 gcc $CFLAGS -o $BUILD/jitlib-core.o -c myjit/jitlib-core.c && \
 ld -T kernel64.ld -o $BUILD/KERNEL64.SYS $BUILD/main.o $BUILD/kernel.o $BUILD/stdlib.o $BUILD/sys.o $BUILD/jitlib-core.o && \
 pushd $BUILD && \
 mkdir -p disk && \
 gzip -cd os.img.gz > os.img && \
 losetup -d $LOOPDEV &>/dev/null && \
 losetup -o32256 $LOOPDEV os.img && \
 sudo mount -tvfat -ouser,group $LOOPDEV ./disk && \
 sudo cp KERNEL64.SYS ./disk/ && \
 sleep 0.1 && \
 sudo umount ./disk && \
 losetup -d $LOOPDEV && \
 popd
