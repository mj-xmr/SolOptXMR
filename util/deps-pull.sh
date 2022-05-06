#!/bin/sh -e




cd externals/tsqsim/
./util/deps-pull.sh
cd ../..

sudo apt install gfortran libffi-dev
#sudo apt install libboost-all-dev # Only JSON is needed for now
#sudo apt install libffi-dev
sudo apt install rapidjson-dev

pip3 install -r requirements.txt
