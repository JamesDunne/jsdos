#!/bin/bash
pushd ../build &>/dev/null
qemu-system-x86_64 -smp 4,cores=4 os.img
popd &>/dev/null
