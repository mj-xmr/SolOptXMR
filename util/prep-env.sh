#!/bin/sh -e

mkdir -p build/bin/

cd externals/tsqsim/
#./util/prep-env.sh
cd ../..

externals/tsqsim/util/prep-env.sh
