#!/bin/bash

pushd ../release
cp ../build/os.img .
rm os.img.gz
gzip os.img
mv os.img.gz os-`date +%Y%m%d`.img.gz
popd
