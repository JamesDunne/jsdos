#!/bin/bash
CFLAGS='-fno-builtin -nostdlib -nostartfiles -nodefaultlibs -std=c99 -pedantic'
BUILD='../build'

gcc $CFLAGS -o $BUILD/main.o -c main.c && \
 gcc $CFLAGS -o $BUILD/kernel.o -c kernel.c && \
 gcc $CFLAGS -o $BUILD/sys.o -c sys.c && \
 gcc $CFLAGS -o $BUILD/stdlib.o -c stdlib.c && \
 gcc $CFLAGS -o $BUILD/jitlib-core.o -c myjit/jitlib-core.c && \
 ld -T kernel64.ld -o $BUILD/KERNEL64.SYS $BUILD/main.o $BUILD/kernel.o $BUILD/stdlib.o $BUILD/sys.o $BUILD/jitlib-core.o && \
 pushd $BUILD && \
 losetup -o32256 /dev/loop0 os.img && \
 sudo mount -tvfat -ouser,group /dev/loop0 ./disk && \
 sudo cp KERNEL64.SYS ./disk/ && \
 sleep 0.25 && \
 sudo umount ./disk && \
 losetup -d /dev/loop0 && \
 popd
