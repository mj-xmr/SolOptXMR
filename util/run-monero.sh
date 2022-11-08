#!/bin/bash -e

cd build/monero/build/bin
./monerod --enable-dns-blocklist --prune-blockchain --zmq-pub tcp://0.0.0.0:18083 --restricted-rpc
