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
if [ "$(uname)" == "Linux" ]; then
./ci-default -o "VERBOSE_FUNCTIONS=OFF"
else
./ci-default -o "VERBOSE_FUNCTIONS=ON"
fi

if [ $# -eq 1 ]; then
	echo "Production test not requested. Exiting."
	exit 0
fi
echo "Testing the entire production chain:"
# TODO: Make optional & derive a build script from tsqsim/util/build.py:
export R_HOME=/usr/lib/R && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$R_HOME/lib
if [ "$(uname)" == "Linux" ]; then
	python3 src/prod.py
fi
#else
	# TODO: Mac OSX suffers from an endless loop (?) in prod.py
	#python3 src/prod.py
	#cd build/default-static-release/bin/
	#./opti --out "/tmp" # todo: should be defaulted in the CI App's CLI
#fi

