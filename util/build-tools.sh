#!/bin/bash -e


DEPS="ethtool wakeonlan git cmake libuv1-dev libzmq3-dev libsodium-dev libpgm-dev libnorm-dev libgss-dev libcurlpp-dev libhwloc-dev libuv1-dev libssl-dev libreadline-dev"

echo "Installing the following dependencies:"
echo "$DEPS"
sudo apt install $DEPS


./util/build-p2pool.sh
#./util/build-xmrig.sh
