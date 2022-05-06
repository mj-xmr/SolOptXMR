#!/bin/sh -e

#sudo apt install gfortran libffi-dev
#sudo apt install libffi-dev
pip3 install -r requirements.txt


cd externals/tsqsim/
./util/deps-pull.sh
cd ../..
