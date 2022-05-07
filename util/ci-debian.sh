#!/bin/sh -e

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

python3 src/tests.py
# Now test unpickling:
python3 src/tests.py

util/build-debian.sh

echo "Testing the entire production chain:"
# TODO: Make optional & derive a build script from tsqsim/util/build.py:
export R_HOME=/usr/lib/R && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib
python3 src/prod.py
