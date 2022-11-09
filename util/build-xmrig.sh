#!/bin/bash -e

VER=6.18.1
#https://github.com/xmrig/xmrig/releases
REPO=xmrig
DIR=build


DIR_THIS="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mkdir -p $DIR && cd $DIR
if [ -d $REPO ]; then
	ls
	rm $REPO -fr
fi

git clone --recursive https://github.com/$REPO/$REPO.git
cd $REPO
git checkout v${VER}
git apply "$DIR_THIS/patches/xmrig-${VER}.patch" # Here be dragons!
mkdir -p $DIR && cd $DIR
cmake .. -G "CodeBlocks - Unix Makefiles"
make -j$(nproc)
#make

