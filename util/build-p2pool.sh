#!/bin/bash -e

VER=2.5
# https://github.com/SChernykh/p2pool/releases
REPO=p2pool
DIR=build

mkdir -p $DIR && cd $DIR
if [ ! -d $REPO ]; then
	git clone --recursive https://github.com/SChernykh/$REPO.git
fi
cd $REPO
git checkout v${VER}
mkdir -p $DIR && cd $DIR
cmake ..
make -j`nproc`

