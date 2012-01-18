#!/bin/bash
IMGFILE="jsdos-`date +%Y%m%d`.img"

pushd ../build &>/dev/null
qemu-system-x86_64 -smp 4,cores=4 -no-kvm -hda "$IMGFILE" -name "JSDOS"
popd &>/dev/null
