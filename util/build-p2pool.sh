#!/bin/bash -e

#VER=2.5
# https://github.com/SChernykh/p2pool/releases
REPO=p2pool
DIR=build
HASH_LATEST_TAG=6cf1b13500795260fd9b98de393ecd19f5aabd5d # 2.5

mkdir -p $DIR && cd $DIR
if [ ! -d $REPO ]; then
	git clone --recursive https://github.com/SChernykh/$REPO.git
fi
cd $REPO
#git checkout v${VER}
git checkout $HASH_LATEST_TAG
mkdir -p $DIR && cd $DIR
cmake ..
make -j`nproc`

