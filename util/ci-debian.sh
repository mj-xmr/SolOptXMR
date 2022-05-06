#!/bin/sh -e

GEN="CodeBlocks - Unix Makefiles"

#cd externals/tsqsim/
#./util/prep-env.sh
#./util/prep-env.sh
#./util/deps-pull.sh
#./util/deps-build.sh
#./ci-default
#cd ../..

#./util/deps-build.sh
#./ci-default
#cd ../..

#python3 src/tests.py
# Now test unpickling:
#python3 src/tests.py

mkdir -p build && cd build
cmake .. -G "${GEN}" -D BUILD_QT=OFF
make

# TODO: Make optional & derive from tsqsim/util/build.py:
export R_HOME=/usr/lib/R && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib

src/opti/opti --help
src/opti/opti --horizon-days 2 --start-day 30
src/opti/opti --help

echo "Testing the entire production chain:"
cd ..
python3 src/prod.py
