#!/bin/bash

pushd ../build
qemu-system-x86_64 -smp 4,cores=4 os.img
popd
