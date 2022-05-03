#!/bin/sh -e

cd externals/tsqsim/
./ci-default
cd ../..

python3 src/tests.py
# Now test unpickling:
python3 src/tests.py

