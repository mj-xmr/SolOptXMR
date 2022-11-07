#!/bin/bash -e

HOST=$1
WALLET_ADDRESS=$2

cd build/p2pool/build
./p2pool --host $HOST --wallet $WALLET_ADDRESS

