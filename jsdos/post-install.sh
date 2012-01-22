#!/bin/bash
BUILD='../build'
LOOPDEV='/dev/loop7'
IMGFILE="jsdos-`date +%Y%m%d`.img"

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
 sleep 0.05 && \
 echo Unmounting $IMGFILE... && \
 sudo umount ./disk && \
 losetup -d $LOOPDEV && \
 echo $IMGFILE setup complete. && \
 popd &>/dev/null
