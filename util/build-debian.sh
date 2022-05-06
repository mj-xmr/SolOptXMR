#!/bin/sh -e

GEN="CodeBlocks - Unix Makefiles"

mkdir -p build && cd build
cmake .. -G "${GEN}" -D BUILD_QT=OFF
make

# TODO: Make optional & derive a build script from tsqsim/util/build.py:
export R_HOME=/usr/lib/R && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib

src/opti/opti --help
src/opti/opti --horizon-days 2 --start-day 30
src/opti/opti --help
