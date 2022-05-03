#!/bin/sh -e

cd externals/tsqsim/
./util/prep-env.sh
./util/deps-pull.sh
./util/deps-build.sh
./ci-default
cd ../..

python3 src/tests.py
# Now test unpickling:
python3 src/tests.py

