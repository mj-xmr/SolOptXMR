#!/bin/bash -e


DEPS="ethtool wakeonlan git ccache cmake msr-tools libuv1-dev libzmq3-dev libsodium-dev libpgm-dev libnorm-dev libgss-dev libcurl4-gnutls-dev libhwloc-dev libuv1-dev libssl-dev libreadline-dev"

echo "Installing the following dependencies:"
echo "$DEPS"
sudo apt install $DEPS


./util/build-p2pool.sh
./util/build-xmrig.sh
./util/build-monero.sh
