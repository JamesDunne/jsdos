#!/bin/sh
scons -Q -j 4 && ./post-install.sh && ./run.sh
