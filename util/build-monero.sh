#!/bin/bash -e

# Uses the Monero blockchain itself to extract the raw data.
# Warning! It needs a lot of disk space to do this. Expect even ~100 GB.

NPROC=2
REPO=monero
DIR=build
TARGETS="daemon"
AUTO_INSTALL_DEPS=false

DEPS="build-essential cmake pkg-config libssl-dev libzmq3-dev libunbound-dev libsodium-dev libunwind8-dev liblzma-dev libreadline6-dev libexpat1-dev libpgm-dev libhidapi-dev libusb-1.0-0-dev libprotobuf-dev protobuf-compiler libudev-dev libboost-chrono-dev libboost-date-time-dev libboost-filesystem-dev libboost-locale-dev libboost-program-options-dev libboost-regex-dev libboost-serialization-dev libboost-system-dev libboost-thread-dev python3 ccache"

if [ ! -z $1 ]; then
	NPROC=$1
fi

if [ ! -z $2 ]; then
	AUTO_INSTALL_DEPS=true
fi

echo "The '$REPO' dependencies are the following:"
echo "$DEPS"
echo ""
if [ $AUTO_INSTALL_DEPS == true ]; then
	q='Y'
else
	echo "Would you like to install them automatically? (Y/n)"
	read q
fi
if [ $q == 'n' ]; then
	echo "Skipping the installation of the dependencies."
else
	echo "installing dependencies of '$REPO'."
	sudo apt update
	sudo apt install $DEPS
fi 
mkdir -p $DIR && cd $DIR
if [ ! -d $REPO ]; then
	git clone --recursive https://github.com/monero-project/$REPO.git
fi
cd $REPO
git checkout $(git describe --tags $(git rev-list --tags --max-count=1)) # Checkout the latest tag (master is risky)
git submodule init && git submodule update --remote; git submodule sync && git submodule update
mkdir -p $DIR && cd $DIR
# Mix some ghetto tricks to minimize the build size:
cmake ../ -DCMAKE_BUILD_TYPE=Release -DSTRIP_TARGETS=ON -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF -DARCH="default"
make -j$NPROC $TARGETS

echo "Now synchronize the blockchain with ./util/run-monero.sh"

