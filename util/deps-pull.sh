#!/bin/bash -e

cd externals/tsqsim/
./util/deps-pull.sh
cd ../..

if [ "$(uname)" == "Darwin" ]; then
	HOMEBREW_NO_AUTO_UPDATE=1 brew install rapidjson libffi
elif [ "$(uname)" == "Linux" ]; then
	sudo apt install gfortran libffi-dev
	#sudo apt install libboost-all-dev # Only JSON is needed for now
	#sudo apt install libffi-dev
	sudo apt install rapidjson-dev
fi

pip3 install -r requirements.txt
