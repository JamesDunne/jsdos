#!/bin/bash
pushd ../build &>/dev/null
qemu-system-x86_64 -curses -smp 4,cores=4 -no-kvm os.img
popd &>/dev/null
