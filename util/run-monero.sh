#!/bin/bash -e

cd build/monero/build/bin
 ./monerod --enable-dns-blocklist --prune-blockchain
#./monerod --enable-dns-blocklist --prune-blockchain --zmq-pub tcp://127.0.0.1:18083 

