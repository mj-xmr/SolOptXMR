#!/bin/sh -e

GEN="CodeBlocks - Unix Makefiles"

#cd externals/tsqsim/
#./util/prep-env.sh
#./util/deps-pull.sh
#./util/deps-build.sh
#./ci-default
#cd ../..

#python3 src/tests.py
# Now test unpickling:
#python3 src/tests.py

mkdir -p build && cd build
cmake .. -G "${GEN}" -D BUILD_QT=OFF
make
