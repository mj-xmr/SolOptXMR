#!/bin/bash -e

WALLET_ADDRESS=$1
HOST=localhost
if [ ! -z $2 ]; then
	HOST=$2
fi


cd build/p2pool/build
./p2pool --host $HOST --wallet $WALLET_ADDRESS

