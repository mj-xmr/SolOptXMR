#!/bin/bash -e

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

#util/build-debian.sh
./ci-default --no-tests

echo "Testing the entire production chain:"
# TODO: Make optional & derive a build script from tsqsim/util/build.py:
export R_HOME=/usr/lib/R && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib
if [ "$(uname)" == "Linux" ]; then
	python3 src/prod.py
	# TODO: Mac OSX suffers from an endless loop
fi


