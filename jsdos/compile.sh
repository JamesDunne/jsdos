#!/bin/bash
# NOTE: -O2 causes runtime failure in qemu
CFLAGS='-fstrength-reduce -fomit-frame-pointer -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -std=c99 -pedantic -Os -O1 -funroll-loops -DJSDOS_DEBUG -I./include'
BUILD='../build'
LOOPDEV='/dev/loop7'
IMGFILE="jsdos-`date +%Y%m%d`.img"

echo Compiling... && \
 nasm support.asm -o $BUILD/support.o -f elf64 && \
 gcc $CFLAGS -o $BUILD/start.o -c start.c && \
 gcc $CFLAGS -o $BUILD/kernel.o -c kernel.c && \
 gcc $CFLAGS -o $BUILD/sys.o -c sys.c && \
 gcc $CFLAGS -o $BUILD/stdlib.o -c stdlib.c && \
 gcc $CFLAGS -o $BUILD/stdio.o -c stdio.c && \
 gcc $CFLAGS -o $BUILD/jitlib-core.o -c myjit/jitlib-core.c && \
 echo Linking $BUILD/KERNEL64.SYS... && \
 ld -T jsdos.ld -o $BUILD/KERNEL64.SYS $BUILD/start.o $BUILD/sys.o $BUILD/support.o $BUILD/kernel.o $BUILD/stdlib.o $BUILD/stdio.o $BUILD/jitlib-core.o && \
 pushd $BUILD &>/dev/null && \
 echo Extracting $IMGFILE.gz... && \
 mkdir -p disk && \
 gzip -cd vanilla.img.gz > $IMGFILE && \
 echo losetup $LOOPDEV $IMGFILE... && \
 (losetup -d $LOOPDEV &>/dev/null; true) && \
 losetup -o32256 $LOOPDEV $IMGFILE && \
 echo Mounting $IMGFILE... && \
 sudo mount -tvfat $LOOPDEV ./disk && \
 echo Updating KERNEL64.SYS... && \
 sudo cp KERNEL64.SYS ./disk/ && \
 sleep 0.1 && \
 echo Unmounting $IMGFILE... && \
 sudo umount ./disk && \
 losetup -d $LOOPDEV && \
 echo $IMGFILE setup complete. && \
 popd &>/dev/null
